module IOBuffer

  use iso_c_binding

  interface
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

     subroutine for_double_buf_syncpool( pool_id ) &
          & bind (c,name='for_double_buf_syncpool')
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in):: pool_id
     end subroutine for_double_buf_syncpool

     subroutine for_int_buf_construct(maxmem, blocksize, &
          & storagepolicy, nthread, pool_id) &
          & bind(c,name='for_int_buf_construct')
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in) :: maxmem
       integer(kind=C_INT64_T), intent(in) :: blocksize
       integer(kind=C_INT64_T), intent(in) :: storagepolicy
       integer(kind=C_INT64_T), intent(in) :: nthread
       integer(kind=C_INT64_T), intent(out) :: pool_id       
     end subroutine for_int_buf_construct

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

     subroutine for_int_buf_syncpool( pool_id ) &
          & bind (c,name='for_int_buf_syncpool')
       use iso_c_binding
       implicit none
       integer(kind=C_INT64_T), intent(in):: pool_id
     end subroutine for_int_buf_syncpool

  end interface

  contains

    ! Routine to call for direct memory pointer access, memory 
    ! managed by IOBuffer library 

     subroutine for_double_buf_get_constpointer( unit, block, values, threadnum )

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

       real(kind=C_DOUBLE), intent(out), pointer:: values(:)

       integer(kind=C_INT64_T), intent(in) :: threadnum

       integer(kind=C_INT64_T) blocksize
       type(c_ptr) :: cptr


       call for_double_buf_get_c_pointer( unit, block, cptr, threadnum, blocksize )

       call c_f_pointer(cptr, values, [blocksize])
       
     end subroutine for_double_buf_get_constpointer


    ! Routine to call for direct memory pointer access, memory 
    ! managed by IOBuffer library 

     subroutine for_int_buf_get_constpointer( unit, block, values, threadnum )

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

       integer(kind=C_INT64_T), intent(out), pointer:: values(:)

       integer(kind=C_INT64_T), intent(in) :: threadnum

       integer(kind=C_INT64_T) blocksize
       type(c_ptr) :: cptr


       call for_int_buf_get_c_pointer( unit, block, cptr, threadnum, blocksize )

       call c_f_pointer(cptr, values, [blocksize])
       
     end subroutine for_int_buf_get_constpointer

end module IOBuffer
