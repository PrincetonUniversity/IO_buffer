module IOBuffer

  use iso_c_binding

  interface
     subroutine for_double_buf_construct(maxmem, blocksize, &
          & storagepolicy, nthread, pool_id) &
          & bind(c,name='for_double_buf_construct')
       integer*8, intent(in) :: maxmem
       integer*8, intent(in) :: blocksize
       integer*8, intent(in) :: storagepolicy
       integer*8, intent(in) :: nthread

       integer*8, intent(out) :: pool_id       
     end subroutine for_double_buf_construct

     subroutine for_double_buf_changebuffersize( pool_id, maxmem) &
          & bind (c,name='for_double_buf_changebuffersize')
       integer*8, intent(in) :: pool_id
       integer*8, intent(in) :: maxmem
     end subroutine for_double_buf_changebuffersize

     subroutine for_double_buf_return_pointer( unit, block, threadnum) &
          & bind (c,name='for_double_buf_free_c_pointer')
       integer*8, intent(in) :: unit
       integer*8, intent(in) :: block
       integer*8, intent(in) :: threadnum
     end subroutine for_double_buf_return_pointer

     subroutine for_double_buf_syncpool( pool_id ) &
          & bind (c,name='for_double_buf_syncpool')
       integer*8, intent(in):: pool_id
     end subroutine for_double_buf_syncpool

     subroutine for_int_buf_construct(maxmem, blocksize, &
          & storagepolicy, nthread, pool_id) &
          & bind(c,name='for_int_buf_construct')
       integer*8, intent(in) :: maxmem
       integer*8, intent(in) :: blocksize
       integer*8, intent(in) :: storagepolicy
       integer*8, intent(in) :: nthread

       integer*8, intent(out) :: pool_id       
     end subroutine for_int_buf_construct

     subroutine for_int_buf_changebuffersize( pool_id, maxmem) &
          & bind (c,name='for_int_buf_changebuffersize')
       integer*8, intent(in) :: pool_id
       integer*8, intent(in) :: maxmem
     end subroutine for_int_buf_changebuffersize

     subroutine for_int_buf_return_pointer( unit, block, threadnum) &
          & bind (c,name='for_int_buf_free_c_pointer')
       integer*8, intent(in) :: unit
       integer*8, intent(in) :: block
       integer*8, intent(in) :: threadnum
     end subroutine for_int_buf_return_pointer

     subroutine for_int_buf_syncpool( pool_id ) &
          & bind (c,name='for_int_buf_syncpool')
       integer*8, intent(in):: pool_id
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
            import :: c_ptr
            integer*8, intent(in) :: unit
            integer*8, intent(in) :: block
            type(c_ptr), intent(out) :: p
            integer*8, intent(in) :: threadnum
            integer*8, intent(out) :: blocksize
          end subroutine for_double_buf_get_c_pointer
       end interface

       integer*8, intent(in) :: unit
       integer*8, intent(in) :: block

       real*8, intent(out), pointer:: values(:)

       integer*8, intent(in) :: threadnum

       integer*8 blocksize
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
            import :: c_ptr
            integer*8, intent(in) :: unit
            integer*8, intent(in) :: block
            type(c_ptr), intent(out) :: p
            integer*8, intent(in) :: threadnum
            integer*8, intent(out) :: blocksize
          end subroutine for_int_buf_get_c_pointer
       end interface

       integer*8, intent(in) :: unit
       integer*8, intent(in) :: block

       integer*8, intent(out), pointer:: values(:)

       integer*8, intent(in) :: threadnum

       integer*8 blocksize
       type(c_ptr) :: cptr


       call for_int_buf_get_c_pointer( unit, block, cptr, threadnum, blocksize )

       call c_f_pointer(cptr, values, [blocksize])
       
     end subroutine for_int_buf_get_constpointer

end module IOBuffer
