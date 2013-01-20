#include "fortran_api.hpp"

#include "fortranAPI_handler.hpp"

#include <iostream>

extern "C" 
void for_buf_construct_( const FINT& maxmem, const FINT& blocksize, const FINT& storagepolicy, const FINT& nthread, FINT& pool_id ){

  std::cout << "Called for_buf_construct\n";

  std::cout << "maxmem        =" << maxmem << '\n'
	    << "blocksize     =" << blocksize << '\n'
	    << "storagepolicy =" << storagepolicy << '\n'
            << "nthread       =" << nthread << '\n';

  pool_id = fortranapi::get().construct(maxmem, 
					blocksize, 
					storagepolicy, 
					nthread);

}

extern "C" 
void for_buf_openfile_( const FINT& pool_id, const FINT& unit, const char* filename, const int& length){

  std::string fn;
  for (int i=0; i < length; ++i) fn += filename[i];

  std::cout << "Called for_buf_openfile\n";

  std::cout << "pool_id  =" << pool_id << '\n'
	    << "unit     =" << unit << '\n'
	    << "filename =" << fn << '\n'
            << "length   =" << length << '\n';

  fortranapi::get().openfile(pool_id, unit, fn);

}

extern "C"
void for_buf_writeelement_( const FINT& unit, const FINT& pos, const double& value, const FINT& threadnum){

  std::cout << "Called for_buf_writeElement\n";

  std::cout << "unit     =" << unit << '\n'
	    << "pos      =" << pos << '\n'
	    << "value    =" << value << '\n'
            << "threadnum=" << threadnum << '\n';

  fortranapi::get().writeElement(unit, pos, value, threadnum);
}

extern "C"
void for_buf_writearray_( const FINT& unit, const FINT& pos, const FINT& N, const double* values, const FINT& threadnum){

  std::cout << "Called for_buf_writeArray\n";

  std::cout << "unit     =" << unit << '\n'
	    << "pos      =" << pos << '\n'
	    << "N        =" << N << '\n'
	    << "value[0] =" << values[0] << '\n'
            << "threadnum=" << threadnum << '\n';


  fortranapi::get().writeArray(unit, pos, N, values, threadnum);
}

extern "C"
void for_buf_readelement_( const FINT& unit, const FINT& pos, double& value, const FINT& threadnum){

  std::cout << "Called for_buf_readElement\n";

  std::cout  << "unit     =" << unit << '\n'
	     << "pos      =" << pos << '\n'
	     << "threadnum=" << threadnum << '\n';

  value = fortranapi::get().readElement(unit, pos, threadnum);
}

extern "C"
void for_buf_readarray_( const FINT& unit, const FINT& pos, const FINT& N, double* values, const FINT& threadnum){

  std::cout << "Called for_buf_readElement\n";

  std::cout  << "unit     =" << unit << '\n'
	     << "pos      =" << pos << '\n'
	    << "N        =" << N << '\n'
	     << "threadnum=" << threadnum << '\n';

  fortranapi::get().readArray(unit, pos, N, values, threadnum);
}

// close file, flush before closing
extern "C"
void for_buf_closefile_( const FINT& unit){
  
  std::cout << "Called for_buf_closefile\n";

  std::cout << "unit     =" << unit << '\n';

  fortranapi::get().closefile(unit);
}

// delete file, free memory, no flush
extern"C"
void for_buf_removefile_( const FINT& unit){

  std::cout << "Called for_buf_removefile\n";

  std::cout << "unit     =" << unit << '\n';

  fortranapi::get().removefile(unit);
}

// write all buffers to file
extern "C"
void for_buf_flushfile_( const FINT& unit){

  std::cout << "Called for_buf_flushfile\n";

  std::cout << "unit     =" << unit << '\n';

  fortranapi::get().flushfile(unit);
}

// sync all buffers to file
extern "C"
void for_buf_syncfile_( const FINT& unit){

  std::cout << "Called for_buf_syncfile\n";

  std::cout << "unit     =" << unit << '\n';

  fortranapi::get().syncfile(unit);
}

// sync all buffers to file
extern "C"
void for_buf_syncpool_( const FINT& pool_id){

  std::cout << "Called for_buf_syncpool\n";

  std::cout << "unit     =" << pool_id << '\n';

  fortranapi::get().syncpool(pool_id);
}

// close pool, free all memory, write all buffers to the file
extern "C" 
void for_buf_closepool_( const FINT& pool_id ){

  std::cout << "Called for_buf_closepool\n";

  std::cout << "pool_id     =" << pool_id << '\n';

  fortranapi::get().closepool(pool_id);
}

// close pool, free all memory, erase all files
extern "C" 
void for_buf_removepool_( const FINT& pool_id ){

  std::cout << "Called for_buf_removepool\n";

  std::cout << "pool_id     =" << pool_id << '\n';

  fortranapi::get().removepool(pool_id);
}

// flush pool
extern "C"
void for_buf_flushpool_( const FINT& pool_id ){

  std::cout << "Called for_buf_flushpool\n";

  std::cout << "pool_id     =" << pool_id << '\n';

  fortranapi::get().flushpool(pool_id);
}
