#include "buffer.hpp"
#include "buffer.tpp"
#include "mapper.hpp"
#include "LiFo_policy.hpp"
#include "fortranAPI_handler.hpp"
#include "fortran_api.hpp"

void test_fortranapi(){
  FINT pool_id;
  for_buf_construct_(10000,50000,0,1,pool_id);

  for_buf_openfile_(pool_id, 48, "buf_F1.dat",10);

  for (size_t i = 0; i < 2928592; ++i){
    for_buf_writeelement_(48,i,1.281*i*i+0.5892*i,1);
  }

  double value;

  for_buf_readelement_(48,2583,value,1);

  if (value != 1.281*2583*2583+0.5892*2583)
    std::cerr << "ERROR: Elemenct 2583 is wrong!\n";

  for_buf_removefile_(48);
}

void test_policy(){

  const size_t N=50;

  std::shared_ptr<policy_LiLo<double> > pl(new policy_LiLo<double>(10,N));

  std::weak_ptr<mapper<double> > b1 = mapper<double>::factory("buffer1.dat",1,pl);
  std::weak_ptr<mapper<double> > b2 = mapper<double>::factory("buffer2.dat",5,pl);
  std::weak_ptr<mapper<double> > b3 = mapper<double>::factory("buffer3.dat",52,pl);

  if (!b1.lock())
    std::cerr << "ERROR, mapper does not exist\n";

  for (size_t i = 0; i < 7285; ++i){
    b1.lock()->set(i, 0.852*i,1);
  }
  
  if (b1.lock()->get(358,1) != 305.016)
    std::cerr << "ERROR: b1get gives wrong value at 358";

  if (b1.lock()->get(871,1) != 742.092)
    std::cerr << "ERROR: b1get gives wrong value at 871";

  if (b1.lock()->get(872,1) != 742.944)
    std::cerr << "ERROR: b1get gives wrong value " << b1.lock()->get(872,1)<< " at 872";

  pl.reset();

  if (b1.lock())
    std::cerr << "ERROR: b1 not properly cleaned up";

  std::cout << "If no \"ERROR\" above, then test stage 1 is ok\n";
}

int main(int, char**){

  test_policy();
	      
  test_fortranapi();

}
