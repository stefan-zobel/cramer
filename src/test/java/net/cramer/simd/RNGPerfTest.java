package net.cramer.simd;

import java.util.Arrays;

public class RNGPerfTest {

    private static final int ITERS = 100_001_792 / 2_048;

    public static void main(String[] args) {
        setup();
        timeSfc64();
        timeXor1024();
    }

    private static void timeSfc64() {
        long[] rnd = new long[RNG.FETCH_SIZE];
        // warmup
        RNG.next2048LongsSfc64(rnd);

        long start = System.currentTimeMillis();
        for (int i = 1; i <= ITERS; ++i) {
            RNG.next2048LongsSfc64(rnd);
        }
        long end = System.currentTimeMillis();
        System.out.println("Sfc64   took: " + (end - start) + " ms\n");
        System.out.println(Arrays.toString(rnd) + "\n");
    }

    private static void timeXor1024() {
        long[] rnd = new long[RNG.FETCH_SIZE];
        // warmup
        RNG.next2048LongsXor1024(rnd);

        long start = System.currentTimeMillis();
        for (int i = 1; i <= ITERS; ++i) {
            RNG.next2048LongsXor1024(rnd);
        }
        long end = System.currentTimeMillis();
        System.out.println("Xor1024 took: " + (end - start) + " ms\n");
        System.out.println(Arrays.toString(rnd) + "\n");
    }

    private static void setup() {
        long[] seed = new long[RNG.XOR1024_SEED_LENGTH];
        for (int i = 0; i < seed.length; ++i) {
            seed[i] = i + 1;
        }
        RNG.seedSfc64(seed);
        RNG.seedXor1024(seed);
    }
}
