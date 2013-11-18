#include "fortranAPI_handler.hpp"
#include "buffer.hpp"
#include "mapper.hpp"
#include "LiFo_policy.hpp"

#include <mutex>

void calcrc(unsigned short& crc, unsigned char *ptr, size_t count)
{
    unsigned char i;
    {
        crc = crc ^ (unsigned short)*ptr++ << 8;
        i = 8;
        do
        {
            if (crc & 0x8000)
                crc = crc << 1 ^ 0x1021;
            else
                crc = crc << 1;
        } while(--i);
    } while (--count > 0);
}

template <>
std::string fortranapi<FINT>::my_name() const {
  return "FINT";
}

template <class T>
struct T_checker{
    size_t index;
    T*  pp;
};

template <class T>
unsigned short fortranapi<T>::get_checksum_pools() const{
    unsigned short crc(0);

    T_checker<abstract_policy<T> > checker;
    checker.index = 0;

    for (auto p = pools.begin(); p != pools.end(); ++p){
	if (*p){
	    checker.pp = &(*(*p));
	    size_t count = (sizeof(checker)) / (sizeof(unsigned short));
	    calcrc(crc, reinterpret_cast<unsigned char *>(&checker), count);
	}
	++checker.index;
    }

    return crc;
}

template <class T>
unsigned short fortranapi<T>::get_checksum_files() const{
    unsigned short crc(0);

    T_checker<mapper<T> > checker;
    checker.index = 0;

    for (auto p = files.begin(); p != files.end(); ++p){
	if (*p){
	    checker.pp = *p;
	    size_t count = (sizeof(checker)) / (sizeof(unsigned short));
	    calcrc(crc, reinterpret_cast<unsigned char *>(&checker), count);
	}
	++checker.index;
    }

    return crc;
}

template <>
std::string fortranapi<double>::my_name() const {
  return "double";
}

template <class T> 
typename fortranapi<T>::p_abstract_policy fortranapi<T>::getpolicy( FINT pool_id ){

#ifndef FORBUF_FAST
  check_for_corruption();
#endif

  if (static_cast<size_t>(pool_id) >= pools.size()){
    std::cerr << "Pool number "<< pool_id<< " too big, not smaller than " 
              << pools.size()<< "\n";
    throw E_invalid_pool_id(pool_id);
  }

  p_abstract_policy pp( pools[pool_id]);

  if (!pp){
    std::cerr << "Pool number " << pool_id << " not associated\n";
    throw E_invalid_pool_id(pool_id);
  }

  return pp;
}

template <class T>
void fortranapi<T>::output_all_known(){
  for (size_t i = 0; i < files.size(); ++i){
      if (pw_mapper p = files[i])
	std::cout << i << ' ' << p->filename() << '\n';
    } 
}

void outputfiles(){
    std::cerr << "The FINT buffer knows the file ids:\n";
    fortranapi<FINT>::get().output_all_known();

    std::cerr << "The double buffer knows the file ids:\n";
    fortranapi<double>::get().output_all_known();
}

template <class T>
void fortranapi<T>::check_for_corruption() const{
    unsigned short new_crc(get_checksum_files());

    if (new_crc != crc_files){
	std::cerr << "INVALID checksum for files!!" << '\n';
	throw E_checksum_error();
    }

    new_crc = get_checksum_pools();
    if (new_crc != crc_pools){
	std::cerr << "INVALID checksum for pools!!" << '\n';
	throw E_checksum_error();
    }
}

template <class T>
void fortranapi<T>::update_checksums(){
    crc_files = get_checksum_files();
    crc_pools = get_checksum_pools();
}

#ifndef FORBUF_FAST
template <class T> 
typename fortranapi<T>::pw_mapper fortranapi<T>::getfilep( int index){
  pw_mapper p(files[index]);
  check_for_corruption();
  if (!p) {

    std::cout << "Error: Unknown file id " << index << '\n';

    std::cerr << "I am a " << my_name() << " buffer\n";

    outputfiles();

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

#ifndef FORBUF_FAST
  update_checksums();
#endif

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
  if (static_cast<size_t>(unit) >= files.size()){
    files.resize(unit+1);
  } 
  if (files[unit]){
    std::cerr << "ERROR: Unit number " << unit << " already opened.\n";
    throw E_unit_number_already_open(unit);
  }

  files[unit] = mapper<T>::factory(filename, 
				   unit, 
				   getpolicy(pool_id),
				   reopen
				   ).get();
#ifndef FORBUF_FAST
  update_checksums();
#endif
#ifdef DEBUG_FORBUF 
  std::cerr << "now: ";
  outputfiles();
#endif
}

template <class T>
void fortranapi<T>::get_c_pointer( const FINT& unit, 
				   const FINT& block, 
				   T** values,
				   const FINT& threadnum,
				   FINT& blocksize){
    size_t bs;
    getfilep(unit)->get_pointer(block, values, threadnum, bs);
    blocksize = bs;
}

template <class T>
void fortranapi<T>::free_c_pointer( const FINT& unit, 
				    const FINT& block){    
    getfilep(unit)->free_pointer(block);
}

template <class T> 
void fortranapi<T>::closefile( const FINT& unit){
  getfilep(unit)->get_policy()->remove_mapper(unit);
#ifdef DEBUG_FORBUF
  std::cerr << "REMOVING FILE " << unit << "\n";
  std::cerr.flush();
#endif
  files[unit] = NULL;
#ifndef FORBUF_FAST
  update_checksums();
#endif
}

template <class T> 
void fortranapi<T>::removefile( const FINT& unit){

  std::ostringstream oss;
  oss << "rm " << getfilep(unit)->filename() << '\n';

  getfilep(unit)->get_policy()->remove_mapper(unit, false);
  files[unit] = NULL; // remove shared_ptr, 
  // not getfilep(unit).reset(), that would just reset the temporary 
#ifndef FORBUF_FAST
  update_checksums();
#endif
  std::cout << "Execute " << oss.str() << '\n';
  if (std::system(oss.str().c_str()) == -1)
    std::cout << "Error on command " << oss.str() << '\n';
}

template <class T> 
void fortranapi<T>::flushfile( const FINT& unit){
  getfilep(unit)->flush();
}

template <class T> 
void fortranapi<T>::syncfile( const FINT& unit){
  getfilep(unit)->get_policy()->sync(unit);

#ifdef DEBUG_FORBUF
  outputfiles();
#endif
}

template <class T> 
void fortranapi<T>::syncpool( const FINT& poolid){
  getpolicy(poolid)->sync();

#ifdef DEBUG_FORBUF
  outputfiles();
#endif
}

template <class T> 
void fortranapi<T>::closepool( const FINT& poolid){
  std::cout << "closing pool responsible for:\n"; std::cerr.flush();
  auto pp(getpolicy(poolid));
  output_all_known();
  for (auto p = files.begin(); p!=files.end(); ++p)
    {
      pw_mapper pm(*p); 
      if (pm){
	if (pm->get_policy() == pp){
	  *p= NULL;
	}	
      }
    }
  std::cerr << "reseting pool\n"; std::cerr.flush();
  pools[poolid].reset();

#ifndef FORBUF_FAST
  update_checksums();
#endif

  std::cerr << "pool reset\n"; std::cerr.flush();
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
      if (pm){
	if (pm->get_policy() == pp){
	  oss << pm->filename() << ' ';
	  *p= NULL;
	}	
      }
    }
  pools[poolid].reset();
#ifndef FORBUF_FAST
  update_checksums();
#endif

#ifdef DEBUG_FORBUF
  std::cout << "Execute " << oss.str() << '\n';
#endif
  if (std::system(oss.str().c_str()) == -1)
    std::cout << "Error on command " << oss.str() << '\n';
}

template <class T>
void fortranapi<T>::flushpool( const FINT& poolid){
  pools[poolid]->return_all_mem();
}

template class fortranapi<double>;
template class fortranapi<FINT>;
