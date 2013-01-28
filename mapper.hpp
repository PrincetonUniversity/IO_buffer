#ifndef MAPPER_F_FLO
#define MAPPER_F_FLO

#include "abstract_policy.hpp"
#include <list>
#include <mutex>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <exception>
#include <memory>
#include <atomic>
#include "buffer.hpp"

#define FINT long long

struct statistics{
  size_t n_read_buff;
  size_t n_read_file;
  size_t n_write_buff;
  size_t n_write_file;
  size_t n_IO_avoided;
  size_t n_fastwrite;
  size_t n_fastread;
  size_t n_not_modified;
  size_t n_blanked;

  statistics(): n_read_buff(0),
		n_read_file(0),
		n_write_buff(0),
		n_write_file(0),
		n_IO_avoided(0),
		n_fastwrite(0),
		n_fastread(0),
		n_not_modified(0),
		n_blanked(0)
  {};

  void dump(std::string filename){
    std::ofstream ofs(filename);
    ofs << n_read_buff << " individual reads\n";
    ofs << n_read_file << " chunks read from disk\n";
    ofs << n_write_buff << " individual writes\n";
    ofs << n_write_file << " chunks written to disk\n";
    ofs << n_IO_avoided << " non-optimized (chunk miss) IO operations in RAM\n";
    ofs << n_fastwrite  << " optimized IO operations in RAM\n";
    ofs << n_fastread   << " optimized IO operations in RAM\n";

    ofs << n_not_modified << " writes avoided as chunk not modified in RAM\n";
    ofs << n_blanked << " reads saved because chunk not on file\n";
  }
};



template <class T>
class mapper{
public:

  typedef std::shared_ptr<abstract_policy<T> > p_abstract_policy;

  typedef std::shared_ptr<mapper<T> > p_mapper;

  typedef typename node<T>::p_node p_node;
  typedef typename node<T>::chunk chunk;
 
  ~mapper(){     
    p_abstract_policy pol(policy.lock());
      // if the pol is already destroyed, the mem has already been
      // deallocated,
    stat.dump(filename() + ".stat");
    if (pol){
      pol->return_all_mem(my_mapperid);
    }};

  void set(size_t pos, 
	   const T& t, 
	   size_t threadnum) { set_(pos, t, threadnum);};
  void set(size_t pos, 
	   size_t N, 
	   const T* t, 
	   size_t threadnum) { set_(pos, N, t, threadnum);};

  T get(size_t pos, size_t threadnum) { return get_(pos, threadnum);};  
  void get(size_t pos, 
	   size_t N, 
	   T* t, 
	   size_t threadnum) { return get_(pos, N, t, threadnum);};  

  // write all buffers to disk
  void flush(){ write_buffers_(true);};
  // free all buffers without saving
  void forget(){ write_buffers_(false);};

  void sync(size_t chunk_index){ sync_chunk_(chunk_index);};

  // API with pool
  chunk release_chunk( size_t chunk_index,
		       bool save=true){ return release_chunk_(chunk_index, save);};

  const std::string& filename() const { return file_data.filename();};

  p_abstract_policy get_policy(){ return policy.lock();};

  static p_mapper
    factory(const std::string& fn, 
	    size_t index,
	    p_abstract_policy pap){
    p_mapper pm(new mapper(fn,index,pap));
    pap->assign_mapper(pm,index);
    return pm;
  };
  
private:

  typedef std::weak_ptr<abstract_policy<T> > pw_abstract_policy;
  pw_abstract_policy policy;

  // always remember last access point, to fast 
  // track consecutive IO to one chunk 
  struct tcurrentinfo{
    // adress where chunk in memory is stored
    typename chunk::iterator cchunk;

    // index of chunk
    volatile size_t index;

    // writeindex: seperate index for writing, to correctly 
    // update the status of the node if we write for the first time
    size_t writeindex;

    // mutex to assert that if one thread frees the node this ci points to,
    // nothing bad happens. Will lock this mutex to (a) use the cchunk and
    // (b) free the associated node
    std::mutex mut_ex;

    tcurrentinfo():index(-1),
		   writeindex(-1)
    {};

  };

  std::atomic_int nodeusecount;
  std::atomic_bool nodeusebarrier;

  std::vector<tcurrentinfo> currentinfo;

  size_t chunk_size;

  filer<T> file_data;  
  
  size_t my_mapperid;	     

  std::vector<node<T> > nodes;  	     

  statistics stat;

  mapper(const std::string& bufferfile,
	 size_t ind,
	 p_abstract_policy m):
    policy(m),
    currentinfo(policy.lock()->nthread()),
    chunk_size(m->chunk_size()),
    file_data(bufferfile.c_str(), chunk_size),
    my_mapperid(ind)
  {    
    nodeusecount.store(0);
    nodeusebarrier.store(false);
  };    

  void sync_chunk_( size_t chunk_index){
    node<T>& cn(nodes[chunk_index]);
    if ( cn.status==node<T>::modified)
      file_data.write_chunk(chunk_index,&cn.data[0]);    
  };

  chunk release_chunk_(size_t chunk_index, bool save){    

    while (nodeusebarrier.exchange(true)){};
    ++nodeusecount;
    nodeusebarrier = false;

    node<T>& cn(nodes[chunk_index]);

    // lock access to the chunk node
    std::lock_guard<std::mutex> lg(cn.mut_ex);

    //lock all the current_info muteces, 
    // if they point to node in question, wait for them
    std::for_each(currentinfo.begin(),
		  currentinfo.end(),
		  [chunk_index](tcurrentinfo& t){
		    // t.index is guaranteed to only be pointed
		    // to an unlocked node. since we lock the
		    // node that is about to expire above,
		    // the check below is save
		    if (chunk_index == t.index){
		      std::lock_guard<std::mutex> lg(t.mut_ex);
		      // recheck, since we lock only inside the if, the
		      // index value might have changed
		      // we do it this way to avoid unnecessarily locking
		      // all the mut_exes
		      if (chunk_index == t.index){
			t.index=-1; t.writeindex=-1;
		      }
		    }});
    // this thread now has exclusive access to this node

    // if it is modified, need to write changes
    if ( save && cn.status==node<T>::modified){
      
      file_data.write_chunk(chunk_index,&cn.data[0]);
      ++stat.n_write_file;
    }else{
      ++stat.n_not_modified;
    }

    cn.status=node<T>::stored;
    chunk c;
    c.swap(cn.data);
    // the lock_guard frees the mut_ex
    --nodeusecount;
    return c;    
  };

  // write all buffers to file, free all the memory
  void write_buffers_(bool save){    
    policy.lock()->return_all_mem(my_mapperid, save);
  };

  // find pointer to node and update ci
  // precondition: ci.mutex is locked
  // postcondition: ci.mutex is still locked, and its data
  // pointer points to correct node
  void prepare_node( size_t index, 
		     tcurrentinfo& ci, 
		     size_t threadnum,
		     const bool modify){

    // mark currentinfo als invalid, free mutex for ci while we search for node

    ci.index = -1;
    ci.writeindex = -1;
    ci.mut_ex.unlock();
    // mut_ex is unlocked to avoid possible deadlock while we search for memory

    // if too small nodes, then we have a problem: need to
    // resize without breaking other threads 

    // Herb Sutters spinlock
    while (nodeusebarrier.exchange(true)){};
    if (index >= nodes.size()){
      // problem: if we just resize, then
      // we might copy, and that will break many things,
      // so we need to wait for all other threads
      while (nodeusecount){};
      nodes.resize(index+20);
    }
    ++nodeusecount;
    nodeusebarrier = false;

    node<T>& cn(nodes[index]);    
    std::lock_guard<std::mutex> lg(cn.mut_ex);

    // if cn.data not associated, we need to get memory and potentially load it
    if (!cn.data.size()){      

      // get chunk of memory
      cn.data = policy.lock()->get_memory(my_mapperid, index, threadnum);

      if (cn.status == node<T>::stored){
	// load it from disk if it is on disk
	file_data.read_chunk(index, &(cn.data[0]));
	++stat.n_read_file;
      }else{
	// blank the memory, as it still contains swapped-out block
	std::fill(cn.data.begin(), cn.data.end(), 0);
	++stat.n_blanked;
      }

      cn.status=node<T>::loaded;
	
    }

    // lock mutex again, update information
    ci.mut_ex.lock();
    ci.index = index;
    ci.cchunk = cn.data.begin();

    if (modify){
      cn.status = node<T>::modified;
      ci.writeindex = index;
    }

    --nodeusecount;
    // lock_guard frees mutex for cn here
  };

  // change value at pos to t
  void set_(size_t pos, const T& t, size_t threadnum){
    size_t index=pos / chunk_size;
    size_t offset=pos%chunk_size;

    ++stat.n_write_buff;

    tcurrentinfo& ci(currentinfo[threadnum]);
    std::lock_guard<std::mutex> lg(ci.mut_ex);

    // if node is not correct, set mutex free again, ask for correct one
    if (index != ci.writeindex){
      prepare_node( index, ci, threadnum, true);
    }else{
      ++stat.n_fastwrite;
    }

    // node is now correct, do atomic write
    *(ci.cchunk + offset) = t;

  };

  // change N values starting from pos to t
  void set_(size_t pos, size_t N, const T* t, size_t threadnum){
    size_t index=pos / chunk_size;
    size_t offset=pos%chunk_size;

    tcurrentinfo& ci(currentinfo[threadnum]);
    std::lock_guard<std::mutex> lg(ci.mut_ex);

    if (index != ci.writeindex){
      prepare_node( index, ci, threadnum, true);
    }

    const T* p_source(t);
    auto p_target(ci.cchunk + offset);

    for (size_t i =0; i < N; ++i){
      if (offset == chunk_size){
	offset = 0;
	prepare_node(++index, ci, threadnum, true);
	p_target = ci.cchunk;
      }
      *(p_target++) = *(p_source++);
    }
    stat.n_write_buff += N;
  };

  T get_(size_t pos, size_t threadnum){
    size_t index=pos / chunk_size;
    size_t offset=pos%chunk_size;
 
    ++stat.n_read_buff;

    tcurrentinfo& ci(currentinfo[threadnum]);
    std::lock_guard<std::mutex> lg(ci.mut_ex);

    if (index != ci.index){
      prepare_node(index, ci, threadnum, false);
    }else{
      ++stat.n_fastread;
    }

    return *(ci.cchunk + offset);
  };

  void get_(size_t pos, size_t N, T* t, size_t threadnum){
    size_t index=pos / chunk_size;
    size_t offset=pos%chunk_size;
    
    tcurrentinfo& ci(currentinfo[threadnum]);
    std::lock_guard<std::mutex> lg(ci.mut_ex);

    if (index != ci.writeindex){
      prepare_node( index, ci, threadnum, false);
    }

    T* p_target(t);
    auto p_source(ci.cchunk + offset);

    for (size_t i =0; i < N; ++i){
      if (offset == chunk_size){
	offset = 0;
	prepare_node( ++index, ci, threadnum, false);
	p_source = ci.cchunk;
      }
      *(p_target++) = *(p_source++);
    }
    stat.n_read_buff += N;
  };
};

#endif


