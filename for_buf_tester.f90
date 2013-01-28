program for_buf_tester

use omp_lib

implicit none

real(kind=8),dimension(:,:),allocatable::testmat
integer::numThreadsWrite
integer::numThreadsRead
integer::threadID
integer::dim1,dim2
integer::blockSize
integer::numBlocks
integer::i,j,c,iostat
integer::for_buf_poolID
real(kind=8)::tmp
integer(kind=4),parameter::seed=86456
integer,parameter::unitInp=20
integer,parameter::unitFile=22

! setup the calculation
open(unit=unitInp,status="old",action="read",file="forbuf.inp")
read(unitInp,*) numThreadsWrite
read(unitInp,*) numThreadsRead
read(unitInp,*) dim1
read(unitInp,*) dim2
read(unitInp,*) blockSize
read(unitInp,*) numBlocks
close(unitInp)

write(*,*) "Configuration for this test:"
write(*,*) "Threads for writing ",numThreadsWrite
write(*,*) "Threads for reading ",numThreadsRead
write(*,*) "Dimesions of matrix ",dim1,dim2
write(*,*) "Buffer blocksize ",blockSize
write(*,*) "Buffer number of blocks ",numBlocks
flush(6)

! initialize our matrix with random numbers
call srand(seed)
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

! initialize our for_buf
call for_buf_construct(numBlocks,blockSize,0,max(numThreadsRead,numThreadsWrite),for_buf_poolID)
call for_buf_openfile(for_buf_poolID,'forbuf.dat',unitFile)

! write them out
call omp_set_num_threads(numThreadsWrite)

!$omp parallel &
!$omp default(none) &
!$omp private(c,threadID) &
!$omp shared(testmat,dim1,dim2)
!$omp do schedule(static)
do j=1,dim2
   c = j*dim1
   threadID = OMP_get_thread_num()+1
   do i=1,dim1
      ! i am aware that this is really stupid. but so is tiger.
      call for_buf_writeElement(unitFile,c,testmat(i,j),threadID)
      c = c + 1
   enddo
enddo
!$omp end do nowait
!$omp end parallel

! synchronize stuff to disk for posteriori analysis
call for_buf_flushpool(for_buf_poolID)

! read in and compare to the stored data
call omp_set_num_threads(numThreadsRead)

!$omp parallel &
!$omp default(none) &
!$omp private(c,threadID,tmp) &
!$omp shared(testmat,dim1,dim2)
!$omp do schedule(static)
do j=1,dim2
   c = j*dim1
   threadID = OMP_get_thread_num()+1
   do i=1,dim1
      ! i am aware that this is really stupid. but so is tiger.
      call for_buf_readElement(unitFile,c,tmp,threadID)
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

! close the for buf
call for_buf_closepool(for_buf_poolID)

deallocate(testmat,stat=iostat)
if(iostat .ne. 0) then
   write(*,*) "ERROR: IOStatus in deallocate not zero. ",iostat
   flush(6)
   stop 
endif

end program for_buf_tester