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
#include <cstring>
#include <atomic>
#include "buffer.hpp"
#include "spin_lock.hpp"

#define FINT long long

typedef std::mutex spin_mutex;

struct statistics{
  size_t n_read_buff;
  size_t n_read_file;
  size_t n_write_buff;
  size_t n_write_file;
  size_t n_IO_avoided;
  size_t n_fastwrite;
  size_t n_fastread;
  size_t n_not_modified;
  size_t n_get_pointer;
  size_t n_free_pointer;
  size_t n_blanked;
  size_t n_undead;
  size_t n_free_delete;

  statistics(): n_read_buff(0),
		n_read_file(0),
		n_write_buff(0),
		n_write_file(0),
		n_IO_avoided(0),
		n_fastwrite(0),
		n_fastread(0),
		n_not_modified(0),
		n_get_pointer(0),
		n_free_pointer(0),
		n_blanked(0),
                n_undead(0),
                n_free_delete(0)
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
    ofs << n_get_pointer << " requests for getting raw pointer\n";
    ofs << n_free_pointer << " requests to free raw pointer\n";
    ofs << n_not_modified << " writes avoided as chunk not modified in RAM\n";
    ofs << n_blanked << " reads saved because chunk not on file\n";
    ofs << n_undead << " attempts to free a burrowed node\n";
    ofs << n_free_delete << " times that free_pointer really frees the memory\n";
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
    // the policy has taken care of freeing the memory already,
    // otherwise we have serious problems
#ifdef COLLECT_STATISTICS
    stat.dump(filename() + ".stat");
#endif    
    ensure_all_chunks_stored_();
    nodes.clear();
    currentinfo.clear();
  };

  void set(size_t pos, 
	   const T& t, 
	   size_t threadnum) { set_(pos, t, threadnum);};
  void set(size_t pos, 
	   size_t N, 
	   const T* t, 
	   size_t threadnum) { set_(pos, N, t, threadnum);};
  void setchunk(size_t N, 
		const T* t, 
		size_t threadnum) { setchunk_(N, t, threadnum);};

  void atomic_add(size_t pos, 
		  const T& t, 
		  size_t threadnum) { set_(pos, t, threadnum);};
  void atomic_add(size_t pos, 
		  size_t N, 
		  const T* t, 
		  size_t threadnum) { set_(pos, N, t, threadnum);};


  T get(size_t pos, size_t threadnum) { return get_(pos, threadnum);};  
  void get(size_t pos, 
	   size_t N, 
	   T* t, 
	   size_t threadnum) { get_(pos, N, t, threadnum);};  

  void getchunk(size_t N, 
		T* t,
		size_t threadnum) { getchunk_(N, t, threadnum);};  


    void get_pointer(size_t N,
		     T** values,
		     size_t threadnum,
		     size_t& blocksize){ blocksize = chunk_size; 
	getpointer_(N,values, threadnum);};

    void free_pointer(size_t N){ freepointer_(N);};


  // write all buffers to disk
  void flush(){ write_buffers_(true);};
  // free all buffers without saving
  void forget(){ write_buffers_(false);};

  // 
  void remove_temporary_chunks(){ ensure_all_chunks_stored_();};

  // write buffer to disk if modified, but keep it in memory
  void sync(size_t chunk_index){ sync_chunk_(chunk_index);};

  // API with pool, save determines whether we write chunk to disk
  // before returning, force whether we will also release 
  // a chunk currently used by another thread
  chunk release_chunk( size_t chunk_index,
		       bool save=true,
		       bool force=true){ 
    return release_chunk_(chunk_index, save, force);};

  const std::string& filename() const { return file_data.filename();};

  p_abstract_policy get_policy(){ return policy.lock();};

  static p_mapper
    factory(const std::string& fn, 
	    size_t index,
	    p_abstract_policy pap,
	    bool reopen = false){
    p_mapper pm(new mapper( fn, index, pap, reopen));
    pap->assign_mapper( pm, index);
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

    // alive ... provide fast access to node index, managed by policy
    // zombie ... chunk already freed by policy, but still here
    //            because one thread needs it
    // ghost ... like zombie, but no save if it finally dies
    enum status_type {alive, zombie, ghost};
  
    status_type status;

    tcurrentinfo():index(-1),
		   writeindex(-1),
		   status(alive)
    {};

  };

  std::atomic_int nodeusecount;
  std::atomic_bool nodeusebarrier;

  std::atomic_int nodereleasecount;
  std::atomic_bool nodereleasebarrier;

  std::vector<tcurrentinfo> currentinfo;

  size_t chunk_size;

  filer<T> file_data;  
  
  size_t my_mapperid;	     

  std::vector<node<T> > nodes;  	     

  statistics stat;

  mapper(const std::string& bufferfile,
	 size_t ind,
	 p_abstract_policy m,
	 bool reopen = false):
    policy(m),
    currentinfo(policy.lock()->nthread()),
    chunk_size(m->chunk_size()),
    file_data(bufferfile.c_str(), chunk_size, reopen),
    my_mapperid(ind)
  {    

    if (reopen){
      // if we reopen, we have to mark all nodes as stored
      nodes.resize(file_data.filesize());      
      std::for_each(nodes.begin(), 
		    nodes.end(),
		    [](node<T>& n){ n.status = node<T>::stored;});
    }     

    nodeusecount.store(0);
    nodeusebarrier.store(false);
    nodereleasecount.store(0);
    nodereleasebarrier.store(false);
  };    

  void sync_chunk_( size_t chunk_index){
    if (chunk_index >= nodes.size())
      return;
    node<T>& cn(nodes[chunk_index]);
    if ( cn.status==node<T>::modified){
      file_data.write_chunk(chunk_index,&cn.data[0]);    
      cn.status = node<T>::stored;
    }
  };

  bool deprecate_use(size_t chunk_index,
		     bool save,
		     bool force){

    node<T>& cn(nodes[chunk_index]);
    if (cn.borrowed){        
        cn.undead = true;
#ifdef COLLECT_STATISTICS
        ++stat.n_undead;
#endif
        return false;
    }
        
    bool success(true);

    //lock all the current_info muteces, 
    // if they point to node in question, wait for them
    std::for_each(currentinfo.begin(),
		  currentinfo.end(),
		  [chunk_index, save, force, &success](tcurrentinfo& t){
		    // t.index is guaranteed to only be pointed
		    // to an unlocked node. since we lock the
		    // node that is about to expire above,
		    // only possibility is that some chunk already points at 
		    // this node, then we cannot deallocate 
		    if (chunk_index == t.index){
		      if (force){
			t.index = -1;
			t.writeindex = -1;
		      }else{
			if (save){
			  t.status=tcurrentinfo::zombie;
			}else{
			  t.status=tcurrentinfo::ghost;
			}
			success = false;
		      }
		    }
		    });

    return success;
  }

  void store_chunk_(node<T>& cn,
		    size_t chunk_index,
		    bool save){    
    // if it is modified, need to write changes
    if ( save && cn.status==node<T>::modified){
      
      file_data.write_chunk(chunk_index,&cn.data[0]);
#ifdef COLLECT_STATISTICS
      ++stat.n_write_file;

    }else{
      ++stat.n_not_modified;
#endif
    }
    cn.status=node<T>::stored;
  };

  void ensure_all_chunks_stored_(){
    std::for_each(currentinfo.begin(),
		  currentinfo.end(),
		  [this](tcurrentinfo& t){
		    this->invalidate_info(t);
		  });
  }

  chunk release_chunk_(size_t chunk_index, // index of chunk to release
		       bool save, // save to disk ?
		       bool force) // force delete also if chunk used elsewhere
  { 

    if (chunk_index >= nodes.size()){
      std::cerr << "ERROR: releasing chunk "<<chunk_index
		<<" we do not have - bad\n";
      throw (E_invalid_mapper_id(chunk_index));
    }      

    while (nodereleasebarrier.exchange(true)){};
    ++nodereleasecount;
    nodereleasebarrier = false;

    node<T>& cn(nodes[chunk_index]);

    // lock access to the chunk node
    std::lock_guard< decltype(cn.mut_ex) > lg(cn.mut_ex);

    // this thread now has exclusive access to this node
        
    // ask whether chunk can be freed
    if ( ! deprecate_use(chunk_index, save, force) ){
      // chunk is in use: create new chunk to return to policy
      --nodereleasecount;
      chunk c(chunk_size);      
      return c;
    }

    store_chunk_(cn, chunk_index, save);

    chunk c;
    c.swap(cn.data);
    // the lock_guard frees the mut_ex
    --nodereleasecount;

    return c;    
  };

  void invalidate_info( tcurrentinfo& ci ){

    size_t oldindex = ci.index;
    // mark currentinfo als invalid
    ci.index = -1;
    ci.writeindex = -1;

    if (oldindex != static_cast<size_t>(-1)){
      if (oldindex < nodes.size()){
	node<T>& cn(nodes[oldindex]);      
	std::lock_guard< decltype(cn.mut_ex) > lg(cn.mut_ex);
	
	if (ci.status != tcurrentinfo::alive){
	  bool save = true;
	  if (ci.status == tcurrentinfo::ghost)
	    save = false;
	  
	  if (deprecate_use(oldindex, save, false)){
	    // we can now free chunk, no one uses it any more
	    store_chunk_(cn, oldindex, save);
	    cn.data.resize(0);
	    cn.data.shrink_to_fit();
	  }
	}    
      }  
    }    
    ci.status = tcurrentinfo::alive;    

  }

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

    // if too small nodes, then we have a problem: need to
    // resize without breaking other threads 

    // Herb Sutters spinlock
    while (nodeusebarrier.exchange(true)){};
    if (index >= nodes.size()){
      // problem: if we just resize, then
      // we might copy, and that will break many things,
      // so we need to wait for all other threads
      while (nodeusecount){};
      // also assert that no node is being released right now
      while (nodereleasebarrier.exchange(true)){};
      while (nodereleasecount){};
      nodes.resize(index+20);
      nodereleasebarrier = false;
    }
    ++nodeusecount;

    invalidate_info(ci);

    nodeusebarrier = false;

    node<T>& cn(nodes[index]);    
    std::lock_guard< decltype(cn.mut_ex) > lg(cn.mut_ex);

    // if cn.data not associated, we need to get memory and potentially load it
    if (!cn.data.size()){      

      // get chunk of memory
      cn.data = policy.lock()->get_memory(my_mapperid, index, threadnum);
      cn.undead = false;

      if (cn.status == node<T>::stored){
	// load it from disk if it is on disk
	file_data.read_chunk(index, &(cn.data[0]));
#ifdef COLLECT_STATISTICS
	++stat.n_read_file;
#endif
      }else{
	// blank the memory, as it still contains swapped-out block
	std::fill(cn.data.begin(), cn.data.end(), 0);
#ifdef COLLECT_STATISTICS
	++stat.n_blanked;
#endif
      }

      cn.status=node<T>::loaded;
      
    }

    // lock mutex again, update information
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

#ifdef COLLECT_STATISTICS
    ++stat.n_write_buff;
#endif

    tcurrentinfo& ci(currentinfo[threadnum]);

    // if node is not correct, set mutex free again, ask for correct one
    if (index != ci.writeindex){
      prepare_node( index, ci, threadnum, true);
    }
#ifdef COLLECT_STATISTICS
else{
      ++stat.n_fastwrite;
    }
#endif

    // node is now correct, do write
    *(ci.cchunk + offset) = t;

  };

  // change N values starting from pos to t
  void set_(size_t pos, size_t N, const T* t, size_t threadnum){
    size_t index=pos / chunk_size;
    size_t offset=pos%chunk_size;

    tcurrentinfo& ci(currentinfo[threadnum]);

    if (index != ci.writeindex){
      prepare_node( index, ci, threadnum, true);
    }

    const T* p_source(t);
    auto p_target(ci.cchunk + offset);

    for (size_t i =0; i < N; ++i){
      if (offset++ == chunk_size){
	offset = 0;
	prepare_node(++index, ci, threadnum, true);
	p_target = ci.cchunk;
      }
      *(p_target++) = *(p_source++);
    }
#ifdef COLLECT_STATISTICS
    stat.n_write_buff += N;
#endif
  };

  void setchunk_(size_t index, const T* t, size_t threadnum){

#ifdef COLLECT_STATISTICS
    ++stat.n_read_buff;
#endif

    tcurrentinfo& ci(currentinfo[threadnum]);

    if (index != ci.index){
      prepare_node(index, ci, threadnum, true);
    }

#ifdef COLLECT_STATISTICS
    ++stat.n_read_buff += chunk_size;
#endif

    const T* p_source(t);
    T* p_target(&(*ci.cchunk));

    memcpy(p_target, p_source, chunk_size * sizeof(T) );
  };

  T get_(size_t pos, size_t threadnum){
    size_t index=pos / chunk_size;
    size_t offset=pos%chunk_size;
 
#ifdef DEBUG_FORBUF
    std::cout << "Inside get_element\n";
    std::cout.flush();
#endif

#ifdef COLLECT_STATISTICS
    ++stat.n_read_buff;
#endif

    tcurrentinfo& ci(currentinfo[threadnum]);

    if (index != ci.index){
#ifdef DEBUG_FORBUF
      std::cout << "Need prepare_node in get_element\n";
      std::cout.flush();
#endif
      prepare_node(index, ci, threadnum, false);
    }
#ifdef COLLECT_STATISTICS
    else{
      ++stat.n_fastread;
    }
#endif

#ifdef DEBUG_FORBUF
    std::cout << "Obtained pointer in get_element\n";
    std::cout.flush();
#endif

    return *(ci.cchunk + offset);
  };

  // atomic add at pos to t
  void atomic_add_(size_t pos, const T& t, size_t threadnum){
    size_t index=pos / chunk_size;
    size_t offset=pos%chunk_size;

#ifdef COLLECT_STATISTICS
    ++stat.n_write_buff;
#endif

    tcurrentinfo& ci(currentinfo[threadnum]);

    // if node is not correct, set mutex free again, ask for correct one
    if (index != ci.writeindex){
      prepare_node( index, ci, threadnum, true);
    }
#ifdef COLLECT_STATISTICS
else{
      ++stat.n_fastwrite;
    }
#endif

    std::lock_guard<std::mutex> lg(nodes[index].mut_ex);
    *(ci.cchunk + offset) += t;    

  };

  // atomic vector add at pos to t
  void atomic_add_(size_t pos, size_t N, const T* t, size_t threadnum){
    size_t index=pos / chunk_size;
    size_t offset=pos%chunk_size;

    tcurrentinfo& ci(currentinfo[threadnum]);

    if (index != ci.writeindex){
      prepare_node( index, ci, threadnum, true);
    }

    const T* p_source(t);
    auto p_target(ci.cchunk + offset);

    nodes[index].mut_ex.lock();

    for (size_t i =0; i < N; ++i){
      // check whether we are at end of block
      if (offset++ == chunk_size){	
	offset = 0;
	// release previous lock
	nodes[index].mut_ex.unlock();
	prepare_node(++index, ci, threadnum, true);
	// lock next node
	nodes[index].mut_ex.lock();
	p_target = ci.cchunk;
      }
      *(p_target++) += *(p_source++);
    }

    nodes[index].mut_ex.unlock();

#ifdef COLLECT_STATISTICS
    stat.n_write_buff += N;
#endif
  };

  void get_(size_t pos, size_t N, T* t, size_t threadnum){
    size_t index=pos / chunk_size;
    size_t offset=pos%chunk_size;
    
    tcurrentinfo& ci(currentinfo[threadnum]);

    if (index != ci.writeindex){
      prepare_node( index, ci, threadnum, false);
    }

    T* p_target(t);
    auto p_source(ci.cchunk + offset);

    for (size_t i =0; i < N; ++i){
      if (offset++ == chunk_size){
	offset = 0;
	prepare_node( ++index, ci, threadnum, false);
	p_source = ci.cchunk;
      }
      *(p_target++) = *(p_source++);
    }
#ifdef COLLECT_STATISTICS
    stat.n_read_buff += N;
#endif
  };

  void getchunk_(size_t index, T* t, size_t threadnum){

#ifdef COLLECT_STATISTICS
    ++stat.n_read_buff;
#endif

    tcurrentinfo& ci(currentinfo[threadnum]);

    if (index != ci.index){
      prepare_node(index, ci, threadnum, false);
    }

#ifdef COLLECT_STATISTICS
    ++stat.n_read_buff += chunk_size;
#endif

    T* p_target(t);
    const T* p_source(&(*ci.cchunk));

    memcpy(p_target, p_source, chunk_size * sizeof(T) );
  };

  void getpointer_(size_t index, T** t, size_t threadnum){

#ifdef COLLECT_STATISTICS
    ++stat.n_get_pointer;
#endif    

    tcurrentinfo& ci(currentinfo[threadnum]);

    if (index != ci.index){
      prepare_node(index, ci, threadnum, false);
    }
    
    // increase the borrow count of the node
    while (nodereleasebarrier.exchange(true)){};
    ++nodereleasecount;
    nodereleasebarrier = false;

    node<T>& cn(nodes[index]);

    // lock access to the chunk node
    std::lock_guard< decltype(cn.mut_ex) > lg(cn.mut_ex);

    ++cn.borrowed;	
    
    (*t) = &(*ci.cchunk);    
    
    --nodereleasecount;
  };

  void freepointer_(size_t index){

#ifdef COLLECT_STATISTICS
    ++stat.n_free_pointer;
#endif
    // release chunk if pointer is freed, save = true to write to disk;
    //     force = false (will not delete if other chunk uses it); borrow = true
    //                   save  force  borrow
    while (nodereleasebarrier.exchange(true)){};
    ++nodereleasecount;
    nodereleasebarrier = false;

    node<T>& cn(nodes[index]);

    // lock access to the chunk node
    std::lock_guard< decltype(cn.mut_ex) > lg(cn.mut_ex);

    // this thread now has exclusive access to this node
        
    --cn.borrowed;
    
    if (!cn.borrowed){

        if (cn.undead){
            // ask whether chunk can be freed
            if ( deprecate_use(index, true, false) ){
              
                store_chunk_(cn, index, true);

                chunk c;
                c.swap(cn.data);
#ifdef COLLECT_STATISTICS
                ++stat.n_free_delete;
#endif
                // the lock_guard frees the mut_ex

            }// else    
            //chunk is in use and will be freed by currentinfo-change               
        }
    }
    --nodereleasecount;
  }
};

#endif


