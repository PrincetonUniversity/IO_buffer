#include "buffer.hpp"
#include "buffer.tpp"
#include "mapper.hpp"

int main(int argc, char** argv){

  const size_t N=50;

  mapper<double,N> maps("buffer.dat");

  for (size_t i = 0; i < 7285; ++i){
    std::cerr <<"setting "<<i<<' ';
    maps.set(i, 0.852*i);
  }
  
  std::cout << maps.get(358) << '\n';
  std::cout << maps.get(871) << '\n';
  std::cout << maps.get(872) << '\n';

}
