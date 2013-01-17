#ifndef POOLS_F_MEMORY_FLO
#define POOLS_F_MEMORY_FLO


/* pool:  liste aller free nodes
          liste aller stored nodes
	 
   mapper: vector aller nodes der datei
           liste aller nodes in memory

	  */



template <class T>
class mem_pool{

public:

  void return_node(chunk&);

  void request_node(p_node);
private:

  size_t  max_p_in_mem;
  
  std::list<p_node> free_nodes;

  std::list<p_node> used_nodes;
};

#endif
