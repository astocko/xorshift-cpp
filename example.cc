#include <iostream>
#include <iterator>
#include <vector>

#include "xoro.hpp"
using namespace xorshift;

template<typename T>
std::ostream & operator<<(std::ostream & os, std::vector<T> vec)
{
  os<<"[ ";
  if (vec.size() != 0) {
    std::copy(vec.begin(), vec.end()-1, std::ostream_iterator<T>(os, ", "));
    os<<vec.back();
  }
  os<<" ]";
  return os;
}

int main() {

  SplitMix64 sm(0);
  std::cout << sm.Next() << std::endl;

  Rng<Xorshift1024> rng;
  auto vals = rng.RunWithSM(0, 20);

  return 0;
}
