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

import java.util.Objects;

public final class RNG {

    public static final int FETCH_SIZE = 8 * 256;
    public static final int SFC64_SEED_LENGTH = 8;
    public static final int XOR1024_SEED_LENGTH = 16 * 8;

    static {
        try {
            System.loadLibrary("vector_avx2");
        } catch (Throwable t) {
            t.printStackTrace();
            throw t;
        }
    }

    public static void next2048LongsSfc64(long[] random) {
        checkRandom(random);
        sfc64Large(random);
    }

    public static void next2048LongsXor1024(long[] random) {
        checkRandom(random);
        xor1024Large(random);
    }

    public static void seedSfc64(long[] seed) {
        checkSeed(seed, SFC64_SEED_LENGTH);
        initSfc64(seed);
    }

    public static void seedXor1024(long[] seed) {
        checkSeed(seed, XOR1024_SEED_LENGTH);
        initXor1024(seed);
    }

    private static void checkRandom(long[] random) {
        if (Objects.requireNonNull(random, "random").length < FETCH_SIZE) {
            throw new IllegalArgumentException("random array must have at least length " + FETCH_SIZE);
        }
    }

    private static void checkSeed(long[] seed, int minSeedLength) {
        if (Objects.requireNonNull(seed, "seed").length < minSeedLength) {
            throw new IllegalArgumentException("seed length must be at least " + minSeedLength);
        }
        for (int i = 0; i < minSeedLength; ++i) {
            if (seed[i] == 0L) {
                throw new IllegalArgumentException("seed value 0 at index " + i + " (seed values should be != 0)");
            }
        }
    }

    private static native void initXor1024(long[] a);

    private static native void xor1024Large(long[] a);

    private static native long initSfc64(long[] a);

    private static native void sfc64Large(long[] a);

    private RNG() {
        throw new AssertionError();
    }
}
