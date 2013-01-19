#include "fortranAPI_handler.hpp"
#include "buffer.hpp"
#include "buffer.tpp"
#include "mapper.hpp"
#include "LiFo_policy.hpp"

p_abstract_policy fortranapi::getpolicy( FINT pool_id ){

  if (pool_id >= pools.size())
    throw E_invalid_pool_id(pool_id);

  p_abstract_policy pp( pools[pool_id]);

  if (!pp)
    throw E_invalid_pool_id(pool_id);

  return pp;
}

p_mapper fortranapi::getfilep( FINT index){
  p_mapper p(files[index]);
  if (!p) 
    throw E_unknown_file_id(index);
  return p;
}

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
  files[unit] = mapper<double>::factory(filename, unit, getpolicy(pool_id));
}

void fortranapi::writeElement( const FINT& unit,
				       const FINT& pos,
				       const double& value,
				       const FINT& threadnum){
  getfilep(unit)->set(pos, value, threadnum);
}

double fortranapi::readElement( const FINT& unit,
				const FINT& pos,
				const FINT& threadnum){
  return getfilep(unit)->get(pos, threadnum);
}

void fortranapi::closefile( const FINT& unit){
  getfilep(unit)->get_policy()->remove_mapper(unit);
  files[unit].reset();
}

void fortranapi::removefile( const FINT& unit){
  getfilep(unit)->get_policy()->remove_mapper(false);
  std::ostringstream oss;
  oss << "rm " << getfilep(unit)->filename() << '\n';
  files[unit].reset(); // remove shared_ptr, 
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
      p_mapper pm(p->second.lock()); 
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
