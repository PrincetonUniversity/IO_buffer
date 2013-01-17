#ifndef MAPPER_F_FLO
#define MAPPER_F_FLO

#include <list>
#include <map>
#include <vector>
#include <unordered_map>
#include "buffer.hpp"

template< class T >
struct node{

  typedef std::vector<T> chunk;
  typedef std::shared_ptr<node> p_node;

  enum status_type {modified, empty, loaded, stored, free};
  
  status_type status;
    
  chunk data;

  size_t unit;

  size_t index;

  bool locked;

  node():
    status(empty),
    data()
    
  {}
};

template <class T, template<class> class tpolicy >
class mapper{

public:

  typedef node<T>::p_node p_node;
  typedef node<T>::chunk chunk;
 
  mapper(std::string bufferfile,
	 tpolicy m):
    policy(m),
    nblock(policy->nblock()),
    file_data(bufferfile.c_str(), nblock)
  {}  

  ~mapper(){ write_buffers_();}

  void set(size_t pos, const T& t) { set_(pos, t);};
  T get(size_t pos) { return get_(pos);};  

  void flush(){ write_buffers_();}

// API with pool

  chunk&& release_chunk( size_t pos ){ return release_chunk_(pos)};

private:
 
  tpolicy policy;

  size_t nblock;

  filer<T> file_data;
  
  size_t max_p_in_mem;

  std::vector<node> nodes;  

  chunk&& release_chunk_(size_t pos){
    node& cn(nodes[pos]);
    // if it is modified, need to write changes
    if (p->status==node::modified)
      file_data.write_chunk(pos,&p->data[0]);
    pn->status=node::stored;
    chunk c;
    c.swap(cn.data);
    return std::move(c);    
  }


  // write all buffers to file, free all the memory
  void write_buffers_(){    
    policy.return_all_mem();
  };

  // find node
  node& get_node( size_t index ){
    // if too small nodes, then resize to a bit larger, 
    // to avoid much resizing in the beginning
      if (index >= nodes.size())
	  nodes.resize(index+20);

      node& cn(nodes[index]);    
      // if cn.data already associated, we do not need to do anything more
      if (cn.data.size()) 
	  return cn;
      
      // get chunk of memory
      cn.data = policy.get_memory(index);

      if (cn.status == node::stored){
	  // load it from disk if it is on disk
	  file_data.read_chunk(index, &(cn.data[0]));
      }else{
	  // blank the memory, as it still contains swapped-out block
	  std::fill(cn.data.begin(), cn.data.end(), 0);
      }

      cn.status=node::loaded;

      return cn;
  }

  // change value at pos to t
  void set_(size_t pos, const T& t){
    size_t index=pos / nblock;
    size_t offset=pos%nblock;

    node& cn(get_node(index));

    cn.status=node::modified;
    cn.data[offset] = t;
  };

  T get_(size_t pos){
    size_t index=pos / nblock;
    size_t offset=pos%nblock;

    return get_node(index).data[offset];
  };
};

// This very simple policy implements a single last-in-first-out
// memory model, with a max memory footprint of max_in_mem chunks

template< class T >
class policy_LiFo{
public:
  typedef node<T>::chunk chunk;
 
  policy_LiFo(mapper<T, policy_LiFo> m,
	      size_t max_in_mem, 
	      size_t nblock): 
    m_(m)
    max_in_mem_(max_in_mem),
    nblock_(nblock),
  {}; 

  // free all memory owned by  
  void return_all_mem(){ return_all_mem_();};

  // get a chunk of memory for buffer, at pos
  void get_memory(size_t pos, chunk& d){ 
    get_memory_(pos, d);}

  size_t nblock(){ return nblock_;}
 
private:
  policy_LiFo();

  mapper<T, policy_LiFo>* m_;

  std::list<size_t> in_memory;

  size_t nblock_;
  size_t max_in_mem_;

  void return_all_mem_(){ 
    while (in_memory.size()) 
      m_->release_chunk(in_memory.pop_back());
  }

  chunk&& get_memory_(size_t pos){ 
    in_memory.push_front(pos); 
    if ( in_memory.size() < max_in_mem_){
      chunk c(nblock_,0);
      // std::move avoids unnecessary copies
      return std::move(c);
    }else{
      return m_->release_chunk(in_memory.pop_back());
    }
  }
};

#endif


