#ifndef FORTRAN_API_DEF
#define FORTRAN_API_DEF

#define FINT long long 

/*
  construct memory buffer in RAM, maxmem number of blocks in mem, 
  handled in chunks of blocksize, and concurrent access by nthred threads 

 Policies currently supported
  0        LiFo policy, if RAM runs low keep old vectors in RAM,
	   additionally keep two buffers per thread for short-term buffering
  1        LiLo policy, if RAM runs low keep youngest vectors in RAM

  return integer pool_id as handle to the constructed buffer
*/
extern "C" 
void for_buf_construct_( const FINT& maxmem, const FINT& blocksize, const FINT& storagepolicy, const FINT& nthread, FINT& pool_id );

/* 
   open file, and connect to logical unit number unit,
   use pool pool_id for buffering, length is not needed in fortran interface 
*/

extern "C" 
void for_buf_openfile_( const FINT& pool_id, const FINT& unit, const char* filename, const int& length);

/* write one element to the file unit at position pos */
extern "C"
void for_buf_writeelement_( const FINT& unit, const FINT& pos, const double& value, const FINT& threadnum);

/* write a block of elements to the file unit into block block */
extern "C"
void for_buf_writeblock_( const FINT& unit, const FINT& block, const double* values, const FINT& threadnum);

/* write N elements to the file unit starting at position pos */
extern "C"
void for_buf_writearray_( const FINT& unit, const FINT& pos, const FINT& N, const double* values, const FINT& threadnum);

/* read one element from the file unit at position pos */
extern "C"
void for_buf_readelement_( const FINT& unit, const FINT& pos, double& value, const FINT& threadnum);

/* read a block of elements from file unit and block block */
extern "C"
void for_buf_readblock_( const FINT& unit, const FINT& block, const double* values, const FINT& threadnum);

/* read N elements from the file unit starting at position pos */
extern "C"
void for_buf_readarray_( const FINT& unit, const FINT& pos, const FINT& N, double* values, const FINT& threadnum);

// close file or entire pool, save content to disk before closing
extern "C"
void for_buf_closefile_( const FINT& unit);
extern "C" 
void for_buf_closepool_( const FINT& pool_id );

// delete file, or entire pool, free memory, no flush, remove 
// all intermediate files! potentially save IO for writing
// temporary data. Note that unmodified memory will never be written
extern"C"
void for_buf_removefile_( const FINT& unit);
extern "C" 
void for_buf_removepool_( const FINT& pool_id );

// write all memory buffers to file and free memory, without closing file
extern "C"
void for_buf_flushfile_( const FINT& unit);
extern "C"
void for_buf_flushpool_( const FINT& pool_id );

// save all memory buffers for one file or entire pool to disk, 
// without closing file or freeing memory
extern "C"
void for_buf_syncfile_( const FINT& unit);
extern "C"
void for_buf_syncpool_( const FINT& pool_id);

#endif
