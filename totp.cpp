#include <cctype>
#include <cstring>
#include <ctime>
// #include <iostream>
#include <openssl/hmac.h>
#include <openssl/rand.h>
#include <stdexcept>
#include <string>
#include <vector>

// Base32 decode (simple, assumes uppercase, no padding)
std::vector<uint8_t> base32_decode(const std::string &encoded) {
  const std::string base32_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
  std::vector<uint8_t> bytes;
  int buffer = 0, bitsLeft = 0;
  for (char c : encoded) {
    if (c == '=')
      break;
    int val = base32_chars.find(toupper(c));
    if (val == std::string::npos)
      continue;
    buffer = (buffer << 5) | val;
    bitsLeft += 5;
    if (bitsLeft >= 8) {
      bitsLeft -= 8;
      bytes.push_back((buffer >> bitsLeft) & 0xFF);
    }
  }
  return bytes;
}
std::string base32_encode(const unsigned char *data, size_t length) {
  const char *base32_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
  std::string result;
  int buffer = data[0], bitsLeft = 8;
  int index = 1;
  while (bitsLeft > 0 || index < length) {
    if (bitsLeft < 5) {
      if (index < length) {
        buffer <<= 8;
        buffer |= data[index++] & 0xFF;
        bitsLeft += 8;
      } else {
        int pad = 5 - bitsLeft;
        buffer <<= pad;
        bitsLeft += pad;
      }
    }
    int val = (buffer >> (bitsLeft - 5)) & 0x1F;
    bitsLeft -= 5;
    result += base32_chars[val];
  }
  return result;
}

std::string generate_base32_secret(int bytes_len = 20) {
  unsigned char buf[bytes_len];
  if (!RAND_bytes(buf, bytes_len)) {
    throw std::runtime_error("Random generation failed");
  }
  return base32_encode(buf, bytes_len);
}

uint32_t generate_totp(const std::string &base32_secret,
                       uint64_t time_step = 30) {
  auto key = base32_decode(base32_secret);
  uint64_t epoch = std::time(nullptr) / time_step;

  unsigned char msg[8];
  for (int i = 7; i >= 0; --i) {
    msg[i] = epoch & 0xFF;
    epoch >>= 8;
  }

  unsigned char *hash =
      HMAC(EVP_sha1(), key.data(), key.size(), msg, 8, nullptr, nullptr);
  int offset = hash[19] & 0x0F;
  uint32_t bin_code =
      ((hash[offset] & 0x7F) << 24) | ((hash[offset + 1] & 0xFF) << 16) |
      ((hash[offset + 2] & 0xFF) << 8) | (hash[offset + 3] & 0xFF);
  return bin_code % 1000000;
}

// int main(int argc, char **argv) {
//   if (argv[1] && strlen(argv[1]) == 32) {
//     uint32_t totp = generate_totp(argv[1]);

//     std::cout << totp << std::endl;
//   } else {
//     std::string randsec = generate_base32_secret();

//     std::cout << "Secret: " << randsec << std::endl;

//     uint32_t totp = generate_totp(randsec);
//     std::cout << "OTP: " << totp << std::endl;
//   }
//   return 0;
// }
