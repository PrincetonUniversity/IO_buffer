#ifndef ABSTRACTPOL_F_FLO
#define ABSTRACTPOL_F_FLO

#include <list>
#include <map>
#include <vector>
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

  virtual ~abstract_policy(){};

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
  chunk get_memory(size_t index,
		   size_t pos,
		   size_t threadnum){ 
    return get_memory_(index,pos,threadnum); };

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

  virtual chunk get_memory_(size_t index, 
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

#endif
