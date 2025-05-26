#include <cmath>
#include <immintrin.h> // For AVX2 intrinsics
#include <iostream>
#include <cstring>

bool hnsearch(const char *haystack, const char *needle)
{
  size_t haystack_len = strlen(haystack);
  size_t needle_len = strlen(needle);

  if (haystack_len < needle_len)
    return false;

  int badchar[256];
  for (int i = 0; i < 256; i++)
    badchar[i] = needle_len;
  for (size_t i = 0; i < needle_len - 1; i++)
    badchar[(unsigned char)needle[i]] = needle_len - 1 - i;

  char last_char = needle[needle_len - 1];
  __m256i last_char_vector = _mm256_set1_epi8(last_char);

  size_t i = needle_len - 1;

  while (i < haystack_len)
  {
    if (i + 32 <= haystack_len)
    {
      __m256i haystack_vector = _mm256_loadu_si256((__m256i *)(haystack + i - needle_len + 1));
      __m256i cmp_result = _mm256_cmpeq_epi8(haystack_vector, last_char_vector);
      int mask = _mm256_movemask_epi8(cmp_result);

      if (mask == 0)
      {
        i += 32;
        continue;
      }

      while (mask != 0)
      {
        int pos = __builtin_ctz(mask);
        size_t check_pos = i - needle_len + 1 + pos;

        if (check_pos + needle_len <= haystack_len)
        {
          bool match = true;
          for (size_t j = 0; j < needle_len; j++)
          {
            if (haystack[check_pos + j] != needle[j])
            {
              match = false;
              break;
            }
          }
          if (match)
            return true;
        }

        mask &= (mask - 1);
      }

      i += badchar[(unsigned char)haystack[i]];
    }
    else
    {
      size_t j = needle_len - 1;
      while (j < needle_len && haystack[i - (needle_len - 1 - j)] == needle[j])
        j--;

      if (j == SIZE_MAX) // full match
        return true;

      i += std::max<size_t>(1, badchar[(unsigned char)haystack[i]]);
    }
  }

  return false;
}

// #include <fstream>

// // #include "patterns.cpp"
// // const char* E_READY = "\"t\":\"READY\"";
// const char *pattern = "\"t\":\"READY\"";
// size_t pattern_len = strlen(pattern);

// int main()
// {
//   const char *guild_update = "{\"t\":\"READY\",\"s\":1,\"op\":0,\"d\":{\"_trace\"";
//   size_t data_len = strlen(guild_update);

//   bool found = hnsearch(guild_update, pattern);

//   std::cout << "Matched : " << found << std::endl;
// }