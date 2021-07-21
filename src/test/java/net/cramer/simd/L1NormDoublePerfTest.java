package net.cramer.simd;

import java.util.Arrays;

import net.jamu.matrix.Matrices;
import net.jamu.matrix.MatrixD;

public final class L1NormDoublePerfTest {

    private static final int ITERS = 16000;
    private static final int N = 1024;

    private static void banner() {
        System.out.println("****************************************");
        System.out.println("*         L1NormDoublePerfTest         *");
        System.out.println("****************************************");
    }

    public static void main(String[] args) {
        banner();
        MatrixD mA = Matrices.createD(N, N);
        MatrixD mB = Matrices.createD(N, N);
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                mB.set(i, j, 1.0e-4);
            }
        }

        double[] a = new double[N * N];
        double[] b = new double[N * N];
        Arrays.fill(b, 1.0e-4);

        double mnorm1 = Matrices.distance(mA, mB);
        System.out.println("Matrix double norm1: " + mnorm1);

        double norm1 = SIMD.distanceDouble(a, b, a.length);
        System.out.println("SIMD   double norm1: " + norm1);

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
                .println("Matrix average   : " + sum1 / (1_000_000.0 * (ITERS - 1)) + " ms (" + i + ": " + mnorm1 + ")");
        System.out.println("Matrix average   : " + sum1 / ((ITERS - 1)) + " ns (" + i + ": " + mnorm1 + ")");

        i = 1;
        for (; i <= ITERS; ++i) {
            long start = System.nanoTime();
            norm1 = SIMD.distanceDouble(a, b, a.length);
            long took = System.nanoTime() - start;
            if (i > 1) {
                sum2 += took;
            }
        }
        System.out.println("SIMD   average   : " + sum2 / (1_000_000.0 * (ITERS - 1)) + " ms (" + i + ": " + norm1 + ")");
        System.out.println("SIMD   average   : " + sum2 / ((ITERS - 1)) + " ns (" + i + ": " + norm1 + ")");
        System.out.println("SIMD   advantage : " + (sum2 / sum1));
    }
}
