#include <algorithm>
#include <chrono>
#include <iostream>
#include <iterator>
#include <vector>

#include "xorshift.hpp"

namespace xs = xorshift;

template <typename T>
std::ostream& operator<<(std::ostream& os, std::vector<T> vec) {
  os << "[ ";
  if (vec.size() != 0) {
    std::copy(vec.begin(), vec.end() - 1, std::ostream_iterator<T>(os, ", "));
    os << vec.back();
  }
  os << " ]";
  return os;
}

int main() {
  auto now = [](){
    return std::chrono::system_clock::now().time_since_epoch().count();
  };

  // splitmix64
  xs::SplitMix64 sm64(now());
  std::cout << "SplitMix64: " << sm64.Next() << std::endl;

  // xorshift128
  std::array<uint64_t, xs::Xorshift128::STATE_SIZE> state2;
  std::generate(state2.begin(), state2.end(), now);
  xs::Xorshift128 xs128(state2);
  std::cout << "Xorshift128: " << xs128.Next() << std::endl;

  // xoroshiro128
  std::array<uint64_t, 2> state1;
  state1[0] = now();
  state1[1] = now();
  xs::Xoroshiro128 xoro(state1);
  std::vector<uint64_t> v(20);
  std::cout << "Xoroshiro128: " << xoro.Next() << std::endl;

  // seed splitmix64 with timestamp and then seed xorshift1024 with splitmix64
  auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  auto xs1024 = xs::SeedWithSm64<xs::Xorshift1024>(seed);
  std::vector<uint64_t> vals(20);
  std::generate(vals.begin(), vals.end(),
                [xs1024]() mutable {return xs1024.Next();});
  std::cout << "Xorshift1024: " << vals << std::endl;
  return 0;
}
