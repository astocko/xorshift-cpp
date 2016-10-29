#pragma once

#include <algorithm>
#include <array>
#include <vector>

namespace xorshift {

// Wrapper class for http://xoroshiro.di.unimi.it/splitmix64.c
class SplitMix64 {
 private:
  uint64_t x; /* The state can be seeded with any value. */

 public:
  explicit SplitMix64(uint64_t seed) : x(seed) {}
  explicit SplitMix64(std::array<uint64_t, 1> &seed) : x(seed[0]) {}

  uint64_t Next() {
    uint64_t z = (x += static_cast<uint64_t>(0x9E3779B97F4A7C15));
    z = (z ^ (z >> 30)) * static_cast<uint64_t>(0xBF58476D1CE4E5B9);
    z = (z ^ (z >> 27)) * static_cast<uint64_t>(0x94D049BB133111EB);
    return z ^ (z >> 31);
  }

  static const size_t STATE_SIZE = 1;
};

// Wrapper class for http://xoroshiro.di.unimi.it/xoroshiro128plus.c
class Xoroshiro128 {
 private:
  std::array<uint64_t, 2> s;

  static inline uint64_t Rotl(const uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
  }

 public:
  explicit Xoroshiro128(std::array<uint64_t, 2> &seed) : s(seed) {}

  template <typename T>
  static Xoroshiro128 SeedFromRng(T &rng) {
    std::array<uint64_t, 2> a;
    std::generate(a.begin(), a.end(), [rng]() mutable { return rng.Next(); });
    return Xoroshiro128(a);
  }

  uint64_t Next(void) {
    const uint64_t s0 = s[0];
    uint64_t s1 = s[1];
    const uint64_t result = s0 + s1;

    s1 ^= s0;
    s[0] = Rotl(s0, 55) ^ s1 ^ (s1 << 14);  // a, b
    s[1] = Rotl(s1, 36);                    // c

    return result;
  }

  void Jump() {
    static const std::array<uint64_t, 2> JUMP = {0xbeac0467eba5facb,
                                                 0xd86b048b86aa9922};
    uint64_t s0 = 0;
    uint64_t s1 = 0;

    for (size_t i = 0; i < JUMP.size(); i++) {
      for (auto b = 0; b < 64; b++) {
        if (JUMP[i] & 1ULL << b) {
          s0 ^= s[0];
          s1 ^= s[1];
        }
        Next();
      }
    }

    s[0] = s0;
    s[1] = s1;
  }

  static const size_t STATE_SIZE = 2;
};

// Wrapper class for http://xoroshiro.di.unimi.it/xorshift128plus.c
class Xorshift128 {
 private:
  std::array<uint64_t, 2> s;

 public:
  explicit Xorshift128(std::array<uint64_t, 2> &seed) : s(seed) {}

  template <typename T>
  static Xorshift128 SeedFromRng(T &rng) {
    std::array<uint64_t, 2> a;
    std::generate(a.begin(), a.end(), [rng]() mutable { return rng.Next(); });
    return Xorshift128(a);
  }

  uint64_t Next(void) {
    uint64_t s1 = s[0];
    const uint64_t s0 = s[1];
    const uint64_t result = s0 + s1;
    s[0] = s0;
    s1 ^= s1 << 23;                           // a
    s[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5);  // b, c
    return result;
  }

  void Jump() {
    static const std::array<uint64_t, 2> JUMP = {0x8a5cd789635d2dff,
                                                 0x121fd2155c472f96};
    uint64_t s0 = 0;
    uint64_t s1 = 0;

    for (size_t i = 0; i < JUMP.size(); i++) {
      for (auto b = 0; b < 64; b++) {
        if (JUMP[i] & 1ULL << b) {
          s0 ^= s[0];
          s1 ^= s[1];
        }
        Next();
      }
    }

    s[0] = s0;
    s[1] = s1;
  }

  static const size_t STATE_SIZE = 2;
};

// Wrapper class for http://xoroshiro.di.unimi.it/xorshift1024star.c
class Xorshift1024 {
 private:
  std::array<uint64_t, 16> s;
  int p = 0;

 public:
  explicit Xorshift1024(std::array<uint64_t, 16> &seed) : s(seed), p(0) {}

  template <typename T>
  static Xorshift1024 SeedFromRng(T &rng) {
    std::array<uint64_t, 16> a;
    std::generate(a.begin(), a.end(), [rng]() mutable { return rng.Next(); });
    return Xorshift1024(a);
  }

  uint64_t Next(void) {
    const uint64_t s0 = s[p];
    uint64_t s1 = s[p = (p + 1) & 15];
    s1 ^= s1 << 31;                            // a
    s[p] = s1 ^ s0 ^ (s1 >> 11) ^ (s0 >> 30);  // b,c
    return s[p] * static_cast<uint64_t>(1181783497276652981);
  }

  void Jump(void) {
    static const std::array<uint64_t, 16> JUMP = {
        0x84242f96eca9c41d, 0xa3c65b8776f96855, 0x5b34a39f070b5837,
        0x4489affce4f31a1e, 0x2ffeeb0a48316f40, 0xdc2d9891fe68c022,
        0x3659132bb12fea70, 0xaac17d8efa43cab8, 0xc4cb815590989b13,
        0x5ee975283d71c93b, 0x691548c86c1bd540, 0x7910c41d10a1e6a5,
        0x0b5fc64563b3e2a8, 0x047f7684e9fc949d, 0xb99181f2d8f685ca,
        0x284600e3f30e38c3};

    uint64_t t[16] = {0};
    for (size_t i = 0; i < JUMP.size(); i++) {
      for (auto b = 0; b < 64; b++) {
        if (JUMP[i] & 1ULL << b) {
          for (int j = 0; j < 16; j++) t[j] ^= s[(j + p) & 15];
        }
        Next();
      }
    }

    for (int j = 0; j < 16; j++) s[(j + p) & 15] = t[j];
  }

  static const size_t STATE_SIZE = 16;
};

template <typename T>
T SeedWithSm64(uint64_t seed) {
  SplitMix64 sm(seed);
  return T::SeedFromRng(sm);
}

}  // namespace xorshift
