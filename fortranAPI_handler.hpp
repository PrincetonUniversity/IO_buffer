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
		   const FINT& threadnum);

  void writeArray( const FINT& unit,
		   const FINT& pos,
		   const FINT& N,
		   const T* values,
		   const FINT& threadnum);

  T readElement( const FINT& unit,
		      const FINT& pos,
		      const FINT& threadnum){
    return getfilep(unit)->get(pos, threadnum);}

  void readBlock( const FINT& unit,
		  const FINT& blockid,
		  T* values,
		  const FINT& threadnum);

  void readArray( const FINT& unit,
		  const FINT& pos,
		  const FINT& N,
		  T* values,
		  const FINT& threadnum);

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
