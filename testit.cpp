#include "buffer.hpp"
#include "buffer.tpp"
#include "mapper.hpp"
#include "LiFo_policy.hpp"

int main(int argc, char** argv){

  const size_t N=50;

  std::shared_ptr<policy_LiLo<double> > pl;

  std::weak_ptr<mapper<double> > b1 = mapper<double>::factory("buffer1.dat",1,pl);
  std::weak_ptr<mapper<double> > b2 = mapper<double>::factory("buffer2.dat",5,pl);
  std::weak_ptr<mapper<double> > b3 = mapper<double>::factory("buffer3.dat",52,pl);

  for (size_t i = 0; i < 7285; ++i){
    std::cerr <<"setting "<<i<<' ';
    b1.lock()->set(i, 0.852*i);
  }
  
  std::cout << b1.lock()->get(358) << '\n';
  std::cout << b1.lock()->get(871) << '\n';
  std::cout << b1.lock()->get(872) << '\n';

}
