#ifndef LIFO_POLICY_FLO
#define LIFO_POLICY_FLO

#include "mapper.hpp"
#include <algorithm>

#include "list_policy.hpp"

// This very simple policy implements a single last-in-first-out
// memory model, with a max memory footprint of max_in_mem chunks

#include <exception>

class E_Policy_error: public std::exception{
public:
  const char* message;
  E_Policy_error(const char* m):message(m){};
  
  ~E_Policy_error() throw() {};

  const char* what() const throw(){
    return message;
  }
};


template< class T >
class policy_LiFo: public policy_list<T>{
  using typename policy_list<T>::chunkindex;

  using policy_list<T>::unused_chunks;
  using policy_list<T>::max_in_mem;

public:
  typedef typename node<T>::chunk chunk;
  typedef std::shared_ptr<mapper<T> > p_mapper;

  using abstract_policy<T>::get_mapper;

  policy_LiFo(size_t max_in_mem, 
	      size_t chunk_size,
	      size_t nth): 
    policy_list<T>(max_in_mem, chunk_size, nth),
    threadswap(2*nth, chunkindex(std::string::npos, std::string::npos))
  {
    if (max_in_mem < 2*abstract_policy<T>::nthread())
      throw E_Policy_error("policy_LiFo constructor: "
			   "Too small max_in_mem for number of threads!");
  }; 

  virtual ~policy_LiFo(){
      std::cerr << "LiFo destructor\n";std::cerr.flush();
  }

private:
  policy_LiFo();

  std::vector< chunkindex > threadswap; // Last storage space for swapping out threads

  size_t navail(){
    return max_in_mem() - 2 * abstract_policy<T>::nthread();
  };

  chunk find_memory(size_t threadnum) OVERRIDE{
    if (policy_list<T>::in_memory_size() <= navail() ){
      return chunk(abstract_policy<T>::chunk_size(),0);
    }else{

      if (threadnum >= abstract_policy<T>::nthread())
	throw E_Policy_error("LiFo_policy:find_memory: Threadnum invalid!");

      chunkindex ci(threadswap[threadnum*2]);
      threadswap[threadnum*2] = threadswap[threadnum*2+1];
      threadswap[threadnum*2+1] = policy_list<T>::in_memory_front();
      // undo the push_front in list_policy::get_memory
      policy_list<T>::in_memory_pop_front();

      //check if ci has never been used and thus still contains npos

      if (ci.i_mapper == std::string::npos){
	return chunk(abstract_policy<T>::chunk_size(),0);
      }else{
	auto pm(get_mapper(ci.i_mapper));
	return pm->release_chunk(ci.i_chunk,true , false);
      }      
    }
  }
};

template< class T >
class policy_LiLo: public policy_list<T>{
public:
  typedef typename node<T>::chunk chunk;
  typedef std::shared_ptr<mapper<T> > p_mapper;

  using abstract_policy<T>::get_mapper;

  policy_LiLo(size_t max_in_mem, 
	      size_t chunk_size,
	      size_t nth): 
    policy_list<T>(max_in_mem, chunk_size, nth)
  {}; 

  virtual ~policy_LiLo(){
  }

private:
  policy_LiLo();

  using typename policy_list<T>::chunkindex;

  using policy_list<T>::unused_chunks;
  using policy_list<T>::max_in_mem;

  chunk find_memory(size_t) OVERRIDE{
    if ( policy_list<T>::in_memory_size() < max_in_mem()){
      return chunk(abstract_policy<T>::chunk_size(),0);
    }else{
      auto pm(get_mapper(policy_list<T>::in_memory_back().i_mapper));
      size_t ic(policy_list<T>::in_memory_back().i_chunk);
      policy_list<T>::in_memory_pop_back();
      return pm->release_chunk(ic);
    }
  }
};

#endif
