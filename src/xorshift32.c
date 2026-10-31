/*
 * Copyright (C) 2026 TheCRatTime
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <xorshift32.h>
#include <types.h>

#include <stddef.h>

#define X32S_ROTATE(num) (((num) >> 16) | ((num) << 16))

static X32S_u32 DoXorShift32(X32S_u32 seed) {
  if (seed == 0) {
    seed = 0x811C9DC5;
  }
  
  seed ^= seed << 13;
  seed ^= seed >> 17;
  seed ^= seed << 5;
  
  return seed;
}

X32S_u32 GetXor32Sum(const char* data, size_t len) {
  X32S_u32 hash = 0xBADA110C;
  X32S_u32 char_hash;
  size_t i = 0;

  for (; i < len; i++) {
    char_hash = DoXorShift32(data[i]);
    char_hash = X32S_ROTATE(char_hash);
    
    hash ^= data[i];

    hash ^= ~char_hash;

    hash = DoXorShift32(hash);
  }

  hash >>= 5;
  hash = DoXorShift32(hash);
  hash = X32S_ROTATE(hash);
  hash <<= 5;
  hash = DoXorShift32(hash);

  return hash;
}
