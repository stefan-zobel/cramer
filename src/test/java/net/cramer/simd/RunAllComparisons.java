package net.cramer.simd;

public final class RunAllComparisons {

    public static void main(String[] args) {
        TestInitializeSIMD.main(null);
        L1NormDoublePerfTest.main(null);
        L1NormFloatPerfTest.main(null);
        L2NormDoublePerfTest.main(null);
        L2NormFloatPerfTest.main(null);
        ApproxEqualDoublePerfTest.main(null);
        ApproxEqualFloatPerfTest.main(null);
        System.out.println("****************************************");
        System.out.println("*                FINISHED              *");
        System.out.println("****************************************");
    }
}
