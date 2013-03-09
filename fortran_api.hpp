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
void for_double_buf_construct_( const FINT& maxmem, const FINT& blocksize, const FINT& storagepolicy, const FINT& nthread, FINT& pool_id );

/* change the buffer size to maxmem,
   this does not force the buffer to immediately shrink (or expand)
   free memory by, e.g., flushfile */
extern "C"
void for_double_buf_changebuffersize_( const FINT& pool_id, const FINT& maxmem);

/* 
   open file, and connect to logical unit number unit,
   use pool pool_id for buffering, length is not needed in fortran interface 
*/

extern "C" 
void for_double_buf_openfile_( const FINT& pool_id, const FINT& unit, const char* filename, const FINT& length);

/* 
   reopen existing file, and connect to logical unit number unit,
   use pool pool_id for buffering, length is not needed in fortran interface 
*/

extern "C" 
void for_double_buf_reopenfile_( const FINT& pool_id, const FINT& unit, const char* filename, const FINT& length);

/* write one element to the file unit at position pos */
extern "C"
void for_double_buf_writeelement_( const FINT& unit, const FINT& pos, const double& value, const FINT& threadnum);

/* write a block of elements to the file unit into block block */
extern "C"
void for_double_buf_writeblock_( const FINT& unit, const FINT& block, const double* values, const FINT& threadnum);

/* write N elements to the file unit starting at position pos */
extern "C"
void for_double_buf_writearray_( const FINT& unit, const FINT& pos, const FINT& N, const double* values, const FINT& threadnum);

/* atomically add a value at pos to a unit */
extern "C"
void for_double_buf_atomic_add_( const FINT& unit, const FINT& pos, const double& value, const FINT& threadnum);

/* atomically add a number of values starting at pos to a unit */
extern "C"
void for_double_buf_atomic_add_array_( const FINT& unit, const FINT& pos, const FINT& N, const double* values, const FINT& threadnum);

/* read one element from the file unit at position pos */
extern "C"
void for_double_buf_readelement_( const FINT& unit, const FINT& pos, double& value, const FINT& threadnum);

/* read a block of elements from file unit and block block */
extern "C"
void for_double_buf_readblock_( const FINT& unit, const FINT& block, double* values, const FINT& threadnum);

/* read N elements from the file unit starting at position pos */
extern "C"
void for_double_buf_readarray_( const FINT& unit, const FINT& pos, const FINT& N, double* values, const FINT& threadnum);

// close file or entire pool, save content to disk before closing
extern "C"
void for_double_buf_closefile_( const FINT& unit);
extern "C" 
void for_double_buf_closepool_( const FINT& pool_id );

// delete file, or entire pool, free memory, no flush, remove 
// all intermediate files! potentially save IO for writing
// temporary data. Note that unmodified memory will never be written
extern"C"
void for_double_buf_removefile_( const FINT& unit);
extern "C" 
void for_double_buf_removepool_( const FINT& pool_id );

// write all memory buffers to file and free memory, without closing file
extern "C"
void for_double_buf_flushfile_( const FINT& unit);
extern "C"
void for_double_buf_flushpool_( const FINT& pool_id );

// save all memory buffers for one file or entire pool to disk, 
// without closing file or freeing memory
extern "C"
void for_double_buf_syncfile_( const FINT& unit);
extern "C"
void for_double_buf_syncpool_( const FINT& pool_id);

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
void for_int_buf_construct_( const FINT& maxmem, const FINT& blocksize, const FINT& storagepolicy, const FINT& nthread, FINT& pool_id );

/* change the buffer size to maxmem,
   this does not force the buffer to immediately shrink (or expand)
   free memory by, e.g., flushfile */
extern "C"
void for_int_buf_changebuffersize_( const FINT& pool_id, const FINT& maxmem);

/* 
   open file, and connect to logical unit number unit,
   use pool pool_id for buffering, length is not needed in fortran interface 
*/

extern "C" 
void for_int_buf_openfile_( const FINT& pool_id, const FINT& unit, const char* filename, const FINT& length);

/* 
   reopen existing file, and connect to logical unit number unit,
   use pool pool_id for buffering, length is not needed in fortran interface 
*/

extern "C" 
void for_int_buf_reopenfile_( const FINT& pool_id, const FINT& unit, const char* filename, const FINT& length);

/* write one element to the file unit at position pos */
extern "C"
void for_int_buf_writeelement_( const FINT& unit, const FINT& pos, const FINT& value, const FINT& threadnum);

/* write a block of elements to the file unit into block block */
extern "C"
void for_int_buf_writeblock_( const FINT& unit, const FINT& block, const FINT* values, const FINT& threadnum);

/* write N elements to the file unit starting at position pos */
extern "C"
void for_int_buf_writearray_( const FINT& unit, const FINT& pos, const FINT& N, const FINT* values, const FINT& threadnum);

/* atomically add to one element to the file unit at position pos */
extern "C"
void for_int_buf_atomic_add_( const FINT& unit, const FINT& pos, const FINT& value, const FINT& threadnum);

/* atomically add to N elements of the file unit starting at position pos */
extern "C"
void for_int_buf_atomic_add_array_( const FINT& unit, const FINT& pos, const FINT& N, const FINT* values, const FINT& threadnum);

/* read one element from the file unit at position pos */
extern "C"
void for_int_buf_readelement_( const FINT& unit, const FINT& pos, FINT& value, const FINT& threadnum);

/* read a block of elements from file unit and block block */
extern "C"
void for_int_buf_readblock_( const FINT& unit, const FINT& block, FINT* values, const FINT& threadnum);

/* read N elements from the file unit starting at position pos */
extern "C"
void for_int_buf_readarray_( const FINT& unit, const FINT& pos, const FINT& N, FINT* values, const FINT& threadnum);

// close file or entire pool, save content to disk before closing
extern "C"
void for_int_buf_closefile_( const FINT& unit);
extern "C" 
void for_int_buf_closepool_( const FINT& pool_id );

// delete file, or entire pool, free memory, no flush, remove 
// all intermediate files! potentially save IO for writing
// temporary data. Note that unmodified memory will never be written
extern"C"
void for_int_buf_removefile_( const FINT& unit);
extern "C" 
void for_int_buf_removepool_( const FINT& pool_id );

// write all memory buffers to file and free memory, without closing file
extern "C"
void for_int_buf_flushfile_( const FINT& unit);
extern "C"
void for_int_buf_flushpool_( const FINT& pool_id );

// save all memory buffers for one file or entire pool to disk, 
// without closing file or freeing memory
extern "C"
void for_int_buf_syncfile_( const FINT& unit);
extern "C"
void for_int_buf_syncpool_( const FINT& pool_id);


#endif
