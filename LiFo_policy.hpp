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

  using policy_list<T>::in_memory;
  using policy_list<T>::unused_chunks;
  using policy_list<T>::max_in_mem;

public:
  typedef typename node<T>::chunk chunk;
  typedef std::shared_ptr<mapper<T> > p_mapper;

  using abstract_policy<T>::get_mapper;

  policy_LiFo(size_t max_in_mem, 
	      size_t nblock,
	      size_t nthread): 
    policy_list<T>(max_in_mem, nblock),
    nthread_(nthread),
    threadswap(2*nthread, chunkindex(std::string::npos, std::string::npos))
  {
    if (max_in_mem < 2*nthread)
      throw E_Policy_error("policy_LiFo constructor: "
			   "Too small max_in_mem for number of threads!");
  }; 

  ~policy_LiFo(){
  }

private:
  policy_LiFo();

  size_t nthread_;

  std::vector< chunkindex > threadswap; // Last storage space for swapping out threads

  size_t navail(){
    return max_in_mem() - 2 * nthread_;
  };

  chunk&& find_memory(int threadnum) override{
    if ( in_memory.size() < navail() ){
      return std::move(chunk(abstract_policy<T>::nblock(),0));
    }else{

      chunkindex ci(threadswap[threadnum*2]);
      threadswap[threadnum*2] = threadswap[threadnum*2+1];
      threadswap[threadnum*2+1] = in_memory.front();
      // undo the push_front in list_policy::get_memory
      in_memory.pop_front();

      //check if ci has never been used and thus still contains npos

      if (ci.i_mapper == std::string::npos){
	return std::move(chunk(abstract_policy<T>::nblock(),0));
      }else{
	auto pm(get_mapper(ci.i_mapper));
	return pm->release_chunk(ci.i_chunk);
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
	      size_t nblock): 
    policy_list<T>(max_in_mem, nblock)
  {}; 

  ~policy_LiLo(){
  }

private:
  policy_LiLo();

  using typename policy_list<T>::chunkindex;

  using policy_list<T>::in_memory;
  using policy_list<T>::unused_chunks;
  using policy_list<T>::max_in_mem;

  chunk&& find_memory(int) override{
    if ( in_memory.size() < max_in_mem()){
      chunk c(abstract_policy<T>::nblock(),0);
      // std::move avoids unnecessary copies
      return std::move(c);
    }else{
      auto pm(get_mapper(in_memory.back().i_mapper));
      size_t ic(in_memory.back().i_chunk);
      in_memory.pop_back();
      return pm->release_chunk(ic);
    }
  }
};

#endif
