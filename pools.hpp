#ifndef POOLS_F_MEMORY_FLO
#define POOLS_F_MEMORY_FLO


/* pool:  liste aller free nodes
          liste aller stored nodes
	 
   mapper: vector aller nodes der datei
           liste aller nodes in memory

	  */


class default_pool{

public:

  void return_node(p_node);

private:

  size_t  max_p_in_mem;
  
  

};

#endif
