package net.cramer.simd;

import java.util.Arrays;

import net.jamu.matrix.Matrices;
import net.jamu.matrix.MatrixD;

public final class ApproxEqualDoublePerfTest {

    private static final int ITERS = 16000;
    private static final int N = 1024;

    private static void banner() {
        System.out.println("****************************************");
        System.out.println("*       ApproxEqualDoublePerfTest      *");
        System.out.println("****************************************");
    }

    public static void main(String[] args) {
        banner();
        MatrixD mA = Matrices.createD(N, N);
        MatrixD mB = Matrices.createD(N, N);
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                mB.set(i, j, 0.005);
            }
        }

        double[] a = new double[N * N];
        double[] b = new double[N * N];
        Arrays.fill(b, 0.005);

        boolean mequal = Matrices.approxEqual(mA, mB, 1.0, 0.0);
        System.out.println("Matrix approx. equal double: " + mequal);

        boolean equal = SIMD.approxEqualDouble(a, b, a.length, 1.0, 0.0);
        System.out.println("SIMD   approx. equal double: " + equal);

        double sum1 = 0.0;
        double sum2 = 0.0;

        int i = 1;
        for (; i <= ITERS; ++i) {
            long start = System.nanoTime();
            mequal = Matrices.approxEqual(mA, mB, 1.0, 0.0);
            long took = System.nanoTime() - start;
            if (i > 1) {
                sum1 += took;
            }
        }
        System.out
                .println("Matrix average   : " + sum1 / (1_000_000.0 * (ITERS - 1)) + " ms (" + i + ": " + mequal + ")");
        System.out.println("Matrix average   : " + sum1 / ((ITERS - 1)) + " ns (" + i + ": " + mequal + ")");

        i = 1;
        for (; i <= ITERS; ++i) {
            long start = System.nanoTime();
            equal = SIMD.approxEqualDouble(a, b, a.length, 1.0, 0.0);
            long took = System.nanoTime() - start;
            if (i > 1) {
                sum2 += took;
            }
        }
        System.out.println("SIMD   average   : " + sum2 / (1_000_000.0 * (ITERS - 1)) + " ms (" + i + ": " + equal + ")");
        System.out.println("SIMD   average   : " + sum2 / ((ITERS - 1)) + " ns (" + i + ": " + equal + ")");
        System.out.println("SIMD   advantage : " + (sum2 / sum1));
    }
}
