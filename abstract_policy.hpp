#ifndef ABSTRACTPOL_F_FLO
#define ABSTRACTPOL_F_FLO

#include <list>
#include <mutex>
#include <algorithm>
#include <map>
#include <vector>
#include <exception>
#include <memory>
#include <iostream>
#include "buffer.hpp"

#define FINT long long

class E_invalid_mapper_id: public std::exception{
public:
  FINT sp;
  E_invalid_mapper_id(FINT i):sp(i){    
  };
  
  ~E_invalid_mapper_id() throw() {};

  const char* what() const throw(){
    return "Mapper id unknown!";
  }
};

class E_invalid_node_copy: public std::exception{
public:
  E_invalid_node_copy(){};
  
  ~E_invalid_node_copy() throw() {};

  const char* what() const throw(){
    return "Tried to copy a non-empty node, code is malformed!";
  }
};

template< class T >
struct node{

  typedef std::vector<T> chunk;
  typedef std::shared_ptr<node> p_node;

  enum status_type {modified, empty, loaded, stored};
  
  // counts how many times this node is borrowed out (i.e. a row pointer
  //  has been asked for by getpointer). if borrowed > 0, will not free memory
  //  since thing is in use
  int borrowed;
  
  status_type status;
    
  chunk data;

  std::mutex mut_ex;

  node():
    borrowed(0),
    status(empty),
    data()    
  {};

  // nodes should never be copied
  node(const node&) = delete;

  node(node&& n) noexcept:
  status(n.status)
  {   
    data.swap(n.data);
  }

};

template <class T>
class mapper;

template< class T >
class abstract_policy{
public:
  typedef typename node<T>::chunk chunk;
  typedef std::shared_ptr<mapper<T> > p_mapper;
 
  abstract_policy(size_t chunk_size,
		  size_t nth):    
    nthread_(nth),
    chunk_size_(chunk_size)
  {}; 

  virtual ~abstract_policy(){ terminate_mappers_();};

  void change_buffer_size(size_t N){ change_buffer_size_(N);};

  size_t chunk_size(){ return chunk_size_;}

  size_t nthread() const { return nthread_;};

  // keep track of mapper, assign index i
  // return assigned index
  void assign_mapper(p_mapper m, size_t i){ assign_mapper_(m, i); };

  // free all memory owned by mapper index, save them if save==true
  void return_all_mem( size_t index,
		       bool save = true ){ 
    remove_tmp_chunks_in_mapper_(index);
    return_all_mem_(index, save); };

  // free all memory owned by mapper index, close the file, 
  // save the memory to disk if save==true
  void remove_mapper( size_t index,
		      bool save = true){ 
    remove_tmp_chunks_in_mapper_(index); 
    remove_mapper_(index, save);};

  // sync contents of one file
  void sync( size_t index ){ 
    remove_tmp_chunks_in_mapper_(index); 
    sync_(index);}

  // sync all contents to file
  void sync(){     
    remove_tmp_chunks_in_mappers_(); 
    sync_();}

  // free all memory, save them if save==true
  void return_all_mem(){ 
    remove_tmp_chunks_in_mappers_(); 
    return_all_mem_( true ); };

  // free all memory, save them if save==true
  void forget_all_mem(){ 
    remove_tmp_chunks_in_mappers_(); 
    return_all_mem_( false ); };

  // get a chunk of memory for buffer, at pos
  chunk get_memory(size_t index,
		   size_t pos,
		   size_t threadnum){ 
    return get_memory_(index,pos,threadnum); };

  size_t mapper_count(){ return mappers.size();};

  bool known_mapper( size_t index ){
    return (mappers.find(index)!=mappers.end());
  }

  p_mapper get_mapper(size_t index){
    auto pit(mappers.find(index));
    if (pit == mappers.end()){
      std::cout << "Mapper id " << index << " unknown in get_mapper\n";
      std::cout.flush();
      throw E_invalid_mapper_id(index);
    }
    p_mapper pm(pit->second);
    if (!pm){
      std::cout << "Mapper id " << index << " already closed in get_mapper\n";
      std::cout.flush();
      throw E_invalid_mapper_id(index);
    }
    return pm;
  };

private:
  typedef std::map< size_t, std::shared_ptr<mapper<T> > > t_mappers;
  t_mappers mappers;

protected:
  decltype( mappers.cbegin()) get_mapper_begin() const { 
    return mappers.begin();};
  decltype( mappers.cend()) get_mapper_end() const { 
    return mappers.end();};

private:  

  size_t nthread_;

  std::mutex mutex_mappers;

  size_t chunk_size_;

  virtual void change_buffer_size_(size_t) = 0;

  virtual void sync_(size_t) = 0;

  virtual void sync_() = 0;

  virtual void return_all_mem_(size_t, bool) = 0;

  virtual void return_all_mem_( bool ) = 0;

  void terminate_mappers_(){
    remove_tmp_chunks_in_mappers_(); 
    std::for_each( mappers.begin(), mappers.end(), 
		   []( typename t_mappers::value_type& p){
		     p.second.reset();
		   });    
    mappers.clear();
  }

  void remove_tmp_chunks_in_mappers_(){
    std::for_each( mappers.begin(), mappers.end(), 
		   []( typename t_mappers::value_type& p){
		     p.second->remove_temporary_chunks();
		   });
  }

  void remove_tmp_chunks_in_mapper_( size_t mapper_id ){
    get_mapper(mapper_id)->remove_temporary_chunks();
  }

  virtual chunk get_memory_(size_t index, 
			      size_t pos, 
			      size_t threadnum) = 0;

  void remove_mapper_(size_t index, bool save){    
    std::lock_guard<std::mutex> lock_mapper(mutex_mappers);
    if (mappers.find(index) == mappers.end()) return;
    return_all_mem_(index, save);
    mappers.erase(index);
  }

  void assign_mapper_(p_mapper pm, size_t index) { 
    std::lock_guard<std::mutex> lock_mapper(mutex_mappers);

    if (mappers.find(index) != mappers.end()){ // index already exists!
      std::cout << "Mapper id " << index 
		<< " already in use in assign_mapper\n";

      std::cout << "The following mappers are assigned to this policy:\n";

      std::for_each(mappers.begin(), mappers.end(),[]
		    (typename t_mappers::value_type& p){
		      std::cout << p.second->filename() 
				<< ' ' << p.first << '\n';});

      std::cout.flush();
      throw E_invalid_mapper_id(index);
    }

    mappers[index]=pm;
  };  

};

#endif
