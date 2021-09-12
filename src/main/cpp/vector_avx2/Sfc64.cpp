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


// Chris Doty-Humphrey's 256-bit "Small Fast Counting RNG" (sfc64)


constexpr int MM_HINT_T0 = 1;
// Fetch into all levels of the cache hierarchy
#if defined (_WIN64) || defined (_WIN32)
#define PREFETCH(address) (_mm_prefetch((const char*) (address), MM_HINT_T0))
#else
#define PREFETCH(address) (_mm_prefetch((address), MM_HINT_T0))
#endif


// sfc64 state
static Vec8uq a;
static Vec8uq b;
static Vec8uq c;
static Vec8uq counter = Vec8uq(1, 1, 1, 1, 1, 1, 1, 1);


static void next8Longs(uint64_t* r0, uint64_t* r1, uint64_t* r2, uint64_t* r3, uint64_t* r4, uint64_t* r5, uint64_t* r6, uint64_t* r7) {
    Vec8uq r = a + b + counter;
    counter += 1;
    a = b ^ (b >> 11);
    b = c + (c << 3);
    c = ((c << 24) | (c >> 40)) + r;

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
 * Method:    sfc64Large
 * Signature: ([J)V
 */
JNIEXPORT void JNICALL Java_net_cramer_simd_RNG_sfc64Large
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
 * Method:    initSfc64
 * Signature: ([J)J
 */
JNIEXPORT jlong JNICALL Java_net_cramer_simd_RNG_initSfc64
(JNIEnv* env, jclass, jlongArray array) {
    // we need exactly 8 distinct seed values
    jboolean copy = JNI_FALSE;
    uint64_t* vals = static_cast<uint64_t*>(env->GetPrimitiveArrayCritical(array, &copy));
    uint64_t val0 = vals[0];
    uint64_t val1 = vals[1];
    uint64_t val2 = vals[2];
    uint64_t val3 = vals[3];
    uint64_t val4 = vals[4];
    uint64_t val5 = vals[5];
    uint64_t val6 = vals[6];
    uint64_t val7 = vals[7];
    env->ReleasePrimitiveArrayCritical(array, vals, 0);
    a.insert(0, val0);
    a.insert(1, val1);
    a.insert(2, val2);
    a.insert(3, val3);
    a.insert(4, val4);
    a.insert(5, val5);
    a.insert(6, val6);
    a.insert(7, val7);
    b.insert(0, val0);
    b.insert(1, val1);
    b.insert(2, val2);
    b.insert(3, val3);
    b.insert(4, val4);
    b.insert(5, val5);
    b.insert(6, val6);
    b.insert(7, val7);
    c.insert(0, val0);
    c.insert(1, val1);
    c.insert(2, val2);
    c.insert(3, val3);
    c.insert(4, val4);
    c.insert(5, val5);
    c.insert(6, val6);
    c.insert(7, val7);
    for (int i = 0; i < 12; ++i) {
        next8Longs(&val0, &val1, &val2, &val3, &val4, &val5, &val6, &val7);
    }
    // avoid dead-code elimination
    return val0 ^ val1 ^ val2 ^ val3 ^ val4 ^ val5 ^ val6 ^ val7;
}
#ifdef __cplusplus
}
#endif
