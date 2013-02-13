#include "buffer.hpp"
#include "mapper.hpp"
#include "LiFo_policy.hpp"
#include "fortranAPI_handler.hpp"
#include "fortran_api.hpp"

#include <thread>
#include <atomic>

volatile double finalsum;
std::mutex finalsummutex;

void do_work( size_t threadnum, size_t nthread, size_t RAMsize ){

  double sum(0.);
  
  for (size_t i = threadnum; i < RAMsize; i += nthread){
    for_double_buf_writeelement_(49,i,0.5892*(i%20),threadnum);
  }

  for (size_t i = threadnum; i < RAMsize; i += nthread){
    double value(i);
        for_double_buf_readelement_(49,i,value,threadnum);
    sum += value; 
  }

  for (size_t i = threadnum; i < RAMsize; i += nthread){
        for_double_buf_writeelement_(49,i,0.5892*(i%20),threadnum);
  }

  for (size_t i = threadnum; i < RAMsize; i += nthread){
    double value(i);
        for_double_buf_readelement_(49,i,value,threadnum);
    sum -= value; 
  }

  finalsummutex.lock();
  finalsum += sum;
  finalsummutex.unlock();
}

void test_parallelization(size_t nthread, size_t Nchunk ){

  FINT pool_id;

  const size_t chunksize = 125000; // 1MB chunks
  const size_t RAMsize= Nchunk * chunksize;

  for_double_buf_construct_(Nchunk,chunksize,0,nthread,pool_id);

  for_double_buf_openfile_(pool_id, 49, "buf_F2.dat",10);

  finalsum = 0.;

  std::vector<std::thread*> threads(nthread);

  for (size_t ithread=0; ithread < nthread; ++ithread){
    threads[ithread] = new std::thread(do_work,ithread+1,nthread,RAMsize); 
  }

  for (size_t ithread=0; ithread < nthread; ++ithread){
    threads[ithread]->join();
  }  
  std::cout << "threads complete" << '\n';

  std::cout << finalsum << '\n';
  std::cout << "Prepare to remove" << '\n';

  for_double_buf_removepool_(pool_id);
}

void test_fortranapi(size_t Nchunk){
  FINT pool_id;

  const size_t chunksize = 125000; // 1MB chunks
  const size_t RAMsize= Nchunk * chunksize;

  for_double_buf_construct_(Nchunk,chunksize,0,1,pool_id);

  for_double_buf_openfile_(pool_id, 48, "buf_F1.dat",10);

  double sum(0);
  double value;

  for (size_t i = 0; i < RAMsize; ++i){
    for_double_buf_writeelement_(48,i,0.5892*(i%20),1);
  }
  for (size_t i = 0; i < RAMsize; ++i){
    for_double_buf_readelement_(48,i,value,1);
    sum += value; 
  }
  for (size_t i = 0; i < RAMsize; ++i){
    for_double_buf_writeelement_(48,i,0.5892*(i%20),1);
  }
  for (size_t i = 0; i < RAMsize; ++i){
    for_double_buf_readelement_(48,i,value,1);
    sum -= value; 
  }

  for_double_buf_readelement_(48,2583,value,1);

  if (value != 0.5892*(2583%20))
    std::cerr << "ERROR: Elemenct 2583 is wrong!\n";

  std::cout << sum << '\n';

  for_double_buf_removepool_(pool_id);
}

void test_policy(){

  const size_t N=50;

  std::shared_ptr<policy_LiLo<double> > pl(new policy_LiLo<double>(10,N,1));

  std::weak_ptr<mapper<double> > b1 = mapper<double>::factory("buffer1.dat",1,pl);
  std::weak_ptr<mapper<double> > b2 = mapper<double>::factory("buffer2.dat",5,pl);
  std::weak_ptr<mapper<double> > b3 = mapper<double>::factory("buffer3.dat",52,pl);

  if (!b1.lock())
    std::cerr << "ERROR, mapper does not exist\n";

  for (size_t i = 0; i < 7285; ++i){
    b1.lock()->set(i, 0.852*i,0);
  }
  
  if (b1.lock()->get(358,0) != 305.016)
    std::cerr << "ERROR: b1get gives wrong value at 358";

  if (b1.lock()->get(871,0) != 742.092)
    std::cerr << "ERROR: b1get gives wrong value at 871";

  if (b1.lock()->get(872,0) != 742.944)
    std::cerr << "ERROR: b1get gives wrong value " << b1.lock()->get(872,0)<< " at 872";

  pl.reset();

  if (b1.lock())
    std::cerr << "ERROR: b1 not properly cleaned up";

  std::cout << "If no \"ERROR\" above, then test stage 1 is ok\n";
}

bool enter_number(size_t& n, const char* input, const char* message){
  std::istringstream iss(input);

  iss >> n;
 
  char answer('a');

  do {
    std::cout << n << ' ' << message << ", ok?\n";

    std::cin >> answer;

    if (answer == 'n')
      return false;

    if (answer != 'y')
      std::cout << "Please answer y or n\n";

  } while (answer != 'y');
      
  return true;
}

int main(int argc, char** argv){

  test_policy();

  size_t Nchunks(10);
  size_t Nthreads(0);
	      
  if (argc >= 2){
    if (!enter_number(Nchunks,argv[1],"MB will be processed"))
      return 0;
  }
  if (argc >= 3){
    if (!enter_number(Nthreads,argv[2],"Threads will be launched"))
      return 0;
  }

  if (Nthreads==0){
    std::cout << "Launching with " << Nchunks << " chunks and no threads\n"; 
    test_fortranapi(Nchunks);
  }else{
    std::cout << "Launching with " << Nchunks << " chunks and " << Nthreads << " threads\n"; 
    test_parallelization(Nthreads, Nchunks); 
  }
}
