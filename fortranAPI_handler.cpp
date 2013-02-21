#include "fortranAPI_handler.hpp"
#include "buffer.hpp"
#include "mapper.hpp"
#include "LiFo_policy.hpp"

#include <mutex>

template <class T> 
typename fortranapi<T>::p_abstract_policy fortranapi<T>::getpolicy( FINT pool_id ){

  if (static_cast<size_t>(pool_id) >= pools.size())
    throw E_invalid_pool_id(pool_id);

  p_abstract_policy pp( pools[pool_id]);

  if (!pp)
    throw E_invalid_pool_id(pool_id);

  return pp;
}

#ifndef FORBUF_FAST
template <class T> 
typename fortranapi<T>::pw_mapper fortranapi<T>::getfilep( int index){
  pw_mapper p(files[index]);
  if (!p) {

    std::cerr << "Error: Unknown file id " << index << '\n';

    std::cerr << "I know the file ids:\n";

    for (size_t i = 0; i < files.size(); ++i)
      if (pw_mapper p = files[i])
	std::cerr << i << ' ' << p->filename() << '\n';

    throw E_unknown_file_id(index);

  }
  return p;
}
#endif

template <class T> 
FINT fortranapi<T>::construct(const FINT& maxmem,
			      const FINT& blocksize,
			      const FINT& storagepolicy,
			      const FINT& nthread){
  FINT index=pools.size();

  p_abstract_policy psp;

  switch (storagepolicy){
  case 0: // LiFo
    pools.push_back(p_abstract_policy(new policy_LiFo<T>(maxmem,
							      blocksize,
							      nthread)));
    break;
  case 1: // LiLo
    pools.push_back(p_abstract_policy(new policy_LiLo<T>(maxmem,
							      blocksize, 
							      nthread)));
    break;
  default:
    throw E_unknown_storagepolicy(storagepolicy);
  }

  return index;
}

template <class T> 
void fortranapi<T>::changebuffersize(const FINT& pool_id,
				  const FINT& maxmem){
  getpolicy(pool_id)->change_buffer_size(maxmem);
}

template <class T> 
void fortranapi<T>::openfile(const FINT& pool_id,
			     const FINT& unit,
			     std::string filename,
			     bool reopen){
  if (static_cast<size_t>(unit) > files.size()){
    files.resize(unit+1);
  } 
  files[unit] = mapper<T>::factory(filename, 
					unit, 
					getpolicy(pool_id),
					reopen
					).get();
}

template <class T> 
void fortranapi<T>::writeArray( const FINT& unit,
				const FINT& pos,
				const FINT& N,
				const T* values,
				const FINT& threadnum){
  getfilep(unit)->set(pos, N, values, threadnum);
}

template <class T> 
void fortranapi<T>::writeBlock( const FINT& unit,
				const FINT& blockid,
				const T* values,
				const FINT& threadnum){
  getfilep(unit)->setchunk(blockid, values, threadnum);
}

template <class T> 
void fortranapi<T>::readArray( const FINT& unit,
			    const FINT& pos,
			    const FINT& N,
			    T* values,
			    const FINT& threadnum){
  getfilep(unit)->get(pos, N, values, threadnum);
}

template <class T> 
void fortranapi<T>::readBlock( const FINT& unit,
			    const FINT& blockid,
			    T* values,
			    const FINT& threadnum){
  getfilep(unit)->getchunk(blockid, values, threadnum);
}

template <class T> 
void fortranapi<T>::closefile( const FINT& unit){
  getfilep(unit)->get_policy()->remove_mapper(unit);
  files[unit] = NULL;
}

template <class T> 
void fortranapi<T>::removefile( const FINT& unit){

  std::ostringstream oss;
  oss << "rm " << getfilep(unit)->filename() << '\n';

  getfilep(unit)->get_policy()->remove_mapper(unit, false);
  files[unit] = NULL; // remove shared_ptr, 
  // not getfilep(unit).reset(), that would just reset the temporary 

  std::cerr << "Execute " << oss.str() << '\n';
  if (std::system(oss.str().c_str()) == -1)
    std::cerr << "Error on command " << oss.str() << '\n';
}

template <class T> 
void fortranapi<T>::flushfile( const FINT& unit){
  getfilep(unit)->flush();
}

template <class T> 
void fortranapi<T>::syncfile( const FINT& unit){
  getfilep(unit)->get_policy()->sync(unit);
}

template <class T> 
void fortranapi<T>::syncpool( const FINT& poolid){
  getpolicy(poolid)->sync();
}

template <class T> 
void fortranapi<T>::closepool( const FINT& poolid){
  getpolicy(poolid).reset();
}

template <class T> 
void fortranapi<T>::removepool( const FINT& poolid){
  auto pp(getpolicy(poolid));
  pp->forget_all_mem();
  std::ostringstream oss;
  oss << "rm ";
  
  for (auto p = files.begin(); p!=files.end(); ++p)
    {
      pw_mapper pm(*p); 
      if (pm)
	if (pm->get_policy() == pp){
	  oss << pm->filename() << ' ';
	}
    }
  pp.reset();
  std::cerr << "Execute " << oss.str() << '\n';
  if (std::system(oss.str().c_str()) == -1)
    std::cerr << "Error on command " << oss.str() << '\n';
}

template <class T>
void fortranapi<T>::flushpool( const FINT& poolid){
  pools[poolid]->return_all_mem();
}

template class fortranapi<double>;
template class fortranapi<FINT>;
