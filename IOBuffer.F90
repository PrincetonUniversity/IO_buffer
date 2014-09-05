module IOBuffer
  use iso_c_binding

  interface
     subroutine double_buf_construct(maxmem, blocksize, storagepolicy, nthread, pool_id) bind(c)
       use iso_c_binding 
       implicit none
       integer(kind=C_INT64_T), intent(in) :: maxmem
       integer(kind=C_INT64_T), intent(in) :: blocksize
       integer(kind=C_INT64_T), intent(in) :: storagepolicy
       integer(kind=C_INT64_T), intent(in) :: nthread
       integer(kind=C_INT64_T), intent(in) :: pool_id
     end subroutine double_buf_construct

     subroutine for_double_buf_reopenfile(pool_id, unit, filename, length) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: pool_id
       integer(kind=C_INT64_T), intent(in) :: unit
       character(kind=c_char), intent(in) :: filename(*)
       integer(kind=C_INT64_T), intent(in) :: length
     end subroutine for_double_buf_reopenfile

     subroutine for_double_buf_writeelement(unit, pos, val, threadnum) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: unit
       integer(kind=C_INT64_T), intent(in) :: pos
       real(kind=C_DOUBLE), intent(in) :: val
       integer(kind=C_INT64_T), intent(in) :: threadnum
     end subroutine for_double_buf_writeelement

     subroutine for_double_buf_atomic_add(unit, pos, val, threadnum) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: unit
       integer(kind=C_INT64_T), intent(in) :: pos
       real(kind=C_DOUBLE), intent(in) :: val
       integer(kind=C_INT64_T), intent(in) :: threadnum
     end subroutine for_double_buf_atomic_add

     subroutine for_double_buf_readelement(unit, pos, val, threadnum) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: unit
       integer(kind=C_INT64_T), intent(in) :: pos
       real(kind=C_DOUBLE), intent(in) :: val
       integer(kind=C_INT64_T), intent(in) :: threadnum
     end subroutine for_double_buf_readelement

     subroutine for_double_buf_closefile(unit) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: unit
     end subroutine for_double_buf_closefile

     subroutine for_double_buf_removefile(unit) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: unit 
     end subroutine for_double_buf_removefile

     subroutine for_double_buf_removepool(pool_id ) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: pool_id
     end subroutine for_double_buf_removepool

     subroutine for_double_buf_closepool(pool_id) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: pool_id
     end subroutine for_double_buf_closepool

     subroutine for_double_buf_flushfile(unit) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: unit
     end subroutine for_double_buf_flushfile

     subroutine for_double_buf_flushpool(pool_id) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: pool_id
     end subroutine for_double_buf_flushpool

     subroutine for_double_buf_syncfile(unit) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: unit
     end subroutine for_double_buf_syncfile

     subroutine for_double_buf_syncpool(pool_id) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: pool_id
     end subroutine for_double_buf_syncpool

     subroutine for_double_buf_writeblock(unit, block, vals, threadnum) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: unit
       integer(kind=C_INT64_T), intent(in) :: block
       real(kind=C_DOUBLE), intent(in) :: vals(*)
       integer(kind=C_INT64_T), intent(in) :: threadnum
     end subroutine for_double_buf_writeblock

     subroutine for_double_buf_writearray(unit, pos, N, vals, threadnum) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: unit
       integer(kind=C_INT64_T), intent(in) :: pos
       integer(kind=C_INT64_T), intent(in) :: N
       real(kind=C_DOUBLE), intent(in) :: vals(*)
       integer(kind=C_INT64_T), intent(in) :: threadnum
     end subroutine for_double_buf_writearray

     subroutine for_double_buf_atomic_add_array(unit, pos, N, vals, threadnum) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: unit
       integer(kind=C_INT64_T), intent(in) :: pos
       integer(kind=C_INT64_T), intent(in) :: N
       real(kind=C_DOUBLE), intent(in) :: vals(*)
       integer(kind=C_INT64_T), intent(in) :: threadnum
     end subroutine for_double_buf_atomic_add_array

     subroutine for_double_buf_readblock(unit, block, vals, threadnum) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: unit
       integer(kind=C_INT64_T), intent(in) :: block
       real(kind=C_DOUBLE), intent(in) :: vals(*)
       integer(kind=C_INT64_T), intent(in) :: threadnum
     end subroutine for_double_buf_readblock

     subroutine for_double_buf_readarray(unit, pos, N, vals, threadnum) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: unit
       integer(kind=C_INT64_T), intent(in) :: pos
       integer(kind=C_INT64_T), intent(in) :: N
       real(kind=C_DOUBLE), intent(in) :: vals(*)
       integer(kind=C_INT64_T), intent(in) :: threadnum
     end subroutine for_double_buf_readarray

     subroutine for_double_buf_openfile(pool_id, unit_num, filename, filename_length) &
          & bind(c,name='for_double_buf_openfile')
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: pool_id
       integer(kind=C_INT64_T), intent(in) :: unit_num
       character(kind=c_char), intent(in) :: filename(*)
       integer(kind=C_INT64_T), intent(in) :: filename_length
     end subroutine for_double_buf_openfile

     subroutine for_double_buf_construct(maxmem, blocksize, &
          & storagepolicy, nthread, pool_id) &
          & bind(c,name='for_double_buf_construct')
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: maxmem
       integer(kind=C_INT64_T), intent(in) :: blocksize
       integer(kind=C_INT64_T), intent(in) :: storagepolicy
       integer(kind=C_INT64_T), intent(in) :: nthread

       integer(kind=C_INT64_T), intent(out) :: pool_id       
     end subroutine for_double_buf_construct

     subroutine for_double_buf_changebuffersize( pool_id, maxmem) &
          & bind (c,name='for_double_buf_changebuffersize')
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: pool_id
       integer(kind=C_INT64_T), intent(in) :: maxmem
     end subroutine for_double_buf_changebuffersize

     subroutine for_double_buf_return_pointer( unit, block, threadnum) &
          & bind (c,name='for_double_buf_free_c_pointer')
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: unit
       integer(kind=C_INT64_T), intent(in) :: block
       integer(kind=C_INT64_T), intent(in) :: threadnum
     end subroutine for_double_buf_return_pointer

     subroutine for_int_buf_construct(maxmem, blocksize, storagepolicy, nthread, pool_id) bind(c)
       use iso_c_binding 
       implicit none
       integer(kind=C_INT64_T), intent(in) :: maxmem
       integer(kind=C_INT64_T), intent(in) :: blocksize
       integer(kind=C_INT64_T), intent(in) :: storagepolicy
       integer(kind=C_INT64_T), intent(in) :: nthread
       integer(kind=C_INT64_T), intent(in) :: pool_id
     end subroutine for_int_buf_construct


     subroutine int_buf_construct(maxmem, blocksize, storagepolicy, nthread, pool_id) bind(c)
       use iso_c_binding 
       implicit none
       integer(kind=C_INT64_T), intent(in) :: maxmem
       integer(kind=C_INT64_T), intent(in) :: blocksize
       integer(kind=C_INT64_T), intent(in) :: storagepolicy
       integer(kind=C_INT64_T), intent(in) :: nthread
       integer(kind=C_INT64_T), intent(in) :: pool_id
     end subroutine int_buf_construct

     subroutine for_int_buf_reopenfile(pool_id, unit, filename, length) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: pool_id
       integer(kind=C_INT64_T), intent(in) :: unit
       character(kind=c_char), intent(in) :: filename(*)
       integer(kind=C_INT64_T), intent(in) :: length
     end subroutine for_int_buf_reopenfile

     subroutine for_int_buf_writeelement(unit, pos, val, threadnum) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: unit
       integer(kind=C_INT64_T), intent(in) :: pos
       integer(kind=C_INT64_T), intent(in) :: val
       integer(kind=C_INT64_T), intent(in) :: threadnum
     end subroutine for_int_buf_writeelement

     subroutine for_int_buf_atomic_add(unit, pos, val, threadnum) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: unit
       integer(kind=C_INT64_T), intent(in) :: pos
       integer(kind=C_INT64_T), intent(in) :: val
       integer(kind=C_INT64_T), intent(in) :: threadnum
     end subroutine for_int_buf_atomic_add

     subroutine for_int_buf_readelement(unit, pos, val, threadnum) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: unit
       integer(kind=C_INT64_T), intent(in) :: pos
       integer(kind=C_INT64_T), intent(in) :: val
       integer(kind=C_INT64_T), intent(in) :: threadnum
     end subroutine for_int_buf_readelement

     subroutine for_int_buf_closefile(unit) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: unit
     end subroutine for_int_buf_closefile

     subroutine for_int_buf_removefile(unit) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: unit 
     end subroutine for_int_buf_removefile

     subroutine for_int_buf_removepool(pool_id ) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: pool_id
     end subroutine for_int_buf_removepool

     subroutine for_int_buf_closepool(pool_id) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: pool_id
     end subroutine for_int_buf_closepool

     subroutine for_int_buf_flushfile(unit) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: unit
     end subroutine for_int_buf_flushfile

     subroutine for_int_buf_flushpool(pool_id) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: pool_id
     end subroutine for_int_buf_flushpool

     subroutine for_int_buf_syncfile(unit) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: unit
     end subroutine for_int_buf_syncfile

     subroutine for_int_buf_syncpool(pool_id) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: pool_id
     end subroutine for_int_buf_syncpool

     subroutine for_int_buf_writeblock(unit, block, vals, threadnum) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: unit
       integer(kind=C_INT64_T), intent(in) :: block
       integer(kind=C_INT64_T), intent(in) :: vals(*)
       integer(kind=C_INT64_T), intent(in) :: threadnum
     end subroutine for_int_buf_writeblock

     subroutine for_int_buf_writearray(unit, pos, N, vals, threadnum) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: unit
       integer(kind=C_INT64_T), intent(in) :: pos
       integer(kind=C_INT64_T), intent(in) :: N
       integer(kind=C_INT64_T), intent(in) :: vals(*)
       integer(kind=C_INT64_T), intent(in) :: threadnum
     end subroutine for_int_buf_writearray

     subroutine for_int_buf_atomic_add_array(unit, pos, N, vals, threadnum) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) ::unit
       integer(kind=C_INT64_T), intent(in) :: pos
       integer(kind=C_INT64_T), intent(in) :: N
       integer(kind=C_INT64_T), intent(in) :: vals(*)
       integer(kind=C_INT64_T), intent(in) :: threadnum
     end subroutine for_int_buf_atomic_add_array

     subroutine for_int_buf_readblock(unit, block, vals, threadnum) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: unit
       integer(kind=C_INT64_T), intent(in) :: block
       integer(kind=C_INT64_T), intent(in) :: vals(*)
       integer(kind=C_INT64_T), intent(in) :: threadnum
     end subroutine for_int_buf_readblock

     subroutine for_int_buf_readarray(unit, pos, N, vals, threadnum) bind(c)
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: unit
       integer(kind=C_INT64_T), intent(in) :: pos
       integer(kind=C_INT64_T), intent(in) :: N
       integer(kind=C_INT64_T), intent(in) :: vals(*)
       integer(kind=C_INT64_T), intent(in) :: threadnum
     end subroutine for_int_buf_readarray

     subroutine for_int_buf_openfile(pool_id, unit_num, filename, filename_length) &
          & bind(c,name='for_int_buf_openfile')
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: pool_id
       integer(kind=C_INT64_T), intent(in) :: unit_num
       character(kind=c_char), intent(in) :: filename(*)
       integer(kind=C_INT64_T), intent(in) :: filename_length
     end subroutine for_int_buf_openfile

     subroutine for_int_buf_changebuffersize( pool_id, maxmem) &
          & bind (c,name='for_int_buf_changebuffersize')
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: pool_id
       integer(kind=C_INT64_T), intent(in) :: maxmem
     end subroutine for_int_buf_changebuffersize

     subroutine for_int_buf_return_pointer( unit, block, threadnum) &
          & bind (c,name='for_int_buf_free_c_pointer')
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: unit
       integer(kind=C_INT64_T), intent(in) :: block
       integer(kind=C_INT64_T), intent(in) :: threadnum
     end subroutine for_int_buf_return_pointer
  end interface

contains

  ! Routine to call for direct memory pointer access, memory 
  ! managed by IOBuffer library 

  subroutine for_double_buf_get_constpointer( unit, block, vals, threadnum )

    implicit none

    interface
       subroutine for_double_buf_get_c_pointer( unit, block, p, threadnum, blocksize) &
            & bind (c,name='for_double_buf_get_c_pointer')
         use iso_c_binding
         implicit none
         integer(kind=C_INT64_T), intent(in) :: unit
         integer(kind=C_INT64_T), intent(in) :: block
         type(c_ptr), intent(out) :: p
         integer(kind=C_INT64_T), intent(in) :: threadnum
         integer(kind=C_INT64_T), intent(out) :: blocksize
       end subroutine for_double_buf_get_c_pointer
    end interface

    integer(kind=C_INT64_T), intent(in) :: unit
    integer(kind=C_INT64_T), intent(in) :: block

    real(kind=C_DOUBLE), intent(out), pointer:: vals(:)

    integer(kind=C_INT64_T), intent(in) :: threadnum

    integer(kind=C_INT64_T) blocksize
    type(c_ptr) :: cptr


    call for_double_buf_get_c_pointer( unit, block, cptr, threadnum, blocksize )

    call c_f_pointer(cptr, vals, [blocksize])

  end subroutine for_double_buf_get_constpointer


  ! Routine to call for direct memory pointer access, memory 
  ! managed by IOBuffer library 

  subroutine for_int_buf_get_constpointer( unit, block, vals, threadnum )

    implicit none

    interface
       subroutine for_int_buf_get_c_pointer( unit, block, p, threadnum, blocksize) &
            & bind (c,name='for_int_buf_get_c_pointer')
         use iso_c_binding
         implicit none
         integer(kind=C_INT64_T), intent(in) :: unit
         integer(kind=C_INT64_T), intent(in) :: block
         type(c_ptr), intent(out) :: p
         integer(kind=C_INT64_T), intent(in) :: threadnum
         integer(kind=C_INT64_T), intent(out) :: blocksize
       end subroutine for_int_buf_get_c_pointer
    end interface

    integer(kind=C_INT64_T), intent(in) :: unit
    integer(kind=C_INT64_T), intent(in) :: block

    integer(kind=C_INT64_T), intent(out), pointer:: vals(:)

    integer(kind=C_INT64_T), intent(in) :: threadnum

    integer(kind=C_INT64_T) blocksize
    type(c_ptr) :: cptr


    call for_int_buf_get_c_pointer( unit, block, cptr, threadnum, blocksize )

    call c_f_pointer(cptr, vals, [blocksize])

  end subroutine for_int_buf_get_constpointer

end module IOBuffer
