#include "fortran_api.hpp"

// class to handle fortran API

// Now define the API functions

#include <iostream>

extern "C" 
void for_buf_construct( const FINT& maxmem, const FINT& blocksize, const FINT& storagepolicy, const FINT& nthread, FINT& pool_id ){

  std::cout << "Called for_buf_construct\n";

  std::cout << "maxmem        =" << maxmem << '\n'
	    << "blocksize     =" << blocksize << '\n'
	    << "storagepolicy =" << storagepolicy << '\n'
            << "nthread       =" << nthread << '\n';

}

extern "C" 
void for_buf_openfile( const FINT& pool_id, const FINT& unit, const char* filename, const int& length){

  std::string fn;
  for (int i=0; i < length; ++i) fn += filename[i];

  std::cout << "Called for_buf_openfile\n";

  std::cout << "pool_id  =" << pool_id << '\n'
	    << "unit     =" << unit << '\n'
	    << "filename =" << fn << '\n'
            << "length   =" << length << '\n';

}

extern "C"
void for_buf_writeElement( const FINT& unit, const FINT& pos, const double& value, const FINT& threadnum){

  std::cout << "Called for_buf_writeElement\n";

  std::cout << "unit     =" << unit << '\n'
	    << "pos      =" << pos << '\n'
	    << "value    =" << value << '\n'
            << "threadnum=" << threadnum << '\n';

}

extern "C"
void for_buf_readElement( const FINT& unit, const FINT& pos, double& value, const FINT& threadnum){

  std::cout << "Called for_buf_readElement\n";

  std::cout  << "unit     =" << unit << '\n'
	     << "pos      =" << pos << '\n'
	     << "threadnum=" << threadnum << '\n';

}

// close file, flush before closing
extern "C"
void for_buf_closefile( const FINT& unit){
  
  std::cout << "Called for_buf_closefile\n";

  std::cout << "unit     =" << unit << '\n';
}

// delete file, free memory, no flush
extern"C"
void for_buf_removefile( const FINT& unit){

  std::cout << "Called for_buf_removefile\n";

  std::cout << "unit     =" << unit << '\n';
}

// write all buffers to file
extern "C"
void for_buf_flushfile( const FINT& unit){

  std::cout << "Called for_buf_flushfile\n";

  std::cout << "unit     =" << unit << '\n';
}

// close pool, free all memory, write all buffers to the file
extern "C" 
void for_buf_closepool( const FINT& pool_id ){

  std::cout << "Called for_buf_closepool\n";

  std::cout << "pool_id     =" << pool_id << '\n';
}

// close pool, free all memory, erase all files
extern "C" 
void for_buf_removepool( const FINT& pool_id ){

  std::cout << "Called for_buf_removepool\n";

  std::cout << "pool_id     =" << pool_id << '\n';
}

// flush all files
extern "C"
void for_buf_flushall( const FINT& pool_id ){

  std::cout << "Called for_buf_flushall\n";

  std::cout << "pool_id     =" << pool_id << '\n';
}
