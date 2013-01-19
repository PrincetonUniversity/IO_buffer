#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
#include <algorithm>

int main(int argc, char** argv){
  if (argc < 2){
    std::cerr << "Argument count too small, usage:\n binary_to_ascii filename [nblock]\n";
    return -1;
  }
  std::ifstream ifs(argv[1]);
  size_t block_size = 1;
  if (argc > 2){
    std::istringstream iss(argv[1]);
    iss >> block_size;
  }

  if (block_size == 0){
    std::cerr << "Argument count too small, usage:\n binary_to_ascii filename [nblock]\n";
    return -1;
  }

  while (ifs.good()){
    std::vector<double> data(block_size);
    ifs.read((char*) &data[0], block_size * sizeof(double));
    std::for_each(data.begin(),data.end(),[](double d){std::cout << d << ' ';});
    std::cout << '\n';
  }
}
