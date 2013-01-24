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
typedef std::shared_ptr<abstract_policy<double> > p_abstract_policy;

template <class T>
class mapper;
typedef std::shared_ptr<mapper<double> > p_mapper;
typedef mapper<double>* pw_mapper;

class fortranapi: public Singleton<fortranapi>{

public:

  FINT construct(const FINT& maxmem,
		 const FINT& blocksize,
		 const FINT& storagepolicy,
		 const FINT& nthread);

  void openfile(const FINT& pool_id,
		const FINT& unit,
		std::string filename);

  void writeElement( const FINT& unit,
		     const FINT& pos,
		     const double& value,
		     const FINT& threadnum){
    getfilep(unit)->set(pos, value, threadnum);
  };

  void writeArray( const FINT& unit,
		   const FINT& pos,
		   const FINT& N,
		   const double* values,
		   const FINT& threadnum);

  double readElement( const FINT& unit,
		      const FINT& pos,
		      const FINT& threadnum){
    return getfilep(unit)->get(pos, threadnum);}

  void readArray( const FINT& unit,
		  const FINT& pos,
		  const FINT& N,
		  double* values,
		  const FINT& threadnum);

  void closefile( const FINT& unit);

  void removefile( const FINT& unit);

  void flushfile( const FINT& unit);

  void syncfile( const FINT& unit);

  void syncpool( const FINT& poolid);

  void closepool( const FINT& poolid);

  void removepool( const FINT& poolid);

  void flushpool( const FINT& poolid);

private:

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
