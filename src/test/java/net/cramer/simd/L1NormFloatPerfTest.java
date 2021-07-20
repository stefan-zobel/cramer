package net.cramer.simd;

import java.util.Arrays;

import net.jamu.matrix.Matrices;
import net.jamu.matrix.MatrixF;

public final class L1NormFloatPerfTest {

    private static final int ITERS = 16000;
    private static final int N = 2048;

    public static void main(String[] args) {
        MatrixF mA = Matrices.createF(N, N);
        MatrixF mB = Matrices.createF(N, N);
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                mB.set(i, j, 1.0e-4f);
            }
        }

        float[] a = new float[N * N];
        float[] b = new float[N * N];
        Arrays.fill(b, 1.0e-4f);

        float mnorm1 = Matrices.distance(mA, mB);
        System.out.println("Matrix float norm1: " + mnorm1);

        float norm1 = SIMD.distanceFloat(a, b, a.length);
        System.out.println("SIMD   float norm1: " + norm1);

        double sum1 = 0.0;
        double sum2 = 0.0;

        int i = 1;
        for (; i <= ITERS; ++i) {
            long start = System.nanoTime();
            mnorm1 = Matrices.distance(mA, mB);
            long took = System.nanoTime() - start;
            if (i > 1) {
                sum1 += took;
            }
        }
        System.out
                .println("Matrix average : " + sum1 / (1_000_000.0 * (ITERS - 1)) + " ms (" + i + ": " + mnorm1 + ")");
        System.out.println("Matrix average : " + sum1 / ((ITERS - 1)) + " ns (" + i + ": " + mnorm1 + ")");

        i = 1;
        for (; i <= ITERS; ++i) {
            long start = System.nanoTime();
            norm1 = SIMD.distanceFloat(a, b, a.length);
            long took = System.nanoTime() - start;
            if (i > 1) {
                sum2 += took;
            }
        }
        System.out.println("SIMD   average : " + sum2 / (1_000_000.0 * (ITERS - 1)) + " ms (" + i + ": " + norm1 + ")");
        System.out.println("SIMD   average : " + sum2 / ((ITERS - 1)) + " ns (" + i + ": " + norm1 + ")");
        System.out.println("SIMD   advantage : " + (sum2 / sum1));
    }
}
