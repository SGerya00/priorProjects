#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <immintrin.h>

/*Nx = Ny ~ 8000-10000*/
/*Nt ~ 100-120*/

#define XB (4.0)
#define XA (0.0)
#define YB (4.0)
#define YA (0.0)

#define dtype double

dtype calcNodeCoordX(dtype X1, int j, dtype hx) {
    return (X1 + j * hx);
}

dtype calcNodeCoordY(dtype Y1, int i, dtype hy) {
    return (Y1 + i * hy);
}

void initPgrid(dtype* pgrid, int sizeX, int sizeY, dtype hx, dtype hy) {
    dtype Xdif = XB - XA;
    dtype Ydif = YB - YA;

    dtype Xs1 = XA + (Xdif) / 3;
    dtype Xs2 = XA + (Xdif) * 2 / 3;
    dtype Ys1 = YA + (Ydif) * 2 / 3;
    dtype Ys2 = YA + (Ydif) / 3;

    dtype R = 0.1 * fmin(Xdif, Ydif);
    dtype Rsq = R * R;

    int size = sizeX * sizeY;
    for (int k = 0; k < size; k++) {
        int i = k / sizeX;
        int j = k % sizeX;
        dtype Xj = calcNodeCoordX(XA, j, hx);
        dtype Yi = calcNodeCoordY(YA, i, hy);
        if (((Xj - Xs1) * (Xj - Xs1)) + ((Yi - Ys1) * (Yi - Ys1)) < Rsq) {
            pgrid[k] = 0.1;
        }
        else if (((Xj - Xs2) * (Xj - Xs2)) + ((Yi - Ys2) * (Yi - Ys2)) < Rsq) {
            pgrid[k] = -0.1;
        }
        else {
            pgrid[k] = 0.0;
        }
    }
}

void initXYgrid(dtype* XYgrid, int sizeX, int sizeY) {
    int size = sizeX * sizeY;
    for (int k = 0; k < size; k++) {
        XYgrid[k] = 0.0;
    }
}

int convertGridCoordsToIdx(int sizeX, int sizeY, int i, int j) {
    return (j + sizeX * i);
}

void algoStepSingleLine(int rowN, dtype* XYgridCurr, dtype* XYgridNew, dtype const* pgrid,
                        const double generalCoef, const double coef1, const double coef2, const double coef3,
                        const int sizeX, const int sizeY) {
    __m256d vgeneralCoef = _mm256_set1_pd(generalCoef);

    __m256d* vXYgridCurr = (__m256d*)XYgridCurr;
    __m256d* vXYgridNew = (__m256d*)XYgridNew;

    __m256d vcoef1 = _mm256_set1_pd(coef1);
    __m256d vcoef2 = _mm256_set1_pd(coef2);
    __m256d vcoef3 = _mm256_set1_pd(coef3);

    __m256d vpcoef1 = _mm256_set1_pd(2.0);
    __m256d vpcoef2 = _mm256_set1_pd(0.25);

//    dtype delta = 0.0;
//
//    __m256d vdelta = _mm256_set1_pd(0.0);
//    __m256d vd = _mm256_set1_pd(0.0);

    const int blendMaskFirst = 0b0111; //get 1 left from a and 3 right from b
    const int blendMaskLast = 0b1110; //get 3 left from b and 1 right from a

    //first step
    const int i = rowN;
    int fidxcur = convertGridCoordsToIdx(sizeX, sizeY, i, 0);

    __m256d vcur = _mm256_loadu_pd(&(XYgridCurr[fidxcur]));

    int fidx11 = convertGridCoordsToIdx(sizeX, sizeY, i, 0);

    int fidx1m1 = convertGridCoordsToIdx(sizeX, sizeY, i, 0 - 1); //i starts with 1, not 0
    int fidx1p1 = convertGridCoordsToIdx(sizeX, sizeY, i, 0 + 1);

    int fidxm11 = convertGridCoordsToIdx(sizeX, sizeY, i - 1, 0);
    int fidxp11 = convertGridCoordsToIdx(sizeX, sizeY, i + 1, 0);

    int fidxm1m1 = convertGridCoordsToIdx(sizeX, sizeY, i - 1, 0 - 1); //still good, padding added
    int fidxm1p1 = convertGridCoordsToIdx(sizeX, sizeY, i - 1, 0 + 1);
    int fidxp1m1 = convertGridCoordsToIdx(sizeX, sizeY, i + 1, 0 - 1);
    int fidxp1p1 = convertGridCoordsToIdx(sizeX, sizeY, i + 1, 0 + 1);

    /*summands 1,2,3---------------------------------------------------v*/
    __m256d V_1_1 = _mm256_loadu_pd(&(XYgridCurr[fidxm1m1]));
    __m256d V0_1 = _mm256_loadu_pd(&(XYgridCurr[fidx1m1]));
    __m256d V1_1 = _mm256_loadu_pd(&(XYgridCurr[fidxp1m1]));

    __m256d V_10 = _mm256_loadu_pd(&(XYgridCurr[fidxm11]));
    __m256d V00 = vcur;
    __m256d V10 = _mm256_loadu_pd(&(XYgridCurr[fidxp11]));

    __m256d V_11 = _mm256_loadu_pd(&(XYgridCurr[fidxm1p1]));
    __m256d V01 = _mm256_loadu_pd(&(XYgridCurr[fidx1p1]));
    __m256d V11 = _mm256_loadu_pd(&(XYgridCurr[fidxp1p1]));
    //printf("in ASSL, all vector loads successful FS\n"); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    __m256d vsum1 = _mm256_add_pd(V0_1, V01);
    __m256d vsum2 = _mm256_add_pd(V10, V_10);
    __m256d vsum3 = _mm256_add_pd(_mm256_add_pd(V_1_1,V_11),_mm256_add_pd(V1_1,V11));

    __m256d vsum123 = _mm256_fmadd_pd(vcoef3,vsum3,_mm256_fmadd_pd(vcoef2,vsum2,_mm256_mul_pd(vcoef1,vsum1)));
    /*summands 1,2,3---------------------------------------------------^*/
    /*summand 4---------------------------------------------------v*/
    __m256d vp10 = _mm256_loadu_pd(&(pgrid[fidx11]));
    __m256d vp11 = _mm256_loadu_pd(&(pgrid[fidxm11]));
    __m256d vp12 = _mm256_loadu_pd(&(pgrid[fidxp11]));
    __m256d vp13 = _mm256_loadu_pd(&(pgrid[fidx1m1]));
    __m256d vp14 = _mm256_loadu_pd(&(pgrid[fidx1p1]));
    //printf("in ASSL, all p loads successful FS\n"); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    __m256d vsum4 = _mm256_fmadd_pd(vpcoef1,vp10,_mm256_mul_pd(vpcoef2,_mm256_add_pd(_mm256_add_pd(vp11,vp12),_mm256_add_pd(vp13,vp14))));
    /*summand 4---------------------------------------------------^*/
    __m256d vresult = _mm256_mul_pd(vgeneralCoef,_mm256_add_pd(vsum123,vsum4));
    __m256d resFirst = vresult;
    __m256d blendedVector = _mm256_blend_pd(vcur, resFirst, blendMaskFirst);
    //get leftmost from X, 3 others from Y
    //vXYgridNew[(fidxcur / 4)] = blendedVector; //write
    _mm256_storeu_pd(&(XYgridNew[fidxcur]), blendedVector);
    //printf("in ASSL, write to memory successful FS\n"); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//    //calculate delta
//    vd = _mm256_max_pd(_mm256_sub_pd(blendedVector, vcur),
//                       _mm256_sub_pd(vcur, blendedVector));
//    vdelta = _mm256_max_pd(vdelta, vd);
//    //

    for (int k = 1; k < ((sizeX / 4) - 1); k++) {
        int j = k * 4; //256 / sizeof(dtype)
        fidxcur += 4;
        vcur = _mm256_loadu_pd(&(XYgridCurr[fidxcur]));

        fidx11 = convertGridCoordsToIdx(sizeX, sizeY, i, j);

        fidx1m1 = convertGridCoordsToIdx(sizeX, sizeY, i, j - 1);
        fidx1p1 = convertGridCoordsToIdx(sizeX, sizeY, i, j + 1);

        fidxm11 = convertGridCoordsToIdx(sizeX, sizeY, i - 1, j);
        fidxp11 = convertGridCoordsToIdx(sizeX, sizeY, i + 1, j);

        fidxm1m1 = convertGridCoordsToIdx(sizeX, sizeY, i - 1, j - 1);
        fidxm1p1 = convertGridCoordsToIdx(sizeX, sizeY, i - 1, j + 1);
        fidxp1m1 = convertGridCoordsToIdx(sizeX, sizeY, i + 1, j - 1);
        fidxp1p1 = convertGridCoordsToIdx(sizeX, sizeY, i + 1, j + 1);

        /*summands 1,2,3---------------------------------------------------v*/
        V_1_1 = _mm256_loadu_pd(&(XYgridCurr[fidxm1m1]));
        V0_1 = _mm256_loadu_pd(&(XYgridCurr[fidx1m1]));
        V1_1 = _mm256_loadu_pd(&(XYgridCurr[fidxp1m1]));

        V_10 = _mm256_loadu_pd(&(XYgridCurr[fidxm11]));
        V00 = vcur;
        V10 = _mm256_loadu_pd(&(XYgridCurr[fidxp11]));

        V_11 = _mm256_loadu_pd(&(XYgridCurr[fidxm1p1]));
        V01 = _mm256_loadu_pd(&(XYgridCurr[fidx1p1]));
        V11 = _mm256_loadu_pd(&(XYgridCurr[fidxp1p1]));

        vsum1 = _mm256_add_pd(V0_1, V01);
        vsum2 = _mm256_add_pd(V10, V_10);
        vsum3 = _mm256_add_pd(_mm256_add_pd(V_1_1,V_11),_mm256_add_pd(V1_1,V11));

        vsum123 = _mm256_fmadd_pd(vcoef3,vsum3,_mm256_fmadd_pd(vcoef2,vsum2,_mm256_mul_pd(vcoef1,vsum1)));

        /*summands 1,2,3---------------------------------------------------^*/
        /*summand 4---------------------------------------------------v*/
        vp10 = _mm256_loadu_pd(&(pgrid[fidx11]));
        vp11 = _mm256_loadu_pd(&(pgrid[fidxm11]));
        vp12 = _mm256_loadu_pd(&(pgrid[fidxp11]));
        vp13 = _mm256_loadu_pd(&(pgrid[fidx1m1]));
        vp14 = _mm256_loadu_pd(&(pgrid[fidx1p1]));

        vsum4 = _mm256_fmadd_pd(vpcoef1,vp10,_mm256_mul_pd(vpcoef2,_mm256_add_pd(_mm256_add_pd(vp11,vp12),_mm256_add_pd(vp13,vp14))));
        /*summand 4---------------------------------------------------^*/
        vresult = _mm256_mul_pd(vgeneralCoef,_mm256_add_pd(vsum123,vsum4));

        //vXYgridNew[(fidxcur / 4)] = vresult;
        _mm256_storeu_pd(&(XYgridNew[fidxcur]), vresult);

//        //calculate delta
//        vd = _mm256_max_pd(_mm256_sub_pd(vresult, vcur),
//                       _mm256_sub_pd(vcur, vresult));
//        vdelta = _mm256_max_pd(vdelta, vd);
//        //
    }

    //last step
    fidxcur += 4;
    vcur = _mm256_loadu_pd(&(XYgridCurr[fidxcur]));

    fidx11 = convertGridCoordsToIdx(sizeX, sizeY, i, sizeX - 4);

    fidx1m1 = convertGridCoordsToIdx(sizeX, sizeY, i, sizeX - 4 - 1);
    fidx1p1 = convertGridCoordsToIdx(sizeX, sizeY, i, sizeX - 4 + 1);

    fidxm11 = convertGridCoordsToIdx(sizeX, sizeY, i - 1, sizeX - 4);
    fidxp11 = convertGridCoordsToIdx(sizeX, sizeY, i + 1, sizeX - 4);

    fidxm1m1 = convertGridCoordsToIdx(sizeX, sizeY, i - 1, sizeX - 4 - 1);
    fidxm1p1 = convertGridCoordsToIdx(sizeX, sizeY, i - 1, sizeX - 4 + 1);
    fidxp1m1 = convertGridCoordsToIdx(sizeX, sizeY, i + 1, sizeX - 4 - 1);
    fidxp1p1 = convertGridCoordsToIdx(sizeX, sizeY, i + 1, sizeX - 4 + 1);

    /*summands 1,2,3---------------------------------------------------v*/
    V_1_1 = _mm256_loadu_pd(&(XYgridCurr[fidxm1m1]));
    V0_1 = _mm256_loadu_pd(&(XYgridCurr[fidx1m1]));
    V1_1 = _mm256_loadu_pd(&(XYgridCurr[fidxp1m1]));

    V_10 = _mm256_loadu_pd(&(XYgridCurr[fidxm11]));
    V00 = vcur;
    V10 = _mm256_loadu_pd(&(XYgridCurr[fidxp11]));

    V_11 = _mm256_loadu_pd(&(XYgridCurr[fidxm1p1]));
    V01 = _mm256_loadu_pd(&(XYgridCurr[fidx1p1]));
    V11 = _mm256_loadu_pd(&(XYgridCurr[fidxp1p1]));
    //printf("in ASSL, all vector loads successful LS\n"); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    vsum1 = _mm256_add_pd(V0_1, V01);
    vsum2 = _mm256_add_pd(V10, V_10);
    vsum3 = _mm256_add_pd(_mm256_add_pd(V_1_1,V_11),_mm256_add_pd(V1_1,V11));

    vsum123 = _mm256_fmadd_pd(vcoef3,vsum3,_mm256_fmadd_pd(vcoef2,vsum2,_mm256_mul_pd(vcoef1,vsum1)));
    /*summands 1,2,3---------------------------------------------------^*/
    /*summand 4---------------------------------------------------v*/
    vp10 = _mm256_loadu_pd(&(pgrid[fidx11]));
    vp11 = _mm256_loadu_pd(&(pgrid[fidxm11]));
    vp12 = _mm256_loadu_pd(&(pgrid[fidxp11]));
    vp13 = _mm256_loadu_pd(&(pgrid[fidx1m1]));
    vp14 = _mm256_loadu_pd(&(pgrid[fidx1p1]));
    //printf("in ASSL, all p loads successful LS\n"); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    vsum4 = _mm256_fmadd_pd(vpcoef1,vp10,_mm256_mul_pd(vpcoef2,_mm256_add_pd(_mm256_add_pd(vp11,vp12),_mm256_add_pd(vp13,vp14))));
    /*summand 4---------------------------------------------------^*/
    vresult = _mm256_mul_pd(vgeneralCoef,_mm256_add_pd(vsum123,vsum4));

    __m256d resLast = vresult;
    blendedVector = _mm256_blend_pd(vcur, resLast, blendMaskLast);
    //get rightmost from X, 3 others from Y
    //vXYgridNew[(fidxcur / 4)] = blendedVector; //write
    _mm256_storeu_pd(&(XYgridNew[fidxcur]), blendedVector);
    //printf("in ASSL, write to memory successful LS\n"); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//    //calculate delta
//    vd = _mm256_max_pd(_mm256_sub_pd(blendedVector, vcur),
//                       _mm256_sub_pd(vcur, blendedVector));
//    vdelta = _mm256_max_pd(vdelta, vd);
//
//    //find vectorised total delta after iterating through entire matrix (4 dif numbers)
//    const int imm1 = 0b01001110;
//    vd = _mm256_permute4x64_pd(vdelta, imm1); //value of imm8 should be used here
//    vdelta = _mm256_max_pd(vdelta, vd);
//
//    const int imm2 = 0b1111;
//    vd = _mm256_shuffle_pd(vdelta, vdelta, imm2); //has lower latency than permute4x64
//    vdelta = _mm256_max_pd(vd, vdelta);
//    //now vdelta has 4 equal 64bit values, we need one of them
//
//    double delta4[4] = {0, 0, 0, 0};
//    _mm256_storeu_pd((double *) &delta4, vdelta);
//    //storeu and store do not differ in latency and throughput on processor this programme is meant to be run on
//    delta = delta4[0];
//    //now delta has required delta from entire matrix iteration
//
//    return delta;
} //doesn't return anything

double algoStepSingleLineLastIter(int rowN, dtype* XYgridCurr, dtype* XYgridNew, dtype const* pgrid,
                         const double generalCoef, const double coef1, const double coef2, const double coef3,
                         const int sizeX, const int sizeY) {
    __m256d vgeneralCoef = _mm256_set1_pd(generalCoef);

    __m256d* vXYgridCurr = (__m256d*)XYgridCurr;
    __m256d* vXYgridNew = (__m256d*)XYgridNew;

    __m256d vcoef1 = _mm256_set1_pd(coef1);
    __m256d vcoef2 = _mm256_set1_pd(coef2);
    __m256d vcoef3 = _mm256_set1_pd(coef3);

    __m256d vpcoef1 = _mm256_set1_pd(2.0);
    __m256d vpcoef2 = _mm256_set1_pd(0.25);

    dtype delta = 0.0;

    __m256d vdelta = _mm256_set1_pd(0.0);
    __m256d vd = _mm256_set1_pd(0.0);

    const int blendMaskFirst = 0b0111; //get 1 left from a and 3 right from b
    const int blendMaskLast = 0b1110; //get 3 left from b and 1 right from a

    //first step
    int i = rowN;
    int fidxcur = convertGridCoordsToIdx(sizeX, sizeY, i, 0);

    __m256d vcur = _mm256_loadu_pd(&(XYgridCurr[fidxcur])); //!! moving in 4s => aligned

    int fidx11 = convertGridCoordsToIdx(sizeX, sizeY, i, 0);

    int fidx1m1 = convertGridCoordsToIdx(sizeX, sizeY, i, 0 - 1); //i starts with 1, not 0
    int fidx1p1 = convertGridCoordsToIdx(sizeX, sizeY, i, 0 + 1);

    int fidxm11 = convertGridCoordsToIdx(sizeX, sizeY, i - 1, 0);
    int fidxp11 = convertGridCoordsToIdx(sizeX, sizeY, i + 1, 0);

    int fidxm1m1 = convertGridCoordsToIdx(sizeX, sizeY, i - 1, 0 - 1); //still good, padding added
    int fidxm1p1 = convertGridCoordsToIdx(sizeX, sizeY, i - 1, 0 + 1);
    int fidxp1m1 = convertGridCoordsToIdx(sizeX, sizeY, i + 1, 0 - 1);
    int fidxp1p1 = convertGridCoordsToIdx(sizeX, sizeY, i + 1, 0 + 1);

    /*summands 1,2,3---------------------------------------------------v*/
    __m256d V_1_1 = _mm256_loadu_pd(&(XYgridCurr[fidxm1m1]));
    __m256d V0_1 = _mm256_loadu_pd(&(XYgridCurr[fidx1m1]));
    __m256d V1_1 = _mm256_loadu_pd(&(XYgridCurr[fidxp1m1]));

    __m256d V_10 = _mm256_loadu_pd(&(XYgridCurr[fidxm11]));
    __m256d V00 = vcur;
    __m256d V10 = _mm256_loadu_pd(&(XYgridCurr[fidxp11]));

    __m256d V_11 = _mm256_loadu_pd(&(XYgridCurr[fidxm1p1]));
    __m256d V01 = _mm256_loadu_pd(&(XYgridCurr[fidx1p1]));
    __m256d V11 = _mm256_loadu_pd(&(XYgridCurr[fidxp1p1]));

    __m256d vsum1 = _mm256_add_pd(V0_1, V01);
    __m256d vsum2 = _mm256_add_pd(V10, V_10);
    __m256d vsum3 = _mm256_add_pd(_mm256_add_pd(V_1_1,V_11),_mm256_add_pd(V1_1,V11));

    __m256d vsum123 = _mm256_fmadd_pd(vcoef3,vsum3,_mm256_fmadd_pd(vcoef2,vsum2,_mm256_mul_pd(vcoef1,vsum1)));
    /*summands 1,2,3---------------------------------------------------^*/
    /*summand 4---------------------------------------------------v*/
    __m256d vp10 = _mm256_loadu_pd(&(pgrid[fidx11]));
    __m256d vp11 = _mm256_loadu_pd(&(pgrid[fidxm11]));
    __m256d vp12 = _mm256_loadu_pd(&(pgrid[fidxp11]));
    __m256d vp13 = _mm256_loadu_pd(&(pgrid[fidx1m1]));
    __m256d vp14 = _mm256_loadu_pd(&(pgrid[fidx1p1]));

    __m256d vsum4 = _mm256_fmadd_pd(vpcoef1,vp10,_mm256_mul_pd(vpcoef2,_mm256_add_pd(_mm256_add_pd(vp11,vp12),_mm256_add_pd(vp13,vp14))));
    /*summand 4---------------------------------------------------^*/
    __m256d vresult = _mm256_mul_pd(vgeneralCoef,_mm256_add_pd(vsum123,vsum4));
    __m256d resFirst = vresult;
    __m256d blendedVector = _mm256_blend_pd(vcur, resFirst, blendMaskFirst);
    //get leftmost from X, 3 others from Y
    //vXYgridNew[(fidxcur / 4)] = blendedVector; //write
    _mm256_storeu_pd(&(XYgridNew[fidxcur]), blendedVector);

    //calculate delta
    vd = _mm256_max_pd(_mm256_sub_pd(blendedVector, vcur),
                       _mm256_sub_pd(vcur, blendedVector));
    vdelta = _mm256_max_pd(vdelta, vd);
    //

    for (int k = 1; k < ((sizeX / 4) - 1); k++) {
        int j = k * 4; //256 / sizeof(dtype)
        fidxcur += 4;
        vcur = _mm256_loadu_pd(&(XYgridCurr[fidxcur]));

        fidx11 = convertGridCoordsToIdx(sizeX, sizeY, i, j);

        fidx1m1 = convertGridCoordsToIdx(sizeX, sizeY, i, j - 1);
        fidx1p1 = convertGridCoordsToIdx(sizeX, sizeY, i, j + 1);

        fidxm11 = convertGridCoordsToIdx(sizeX, sizeY, i - 1, j);
        fidxp11 = convertGridCoordsToIdx(sizeX, sizeY, i + 1, j);

        fidxm1m1 = convertGridCoordsToIdx(sizeX, sizeY, i - 1, j - 1);
        fidxm1p1 = convertGridCoordsToIdx(sizeX, sizeY, i - 1, j + 1);
        fidxp1m1 = convertGridCoordsToIdx(sizeX, sizeY, i + 1, j - 1);
        fidxp1p1 = convertGridCoordsToIdx(sizeX, sizeY, i + 1, j + 1);

        /*summands 1,2,3---------------------------------------------------v*/
        V_1_1 = _mm256_loadu_pd(&(XYgridCurr[fidxm1m1]));
        V0_1 = _mm256_loadu_pd(&(XYgridCurr[fidx1m1]));
        V1_1 = _mm256_loadu_pd(&(XYgridCurr[fidxp1m1]));

        V_10 = _mm256_loadu_pd(&(XYgridCurr[fidxm11]));
        V00 = vcur;
        V10 = _mm256_loadu_pd(&(XYgridCurr[fidxp11]));

        V_11 = _mm256_loadu_pd(&(XYgridCurr[fidxm1p1]));
        V01 = _mm256_loadu_pd(&(XYgridCurr[fidx1p1]));
        V11 = _mm256_loadu_pd(&(XYgridCurr[fidxp1p1]));

        vsum1 = _mm256_add_pd(V0_1, V01);
        vsum2 = _mm256_add_pd(V10, V_10);
        vsum3 = _mm256_add_pd(_mm256_add_pd(V_1_1,V_11),_mm256_add_pd(V1_1,V11));

        vsum123 = _mm256_fmadd_pd(vcoef3,vsum3,_mm256_fmadd_pd(vcoef2,vsum2,_mm256_mul_pd(vcoef1,vsum1)));

        /*summands 1,2,3---------------------------------------------------^*/
        /*summand 4---------------------------------------------------v*/
        vp10 = _mm256_loadu_pd(&(pgrid[fidx11]));
        vp11 = _mm256_loadu_pd(&(pgrid[fidxm11]));
        vp12 = _mm256_loadu_pd(&(pgrid[fidxp11]));
        vp13 = _mm256_loadu_pd(&(pgrid[fidx1m1]));
        vp14 = _mm256_loadu_pd(&(pgrid[fidx1p1]));

        vsum4 = _mm256_fmadd_pd(vpcoef1,vp10,_mm256_mul_pd(vpcoef2,_mm256_add_pd(_mm256_add_pd(vp11,vp12),_mm256_add_pd(vp13,vp14))));
        /*summand 4---------------------------------------------------^*/
        vresult = _mm256_mul_pd(vgeneralCoef,_mm256_add_pd(vsum123,vsum4));

        //vXYgridNew[(fidxcur / 4)] = vresult;
        _mm256_storeu_pd(&(XYgridNew[fidxcur]), vresult);

        //calculate delta
        vd = _mm256_max_pd(_mm256_sub_pd(vresult, vcur),
                           _mm256_sub_pd(vcur, vresult));
        vdelta = _mm256_max_pd(vdelta, vd);
        //
    }

    //last step
    fidxcur += 4;
    vcur = _mm256_loadu_pd(&(XYgridCurr[fidxcur]));

    fidx11 = convertGridCoordsToIdx(sizeX, sizeY, i, sizeX - 4);

    fidx1m1 = convertGridCoordsToIdx(sizeX, sizeY, i, sizeX - 4 - 1);
    fidx1p1 = convertGridCoordsToIdx(sizeX, sizeY, i, sizeX - 4 + 1);

    fidxm11 = convertGridCoordsToIdx(sizeX, sizeY, i - 1, sizeX - 4);
    fidxp11 = convertGridCoordsToIdx(sizeX, sizeY, i + 1, sizeX - 4);

    fidxm1m1 = convertGridCoordsToIdx(sizeX, sizeY, i - 1, sizeX - 4 - 1);
    fidxm1p1 = convertGridCoordsToIdx(sizeX, sizeY, i - 1, sizeX - 4 + 1);
    fidxp1m1 = convertGridCoordsToIdx(sizeX, sizeY, i + 1, sizeX - 4 - 1);
    fidxp1p1 = convertGridCoordsToIdx(sizeX, sizeY, i + 1, sizeX - 4 + 1);

    /*summands 1,2,3---------------------------------------------------v*/
    V_1_1 = _mm256_loadu_pd(&(XYgridCurr[fidxm1m1]));
    V0_1 = _mm256_loadu_pd(&(XYgridCurr[fidx1m1]));
    V1_1 = _mm256_loadu_pd(&(XYgridCurr[fidxp1m1]));

    V_10 = _mm256_loadu_pd(&(XYgridCurr[fidxm11]));
    V00 = vcur;
    V10 = _mm256_loadu_pd(&(XYgridCurr[fidxp11]));

    V_11 = _mm256_loadu_pd(&(XYgridCurr[fidxm1p1]));
    V01 = _mm256_loadu_pd(&(XYgridCurr[fidx1p1]));
    V11 = _mm256_loadu_pd(&(XYgridCurr[fidxp1p1]));

    vsum1 = _mm256_add_pd(V0_1, V01);
    vsum2 = _mm256_add_pd(V10, V_10);
    vsum3 = _mm256_add_pd(_mm256_add_pd(V_1_1,V_11),_mm256_add_pd(V1_1,V11));

    vsum123 = _mm256_fmadd_pd(vcoef3,vsum3,_mm256_fmadd_pd(vcoef2,vsum2,_mm256_mul_pd(vcoef1,vsum1)));
    /*summands 1,2,3---------------------------------------------------^*/
    /*summand 4---------------------------------------------------v*/
    vp10 = _mm256_loadu_pd(&(pgrid[fidx11]));
    vp11 = _mm256_loadu_pd(&(pgrid[fidxm11]));
    vp12 = _mm256_loadu_pd(&(pgrid[fidxp11]));
    vp13 = _mm256_loadu_pd(&(pgrid[fidx1m1]));
    vp14 = _mm256_loadu_pd(&(pgrid[fidx1p1]));

    vsum4 = _mm256_fmadd_pd(vpcoef1,vp10,_mm256_mul_pd(vpcoef2,_mm256_add_pd(_mm256_add_pd(vp11,vp12),_mm256_add_pd(vp13,vp14))));
    /*summand 4---------------------------------------------------^*/
    vresult = _mm256_mul_pd(vgeneralCoef,_mm256_add_pd(vsum123,vsum4));

    __m256d resLast = vresult;
    blendedVector = _mm256_blend_pd(vcur, resLast, blendMaskLast);
    //get rightmost from X, 3 others from Y
    //vXYgridNew[(fidxcur / 4)] = blendedVector; //write
    _mm256_storeu_pd(&(XYgridNew[fidxcur]), blendedVector);

    //calculate delta
    vd = _mm256_max_pd(_mm256_sub_pd(blendedVector, vcur),
                       _mm256_sub_pd(vcur, blendedVector));
    vdelta = _mm256_max_pd(vdelta, vd);

    //find vectorised total delta after iterating through entire matrix (4 dif numbers)
    const int imm1 = 0b01001110;
    vd = _mm256_permute4x64_pd(vdelta, imm1); //value of imm8 should be used here
    vdelta = _mm256_max_pd(vdelta, vd);

    const int imm2 = 0b1111;
    vd = _mm256_shuffle_pd(vdelta, vdelta, imm2); //has lower latency than permute4x64
    vdelta = _mm256_max_pd(vd, vdelta);
    //now vdelta has 4 equal 64bit values, we need one of them

    double delta4[4] = {0, 0, 0, 0};
    _mm256_storeu_pd((double *) &delta4, vdelta);
    //storeu and store do not differ in latency and throughput on processor this programme is meant to be run on
    delta = delta4[0];
    //now delta has required delta from entire matrix iteration

    return delta;
} //returns delta of a string (1 double)

void swapPointers(void** p1, void** p2) {
    void* ptemp = *p1;
    *p1 = *p2;
    *p2 = ptemp;
}

dtype* mainFuncV2(double* iter0, double* iter1, double* iter2, double* iter3, double* iter4, double* gridp,
                const int sizeX, const int sizeY, const int Nt, const double hx, const double hy) {
    /*initially, grid1 is current and grid2 is new*/
    /*but they will be immediately swapped in loop*/
    double* currIter = iter0;
    double* newIter = iter4;
    //iter1-2-3 are used as intermediates

    dtype hxsq = (hx * hx);
    dtype hysq = (hy * hy);

    int gridSize = sizeX * sizeY;

    dtype deltaOld = 1000.0; /*?*/
    dtype deltaNew = 999.0; /*?*/

    double deltaIter4 = 0.0;
    double deltaIter4temp = 0.0;

    //for use in functions within cycle
    const dtype revsqsum = ((1.0 / hxsq) + (1.0 / hysq));
    const dtype generalCoef = 1.0 / (5.0 * revsqsum); // <---
    const dtype c1 = (1.0 / 2.0) * ((5.0 / hxsq) - (1.0 / hysq)); // <---
    const dtype c2 = (1.0 / 2.0) * ((5.0 / hysq) - (1.0 / hxsq)); // <---
    const dtype c3 = (1.0 / 4.0) * revsqsum; // <---
    //const int bmF = 0b0111; //get 1 left from a and 3 right from b // <---
    //const int bmL = 0b1110; //get 3 left from b and 1 right from a // <---

    for (int k = 0; k < (Nt / 4); k++) {
        /*-------------------------------------------------*/
        int redConst = 25;
        if (k % redConst == redConst - 1) {
            printf("%d iterations complete!\n", redConst);
        }
        /*-------------------------------------------------*/
        //printf("iter %d\n", k); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        //rows go from 1 to sizeY-1

        //step 1 - calc left ladder ---------------------------------------------------------------v
        //
        //6
        //4 5
        //1 2 3
        //0 0 0

        //iter 1
        algoStepSingleLine(1, currIter, iter1, gridp, generalCoef, c1, c2, c3, sizeX, sizeY);
        algoStepSingleLine(2, currIter, iter1, gridp, generalCoef, c1, c2, c3, sizeX, sizeY);
        algoStepSingleLine(3, currIter, iter1, gridp, generalCoef, c1, c2, c3, sizeX, sizeY);

        //iter 2
        algoStepSingleLine(1, iter1, iter2, gridp, generalCoef, c1, c2, c3, sizeX, sizeY);
        algoStepSingleLine(2, iter1, iter2, gridp, generalCoef, c1, c2, c3, sizeX, sizeY);

        //iter 3
        algoStepSingleLine(1, iter2, iter3, gridp, generalCoef, c1, c2, c3, sizeX, sizeY);
        //step 1 - calc left ladder ---------------------------------------------------------------^
        //printf("step 1 done\n"); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        //step 2 - go right ---------------------------------------------------------------v
        //4 8 ... sizex-3
        //0 3 7 ... sizex-2
        //0 0 2 6 ... sizex-1
        //0 0 0 1 5 ... sizex-0

        for(int i = 4; i < sizeX - 1; i++) {
            algoStepSingleLine(i - 0, currIter, iter1, gridp, generalCoef, c1, c2, c3, sizeX, sizeY);
            algoStepSingleLine(i - 1, iter1, iter2, gridp, generalCoef, c1, c2, c3, sizeX, sizeY);
            algoStepSingleLine(i - 2, iter2, iter3, gridp, generalCoef, c1, c2, c3, sizeX, sizeY);
            deltaIter4temp = algoStepSingleLineLastIter(i - 3, iter3, newIter, gridp, generalCoef, c1, c2, c3, sizeX, sizeY);
//            if (deltaIter4temp > deltaIter4) {
//                deltaIter4 = deltaIter4temp;
//            }
            deltaIter4 = fmax(deltaIter4, deltaIter4temp);
        }
        //step 2 - go right ---------------------------------------------------------------^
        //printf("step 2 done\n"); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        //step 3 - calc right ladder ---------------------------------------------------------------v
        //... 0 4 5 6
        //... 0 0 2 3
        //... 0 0 0 1
        //... 0 0 0 0

        algoStepSingleLine(sizeX - 2 - 0, iter1, iter2, gridp, generalCoef, c1, c2, c3, sizeX, sizeY);

        algoStepSingleLine(sizeX - 2 - 1, iter2, iter3, gridp, generalCoef, c1, c2, c3, sizeX, sizeY);
        algoStepSingleLine(sizeX - 2 - 0, iter2, iter3, gridp, generalCoef, c1, c2, c3, sizeX, sizeY);

        double delta1 = algoStepSingleLineLastIter(sizeX - 2 - 2, iter3, newIter, gridp, generalCoef, c1, c2, c3, sizeX, sizeY);
        double delta2 = algoStepSingleLineLastIter(sizeX - 2 - 1, iter3, newIter, gridp, generalCoef, c1, c2, c3, sizeX, sizeY);
        double delta3 = algoStepSingleLineLastIter(sizeX - 2 - 0, iter3, newIter, gridp, generalCoef, c1, c2, c3, sizeX, sizeY);
        deltaNew = fmax(fmax(delta1, delta2), fmax(delta3, deltaIter4));
        //step 3 - calc right ladder ---------------------------------------------------------------^
        //printf("step 3 done\n"); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        swapPointers((void**)&currIter, (void**)&newIter);
        /*step*/

        /*check if new < old*/
        if (deltaNew >= deltaOld) {
            deltaOld = deltaNew;
            printf("\nSomething is going wrong, delta increasing\n");
        }
        else {
            /*nothing*/
        } /*surely compiler will cut "else{}" part out*/
    }

    return currIter;
} /*returns pointer to last iteration grid*/

void* myAlignedAlloc(int alignment, size_t size, void** forFree) {
    void *mem = malloc(size + (alignment -1));
    if (NULL == mem) {
        return NULL;
    }

    char* temp = (char*)mem;
    size_t rest = (size_t)temp % alignment;
    if (0 != rest) {
        temp += (alignment - rest);
    }
    *forFree = mem;
    return temp;
}

int allocateMemory(double** i0, double** i1, double** i2, double** i3, double** i4, int Nx, int Ny, int padding) {
    *i0 = (double*)malloc((Nx * Ny + padding) * sizeof(double));
    if (*i0 == NULL) {
        return(1);
    }
    *i1 = (double*)malloc((Nx * Ny + padding) * sizeof(double));
    if (*i1 == NULL) {
        free(*i0);
        return(1);
    }
    *i2 = (double*)malloc((Nx * Ny + padding) * sizeof(double));
    if (*i2 == NULL) {
        free(*i0);
        free(*i1);
        return(1);
    }
    *i3 = (double*)malloc((Nx * Ny + padding) * sizeof(double));
    if (*i3 == NULL) {
        free(*i0);
        free(*i1);
        free(*i2);
        return(1);
    }
    *i4 = (double*)malloc((Nx * Ny + padding) * sizeof(double));
    if (*i4 == NULL) {
        free(*i0);
        free(*i1);
        free(*i2);
        free(*i3);
        return(1);
    }
    return 0;
}

int main(int argc, char** argv) {
    /*int Nx = atoi(argv[1]);*/ /*j >*/
    /*int Ny = atoi(argv[2]); */ /*i ^*/
    /*int Nt = atoi(argv[3]); */

    int Nx = 8000; /*j >*/
    int Ny = 8000; /*i ^*/
    int Nt = 100;

    dtype hx = (XB - XA) / (Nx - 1);
    dtype hy = (YB - YA) / (Ny - 1);

    /*Xj = X1 + j*hx  */
    /*Yi = Y1 + i*hy  */

    /*n = 0,1,...Nt  */

    double* forFreeIter0;
    double* forFreeIter1;
    double* forFreeIter2;
    double* forFreeIter3;
    double* forFreeIter4;
    const int padding = 2; //one before and one after (in sizeof(double), not sizeof(char))

    if(0 != allocateMemory(&forFreeIter0, &forFreeIter1, &forFreeIter2, &forFreeIter3, &forFreeIter4, Nx, Ny, 2)) {
        printf("Insufficient memory for iteration matrix allocation\n");
        return -1;
    }

    double* iter0 = forFreeIter0 + 1;
    double* iter1 = forFreeIter1 + 1;
    double* iter2 = forFreeIter2 + 1;
    double* iter3 = forFreeIter3 + 1;
    double* iter4 = forFreeIter4 + 1;

    dtype* pgrid;
    pgrid = (double*)malloc(Nx * Ny * sizeof(double));
    if (pgrid == NULL) {
        free(forFreeIter0);
        free(forFreeIter1);
        free(forFreeIter2);
        free(forFreeIter3);
        free(forFreeIter4);
        printf("Insufficient memory for coefficient matrix allocation\n");
        return(-1);
    }

    initXYgrid(iter0, Nx, Ny);
    initXYgrid(iter1, Nx, Ny);
    initXYgrid(iter2, Nx, Ny);
    initXYgrid(iter3, Nx, Ny);
    initXYgrid(iter4, Nx, Ny);
    initPgrid(pgrid, Nx, Ny, hx, hy);

    printf("Allocation and initialization complete!\n"); /*-------------------------------------*/

    dtype* newGrid = mainFuncV2(iter0, iter1, iter2, iter3, iter4, pgrid, Nx, Ny, Nt, hx, hy);

    printf("Calculation complete!\n"); /*-------------------------------------*/

    FILE* fileForOutput = fopen("outputFile1.txt", "wb");
    fwrite(&(newGrid[0]), sizeof(dtype), Nx * Ny, fileForOutput);

    printf("Output to file complete!\n"); /*-------------------------------------*/

    fclose(fileForOutput);

    free(forFreeIter0);
    free(forFreeIter1);
    free(forFreeIter2);
    free(forFreeIter3);
    free(forFreeIter4);
    free(pgrid);

    printf("Deallocation complete!\n"); /*-------------------------------------*/
}
