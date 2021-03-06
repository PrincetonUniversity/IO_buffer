program for_int_buf_tester

    use ifport
    use IOBuffer
use omp_lib

implicit none

integer (kind=8), dimension(:,:),allocatable::testmat
integer::numThreadsWrite
integer::numThreadsRead
integer::threadID
integer::dim1,dim2
integer::blockSize
integer::numBlocks
integer::i,j,c,iostat
integer::for_int_buf_poolID
integer (kind=8)::tmp
integer(kind=4),parameter::myseed=86456
integer(kind=4),parameter::unitInp=200
integer(kind=4),parameter::unitFile=220

integer(kind=4) tmpfour

integer poolintid

! setup the calculation
open(unit=unitInp,status="old",action="read",file="forbuf.inp")
read(unitInp,*) numThreadsWrite
read(unitInp,*) numThreadsRead
read(unitInp,*) dim1
read(unitInp,*) dim2
read(unitInp,*) blockSize
read(unitInp,*) numBlocks
close(unit=unitInp) !please do not close file, causes segfault because POTATOE!

write(*,*) "Configuration for this test:"
write(*,*) "Threads for writing ",numThreadsWrite
write(*,*) "Threads for reading ",numThreadsRead
write(*,*) "Dimesions of matrix ",dim1,dim2
write(*,*) "Buffer blocksize ",blockSize
write(*,*) "Buffer number of blocks ",numBlocks
flush(6)

! initialize our matrix with random numbers
call srand(myseed)
allocate(testmat(dim1,dim2),stat=iostat)
if(iostat .ne. 0) then
   write(*,*) "ERROR: IOStatus in allocate not zero. ",iostat
   flush(6)
   stop 
endif
do j=1,dim2
   do i=1,dim1
      testmat(i,j)=rand()
   enddo
enddo

! initialize our for_int_buf
call for_int_buf_construct(numBlocks,blockSize,0,max(numThreadsRead,numThreadsWrite),for_int_buf_poolID)
write(*,*) "Constructed buffer with following settings:"
write(*,*) "Pool ID ",for_int_buf_poolID
write(*,*) "Maximum concurrency ",max(numThreadsRead,numThreadsWrite)
flush(6)
call for_int_buf_openfile(for_int_buf_poolID, unitFile, 'forbuf.dat',10)

! write them out
tmpfour = numThreadsWrite
call omp_set_num_threads(tmpfour)

!$omp parallel &
!$omp default(none) &
!$omp private(c,threadID,i) &
!$omp shared(testmat,dim1,dim2)
!$omp do schedule(static)
do j=1,dim2
   c = j*dim1
   threadID = OMP_get_thread_num()+1
   do i=1,dim1
      ! i am aware that this is really stupid. but so is tiger.
      call for_int_buf_writeElement(unitFile,c,testmat(i,j),threadID)
      c = c + 1
   enddo
enddo
!$omp end do nowait
!$omp end parallel


call for_int_buf_construct(100, 50,0, 1, poolintid);

! synchronize stuff to disk for posteriori analysis
call for_int_buf_flushpool(for_int_buf_poolID)

write(*,*) "Writing sweep complete. Reading back in..."
flush(6)

! read in and compare to the stored data
tmpfour = numThreadsRead
call omp_set_num_threads(tmpfour)

!$omp parallel &
!$omp default(none) &
!$omp private(c,threadID,tmp,i) &
!$omp shared(testmat,dim1,dim2)
!$omp do schedule(static)
do j=1,dim2
   c = j*dim1
   threadID = OMP_get_thread_num()+1
   do i=1,dim1
      ! i am aware that this is really stupid. but so is tiger.
      call for_int_buf_readElement(unitFile,c,tmp,threadID)
      if(tmp .ne. testmat(i,j)) then
         write(*,*) "ERROR: Mismatch a/b in i/j with threadID ",tmp,testmat(i,j),i,j,threadID
         flush(6)
         stop
      endif
      c = c + 1
   enddo
enddo
!$omp end do nowait
!$omp end parallel

call for_int_buf_openfile(poolintid, 93,"intbuf",6)
call for_int_buf_writeelement(93,52,133,1)
call for_int_buf_writeelement(93,53,133,1)
call for_int_buf_writeelement(93,54,133,1)
call for_int_buf_writeelement(93,55,133,1)
call for_int_buf_writeelement(93,56,133,1)

! close the for buf
!call for_int_buf_removepool(for_int_buf_poolID);

call for_int_buf_closepool(for_int_buf_poolID);

!call for_int_buf_removefile(unitFile)

deallocate(testmat,stat=iostat)
if(iostat .ne. 0) then
   write(*,*) "ERROR: IOStatus in deallocate not zero. ",iostat
   flush(6)
   stop 
endif

write(*,*) "Test successfully completed! :-)"
flush(6)

end program for_int_buf_tester
