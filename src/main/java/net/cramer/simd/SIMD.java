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

    private static native double l2norm_double_n(double[] array, int count, boolean useCriticalRegion);

    private static native float l2norm_float_n(float[] array, int count, boolean useCriticalRegion);

    private SIMD() {
        throw new AssertionError();
    }
}
