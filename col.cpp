#include <iostream>
#include <string>

const int COL_RANGE = (231 - 16) + 1;
const unsigned int FAC = 18776357;

unsigned int color_code(std::string &seed) {
  unsigned int sum = 0;

  for (int l = 0; l <= seed.length(); l++) {
    sum = sum + (seed[l] - '0');
  }

  // PRNG
  sum ^= FAC << 13;
  sum ^= FAC << 17;
  sum ^= FAC << 5;

  return (sum % COL_RANGE) + 16;
}

int main() {
  std::string s1 = std::string("1239129317278893");
  std::cout << color_code(s1) << std::endl;

  std::string s2 = std::string("1239129317271231");
  std::cout << color_code(s2) << std::endl;

  std::string s3 = std::string("1239123117271231");
  std::cout << color_code(s3) << std::endl;

  std::string s4 = std::string("2393238517271231");
  std::cout << color_code(s4) << std::endl;
}