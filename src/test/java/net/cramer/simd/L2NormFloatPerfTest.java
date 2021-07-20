package net.cramer.simd;

import java.util.Arrays;

import net.jamu.complex.ZArrayUtil;
import net.jamu.matrix.Matrices;
import net.jamu.matrix.MatrixF;

public final class L2NormFloatPerfTest {

    private static final int ITERS = 16000;
    private static final int N = 2048;

    public static void main(String[] args) {
        MatrixF mB = Matrices.createF(N, N);
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                mB.set(i, j, 1.0e-4f);
            }
        }

        float[] b = new float[N * N];
        Arrays.fill(b, 1.0e-4f);

        float mnorm2 = ZArrayUtil.l2norm(mB.getArrayUnsafe());
        System.out.println("Matrix float norm2: " + mnorm2);

        float norm2 = SIMD.l2normFloat(b, b.length);
        System.out.println("SIMD   float norm2: " + norm2);

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
            norm2 = SIMD.l2normFloat(b, b.length);
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
