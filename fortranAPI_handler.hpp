#ifndef FOURTRAN_API_HANDLER_FLO
#define FOURTRAN_API_HANDLER_FLO

#include <map>
#include <vector>
#include "mapper.hpp"
#include "singleton.hpp"
#include <exception>
#include <iostream>
#include <sstream>

#define FINT long long 

class E_unknown_storagepolicy: public std::exception{
public:
  FINT sp;
  E_unknown_storagepolicy(FINT i):sp(i){};
  
  ~E_unknown_storagepolicy() throw() {};

  const char* what() const throw(){
    return "Storage policy unknown!";
  }
};

class E_unit_number_already_open: public std::exception{
public:
  FINT sp;
  E_unit_number_already_open(FINT i):sp(i){};
  
  ~E_unit_number_already_open() throw() {};

  const char* what() const throw(){
    return "Unit number already open!";
  }
};

class E_invalid_pool_id: public std::exception{
public:
  FINT sp;
  E_invalid_pool_id(FINT i):sp(i){};
  
  ~E_invalid_pool_id() throw() {};

  const char* what() const throw(){
    return "Pool id invalid!";
  }
};

class E_unknown_file_id: public std::exception{
public:
  FINT sp;
  E_unknown_file_id(FINT i):sp(i){};
  
  ~E_unknown_file_id() throw() {};

  const char* what() const throw(){
    return "File id unknown!";
  }
};

class E_checksum_error: public std::exception{
public:
  E_checksum_error(){};
  
  ~E_checksum_error() throw() {};

  const char* what() const throw(){
    return "Checksum error!";
  }
};

class filestruct;

template <class T>
class abstract_policy;

template <class T>
class mapper;

template <class T>
class fortranapi: public Singleton<fortranapi<T> >{

typedef std::shared_ptr<abstract_policy<T> > p_abstract_policy;
typedef mapper<T>* pw_mapper;

public:

  FINT construct(const FINT& maxmem,
		 const FINT& blocksize,
		 const FINT& storagepolicy,
		 const FINT& nthread);

  void changebuffersize(const FINT& pool_id,
			const FINT& maxmem);

  void openfile(const FINT& pool_id,
		const FINT& unit,
		std::string filename,
		bool reopen = false);

  void writeElement( const FINT& unit,
		     const FINT& pos,
		     const T& value,
		     const FINT& threadnum){
    getfilep(unit)->set(pos, value, threadnum);
  };

  void writeBlock( const FINT& unit,
		   const FINT& blockid,
		   const T* values,
		   const FINT& threadnum){
    getfilep(unit)->setchunk(blockid, values, threadnum);
  };

  void writeArray( const FINT& unit,
		   const FINT& pos,
		   const FINT& N,
		   const T* values,
		   const FINT& threadnum){
  getfilep(unit)->set(pos, N, values, threadnum);
  };

  void atomicaddElement( const FINT& unit,
			 const FINT& pos,
			 const T& value,
			 const FINT& threadnum){
    getfilep(unit)->atomic_add(pos, value, threadnum);
  };

  void atomicaddArray( const FINT& unit,
		   const FINT& pos,
		   const FINT& N,
		   const T* values,
		   const FINT& threadnum){
    getfilep(unit)->atomic_add(pos, N, values, threadnum);
  };  

  T readElement( const FINT& unit,
		 const FINT& pos,
		 const FINT& threadnum){
    return getfilep(unit)->get(pos, threadnum);}

  void readBlock( const FINT& unit,
		  const FINT& blockid,
		  T* values,
		  const FINT& threadnum){
    getfilep(unit)->getchunk(blockid, values, threadnum);
  };

  void readArray( const FINT& unit,
		  const FINT& pos,
		  const FINT& N,
		  T* values,
		  const FINT& threadnum){
    getfilep(unit)->get(pos, N, values, threadnum);
  };
    
  void get_c_pointer( const FINT& unit, 
		      const FINT& block, 
		      T** values,
		      const FINT& threadnum,
		      FINT& blocksize);

  void free_c_pointer( const FINT& unit, const FINT& block);

  void closefile( const FINT& unit);

  void removefile( const FINT& unit);

  void flushfile( const FINT& unit);

  void syncfile( const FINT& unit);

  void syncpool( const FINT& poolid);

  void closepool( const FINT& poolid);

  void removepool( const FINT& poolid);

  void flushpool( const FINT& poolid);

  void output_all_known();

private:

  unsigned short crc_files;
  unsigned short crc_pools;
 
  unsigned short get_checksum_files() const;
  unsigned short get_checksum_pools() const;

  void update_checksums();

  void check_for_corruption() const;

  std::string my_name() const;

  std::vector< pw_mapper > files;

  std::vector< p_abstract_policy > pools;

  pw_mapper getfilep( int file_id )
#ifdef FORBUF_FAST
  { return files[file_id];}
#endif
;
  p_abstract_policy getpolicy( FINT );

};

#endif
