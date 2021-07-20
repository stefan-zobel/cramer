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
bool approx_equal_double(double* a, double* b, int64_t count, double relTol, double absTol);
bool approx_equal_float(float* a, float* b, int64_t count, float relTol, float absTol);


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
        if (count == 0 || a == nullptr || b == nullptr) {
            return JNI_FALSE;
        }
        if (count < 0 || (count & 1) == 1) {
            throwJavaRuntimeException(env, "%s %d", "approx_equal_double - invalid count argument:", count);
            return JNI_FALSE;
        }
        if (relTol < 0.0) {
            throwJavaRuntimeException(env, "%s %f", "approx_equal_double - relTol < 0.0 :", relTol);
            return JNI_FALSE;
        }
        if (absTol < 0.0) {
            throwJavaRuntimeException(env, "%s %f", "approx_equal_double - absTol < 0.0 :", absTol);
            return JNI_FALSE;
        }
        try {
            DoubleArray aa = DoubleArray(env, a, count, useCrit);
            DoubleArray bb = DoubleArray(env, b, count, useCrit);
            return approx_equal_double(aa.ptr(), bb.ptr(), count, relTol, absTol) ? JNI_TRUE : JNI_FALSE;
        }
        catch (const JException& ex) {
            throwJavaRuntimeException(env, "%s %s", "approx_equal_double", ex.what());
        }
        catch (...) {
            throwJavaRuntimeException(env, "%s", "approx_equal_double: caught unknown exception");
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
        if (count == 0 || a == nullptr || b == nullptr) {
            return JNI_FALSE;
        }
        if (count < 0 || (count & 1) == 1) {
            throwJavaRuntimeException(env, "%s %d", "approx_equal_float - invalid count argument:", count);
            return JNI_FALSE;
        }
        if (relTol < 0.0f) {
            throwJavaRuntimeException(env, "%s %f", "approx_equal_float - relTol < 0.0f :", relTol);
            return JNI_FALSE;
        }
        if (absTol < 0.0f) {
            throwJavaRuntimeException(env, "%s %f", "approx_equal_float - absTol < 0.0f :", absTol);
            return JNI_FALSE;
        }
        try {
            FloatArray aa = FloatArray(env, a, count, useCrit);
            FloatArray bb = FloatArray(env, b, count, useCrit);
            return approx_equal_float(aa.ptr(), bb.ptr(), count, relTol, absTol) ? JNI_TRUE : JNI_FALSE;
        }
        catch (const JException& ex) {
            throwJavaRuntimeException(env, "%s %s", "approx_equal_float", ex.what());
        }
        catch (...) {
            throwJavaRuntimeException(env, "%s", "approx_equal_float: caught unknown exception");
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

bool approx_equal_double(double* a, double* b, int64_t count, double relTol, double absTol) {
    Vec8d vAbsTol = Vec8d(absTol);
    Vec8d vecA;
    Vec8d vecB;

    int i;
    for (i = 0; i < count - 7; i += STEP_8) {
        PREFETCH(a + i + 63 * STEP_8);
        PREFETCH(b + i + 63 * STEP_8);
        vecA.load(a + i);
        vecB.load(b + i);
        if (horizontal_or(vecA != vecB)) {
            Vec8d absdiff = abs(vecA - vecB);
            if (horizontal_add(select(absdiff <= vAbsTol, 0.0, select(absdiff <= relTol * max(abs(vecA), abs(vecB)), 0.0, 1.0))) > 0.0) {
                return false;
            }
        }
    }
    for (; i < count; ++i) {
        double ai = a[i];
        double bi = b[i];
        if (ai != bi) {
            double absdiff = std::abs(ai - bi);
            if (!(absdiff <= absTol || absdiff <= relTol * std::max(std::abs(ai), std::abs(bi)))) {
                return false;
            }
        }
    }
    return true;
}

bool approx_equal_float(float* a, float* b, int64_t count, float relTol, float absTol) {
    Vec16f vAbsTol = Vec16f(absTol);
    Vec16f vecA;
    Vec16f vecB;

    int i;
    for (i = 0; i < count - 15; i += STEP_16) {
        PREFETCH(a + i + 63 * STEP_16);
        PREFETCH(b + i + 63 * STEP_16);
        vecA.load(a + i);
        vecB.load(b + i);
        if (horizontal_or(vecA != vecB)) {
            Vec16f absdiff = abs(vecA - vecB);
            if (horizontal_add(select(absdiff <= vAbsTol, 0.0f, select(absdiff <= relTol * max(abs(vecA), abs(vecB)), 0.0f, 1.0f))) > 0.0f) {
                return false;
            }
        }
    }
    for (; i < count; ++i) {
        float ai = a[i];
        float bi = b[i];
        if (ai != bi) {
            float absdiff = std::abs(ai - bi);
            if (!(absdiff <= absTol || absdiff <= relTol * std::max(std::abs(ai), std::abs(bi)))) {
                return false;
            }
        }
    }
    return true;
}
