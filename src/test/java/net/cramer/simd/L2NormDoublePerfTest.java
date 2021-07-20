package net.cramer.simd;

import java.util.Arrays;

import net.jamu.complex.ZArrayUtil;
import net.jamu.matrix.Matrices;
import net.jamu.matrix.MatrixD;

public final class L2NormDoublePerfTest {

    private static final int ITERS = 16000;
    private static final int N = 1024;

    public static void main(String[] args) {
        MatrixD mB = Matrices.createD(N, N);
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                mB.set(i, j, 1.0e-4);
            }
        }

        double[] b = new double[N * N];
        Arrays.fill(b, 1.0e-4);

        double mnorm2 = ZArrayUtil.l2norm(mB.getArrayUnsafe());
        System.out.println("Matrix double norm2: " + mnorm2);

        double norm2 = SIMD.l2normDouble(b, b.length);
        System.out.println("SIMD   double norm2: " + norm2);

        double sum1 = 0.0;
        double sum2 = 0.0;

        int i = 1;
        for (; i <= ITERS; ++i) {
            long start = System.nanoTime();
            mnorm2 = ZArrayUtil.l2norm(mB.getArrayUnsafe());
            long took = System.nanoTime() - start;
            if (i > 1) {
                sum1 += took;
            }
        }
        System.out
                .println("Matrix average : " + sum1 / (1_000_000.0 * (ITERS - 1)) + " ms (" + i + ": " + mnorm2 + ")");
        System.out.println("Matrix average : " + sum1 / ((ITERS - 1)) + " ns (" + i + ": " + mnorm2 + ")");

        i = 1;
        for (; i <= ITERS; ++i) {
            long start = System.nanoTime();
            norm2 = SIMD.l2normDouble(b, b.length);
            long took = System.nanoTime() - start;
            if (i > 1) {
                sum2 += took;
            }
        }
        System.out.println("SIMD   average : " + sum2 / (1_000_000.0 * (ITERS - 1)) + " ms (" + i + ": " + norm2 + ")");
        System.out.println("SIMD   average : " + sum2 / ((ITERS - 1)) + " ns (" + i + ": " + norm2 + ")");
        System.out.println("SIMD   advantage : " + (sum2 / sum1));
    }
}
