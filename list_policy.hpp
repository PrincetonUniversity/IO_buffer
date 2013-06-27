#ifndef LIST_POLICY_FLO
#define LIST_POLICY_FLO

#include "mapper.hpp"
#include <algorithm>

// ICPC does not support override at the moment, so we only use it with g++
#ifdef __INTEL_COMPILER
#define OVERRIDE  
#else
#define OVERRIDE  
#endif

// This very simple policy implements a single last-in-first-out
// memory model, with a max memory footprint of max_in_mem chunks

template< class T >
class policy_list: public abstract_policy<T>{
public:
  typedef typename node<T>::chunk chunk;
  typedef std::shared_ptr<mapper<T> > p_mapper;

  using abstract_policy<T>::get_mapper;

  policy_list(size_t max_in_mem, 
	      size_t chunk_size,
	      size_t nth): 
    abstract_policy<T>(chunk_size,nth),
    in_memory_size_(0),
    max_in_mem_(max_in_mem)
  {}; 

  virtual ~policy_list(){
  }

protected:

  struct chunkindex{
    size_t i_mapper;
    size_t i_chunk;

    chunkindex(size_t im, size_t ic):i_mapper(im),i_chunk(ic){};    
  };

  std::list< chunk > unused_chunks;

  size_t max_in_mem() const { return max_in_mem_;};
  // unfortunately, std::list::size() may be O(N), which
  // makes some routines here ridicuously expensive
  // if we do not keep track of size
  size_t in_memory_size(){ return in_memory_size_;};

  chunkindex& in_memory_back(){ return in_memory.back();};
  chunkindex& in_memory_front(){ return in_memory.front();};

  void in_memory_pop_front(){ --in_memory_size_; in_memory.pop_front();};
  void in_memory_push_front(chunkindex ci){ ++in_memory_size_; in_memory.push_front(ci);};
  void in_memory_pop_back(){ --in_memory_size_; in_memory.pop_back();};
  void in_memory_push_back(chunkindex ci){ ++in_memory_size_; in_memory.push_back(ci);};

private:

  std::list< chunkindex > in_memory;

  std::mutex mutex_in_memory;

  size_t in_memory_size_;

  size_t max_in_mem_;

  policy_list();

  void change_buffer_size_( size_t N ) OVERRIDE {
    max_in_mem_ = N;
  }

  void sync_() OVERRIDE{
        std::for_each(in_memory.begin(), in_memory.end(), 
    		  [this](chunkindex& ci){
    		    this->get_mapper(
			ci.i_mapper)->sync(ci.i_chunk);
    		      });
  }
 
  void sync_(size_t index) OVERRIDE{
    p_mapper pm(get_mapper(index));
    std::for_each(in_memory.begin(), in_memory.end(), 
		  [index, pm](chunkindex& ci){
		    if (index == ci.i_mapper)
		      pm->sync(ci.i_chunk);
		  });
  }
 
  void return_all_mem_(bool save) OVERRIDE { 
    while (in_memory_size()) {
      const chunkindex& ci(in_memory.back());
      auto pm(get_mapper(ci.i_mapper));      
      unused_chunks.push_back(pm->release_chunk(ci.i_chunk, save));
      in_memory_pop_back();      
    }
  }

  void return_all_mem_(size_t index, bool save) OVERRIDE 
  { 
      try{
	  p_mapper pm( get_mapper(index));
	  in_memory.remove_if( [index, 
				this, 
				pm, 
				save](chunkindex& ci) -> bool
			       {
				   if (ci.i_mapper == index){
				       unused_chunks.push_back(pm->release_chunk(ci.i_chunk, save));
				       return true;
				   } else { return false; }}
	      );    
	  in_memory_size_ = in_memory.size();

	  return_tmp_memory(index, save);

      } catch (E_invalid_mapper_id){
	  std::cerr << "list_policy: return_all_mem found stale mapper\n";
      }
  }

  // routine to determine where to find a new memory block
  // guaranteed to only be entered by one thread at a time
  virtual chunk  find_memory(size_t) = 0;

  // routine to free any tmp memory associated with a mapper
  virtual void return_tmp_memory(size_t index, bool save) = 0;

  chunk get_memory_(size_t index, size_t pos, size_t threadnum) OVERRIDE{ 
    std::lock_guard<std::mutex> lock_mapper(mutex_in_memory);
    in_memory_push_front(chunkindex(index,pos)); 
    if ( ! unused_chunks.empty()){
      chunk cn;
      cn.swap(unused_chunks.back());
      unused_chunks.pop_back();
      return cn;
    }
    return find_memory(threadnum);
  };
};

#endif
