package net.cramer.simd;

public final class TestInitializeSIMD {

    public static void main(String[] args) {
        System.out.println(SIMD.l2normDouble(null, 0));
        System.out.println(SIMD.l2normFloat(null, 0));
        System.out.println(SIMD.approxEqualDouble(null, null, 0, 0.01, 0.01));
        System.out.println(SIMD.approxEqualFloat(null, null, 0, 0.01f, 0.01f));
    }
}
