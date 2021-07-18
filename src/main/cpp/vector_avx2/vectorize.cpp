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

#include <float.h>           // DBL_MAX
#include <cmath>             // std::sqrt
#include <algorithm>         // std::max, std::min
#include "vcl/vectorclass.h"
#include <jni.h>

#ifndef DOUBLEARRAY_INCLUDED_
#include "DoubleArray.h"
#endif /* DOUBLEARRAY_INCLUDED_ */

#ifndef FLOATARRAY_INCLUDED_
#include "FloatArray.h"
#endif /* FLOATARRAY_INCLUDED_ */

#ifndef JEXCEPTION_INCLUDED_
#include "JException.h"
#endif /* JEXCEPTION_INCLUDED_ */

#ifndef JEXCEPTIONUTILS_INCLUDED_
#include "JExceptionUtils.h"
#endif /* JEXCEPTIONUTILS_INCLUDED_ */

constexpr double NOT_REACHED_D = -10000.0;
constexpr float NOT_REACHED_F = -10000.0f;
constexpr double DBL_MIN_VALUE = -DBL_MAX;
constexpr int STEP_8 = 8;
constexpr int STEP_16 = 16;
constexpr int MM_HINT_NTA = 0;
constexpr int MM_HINT_T0 = 1;
constexpr int MM_HINT_T1 = 2;

// TODO : should we have a version of the macro which does nothing?
#if defined (_WIN64) || defined (_WIN32)
// Windows version
// Fetch into all levels of the cache hierarchy
#define PREFETCH(address) (_mm_prefetch((const char*) (address), MM_HINT_T0))
#else
// Linux version
// Fetch into all levels of the cache hierarchy
#define PREFETCH(address) (_mm_prefetch((address), MM_HINT_T0))
#endif



float l2_norm_float(float* f, int64_t count);
double l2_norm_double(double* d, int64_t count);


#ifdef __cplusplus
extern "C" {
#endif
    /*
     * Class:     net_cramer_simd_SIMD
     * Method:    l2norm_double_n
     * Signature: ([DIZ)D
     */
    JNIEXPORT jdouble JNICALL Java_net_cramer_simd_SIMD_l2norm_1double_1n
    (JNIEnv* env, jclass, jdoubleArray array, jint count, jboolean useCrit) {
        if (count == 0 || array == nullptr) {
            return 0.0;
        }
        if (count < 0 || (count & 1) == 1) {
            throwJavaRuntimeException(env, "%s %d", "l2norm_double - invalid count argument:", count);
            return NOT_REACHED_D;
        }
        try {
            DoubleArray a = DoubleArray(env, array, count, useCrit);
            return l2_norm_double(a.ptr(), count);
        }
        catch (const JException& ex) {
            throwJavaRuntimeException(env, "%s %s", "l2norm_double", ex.what());
        }
        catch (...) {
            throwJavaRuntimeException(env, "%s", "l2norm_double: caught unknown exception");
        }
        return NOT_REACHED_D;
    }

    /*
     * Class:     net_cramer_simd_SIMD
     * Method:    l2norm_float_n
     * Signature: ([FIZ)F
     */
    JNIEXPORT jfloat JNICALL Java_net_cramer_simd_SIMD_l2norm_1float_1n
    (JNIEnv* env, jclass, jfloatArray array, jint count, jboolean useCrit) {
        if (count == 0 || array == nullptr) {
            return 0.0f;
        }
        if (count < 0 || (count & 1) == 1) {
            throwJavaRuntimeException(env, "%s %d", "l2norm_float - invalid count argument:", count);
            return NOT_REACHED_F;
        }
        try {
            FloatArray a = FloatArray(env, array, count, useCrit);
            return l2_norm_float(a.ptr(), count);
        }
        catch (const JException& ex) {
            throwJavaRuntimeException(env, "%s %s", "l2norm_float", ex.what());
        }
        catch (...) {
            throwJavaRuntimeException(env, "%s", "l2norm_float: caught unknown exception");
        }
        return NOT_REACHED_F;
    }

    /*
     * Class:     net_cramer_simd_SIMD
     * Method:    approx_equal_double_n
     * Signature: ([D[DIDDZ)Z
     */
    JNIEXPORT jboolean JNICALL Java_net_cramer_simd_SIMD_approx_1equal_1double_1n
    (JNIEnv* env, jclass, jdoubleArray a, jdoubleArray b, jint count, jdouble relTol, jdouble absTol, jboolean useCrit) {
        if (count <= 0 || a == nullptr || b == nullptr || relTol < 0.0 || absTol < 0.0) {
            return JNI_FALSE;
        }
        return JNI_FALSE;
    }

    /*
     * Class:     net_cramer_simd_SIMD
     * Method:    approx_equal_float_n
     * Signature: ([F[FIFFZ)Z
     */
    JNIEXPORT jboolean JNICALL Java_net_cramer_simd_SIMD_approx_1equal_1float_1n
    (JNIEnv* env, jclass, jfloatArray a, jfloatArray b, jint count, jfloat relTol, jfloat absTol, jboolean useCrit) {
        if (count <= 0 || a == nullptr || b == nullptr || relTol < 0.0f || absTol < 0.0f) {
            return JNI_FALSE;
        }
        return JNI_FALSE;
    }
#ifdef __cplusplus
}
#endif


// precondition: count must be even!
double l2_norm_double(double* d, int64_t count) {
    double scale = 0.0;
    Vec8d vector;

    int i;
    for (i = 0; i < count - 7; i += STEP_8) {
        PREFETCH(d + i + 63 * STEP_8);
        vector.load(d + i);
        scale = std::max(scale, horizontal_add(abs(vector)));
    }
    for (; i < count; i += 2) {
        double xr = d[i];
        double xi = d[i + 1];
        if (xr != 0.0 || xi != 0.0) {
            scale = std::max(scale, std::abs(xr) + std::abs(xi));
        }
    }
    if (scale == 0.0) {
        return 0.0;
    }
    PREFETCH(d);
    while (scale <= 1.1) {
        scale = scale * 1000.0;
    }
    scale = 1.0 / scale;
    Vec8d factor = Vec8d(scale);
    Vec8d sumsquaredVec = Vec8d(0.0);
    Vec8d scaled;
    for (i = 0; i < count - 7; i += STEP_8) {
        PREFETCH(d + i + 63 * STEP_8);
        vector.load(d + i);
        scaled = factor * vector;
        sumsquaredVec += square(scaled);
    }
    double sumsquared = horizontal_add(sumsquaredVec);
    for (; i < count; ++i) {
        double x = d[i];
        if (x != 0.0) {
            double tmp = scale * x;
            sumsquared += (tmp * tmp);
        }
    }

    return std::sqrt(sumsquared) / scale;
}

// precondition: count must be even!
float l2_norm_float(float* f, int64_t count) {
    float scale = 0.0f;
    Vec16f vector;

    int i;
    for (i = 0; i < count - 15; i += STEP_16) {
        PREFETCH(f + i + 63 * STEP_16);
        vector.load(f + i);
        scale = std::max(scale, horizontal_add(abs(vector)));
    }
    for (; i < count; i += 2) {
        float xr = f[i];
        float xi = f[i + 1];
        if (xr != 0.0f || xi != 0.0f) {
            scale = std::max(scale, std::abs(xr) + std::abs(xi));
        }
    }
    if (scale == 0.0f) {
        return 0.0f;
    }
    PREFETCH(f);
    while (scale <= 1.1f) {
        scale = scale * 1000.0f;
    }
    scale = 1.0f / scale;
    Vec16f factor = Vec16f(scale);
    Vec16f sumsquaredVec = Vec16f(0.0f);
    Vec16f scaled;
    for (i = 0; i < count - 15; i += STEP_16) {
        PREFETCH(f + i + 63 * STEP_16);
        vector.load(f + i);
        scaled = factor * vector;
        sumsquaredVec += square(scaled);
    }
    float sumsquared = horizontal_add(sumsquaredVec);
    for (; i < count; ++i) {
        float x = f[i];
        if (x != 0.0f) {
            float tmp = scale * x;
            sumsquared += (tmp * tmp);
        }
    }

    return std::sqrt(sumsquared) / scale;
}
