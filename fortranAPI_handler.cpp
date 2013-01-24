#include "fortranAPI_handler.hpp"
#include "buffer.hpp"
#include "buffer.tpp"
#include "mapper.hpp"
#include "LiFo_policy.hpp"

p_abstract_policy fortranapi::getpolicy( FINT pool_id ){

  if (static_cast<size_t>(pool_id) >= pools.size())
    throw E_invalid_pool_id(pool_id);

  p_abstract_policy pp( pools[pool_id]);

  if (!pp)
    throw E_invalid_pool_id(pool_id);

  return pp;
}

#ifndef FORBUF_FAST
pw_mapper fortranapi::getfilep( int index){
  pw_mapper p(files[index]);
  if (!p) 
    throw E_unknown_file_id(index);
  return p;
}
#endif

FINT fortranapi::construct(const FINT& maxmem,
			   const FINT& blocksize,
			   const FINT& storagepolicy,
			   const FINT& nthread){
  FINT index=pools.size();

  p_abstract_policy psp;

  switch (storagepolicy){
  case 0: // LiFo
    pools.push_back(p_abstract_policy(new policy_LiFo<double>(maxmem,
							      blocksize,
							      nthread)));
    break;
  case 1: // LiLo
    pools.push_back(p_abstract_policy(new policy_LiLo<double>(maxmem,blocksize)));
    break;
  default:
    throw E_unknown_storagepolicy(storagepolicy);
  }

  return index;
}

void fortranapi::openfile(const FINT& pool_id,
			  const FINT& unit,
			  std::string filename){
  if (static_cast<size_t>(unit) > files.size()){
    files.resize(unit+1);
  } 
  files[unit] = mapper<double>::factory(filename, unit, getpolicy(pool_id)).get();
}

void fortranapi::writeArray( const FINT& unit,
			     const FINT& pos,
			     const FINT& N,
			     const double* values,
			     const FINT& threadnum){
  getfilep(unit)->set(pos, N, values, threadnum);
}

void fortranapi::readArray( const FINT& unit,
			    const FINT& pos,
			    const FINT& N,
			    double* values,
			    const FINT& threadnum){
  getfilep(unit)->get(pos, N, values, threadnum);
}

void fortranapi::closefile( const FINT& unit){
  getfilep(unit)->get_policy()->remove_mapper(unit);
  files[unit] = NULL;
}

void fortranapi::removefile( const FINT& unit){

  std::ostringstream oss;
  oss << "rm " << getfilep(unit)->filename() << '\n';

  getfilep(unit)->get_policy()->remove_mapper(false);
  files[unit] = NULL; // remove shared_ptr, 
  // not getfilep(unit).reset(), that would just reset the temporary 

  std::system(oss.str().c_str());  
}

void fortranapi::flushfile( const FINT& unit){
  getfilep(unit)->flush();
}

void fortranapi::syncfile( const FINT& unit){
  getfilep(unit)->get_policy()->sync(unit);
}

void fortranapi::syncpool( const FINT& poolid){
  getpolicy(poolid)->sync();
}

void fortranapi::closepool( const FINT& poolid){
  getpolicy(poolid).reset();
}

void fortranapi::removepool( const FINT& poolid){
  auto pp(getpolicy(poolid));
  pp->forget_all_mem();
  std::ostringstream oss;
  oss << "rm ";
  
  for (auto p = files.begin(); p!=files.end(); ++p)
    {
      pw_mapper pm(*p); 
      if (pm->get_policy() == pp){
	 oss << pm->filename() << ' ';
      }
    }
  pp.reset();
  std::system(oss.str().c_str());
}

void fortranapi::flushpool( const FINT& poolid){
  pools[poolid]->return_all_mem();
}
