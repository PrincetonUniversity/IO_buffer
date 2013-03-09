#include "fortran_api.hpp"

#include "fortranAPI_handler.hpp"

#include <iostream>

extern "C" 
void for_double_buf_construct_( const FINT& maxmem, const FINT& blocksize, const FINT& storagepolicy, const FINT& nthread, FINT& pool_id ){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_double_buf_construct\n";

  std::cout << "maxmem        =" << maxmem << '\n'
	    << "blocksize     =" << blocksize << '\n'
	    << "storagepolicy =" << storagepolicy << '\n'
            << "nthread       =" << nthread << '\n'
            << "poolID(temp)  =" << pool_id << '\n';

  std::cout.flush();
#endif

  pool_id = fortranapi<double>::get().construct(maxmem, 
					blocksize, 
					storagepolicy, 
					nthread);
  
#ifdef DEBUG_FORBUF
  std::cout << "Calling for_double_buf_construct done\n";

  std::cout << "poolid        =" << pool_id << '\n';

  std::cout.flush();
#endif

}

void for_double_buf_changebuffersize_( const FINT& pool_id,
				const FINT& maxmem ){
  
#ifdef DEBUG_FORBUF
  std::cout << "Called for_double_buf_changebuffersize\n";

  std::cout << "maxmem        =" << maxmem << '\n'
            << "pool_id       =" << pool_id << '\n';

  std::cout.flush();
#endif
    
  fortranapi<double>::get().changebuffersize( pool_id, maxmem);
}

extern "C" 
void for_double_buf_openfile_( const FINT& pool_id, const FINT& unit, const char* filename, const FINT& length){
  
#ifdef DEBUG_FORBUF
  std::cout << "Called for_double_buf_openfile\n";

  std::cout << "pool_id  =" << pool_id << '\n'
	    << "unit     =" << unit << '\n'
            << "length   =" << length << '\n';
  std::cout.flush();
#endif

  std::string fn;
  for (int i=0; i < length; ++i) fn += filename[i];

#ifdef DEBUG_FORBUF
  std::cout << "filename =" << fn << '\n';
  std::cout.flush();
#endif

  fortranapi<double>::get().openfile(pool_id, unit, fn, false);

}

extern "C"
void for_double_buf_reopenfile_( const FINT& pool_id, const FINT& unit, const char* filename, const FINT& length){
  
#ifdef DEBUG_FORBUF
  std::cout << "Called for_double_buf_reopenfile\n";

  std::cout << "pool_id  =" << pool_id << '\n'
	    << "unit     =" << unit << '\n'
            << "length   =" << length << '\n';
  std::cout.flush();
#endif

  std::string fn;
  for (int i=0; i < length; ++i) fn += filename[i];

#ifdef DEBUG_FORBUF
  std::cout << "filename =" << fn << '\n';
  std::cout.flush();
#endif

  fortranapi<double>::get().openfile(pool_id, unit, fn, true);

}

extern "C"
void for_double_buf_writeelement_( const FINT& unit, const FINT& pos, const double& value, const FINT& threadnum){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_double_buf_writeElement\n";

  std::cout << "unit     =" << unit << '\n'
	    << "pos      =" << pos << '\n'
	    << "value    =" << value << '\n'
            << "threadnum=" << threadnum << '\n';
  std::cout.flush();
#endif

  fortranapi<double>::get().writeElement(unit, pos, value, threadnum-1);
}

extern "C"
void for_double_buf_writeblock_( const FINT& unit, const FINT& block, const double* values, const FINT& threadnum){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_double_buf_writeblock\n";
  
  std::cout << "unit     =" << unit << '\n'
            << "block    =" << block << '\n'
            << "threadnum=" << threadnum << '\n';
#endif
  fortranapi<double>::get().writeBlock(unit, block-1, values, threadnum-1);
}

extern "C"
void for_double_buf_writearray_( const FINT& unit, const FINT& pos, const FINT& N, const double* values, const FINT& threadnum){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_double_buf_writeArray\n";

  std::cout << "unit     =" << unit << '\n'
	    << "pos      =" << pos << '\n'
	    << "N        =" << N << '\n'
	    << "value[0] =" << values[0] << '\n'
            << "threadnum=" << threadnum << '\n';
  std::cout.flush();
#endif

  fortranapi<double>::get().writeArray(unit, pos, N, values, threadnum-1);
}

extern "C"
void for_double_buf_atomic_add_( const FINT& unit, const FINT& pos, const double& value, const FINT& threadnum){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_double_buf_atomic_add\n";

  std::cout << "unit     =" << unit << '\n'
	    << "pos      =" << pos << '\n'
	    << "value    =" << value << '\n'
            << "threadnum=" << threadnum << '\n';
  std::cout.flush();
#endif

  fortranapi<double>::get().atomicaddElement(unit, pos, value, threadnum-1);
}

extern "C"
void for_double_buf_atomic_add_array_( const FINT& unit, const FINT& pos, const FINT& N, const double* values, const FINT& threadnum){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_double_buf_atomic_add_array\n";

  std::cout << "unit     =" << unit << '\n'
	    << "pos      =" << pos << '\n'
	    << "N        =" << N << '\n'
	    << "value[0] =" << values[0] << '\n'
            << "threadnum=" << threadnum << '\n';
  std::cout.flush();
#endif

  fortranapi<double>::get().atomicaddArray(unit, pos, N, values, threadnum-1);
}

extern "C"
void for_double_buf_readelement_( const FINT& unit, const FINT& pos, double& value, const FINT& threadnum){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_double_buf_readElement\n";

  std::cout  << "unit     =" << unit << '\n'
	     << "pos      =" << pos << '\n'
	     << "threadnum=" << threadnum << '\n';
  std::cout.flush();
#endif

  value = fortranapi<double>::get().readElement(unit, pos, threadnum-1);

#ifdef DEBUG_FORBUF
  std::cout << "Done with for_double_buf_readElement\n";
  std::cout.flush();
#endif
}

extern "C"
void for_double_buf_readblock_( const FINT& unit, const FINT& block, double* values, const FINT& threadnum){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_double_buf_readblock\n";
  
  std::cout << "unit     =" << unit << '\n'
            << "block    =" << block << '\n'
            << "threadnum=" << threadnum << '\n';
#endif
  fortranapi<double>::get().readBlock(unit, block-1, values, threadnum-1);
}

extern "C"
void for_double_buf_readarray_( const FINT& unit, const FINT& pos, const FINT& N, double* values, const FINT& threadnum){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_double_buf_readElement\n";

  std::cout  << "unit     =" << unit << '\n'
	     << "pos      =" << pos << '\n'
	    << "N        =" << N << '\n'
	     << "threadnum=" << threadnum << '\n';
  std::cout.flush();
#endif

  fortranapi<double>::get().readArray(unit, pos, N, values, threadnum-1);
}

// close file, flush before closing
extern "C"
void for_double_buf_closefile_( const FINT& unit){
  
#ifdef DEBUG_FORBUF
  std::cout << "Called for_double_buf_closefile\n";

  std::cout << "unit     =" << unit << '\n';
  std::cout.flush();
#endif

  fortranapi<double>::get().closefile(unit);
}

// delete file, free memory, no flush
extern"C"
void for_double_buf_removefile_( const FINT& unit){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_double_buf_removefile\n";

  std::cout << "unit     =" << unit << '\n';
  std::cout.flush();
#endif

  fortranapi<double>::get().removefile(unit);
}

// write all buffers to file
extern "C"
void for_double_buf_flushfile_( const FINT& unit){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_double_buf_flushfile\n";

  std::cout << "unit     =" << unit << '\n';
  std::cout.flush();
#endif

  fortranapi<double>::get().flushfile(unit);
}

// sync all buffers to file
extern "C"
void for_double_buf_syncfile_( const FINT& unit){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_double_buf_syncfile\n";

  std::cout << "unit     =" << unit << '\n';
  std::cout.flush();
#endif

  fortranapi<double>::get().syncfile(unit);
}

// sync all buffers to file
extern "C"
void for_double_buf_syncpool_( const FINT& pool_id){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_double_buf_syncpool\n";

  std::cout << "unit     =" << pool_id << '\n';
  std::cout.flush();
#endif

  fortranapi<double>::get().syncpool(pool_id);
}

// close pool, free all memory, write all buffers to the file
extern "C" 
void for_double_buf_closepool_( const FINT& pool_id ){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_double_buf_closepool\n";

  std::cout << "pool_id     =" << pool_id << '\n';
  std::cout.flush();
#endif

  fortranapi<double>::get().closepool(pool_id);

  std::cerr << "Done with closepool\n"; std::cerr.flush();
}

// close pool, free all memory, erase all files
extern "C" 
void for_double_buf_removepool_( const FINT& pool_id ){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_double_buf_removepool\n";

  std::cout << "pool_id     =" << pool_id << '\n';
  std::cout.flush();
#endif

  fortranapi<double>::get().removepool(pool_id);
}

// flush pool
extern "C"
void for_double_buf_flushpool_( const FINT& pool_id ){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_double_buf_flushpool\n";

  std::cout << "pool_id     =" << pool_id << '\n';
  std::cout.flush();
#endif

  fortranapi<double>::get().flushpool(pool_id);
}



extern "C" 
void for_int_buf_construct_( const FINT& maxmem, const FINT& blocksize, const FINT& storagepolicy, const FINT& nthread, FINT& pool_id ){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_int_buf_construct\n";

  std::cout << "maxmem        =" << maxmem << '\n'
	    << "blocksize     =" << blocksize << '\n'
	    << "storagepolicy =" << storagepolicy << '\n'
            << "nthread       =" << nthread << '\n';

  std::cout.flush();
#endif

  pool_id = fortranapi<FINT>::get().construct(maxmem, 
					blocksize, 
					storagepolicy, 
					nthread);

}

void for_int_buf_changebuffersize_( const FINT& pool_id,
				const FINT& maxmem ){
  
#ifdef DEBUG_FORBUF
  std::cout << "Called for_int_buf_changebuffersize\n";

  std::cout << "maxmem        =" << maxmem << '\n'
            << "pool_id       =" << pool_id << '\n';

  std::cout.flush();
#endif
    
  fortranapi<FINT>::get().changebuffersize( pool_id, maxmem);
}

extern "C" 
void for_int_buf_openfile_( const FINT& pool_id, const FINT& unit, const char* filename, const FINT& length){
  
#ifdef DEBUG_FORBUF
  std::cout << "Called for_int_buf_openfile\n";

  std::cout << "pool_id  =" << pool_id << '\n'
	    << "unit     =" << unit << '\n'
            << "length   =" << length << '\n';
  std::cout.flush();
#endif

  std::string fn;
  for (int i=0; i < length; ++i) fn += filename[i];

#ifdef DEBUG_FORBUF
  std::cout << "filename =" << fn << '\n';
  std::cout.flush();
#endif

  fortranapi<FINT>::get().openfile(pool_id, unit, fn, false);

}

extern "C"
void for_int_buf_reopenfile_( const FINT& pool_id, const FINT& unit, const char* filename, const FINT& length){
  
#ifdef DEBUG_FORBUF
  std::cout << "Called for_int_buf_reopenfile\n";

  std::cout << "pool_id  =" << pool_id << '\n'
	    << "unit     =" << unit << '\n'
            << "length   =" << length << '\n';
  std::cout.flush();
#endif

  std::string fn;
  for (int i=0; i < length; ++i) fn += filename[i];

#ifdef DEBUG_FORBUF
  std::cout << "filename =" << fn << '\n';
  std::cout.flush();
#endif

  fortranapi<FINT>::get().openfile(pool_id, unit, fn, true);

}

extern "C"
void for_int_buf_writeelement_( const FINT& unit, const FINT& pos, const FINT& value, const FINT& threadnum){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_int_buf_writeElement\n";

  std::cout << "unit     =" << unit << '\n'
	    << "pos      =" << pos << '\n'
	    << "value    =" << value << '\n'
            << "threadnum=" << threadnum << '\n';
  std::cout.flush();
#endif

  fortranapi<FINT>::get().writeElement(unit, pos, value, threadnum-1);
}

extern "C"
void for_int_buf_writeblock_( const FINT& unit, const FINT& block, const FINT* values, const FINT& threadnum){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_int_buf_writeblock\n";
  
  std::cout << "unit     =" << unit << '\n'
            << "block    =" << block << '\n'
            << "threadnum=" << threadnum << '\n';
#endif
  fortranapi<FINT>::get().writeBlock(unit, block-1, values, threadnum-1);
}

extern "C"
void for_int_buf_writearray_( const FINT& unit, const FINT& pos, const FINT& N, const FINT* values, const FINT& threadnum){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_int_buf_writeArray\n";

  std::cout << "unit     =" << unit << '\n'
	    << "pos      =" << pos << '\n'
	    << "N        =" << N << '\n'
	    << "value[0] =" << values[0] << '\n'
            << "threadnum=" << threadnum << '\n';
  std::cout.flush();
#endif

  fortranapi<FINT>::get().writeArray(unit, pos, N, values, threadnum-1);
}

extern "C"
void for_int_buf_atomic_add_( const FINT& unit, const FINT& pos, const FINT& value, const FINT& threadnum){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_int_buf_atomic_add\n";

  std::cout << "unit     =" << unit << '\n'
	    << "pos      =" << pos << '\n'
	    << "value    =" << value << '\n'
            << "threadnum=" << threadnum << '\n';
  std::cout.flush();
#endif

  fortranapi<FINT>::get().atomicaddElement(unit, pos, value, threadnum-1);
}

extern "C"
void for_int_buf_atomic_add_array_( const FINT& unit, const FINT& pos, const FINT& N, const FINT* values, const FINT& threadnum){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_int_buf_atomic_add_array\n";

  std::cout << "unit     =" << unit << '\n'
	    << "pos      =" << pos << '\n'
	    << "N        =" << N << '\n'
	    << "value[0] =" << values[0] << '\n'
            << "threadnum=" << threadnum << '\n';
  std::cout.flush();
#endif

  fortranapi<FINT>::get().atomicaddArray(unit, pos, N, values, threadnum-1);
}

extern "C"
void for_int_buf_readelement_( const FINT& unit, const FINT& pos, FINT& value, const FINT& threadnum){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_int_buf_readElement\n";

  std::cout  << "unit     =" << unit << '\n'
	     << "pos      =" << pos << '\n'
	     << "threadnum=" << threadnum << '\n';
  std::cout.flush();
#endif

  value = fortranapi<FINT>::get().readElement(unit, pos, threadnum-1);
}

extern "C"
void for_int_buf_readblock_( const FINT& unit, const FINT& block, FINT* values, const FINT& threadnum){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_int_buf_readblock\n";
  
  std::cout << "unit     =" << unit << '\n'
            << "block    =" << block << '\n'
            << "threadnum=" << threadnum << '\n';
#endif
  fortranapi<FINT>::get().readBlock(unit, block-1, values, threadnum-1);
}

extern "C"
void for_int_buf_readarray_( const FINT& unit, const FINT& pos, const FINT& N, FINT* values, const FINT& threadnum){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_int_buf_readElement\n";

  std::cout  << "unit     =" << unit << '\n'
	     << "pos      =" << pos << '\n'
	    << "N        =" << N << '\n'
	     << "threadnum=" << threadnum << '\n';
  std::cout.flush();
#endif

  fortranapi<FINT>::get().readArray(unit, pos, N, values, threadnum-1);
}

// close file, flush before closing
extern "C"
void for_int_buf_closefile_( const FINT& unit){
  
#ifdef DEBUG_FORBUF
  std::cout << "Called for_int_buf_closefile\n";

  std::cout << "unit     =" << unit << '\n';
  std::cout.flush();
#endif

  fortranapi<FINT>::get().closefile(unit);
}

// delete file, free memory, no flush
extern"C"
void for_int_buf_removefile_( const FINT& unit){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_int_buf_removefile\n";

  std::cout << "unit     =" << unit << '\n';
  std::cout.flush();
#endif

  fortranapi<FINT>::get().removefile(unit);
}

// write all buffers to file
extern "C"
void for_int_buf_flushfile_( const FINT& unit){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_int_buf_flushfile\n";

  std::cout << "unit     =" << unit << '\n';
  std::cout.flush();
#endif

  fortranapi<FINT>::get().flushfile(unit);
}

// sync all buffers to file
extern "C"
void for_int_buf_syncfile_( const FINT& unit){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_int_buf_syncfile\n";

  std::cout << "unit     =" << unit << '\n';
  std::cout.flush();
#endif

  fortranapi<FINT>::get().syncfile(unit);
}

// sync all buffers to file
extern "C"
void for_int_buf_syncpool_( const FINT& pool_id){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_int_buf_syncpool\n";

  std::cout << "unit     =" << pool_id << '\n';
  std::cout.flush();
#endif

  fortranapi<FINT>::get().syncpool(pool_id);
}

// close pool, free all memory, write all buffers to the file
extern "C" 
void for_int_buf_closepool_( const FINT& pool_id ){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_int_buf_closepool\n";

  std::cout << "pool_id     =" << pool_id << '\n';
  std::cout.flush();
#endif

  fortranapi<FINT>::get().closepool(pool_id);
}

// close pool, free all memory, erase all files
extern "C" 
void for_int_buf_removepool_( const FINT& pool_id ){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_int_buf_removepool\n";

  std::cout << "pool_id     =" << pool_id << '\n';
  std::cout.flush();
#endif

  fortranapi<FINT>::get().removepool(pool_id);
}

// flush pool
extern "C"
void for_int_buf_flushpool_( const FINT& pool_id ){

#ifdef DEBUG_FORBUF
  std::cout << "Called for_int_buf_flushpool\n";

  std::cout << "pool_id     =" << pool_id << '\n';
  std::cout.flush();
#endif

  fortranapi<FINT>::get().flushpool(pool_id);
}
