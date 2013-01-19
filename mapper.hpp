#ifndef MAPPER_F_FLO
#define MAPPER_F_FLO

#include <list>
#include <map>
#include <vector>
#include <unordered_map>
#include <exception>
#include <memory>
#include "buffer.hpp"

#define FINT long long

class E_invalid_mapper_id: public std::exception{
public:
  FINT sp;
  E_invalid_mapper_id(FINT i):sp(i){};
  
  ~E_invalid_mapper_id() throw() {};

  const char* what() const throw(){
    return "File id unknown!";
  }
};

template< class T >
struct node{

  typedef std::vector<T> chunk;
  typedef std::shared_ptr<node> p_node;

  enum status_type {modified, empty, loaded, stored};
  
  status_type status;
    
  chunk data;

  size_t index;

  bool locked;

  node():
    status(empty),
    data()
    
  {}
};

template <class T>
class mapper;

template< class T >
class abstract_policy{
public:
  typedef typename node<T>::chunk chunk;
  typedef std::shared_ptr<mapper<T> > p_mapper;
 
  abstract_policy(size_t chunk_size): 
    chunk_size_(chunk_size)
  {}; 

  size_t chunk_size(){ return chunk_size_;}

  // keep track of mapper, assign index i
  // return assigned index
  void assign_mapper(p_mapper m, size_t i){ assign_mapper_(m, i); };

  // free all memory owned by mapper index, save them if save==true
  void return_all_mem( size_t index,
		       bool save = true ){ return_all_mem_(index, save); };

  // free all memory owned by mapper index, close the file, 
  // save the memory to disk if save==true
  void remove_mapper( size_t index,
		      bool save = true){ remove_mapper(index, save);};

  void sync( size_t index ){ sync_(index);}
  void sync(){ sync_();}

  // free all memory, save them if save==true
  void return_all_mem(){ return_all_mem_( true ); };
  // free all memory, save them if save==true
  void forget_all_mem(){ return_all_mem_( false ); };

  // get a chunk of memory for buffer, at pos
  chunk&& get_memory(size_t index,
		     size_t pos,
		     size_t threadnum){ return get_memory_(index,pos,threadnum); };

  size_t mapper_count(){ return mappers.size();};

  p_mapper get_mapper(size_t index){
    p_mapper pm(mappers[index]);
    if (!pm) throw E_invalid_mapper_id(index);
    return pm;
  };

private:
  std::map< size_t, std::shared_ptr<mapper<T> > > mappers;

protected:
  decltype( mappers.cbegin()) get_mapper_begin() const { 
    return mappers.begin();};
  decltype( mappers.cend()) get_mapper_end() const { 
    return mappers.end();};

private:  

  size_t chunk_size_;

  virtual void sync_(size_t) = 0;

  virtual void sync_() = 0;

  virtual void return_all_mem_(size_t, bool) = 0;

  virtual void return_all_mem_( bool ) = 0;

  virtual chunk&& get_memory_(size_t index, 
			      size_t pos, 
			      size_t threadnum) = 0;

  void remove_mapper_(size_t index, bool save){
    if (!mappers[index]) return;
    return_all_mem_(index, save);
    get_mapper(index).reset();
  }

  void assign_mapper_(p_mapper pm, size_t index) { 
    p_mapper querymp(mappers[index]);
    if (querymp) // index already exists!
      throw E_invalid_mapper_id(index);

    mappers[index]=pm;
  };  

};

template <class T>
class mapper{
private:
  typedef std::weak_ptr<abstract_policy<T> > pw_abstract_policy;
  pw_abstract_policy policy;
public:

  typedef std::shared_ptr<abstract_policy<T> > p_abstract_policy;

  typedef std::shared_ptr<mapper<T> > p_mapper;

  typedef typename node<T>::p_node p_node;
  typedef typename node<T>::chunk chunk;
 
  ~mapper(){ policy.lock()->return_all_mem(my_index);};

  void set(size_t pos, const T& t, size_t threadnum) { set_(pos, t, threadnum);};
  T get(size_t pos, size_t threadnum) { return get_(pos, threadnum);};  

  // write all buffers to disk
  void flush(){ write_buffers_(true);};
  // free all buffers without saving
  void forget(){ write_buffers_(false);};

  void sync(size_t chunk_index){ sync_chunk_(chunk_index);};

  // API with pool
  chunk&& release_chunk( size_t chunk_index,
			 bool save=true){ return release_chunk_(chunk_index, save);};

  const std::string& filename() const { return file_data.filename();};

  p_abstract_policy get_policy(){ return policy.lock();};

  static p_mapper
    factory(std::string fn, 
	    size_t index,
	    p_abstract_policy pap){
    p_mapper pm(new mapper(fn,pap));
    pap->assign_mapper(pm,index);
    return pm;
  };
  
private:
 
  size_t chunk_size;

  filer<T> file_data;  
  
  size_t my_index;	     

  std::vector<node<T> > nodes;  	     

  mapper(std::string bufferfile,
	 p_abstract_policy m):
    policy(m),
    chunk_size(m->chunk_size()),
    file_data(bufferfile.c_str(), chunk_size)
  {};    

  void sync_chunk_( size_t chunk_index){
    node<T>& cn(nodes[chunk_index]);
    if ( cn.status==node<T>::modified)
      file_data.write_chunk(cn.index,&cn.data[0]);    
  };

  chunk&& release_chunk_(size_t chunk_index, bool save){
    node<T>& cn(nodes[chunk_index]);
    // if it is modified, need to write changes
    if ( save && cn.status==node<T>::modified)
      file_data.write_chunk(cn.index,&cn.data[0]);

    cn.status=node<T>::stored;
    chunk c;
    c.swap(cn.data);
    return std::move(c);    
  };

  // write all buffers to file, free all the memory
  void write_buffers_(bool save){    
    policy.lock()->return_all_mem(my_index, save);
  };

  // find node
  node<T>& get_node( size_t chunk_index, size_t thread_num ){
      std::cerr << "Haaoaue?\n";
      std::cerr.flush();
    // if too small nodes, then resize to a bit larger, 
    // to avoid much resizing in the beginning
      if (chunk_index >= nodes.size())
	  nodes.resize(chunk_index+20);

      node<T>& cn(nodes[chunk_index]);    
      // if cn.data already associated, we do not need to do anything more
      if (cn.data.size()) 
	  return cn;
      
      // get chunk of memory
      p_abstract_policy pp(policy.lock());

      if (!pp)
	std::cerr << "No policy ! \n";
      cn.data = policy.lock()->get_memory(my_index, chunk_index, thread_num);
      std::cerr << "noCrash\n";

      if (cn.status == node<T>::stored){
	  // load it from disk if it is on disk
	  file_data.read_chunk(chunk_index, &(cn.data[0]));
      }else{
	  // blank the memory, as it still contains swapped-out block
	  std::fill(cn.data.begin(), cn.data.end(), 0);
      }

      cn.status=node<T>::loaded;

      return cn;
  };

  // change value at pos to t
  void set_(size_t pos, const T& t, size_t threadnum){
    std::cerr << "set_, " << pos << ' ' << t << ' ' << threadnum << '\n';

    size_t index=pos / chunk_size;
    size_t offset=pos%chunk_size;

    std::cerr << "set_, " << pos << ' ' << t << ' ' << threadnum << '\n';

    node<T>& cn(get_node(index,threadnum));

    cn.status=node<T>::modified;
    cn.data[offset] = t;
  };

  T get_(size_t pos, size_t threadnum){
    size_t index=pos / chunk_size;
    size_t offset=pos%chunk_size;

    return get_node(index,threadnum).data[offset];
  };
};

#endif


