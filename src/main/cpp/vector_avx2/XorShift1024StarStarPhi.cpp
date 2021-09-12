/*
 * Copyright 2021 Stefan Zobel
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

#include "vcl/vectorclass.h"
#include <jni.h>


// XorShift1024StarStar generator from:
// Sebastiano Vigna (2016): An experimental exploration of Marsaglia’s xorshift generators, scrambled
// https://arxiv.org/pdf/1402.6246.pdf
// (this one use a better multiplier constant which eliminates linearity from one of the lowest bits)


constexpr int MM_HINT_T0 = 1;
// Fetch into all levels of the cache hierarchy
#if defined (_WIN64) || defined (_WIN32)
#define PREFETCH(address) (_mm_prefetch((const char*) (address), MM_HINT_T0))
#else
#define PREFETCH(address) (_mm_prefetch((address), MM_HINT_T0))
#endif


// cuurent position
static int pos;
// state
static Vec8uq s[16];


static void next8Longs(uint64_t* r0, uint64_t* r1, uint64_t* r2, uint64_t* r3, uint64_t* r4, uint64_t* r5, uint64_t* r6, uint64_t* r7) {
    Vec8uq s0 = s[pos];
    Vec8uq s1 = s[pos = (pos + 1) & 15];

    s1 ^= (s1 << 31);
    Vec8uq t = s1 ^ s0 ^ (s1 >> 11) ^ (s0 >> 30);
    s[pos] = t;
    Vec8uq r = t * 0x9e3779b97f4a7c13;

    *r0 = r[0];
    *r1 = r[1];
    *r2 = r[2];
    *r3 = r[3];
    *r4 = r[4];
    *r5 = r[5];
    *r6 = r[6];
    *r7 = r[7];
}

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     net_cramer_simd_RNG
 * Method:    xor1024Large
 * Signature: ([J)V
 */
JNIEXPORT void JNICALL Java_net_cramer_simd_RNG_xor1024Large
(JNIEnv* env, jclass, jlongArray array) {
    // array must have length 2048 as we retrieve 2K numbers on each call
    const int SIZE = 8 * 256;
    jboolean copy = JNI_FALSE;
    uint64_t* r = static_cast<uint64_t*>(env->GetPrimitiveArrayCritical(array, &copy));
    PREFETCH(r + SIZE - 8);
    for (int i = 0; i < SIZE; i += 8) {
        next8Longs(&r[i + 0], &r[i + 1], &r[i + 2], &r[i + 3], &r[i + 4], &r[i + 5], &r[i + 6], &r[i + 7]);
    }
    env->ReleasePrimitiveArrayCritical(array, r, 0);
}

/*
 * Class:     net_cramer_simd_RNG
 * Method:    initXor1024
 * Signature: ([J)V
 */
JNIEXPORT void JNICALL Java_net_cramer_simd_RNG_initXor1024
(JNIEnv* env, jclass, jlongArray array) {
    // we need exactly 8 * 16 seed values
    jboolean copy = JNI_FALSE;
    uint64_t* vals = static_cast<uint64_t*>(env->GetPrimitiveArrayCritical(array, &copy));
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 16; ++col) {
            s[col].insert(row, *vals++);
        }
    }
    env->ReleasePrimitiveArrayCritical(array, vals, 0);
}
#ifdef __cplusplus
}
#endif
