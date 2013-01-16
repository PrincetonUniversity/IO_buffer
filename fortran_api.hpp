#ifndef FORTRAN_API_DEF
#define FORTRAN_API_DEF

#define FINT long long 

extern "C" 
void for_buf_construct( const FINT& maxmem, const FINT& blocksize, const FINT& storagepolicy, const FINT& nthread, FINT& pool_id );

extern "C" 
void for_buf_openfile( const FINT& pool_id, const FINT& unit, const char* filename, const int& length);

extern "C"
void for_buf_writeElement( const FINT& unit, const FINT& pos, const double& value, const FINT& threadnum);

extern "C"
void for_buf_readElement( const FINT& unit, const FINT& pos, double& value, const FINT& threadnum);

// close file, flush before closing
extern "C"
void for_buf_closefile( const FINT& unit);

// delete file, free memory, no flush
extern"C"
void for_buf_removefile( const FINT& unit);

// write all buffers to file
extern "C"
void for_buf_flushfile( const FINT& unit);

// close pool, free all memory, write all buffers to the file
extern "C" 
void for_buf_closepool( const FINT& pool_id );

// close pool, free all memory, erase all files
extern "C" 
void for_buf_removepool( const FINT& pool_id );

// flush all files
extern "C"
void for_buf_flushall( const FINT& pool_id );

#endif
