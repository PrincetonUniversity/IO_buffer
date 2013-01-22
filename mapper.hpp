#ifndef MAPPER_F_FLO
#define MAPPER_F_FLO

#include "abstract_policy.hpp"
#include <list>
#include <iostream>
#include <map>
#include <vector>
#include <exception>
#include <memory>
#include "buffer.hpp"

#define FINT long long

struct statistics{
  size_t n_read_buff;
  size_t n_read_file;
  size_t n_write_buff;
  size_t n_write_file;
  size_t n_IO_avoided;
  size_t n_not_modified;
  size_t n_blanked;

  statistics(): n_read_buff(0),
		n_read_file(0),
		n_write_buff(0),
		n_write_file(0),
		n_IO_avoided(0),
		n_not_modified(0),
		n_blanked(0)
  {};

  void dump(std::string filename){
    std::ofstream ofs(filename);
    ofs << n_read_buff << " individual reads\n";
    ofs << n_read_file << " chunks read from disk\n";
    ofs << n_write_buff << " individual writes\n";
    ofs << n_write_file << " chunks written to disk\n";
    ofs << n_IO_avoided << " avoided IO operations as chunk already in RAM\n";

    ofs << n_not_modified << " writes avoided as chunk not modified in RAM\n";
    ofs << n_blanked << " reads suspicuously saved because chunk not on file\n";
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
      pol->return_all_mem(my_index);
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

#ifdef FORBUF_FAST
  // always remember last access point, to fast 
  // track consecutive IO to one chunk 
  typename chunk::iterator currentchunk;
  size_t currentindex;
  size_t currentwriteindex;
#endif

  size_t chunk_size;

  filer<T> file_data;  
  
  size_t my_index;	     

  std::vector<node<T> > nodes;  	     

  statistics stat;

  mapper(const std::string& bufferfile,
	 size_t ind,
	 p_abstract_policy m):
    policy(m),
#ifdef FORBUF_FAST
    currentindex(-1),
    currentwriteindex(-1),
#endif
    chunk_size(m->chunk_size()),
    file_data(bufferfile.c_str(), chunk_size),
    my_index(ind)
  {};    

  void sync_chunk_( size_t chunk_index){
    node<T>& cn(nodes[chunk_index]);
    if ( cn.status==node<T>::modified)
      file_data.write_chunk(chunk_index,&cn.data[0]);    
  };

  chunk release_chunk_(size_t chunk_index, bool save){    
#ifdef FORBUF_FAST
    if (chunk_index==currentindex)
      currentindex = -1;
      currentwriteindex = -1;
#endif
    node<T>& cn(nodes[chunk_index]);
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
    return c;    
  };

  // write all buffers to file, free all the memory
  void write_buffers_(bool save){    
    policy.lock()->return_all_mem(my_index, save);
  };

  // find node
  node<T>& get_node( size_t chunk_index, size_t thread_num ){
    // if too small nodes, then resize to a bit larger, 
    // to avoid much resizing in the beginning
      if (chunk_index >= nodes.size())
	  nodes.resize(chunk_index+20);

      node<T>& cn(nodes[chunk_index]);    

      // if cn.data already associated, we do not need to do anything more
      if (cn.data.size()){
	++stat.n_IO_avoided;
#ifdef FORBUF_FAST
	currentindex = chunk_index;
	currentchunk = cn.data.begin();
#endif
	return cn;
      }
      
      // get chunk of memory
      cn.data = policy.lock()->get_memory(my_index, chunk_index, thread_num);

      if (cn.status == node<T>::stored){
	// load it from disk if it is on disk
	file_data.read_chunk(chunk_index, &(cn.data[0]));
	++stat.n_read_file;
      }else{
	// blank the memory, as it still contains swapped-out block
	std::fill(cn.data.begin(), cn.data.end(), 0);
	++stat.n_blanked;
      }

      cn.status=node<T>::loaded;

#ifdef FORBUF_FAST
      currentindex = chunk_index;
      currentchunk = cn.data.begin();
#endif
      return cn;
  };

  // change value at pos to t
  void set_(size_t pos, const T& t, size_t threadnum){
    size_t index=pos / chunk_size;
    size_t offset=pos%chunk_size;

#ifdef FORBUF_FAST
    if (index != currentwriteindex){
      node<T>& cn(get_node(index,threadnum));
      cn.status=node<T>::modified;
      ++stat.n_write_buff;
      cn.data[offset] = t;
      currentwriteindex = currentindex;
    }else{
      *(currentchunk + offset) = t;
    }
#else    
    node<T>& cn(get_node(index,threadnum));
    cn.status=node<T>::modified;
    ++stat.n_write_buff;
    cn.data[offset] = t;
#endif
  };

  // change N values starting from pos to t
  void set_(size_t pos, size_t N, const T* t, size_t threadnum){
    size_t index=pos / chunk_size;
    size_t offset=pos%chunk_size;

    node<T>& cn(get_node(index,threadnum));
    cn.status=node<T>::modified;

    const T* p(t);
    auto pcn(cn.data.begin());
    pcn += offset;

    for (size_t i =0; i < N; ++i){
      if (offset == chunk_size){
	offset = 0;
	cn = get_node(++index, threadnum);
	cn.status=node<T>::modified;	
	pcn = cn.data.begin();
      }
      *(pcn++) = *(p++);
    }
    stat.n_write_buff += N;
  };

  T get_(size_t pos, size_t threadnum){
    size_t index=pos / chunk_size;
    size_t offset=pos%chunk_size;

    ++stat.n_read_buff;
#ifdef FORBUF_FAST
    if (index==currentindex){
      return *(currentchunk + offset);
    }else{
      return get_node(index,threadnum).data[offset];    
    }
#else
    return get_node(index,threadnum).data[offset];    
#endif
  };

  void get_(size_t pos, size_t N, T* t, size_t threadnum){
    size_t index=pos / chunk_size;
    size_t offset=pos%chunk_size;


    node<T>& cn(get_node(index,threadnum));

    T* p(t);
    auto pcn(cn.data.cbegin());
    pcn += offset;

    for (size_t i =0; i < N; ++i){
      if (offset == chunk_size){
	offset = 0;
	cn = get_node(++index,threadnum);
	pcn = cn.data.begin();
      }
      *(p++) = *(pcn++);
    }
    stat.n_read_buff += N;
  };
};

#endif


