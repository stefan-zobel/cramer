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
package net.cramer.simd;

public final class SIMD {

    private static final boolean USE_CRITICAL = true;

    static {
        try {
            System.loadLibrary("vector_avx2");
        } catch (Throwable t) {
            t.printStackTrace();
            throw t;
        }
    }

    public static double l2normDouble(double[] array, int count) {
        return l2norm_double_n(array, count, USE_CRITICAL);
    }

    public static float l2normFloat(float[] array, int count) {
        return l2norm_float_n(array, count, USE_CRITICAL);
    }

    public static boolean approxEqualDouble(double[] a, double[] b, int count, double relTol, double absTol) {
        return approx_equal_double_n(a, b, count, relTol, absTol, USE_CRITICAL);
    }

    public static boolean approxEqualFloat(float[] a, float[] b, int count, float relTol, float absTol) {
        return approx_equal_float_n(a, b, count, relTol, absTol, USE_CRITICAL);
    }

    public static double distanceDouble(double[] a, double[] b, int count) {
        return distance_double_n(a, b, count, USE_CRITICAL);
    }

    public static float distanceFloat(float[] a, float[] b, int count) {
        return distance_float_n(a, b, count, USE_CRITICAL);
    }

    private static native double l2norm_double_n(double[] array, int count, boolean useCriticalRegion);

    private static native float l2norm_float_n(float[] array, int count, boolean useCriticalRegion);

    private static native boolean approx_equal_double_n(double[] a, double[] b, int count, double relTol, double absTol,
            boolean useCriticalRegion);

    private static native boolean approx_equal_float_n(float[] a, float[] b, int count, float relTol, float absTol,
            boolean useCriticalRegion);

    private static native double distance_double_n(double[] a, double[] b, int count, boolean useCriticalRegion);

    private static native float distance_float_n(float[] a, float[] b, int count, boolean useCriticalRegion);

    private SIMD() {
        throw new AssertionError();
    }
}
