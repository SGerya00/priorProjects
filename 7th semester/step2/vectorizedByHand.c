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

__m256d algoStep(int sizeX, int sizeY, int i, int j,
                 double* XYgridCurr, const double* pgrid,
                 __m256d vgeneralCoef,
                 __m256d vpcoef1, __m256d vpcoef2,
                 __m256d vcoef1, __m256d vcoef2, __m256d vcoef3,
                 __m256d vcur) {
    printf("in algostep\n"); /*!!!*/
    const int fidx11 = convertGridCoordsToIdx(sizeX, sizeY, i, j);

    const int fidx1m1 = convertGridCoordsToIdx(sizeX, sizeY, i, j - 1);
    const int fidx1p1 = convertGridCoordsToIdx(sizeX, sizeY, i, j + 1);

    const int fidxm11 = convertGridCoordsToIdx(sizeX, sizeY, i - 1, j);
    const int fidxp11 = convertGridCoordsToIdx(sizeX, sizeY, i + 1, j);

    const int fidxm1m1 = convertGridCoordsToIdx(sizeX, sizeY, i - 1, j - 1);
    const int fidxm1p1 = convertGridCoordsToIdx(sizeX, sizeY, i - 1, j + 1);
    const int fidxp1m1 = convertGridCoordsToIdx(sizeX, sizeY, i + 1, j - 1);
    const int fidxp1p1 = convertGridCoordsToIdx(sizeX, sizeY, i + 1, j + 1);
    printf("in algostep, all indexes calculated\n"); /*!!!*/
    /*summands 1,2,3---------------------------------------------------v*/
    __m256d V_1_1 = _mm256_loadu_pd(&(XYgridCurr[fidxm1m1])); //!!!!!!!!!
    __m256d V0_1 = _mm256_loadu_pd(&(XYgridCurr[fidx1m1]));   //!!!!!!!!!
    __m256d V1_1 = _mm256_loadu_pd(&(XYgridCurr[fidxp1m1]));  //!!!!!!!!! unaligned

    __m256d V_10 = _mm256_loadu_pd(&(XYgridCurr[fidxm11]));   //!! moving in 4s => aligned
    __m256d V00 = vcur;                                         //!! moving in 4s => aligned
    __m256d V10 = _mm256_loadu_pd(&(XYgridCurr[fidxp11]));    //!! moving in 4s => aligned

    __m256d V_11 = _mm256_loadu_pd(&(XYgridCurr[fidxm1p1]));  //!!!!!!!!!
    __m256d V01 = _mm256_loadu_pd(&(XYgridCurr[fidx1p1]));    //!!!!!!!!!
    __m256d V11 = _mm256_loadu_pd(&(XYgridCurr[fidxp1p1]));   //!!!!!!!!! unaligned
    printf("in algostep, all loaded\n"); /*!!!*/
    __m256d vsum1 = _mm256_add_pd(V0_1, V01);
    __m256d vsum2 = _mm256_add_pd(V10, V_10);
    __m256d vsum3 = _mm256_add_pd(
                                  _mm256_add_pd(V_1_1,
                                                   V_11),
                                  _mm256_add_pd(V1_1,
                                                   V11)
                                  );

    __m256d vsum123 = _mm256_add_pd(_mm256_mul_pd(vcoef3,
                                                     vsum3),
                                    _mm256_add_pd(_mm256_mul_pd(vcoef2,
                                                                      vsum2),
                                                     _mm256_mul_pd(vcoef1,
                                                                      vsum1)
                                                     )
                                    );
    printf("in algostep, summands123 calculated\n"); /*!!!*/
    /*summands 1,2,3---------------------------------------------------^*/
    /*summand 4---------------------------------------------------v*/
    __m256d vp10 = _mm256_loadu_pd(&(pgrid[fidx11]));  //!! moving in 4s => aligned
    __m256d vp11 = _mm256_loadu_pd(&(pgrid[fidxm11])); //!! moving in 4s => aligned
    __m256d vp12 = _mm256_loadu_pd(&(pgrid[fidxp11])); //!! moving in 4s => aligned
    __m256d vp13 = _mm256_loadu_pd(&(pgrid[fidx1m1])); //!!!!!!!!!
    __m256d vp14 = _mm256_loadu_pd(&(pgrid[fidx1p1])); //!!!!!!!!! unaligned
    printf("in algostep, vp loaded\n"); /*!!!*/
    __m256d vsum4 = _mm256_add_pd(_mm256_mul_pd(vpcoef1,
                                                   vp10),
                                  _mm256_mul_pd(vpcoef2,
                                                   _mm256_add_pd(_mm256_add_pd(vp11,
                                                                                     vp12),
                                                                    _mm256_add_pd(vp13,
                                                                                     vp14)
                                                                    )
                                                   )
                                  );
    /*summand 4---------------------------------------------------^*/
    __m256d vresult = _mm256_mul_pd(vgeneralCoef,
                                    _mm256_add_pd(vsum123,
                                                     vsum4)
                                    );
    printf("out algostep\n"); /*!!!*/
    return vresult;
} //return element that should be added to matrix on corresponding ij

dtype algoStepsV2vectorised(dtype* XYgridCurr, dtype* XYgridNew, dtype const* pgrid,
                            dtype hxsq, dtype hysq, int sizeX, int sizeY) {
    const dtype revsqsum = ((1.0 / hxsq) + (1.0 / hysq));
    const dtype generalCoef = 1.0 / (5.0 * revsqsum);
    __m256d vgeneralCoef = _mm256_set1_pd(generalCoef);

    __m256d* vXYgridCurr = (__m256d*)XYgridCurr;
    __m256d* vXYgridNew = (__m256d*)XYgridNew;
    //__m256d* vpgrid = (__m256d*)pgrid;

    const dtype coef1 = (1.0 / 2.0) * ((5.0 / hxsq) - (1.0 / hysq));
    const dtype coef2 = (1.0 / 2.0) * ((5.0 / hysq) - (1.0 / hxsq));
    const dtype coef3 = (1.0 / 4.0) * revsqsum;

    __m256d vcoef1 = _mm256_set1_pd(coef1);
    __m256d vcoef2 = _mm256_set1_pd(coef2);
    __m256d vcoef3 = _mm256_set1_pd(coef3);

    __m256d vpcoef1 = _mm256_set1_pd(2.0);
    __m256d vpcoef2 = _mm256_set1_pd(0.25);

    dtype delta = 0.0;

    __m256d vdelta = _mm256_set1_pd(0.0);
    __m256d vd = _mm256_set1_pd(0.0);

    __m256d blendMaskFirst = _mm256_set_pd(0.0, 1.0, 1.0, 1.0);
    __m256d blendMaskLast = _mm256_set_pd(1.0, 1.0, 1.0, 0.0);

    //printf("preps before loop are done!\n"); /*!!!*/
    /*main loop --------------------------------------------------------------------v*/
    for (int i = 1; i < (sizeY - 1); i++) {
        //printf("in algostep, row %d\n", i); /*!!!*/
        //first step
        int fidxcur = convertGridCoordsToIdx(sizeX, sizeY, i, 0);
        //printf("before loading\n"); /*!!!*/
//        if(i == 2000) {
//            printf("before loading\n"); /*!!!*/
//        }
        __m256d vcur = _mm256_loadu_pd(&(XYgridCurr[fidxcur])); //!! moving in 4s => aligned
        //printf("before algostep 1\n"); /*!!!*/
//        if(i == 2000) {
//            printf("before algostep 1\n"); /*!!!*/
//        }
        //__m256d resFirst = algoStep(sizeX, sizeY, i, 0, XYgridCurr, pgrid, vgeneralCoef,
        //                            vpcoef1, vpcoef2, vcoef1, vcoef2, vcoef3, vcur);
        // re re re
        int fidx11 = convertGridCoordsToIdx(sizeX, sizeY, i, 0);

        int fidx1m1 = convertGridCoordsToIdx(sizeX, sizeY, i, 0 - 1);
        int fidx1p1 = convertGridCoordsToIdx(sizeX, sizeY, i, 0 + 1);

        int fidxm11 = convertGridCoordsToIdx(sizeX, sizeY, i - 1, 0);
        int fidxp11 = convertGridCoordsToIdx(sizeX, sizeY, i + 1, 0);

        int fidxm1m1 = convertGridCoordsToIdx(sizeX, sizeY, i - 1, 0 - 1);
        int fidxm1p1 = convertGridCoordsToIdx(sizeX, sizeY, i - 1, 0 + 1);
        int fidxp1m1 = convertGridCoordsToIdx(sizeX, sizeY, i + 1, 0 - 1);
        int fidxp1p1 = convertGridCoordsToIdx(sizeX, sizeY, i + 1, 0 + 1);
        /*summands 1,2,3---------------------------------------------------v*/
        __m256d V_1_1 = _mm256_loadu_pd(&(XYgridCurr[fidxm1m1])); //!!!!!!!!!
        __m256d V0_1 = _mm256_loadu_pd(&(XYgridCurr[fidx1m1]));   //!!!!!!!!!
        __m256d V1_1 = _mm256_loadu_pd(&(XYgridCurr[fidxp1m1]));  //!!!!!!!!! unaligned

        __m256d V_10 = _mm256_loadu_pd(&(XYgridCurr[fidxm11]));   //!! moving in 4s => aligned
        __m256d V00 = vcur;                                         //!! moving in 4s => aligned
        __m256d V10 = _mm256_loadu_pd(&(XYgridCurr[fidxp11]));    //!! moving in 4s => aligned

        __m256d V_11 = _mm256_loadu_pd(&(XYgridCurr[fidxm1p1]));  //!!!!!!!!!
        __m256d V01 = _mm256_loadu_pd(&(XYgridCurr[fidx1p1]));    //!!!!!!!!!
        __m256d V11 = _mm256_loadu_pd(&(XYgridCurr[fidxp1p1]));   //!!!!!!!!! unaligned

        __m256d vsum1 = _mm256_add_pd(V0_1, V01);
        __m256d vsum2 = _mm256_add_pd(V10, V_10);
        __m256d vsum3 = _mm256_add_pd(
                _mm256_add_pd(V_1_1,
                              V_11),
                _mm256_add_pd(V1_1,
                              V11)
        );

//        __m256d vsum123 = _mm256_add_pd(_mm256_mul_pd(vcoef3,
//                                                      vsum3),
//                                        _mm256_add_pd(_mm256_mul_pd(vcoef2,
//                                                                    vsum2),
//                                                      _mm256_mul_pd(vcoef1,
//                                                                    vsum1)
//                                        )
//        );
        __m256d vsum123 = _mm256_fmadd_pd(vcoef3,
                                  vsum3,
                                  _mm256_fmadd_pd(vcoef2,
                                                  vsum2,
                                                  _mm256_mul_pd(vcoef1,
                                                                vsum1)
                                  )
        );
        /*summands 1,2,3---------------------------------------------------^*/
        /*summand 4---------------------------------------------------v*/
        __m256d vp10 = _mm256_loadu_pd(&(pgrid[fidx11]));  //!! moving in 4s => aligned
        __m256d vp11 = _mm256_loadu_pd(&(pgrid[fidxm11])); //!! moving in 4s => aligned
        __m256d vp12 = _mm256_loadu_pd(&(pgrid[fidxp11])); //!! moving in 4s => aligned
        __m256d vp13 = _mm256_loadu_pd(&(pgrid[fidx1m1])); //!!!!!!!!!
        __m256d vp14 = _mm256_loadu_pd(&(pgrid[fidx1p1])); //!!!!!!!!! unaligned

//        __m256d vsum4 = _mm256_add_pd(_mm256_mul_pd(vpcoef1,
//                                                    vp10),
//                                      _mm256_mul_pd(vpcoef2,
//                                                    _mm256_add_pd(_mm256_add_pd(vp11,
//                                                                                vp12),
//                                                                  _mm256_add_pd(vp13,
//                                                                                vp14)
//                                                    )
//                                      )
//        );
        __m256d vsum4 = _mm256_fmadd_pd(vpcoef1,
                                        vp10,
                                        _mm256_mul_pd(vpcoef2,
                                                         _mm256_add_pd(_mm256_add_pd(vp11,
                                                                                           vp12),
                                                                          _mm256_add_pd(vp13,
                                                                                           vp14)
                                                                          )
                                                         )
                                        );
        /*summand 4---------------------------------------------------^*/
        __m256d vresult = _mm256_mul_pd(vgeneralCoef,
                                        _mm256_add_pd(vsum123,
                                                      vsum4)
        );
        __m256d resFirst = vresult;
        // re re re
        //printf("after algostep 1\n"); /*!!!*/
//        if(i == 2000) {
//            printf("after algostep 1\n"); /*!!!*/
//        }
        __m256d blendedVector = _mm256_blendv_pd(vcur, resFirst, blendMaskFirst);
        //get leftmost from X, 3 others from Y
        vXYgridNew[(fidxcur / 4)] = blendedVector; //write

        //calculate delta
        vd = _mm256_max_pd(_mm256_sub_pd(vXYgridNew[(fidxcur / 4)], vXYgridCurr[(fidxcur / 4)]),
                           _mm256_sub_pd(vXYgridCurr[(fidxcur / 4)], vXYgridNew[(fidxcur / 4)]));
        vdelta = _mm256_max_pd(vdelta, vd);
        //
        //printf("first elems are done!\n"); /*!!!*/
        //printf("now iterating through the rest!\n"); /*!!!*/
//        if(i == 2000) {
//            printf("now trying the rest\n"); /*!!!*/
//        }
        for (int k = 1; k < ((sizeX / 4) - 1); k++) {
//            if(i == 2000) {
//                printf("k: %d\nfidxcur: %d\n", k, fidxcur); /*!!!*/
//            }
            int j = k * 4; //256 / sizeof(dtype)
            fidxcur += 4;
            vcur = _mm256_loadu_pd(&(XYgridCurr[fidxcur])); //!! moving in 4s => aligned
//            if(i == 2000) {
//                printf("load %d successful\n", k); /*!!!*/
//            }
            //__m256d res = algoStep(sizeX, sizeY, i, j, XYgridCurr, pgrid, vgeneralCoef,
            //                       vpcoef1, vpcoef2, vcoef1, vcoef2, vcoef3, vcur);
            // re re re
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
            V_1_1 = _mm256_loadu_pd(&(XYgridCurr[fidxm1m1])); //!!!!!!!!!
            V0_1 = _mm256_loadu_pd(&(XYgridCurr[fidx1m1]));   //!!!!!!!!!
            V1_1 = _mm256_loadu_pd(&(XYgridCurr[fidxp1m1]));  //!!!!!!!!! unaligned

            V_10 = _mm256_loadu_pd(&(XYgridCurr[fidxm11]));   //!! moving in 4s => aligned
            V00 = vcur;                                         //!! moving in 4s => aligned
            V10 = _mm256_loadu_pd(&(XYgridCurr[fidxp11]));    //!! moving in 4s => aligned

            V_11 = _mm256_loadu_pd(&(XYgridCurr[fidxm1p1]));  //!!!!!!!!!
            V01 = _mm256_loadu_pd(&(XYgridCurr[fidx1p1]));    //!!!!!!!!!
            V11 = _mm256_loadu_pd(&(XYgridCurr[fidxp1p1]));   //!!!!!!!!! unaligned
//            if(i == 2000) {
//                printf("load Vs %d successful\n", k); /*!!!*/
//            }

            vsum1 = _mm256_add_pd(V0_1, V01);
            vsum2 = _mm256_add_pd(V10, V_10);
            vsum3 = _mm256_add_pd(
                    _mm256_add_pd(V_1_1,
                                  V_11),
                    _mm256_add_pd(V1_1,
                                  V11)
            );

//            vsum123 = _mm256_add_pd(_mm256_mul_pd(vcoef3,
//                                                          vsum3),
//                                            _mm256_add_pd(_mm256_mul_pd(vcoef2,
//                                                                        vsum2),
//                                                          _mm256_mul_pd(vcoef1,
//                                                                        vsum1)
//                                            )
//            );
            vsum123 = _mm256_fmadd_pd(vcoef3,
                                      vsum3,
                                      _mm256_fmadd_pd(vcoef2,
                                                         vsum2,
                                                         _mm256_mul_pd(vcoef1,
                                                                          vsum1)
                                                         )
                                      );
//            if(i == 2000) {
//                printf("summands 1,2,3 %d successful\n", k); /*!!!*/
//            }
            /*summands 1,2,3---------------------------------------------------^*/
            /*summand 4---------------------------------------------------v*/
            vp10 = _mm256_loadu_pd(&(pgrid[fidx11]));  //!! moving in 4s => aligned
            vp11 = _mm256_loadu_pd(&(pgrid[fidxm11])); //!! moving in 4s => aligned
            vp12 = _mm256_loadu_pd(&(pgrid[fidxp11])); //!! moving in 4s => aligned
            vp13 = _mm256_loadu_pd(&(pgrid[fidx1m1])); //!!!!!!!!!
            vp14 = _mm256_loadu_pd(&(pgrid[fidx1p1])); //!!!!!!!!! unaligned
//            if(i == 2000) {
//                printf("load Ps %d successful\n", k); /*!!!*/
//            }

//            vsum4 = _mm256_add_pd(_mm256_mul_pd(vpcoef1,
//                                                        vp10),
//                                          _mm256_mul_pd(vpcoef2,
//                                                        _mm256_add_pd(_mm256_add_pd(vp11,
//                                                                                    vp12),
//                                                                      _mm256_add_pd(vp13,
//                                                                                    vp14)
//                                                        )
//                                          )
//            );
            vsum4 = _mm256_fmadd_pd(vpcoef1,
                                    vp10,
                                    _mm256_mul_pd(vpcoef2,
                                                     _mm256_add_pd(_mm256_add_pd(vp11,
                                                                                       vp12),
                                                                      _mm256_add_pd(vp13,
                                                                                       vp14)
                                                                      )
                                                     )
                                    );
            /*summand 4---------------------------------------------------^*/
//            if(i == 2000) {
//                printf("summand 4 %d successful\n", k); /*!!!*/
//            }
            vresult = _mm256_mul_pd(vgeneralCoef,
                                            _mm256_add_pd(vsum123,
                                                          vsum4)
            );
//            if(i == 2000) {
//                printf("result %d successful\n", k); /*!!!*/
//            }
            vXYgridNew[(fidxcur / 4)] = vresult;
//            if(i == 2000) {
//                printf("write to vectorised matrix %d successful\n", k); /*!!!*/
//            }
            // re re re
//            if(i == 2000) {
//                printf("iteration %d successful\n", k); /*!!!*/
//            }
            //calculate delta
            vd = _mm256_max_pd(_mm256_sub_pd(vXYgridNew[(fidxcur / 4)], vXYgridCurr[(fidxcur / 4)]),
                               _mm256_sub_pd(vXYgridCurr[(fidxcur / 4)], vXYgridNew[(fidxcur / 4)]));
            vdelta = _mm256_max_pd(vdelta, vd);
            //
        }
        //printf("all elems except last are done!\n"); /*!!!*/
//        if(i == 2000) {
//            printf("all elems except last are done!\n"); /*!!!*/
//        }
        //last step
        fidxcur += 4;
        vcur = _mm256_loadu_pd(&(XYgridCurr[fidxcur])); //!! moving in 4s => aligned
        //__m256d resLast = algoStep(sizeX, sizeY, i, sizeX - 4, XYgridCurr, pgrid, vgeneralCoef,
        //                           vpcoef1, vpcoef2, vcoef1, vcoef2, vcoef3, vcur);
        // re re re
        //printf("in algostep last\n"); /*!!!*/
//        if(i == 2000) {
//            printf("in algostep last\n"); /*!!!*/
//        }
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
        V_1_1 = _mm256_loadu_pd(&(XYgridCurr[fidxm1m1])); //!!!!!!!!!
        V0_1 = _mm256_loadu_pd(&(XYgridCurr[fidx1m1]));   //!!!!!!!!!
        V1_1 = _mm256_loadu_pd(&(XYgridCurr[fidxp1m1]));  //!!!!!!!!! unaligned

        V_10 = _mm256_loadu_pd(&(XYgridCurr[fidxm11]));   //!! moving in 4s => aligned
        V00 = vcur;                                         //!! moving in 4s => aligned
        V10 = _mm256_loadu_pd(&(XYgridCurr[fidxp11]));    //!! moving in 4s => aligned

        V_11 = _mm256_loadu_pd(&(XYgridCurr[fidxm1p1]));  //!!!!!!!!!
        V01 = _mm256_loadu_pd(&(XYgridCurr[fidx1p1]));    //!!!!!!!!!
        V11 = _mm256_loadu_pd(&(XYgridCurr[fidxp1p1]));   //!!!!!!!!! unaligned

        vsum1 = _mm256_add_pd(V0_1, V01);
        vsum2 = _mm256_add_pd(V10, V_10);
        vsum3 = _mm256_add_pd(
                _mm256_add_pd(V_1_1,
                              V_11),
                _mm256_add_pd(V1_1,
                              V11)
        );

//        vsum123 = _mm256_add_pd(_mm256_mul_pd(vcoef3,
//                                              vsum3),
//                                _mm256_add_pd(_mm256_mul_pd(vcoef2,
//                                                            vsum2),
//                                              _mm256_mul_pd(vcoef1,
//                                                            vsum1)
//                                )
//        );
        vsum123 = _mm256_fmadd_pd(vcoef3,
                                  vsum3,
                                  _mm256_fmadd_pd(vcoef2,
                                                     vsum2,
                                                     _mm256_mul_pd(vcoef1,
                                                                      vsum1)
                                                     )
                                  );
        /*summands 1,2,3---------------------------------------------------^*/
        /*summand 4---------------------------------------------------v*/
        vp10 = _mm256_loadu_pd(&(pgrid[fidx11]));  //!! moving in 4s => aligned
        vp11 = _mm256_loadu_pd(&(pgrid[fidxm11])); //!! moving in 4s => aligned
        vp12 = _mm256_loadu_pd(&(pgrid[fidxp11])); //!! moving in 4s => aligned
        vp13 = _mm256_loadu_pd(&(pgrid[fidx1m1])); //!!!!!!!!!
        vp14 = _mm256_loadu_pd(&(pgrid[fidx1p1])); //!!!!!!!!! unaligned

//        vsum4 = _mm256_add_pd(_mm256_mul_pd(vpcoef1,
//                                            vp10),
//                              _mm256_mul_pd(vpcoef2,
//                                            _mm256_add_pd(_mm256_add_pd(vp11,
//                                                                        vp12),
//                                                          _mm256_add_pd(vp13,
//                                                                        vp14)
//                                            )
//                              )
//        );
        vsum4 = _mm256_fmadd_pd(vpcoef1,
                                vp10,
                                _mm256_mul_pd(vpcoef2,
                                              _mm256_add_pd(_mm256_add_pd(vp11,
                                                                          vp12),
                                                            _mm256_add_pd(vp13,
                                                                          vp14)
                                              )
                                )
        );
        /*summand 4---------------------------------------------------^*/
        vresult = _mm256_mul_pd(vgeneralCoef,
                                _mm256_add_pd(vsum123,
                                              vsum4)
        );
        //printf("out algostep last\n"); /*!!!*/
//        if(i == 2000) {
//            printf("out algostep last\n"); /*!!!*/
//        }
        __m256d resLast = vresult;
        // re re re
        blendedVector = _mm256_blendv_pd(vcur, resLast, blendMaskLast);
        //get rightmost from X, 3 others from Y
        vXYgridNew[(fidxcur / 4)] = blendedVector; //write

        //calculate delta
        vd = _mm256_max_pd(_mm256_sub_pd(vXYgridNew[(fidxcur / 4)], vXYgridCurr[(fidxcur / 4)]),
                           _mm256_sub_pd(vXYgridCurr[(fidxcur / 4)], vXYgridNew[(fidxcur / 4)]));
        vdelta = _mm256_max_pd(vdelta, vd);
        //printf("last elems are done!\n"); /*!!!*/
//        if(i == 2000) {
//            printf("last elems are done!\n"); /*!!!*/
//        }
        //
    }
    /*main loop --------------------------------------------------------------------^*/

    //find vectorised total delta after iterating through entire matrix (4 dif numbers)
    int imm8 = 0b01001110;
    vd = _mm256_permute4x64_pd(vdelta, 0b01001110); //value of imm8 should be used here
    vdelta = _mm256_max_pd(vdelta, vd);

    imm8 = 0b1111;
    vd = _mm256_shuffle_pd(vdelta, vdelta, 0b1111); //has lower latency than permute4x64
    vdelta = _mm256_max_pd(vd, vdelta);
    //now vdelta has 4 equal 64bit values, we need one of them

    double delta4[4] = {0, 0, 0, 0};
    _mm256_storeu_pd((double *) &delta4, vdelta);
    //storeu and store do not differ in latency and throughput on processor this programme is meant to be run on
    delta = delta4[0];
    //now delta has required delta from entire matrix iteration

    return delta;
}

void swapPointers(void** p1, void** p2) {
    void* ptemp = *p1;
    *p1 = *p2;
    *p2 = ptemp;
}

dtype* mainFunc(dtype* grid1, dtype* grid2, dtype* gridp,
                int sizeX, int sizeY, int Nt, dtype hx, dtype hy) {
    /*initially, grid1 is current and grid2 is new*/
    /*but they will be immediately swapped in loop*/
    dtype* currg = grid2;
    dtype* newg = grid1;

    dtype hxsq = (hx * hx);
    dtype hysq = (hy * hy);

    int gridSize = sizeX * sizeY;

    dtype deltaOld = 1000.0; /*?*/
    dtype deltaNew = 999.0; /*?*/

    for (int k = 0; k < Nt; k++) {
        /*-------------------------------------------------*/
        int redConst = 50;
        if (k % redConst == redConst - 1) {
            printf("%d iterations complete!\n", redConst);
        }
        /*-------------------------------------------------*/
        swapPointers((void**)&currg, (void**)&newg);
        /*step*/
        //printf("ITERATION %d:\n", k); /*!!!*/
        deltaNew = algoStepsV2vectorised(currg, newg, gridp, hxsq, hysq, sizeX, sizeY);

        /*check if new < old*/
        if (deltaNew >= deltaOld) {
            printf("\nSomething is going wrong, delta increasing\n");
        }
        else {
            /*nothing*/
        } /*surely compiler will cut "else{}" part out*/
    }

    return newg;
} /*returns pointer to last iteration grid*/

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

    //dtype* XYgrid1 = (dtype*)malloc(Nx * Ny * sizeof(dtype));
    //dtype* XYgrid2 = (dtype*)malloc(Nx * Ny * sizeof(dtype));
    dtype* XYgrid1; //this pointer is the one that shall be freed
    XYgrid1 = (double*)malloc(2 * Nx * Ny * sizeof(double)); //!!!! unaligned
    if (XYgrid1 == NULL) {
        exit(1);
    }
    dtype* XYgrid2 = XYgrid1 + (Nx * Ny); //do not free with this pointer

    initXYgrid(XYgrid1, Nx, Ny);
    initXYgrid(XYgrid2, Nx, Ny);

    //dtype* pgrid = (dtype*)malloc(Nx * Ny * sizeof(dtype));
    dtype* pgrid;
    pgrid = (double*)malloc(Nx * Ny * sizeof(double)); //!!!! unaligned
    if (pgrid == NULL) {
        free(XYgrid1);
        exit(1);
    }
    initPgrid(pgrid, Nx, Ny, hx, hy);

    printf("Allocation and initialization complete!\n"); /*-------------------------------------*/

    dtype* newGrid = mainFunc(XYgrid1, XYgrid2, pgrid, Nx, Ny, Nt, hx, hy);

    printf("Calculation complete!\n"); /*-------------------------------------*/

    FILE* fileForOutput = fopen("outputFile1.txt", "wb");
    fwrite(&(newGrid[0]), sizeof(dtype), Nx * Ny, fileForOutput);

    printf("Output to file complete!\n"); /*-------------------------------------*/

    fclose(fileForOutput);

    free(pgrid);
    free(XYgrid1);
    //free(XYgrid2);

    printf("Deallocation complete!\n"); /*-------------------------------------*/
}
