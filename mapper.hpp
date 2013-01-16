#ifndef MAPPER_F_FLO
#define MAPPER_F_FLO

#include <list>
#include <map>
#include <vector>
#include <unordered_map>
#include "buffer.hpp"

struct node{

  typedef std::vector<T> chunk;

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

typedef std::shared_ptr<node> p_node;

template <class T, class pool >
class mapper{

public:

  mapper(std::string bufferfile,
	 std::shared_ptr<pool> m):
    my_pool(m),
    nblock(mp->nblock()),
    file_data(bufferfile.c_str(), nblock)
  {}  

  ~mapper(){ write_buffers_();}

  void set(size_t pos, const T& t) { set_(pos, t);};
  T get(size_t pos) { return get_(pos);};  

  void flush(){ write_buffers_();}

// API with pool

  void release_node(p_node p){ release_node_(p)};

private:

  size_t nblock;

  std::shared_ptr<pool> my_pool;

  size_t nblock;

  filer<T> file_data;
  
  std::list<size_t> in_memory;

  size_t max_p_in_mem;

  std::vector<node> nodes;  

  void release_node_( p_node p ){
    node& cn(nodes[pos]);
    // if it is modified, need to write changes
    if (cn.status==node::modified)
      file_data.write_chunk(in_memory.back(),&cn.data[0]);
    cn.status=node::stored;
    // remove entry from list
    in_memory.pop_back();
    
    return cn.data;    
  }

  // return reference to container that is no longer needed
  chunk& pop_memory(){
    // find node associated with that memory
    node& cn(nodes[in_memory.back()]);
    // if it is modified, need to write changes
    if (cn.status==node::modified)
      file_data.write_chunk(in_memory.back(),&cn.data[0]);
    // swap cn.data with empty new container
    cn.status=node::stored;
    // remove entry from list
    in_memory.pop_back();
    return cn.data;
  };

  // write all buffers to file, free all the memory
  void write_buffers_(){    
    while (in_memory.size()) 
      pop_memory().resize(0);
  };

  // find node
  node& get_node( size_t index ){
      if (index >= nodes.size())
	  nodes.resize(index+20);

      node& cn(nodes[index]);    
      // if cn.data already associated, we do not need to do anything more
      if (cn.data.size()) 
	  return cn;
      
      // get memory
      mem_policy.get_memory(cn.data);

      if (cn.status == node::stored){
	  // load it from disk if it is on disk
	  file_data.read_chunk(index, &(cn.data[0]));
      }else{
	  // blank the memory, as it still contains swapped-out block
	  std::fill(cn.data.begin(), cn.data.end(), 0);
      }

      cn.status=node::loaded;
      in_memory.push_front(index);

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

#endif


