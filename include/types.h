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

#ifndef XOR32SUM_TYPES_H_
#define XOR32SUM_TYPES_H_

#include <limits.h>

typedef unsigned char X32S_u8;

#if UINT_MAX == 4294967295

typedef unsigned int  X32S_u32;

#elif ULONG_MAX == 4294967295

typedef unsigned long X32S_u32;

#else
# error "Cannot define X32S_u32 type"
#endif

#endif /* XOR32SUM_TYPES_H_ */
