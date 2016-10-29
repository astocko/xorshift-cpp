# xorshift-cpp

A wrapper for the splitmix64, xoroshiro128+, xorshift128+, and xorshift1024* PRNGs. Derived from their respective public-domain C implementations. 

See [LICENSE](LICENSE) for details.

# algorithms 

Please see [http://xoroshiro.di.unimi.it](http://xoroshiro.di.unimi.it/) for an overview of the PRNGs and their preferred use cases. For parallel simulations, xorshift1024* is recommended, otherwise xoroshiro128+. splitmix64 is convenient for seeding PRNG states.

# examples

[example.cc](example.cc)

```cpp
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
```

# license
[LICENSE](LICENSE)
