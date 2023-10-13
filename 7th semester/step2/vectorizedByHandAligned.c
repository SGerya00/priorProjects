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

dtype algoStepsV2vectorised(dtype* XYgridCurr, dtype* XYgridNew, dtype const* pgrid,
                            dtype hxsq, dtype hysq, int sizeX, int sizeY) {
    const dtype revsqsum = ((1.0 / hxsq) + (1.0 / hysq));
    const dtype generalCoef = 1.0 / (5.0 * revsqsum);
    __m256d vgeneralCoef = _mm256_set1_pd(generalCoef);

    __m256d* vXYgridCurr = (__m256d*)XYgridCurr;
    __m256d* vXYgridNew = (__m256d*)XYgridNew;

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

    const int blendMaskFirst = 0b0111; //get 1 left from a and 3 right from b
    const int blendMaskLast = 0b1110; //get 3 left from b and 1 right from a
    const int permuteMask255_192to63_0 = 0b10010011; //reorder abcd to bcda
    const int permuteMask0_63to255_192 = 0b00111001; //reorder abcd to dabc

    __m256d zeroVector = _mm256_setzero_pd();

    __m256d VprevB;
    __m256d VprevC;
    __m256d VprevT;

    __m256d VcurB;
    __m256d VcurC;
    __m256d VcurT;

    __m256d VprevP;
    __m256d VcurP;

    //printf("preps before loop are done!\n"); /*!!!*/
    /*main loop --------------------------------------------------------------------v*/
    for (int i = 1; i < (sizeY - 1); i++) {
        //printf("in algostep, row %d\n", i); /*!!!*/
        //first step
        int fidxcur = convertGridCoordsToIdx(sizeX, sizeY, i, 0);
        //printf("before loading\n"); /*!!!*/
        __m256d vcur = _mm256_load_pd(&(XYgridCurr[fidxcur])); //!moving in 4s => aligned
        //printf("before algostep 1\n"); /*!!!*/
        // re re re
        int fidx11 = convertGridCoordsToIdx(sizeX, sizeY, i, 0);

        int fidxm11 = convertGridCoordsToIdx(sizeX, sizeY, i - 1, 0);
        int fidxp11 = convertGridCoordsToIdx(sizeX, sizeY, i + 1, 0);

        /*summands 1,2,3---------------------------------------------------v*/
        //!load middle vectors in an aligned manner -------------------------------------------------------------------v
        __m256d V_10 = _mm256_load_pd(&(XYgridCurr[fidxm11]));   //!! moving in 4s => aligned
        __m256d V00 = vcur;                                         //!! moving in 4s => aligned
        __m256d V10 = _mm256_load_pd(&(XYgridCurr[fidxp11]));    //!! moving in 4s => aligned
        //!load middle vectors in an aligned manner -------------------------------------------------------------------^

        //!load left vectors in an aligned manner ---------------------------------------------------------------------v
        __m256d V_1_4 = zeroVector; //below current, prev step (step = 4) //imitation
        __m256d V_1_1 = _mm256_blend_pd(V_1_4, V_10, blendMaskFirst); //get 1 right from x and 3 left from y
	    //now we have [a1 a2 a3 0] that we need to turn to [0 a1 a2 a3] where a is V_10
        V_1_1 = _mm256_permute4x64_pd(V_1_1, permuteMask255_192to63_0);

        __m256d V0_4 = zeroVector; //current level, prev step (step = 4) //imitation
        __m256d V0_1 = _mm256_blend_pd(V0_4, V00, blendMaskFirst); //get 1 right from x and 3 left from y
	    V0_1 = _mm256_permute4x64_pd(V0_1, permuteMask255_192to63_0);

        __m256d V1_4 = zeroVector; //above current, prev step (step = 4) //imitation
        __m256d V1_1 = _mm256_blend_pd(V1_4, V10, blendMaskFirst); //get 1 right from x and 3 left from y
	    V1_1 = _mm256_permute4x64_pd(V1_1, permuteMask255_192to63_0);
        //!load left vectors in an aligned manner ---------------------------------------------------------------------^

        //!load right vectors in an aligned manner --------------------------------------------------------------------v
        __m256d V_14 = _mm256_load_pd(&(XYgridCurr[fidxm11 + 4])); //below current, next step (step = 4)
        __m256d V_11 = _mm256_blend_pd(V_14, V_10, blendMaskLast); //get 1 left from x and 3 right from y
	    //now we have [a1 b2 b3 b4] that we need to reorder to [b2 b3 b4 a1] where a is V_14 b is V_10
	    V_11 = _mm256_permute4x64_pd(V_11, permuteMask0_63to255_192);

        __m256d V04 = _mm256_load_pd(&(XYgridCurr[fidx11 + 4])); //current level, next step (step = 4)
        __m256d V01 = _mm256_blend_pd(V04, V00, blendMaskLast); //get 1 left from x and 3 right from y
	    V01 = _mm256_permute4x64_pd(V01, permuteMask0_63to255_192);

        __m256d V14 = _mm256_load_pd(&(XYgridCurr[fidxp11 + 4])); //above current, next step (step = 4)
        __m256d V11 = _mm256_blend_pd(V14, V10, blendMaskLast); //get 1 left from x and 3 right from y
        V11 = _mm256_permute4x64_pd(V11, permuteMask0_63to255_192);
	    //!load right vectors in an aligned manner --------------------------------------------------------------------^

        __m256d vsum1 = _mm256_add_pd(V0_1, V01);
        __m256d vsum2 = _mm256_add_pd(V10, V_10);
        __m256d vsum3 = _mm256_add_pd(_mm256_add_pd(V_1_1,V_11),_mm256_add_pd(V1_1,V11));

        __m256d vsum123 = _mm256_fmadd_pd(vcoef3,vsum3,
                                  _mm256_fmadd_pd(vcoef2,vsum2,
                                                  _mm256_mul_pd(vcoef1,vsum1)));
        /*summands 1,2,3---------------------------------------------------^*/
        /*summand 4---------------------------------------------------v*/
        __m256d vp10 = _mm256_load_pd(&(pgrid[fidx11]));  //!! moving in 4s => aligned (current level)
        __m256d vp11 = _mm256_load_pd(&(pgrid[fidxm11])); //!! moving in 4s => aligned (below current)
        __m256d vp12 = _mm256_load_pd(&(pgrid[fidxp11])); //!! moving in 4s => aligned (above current)
        //!load left and right vectors in an aligned manner -----------------------------------------------------------v
        __m256d vp10_4 = zeroVector; //current level, prev step (step = 4) //imitation
        __m256d vp13 = _mm256_blend_pd(vp10_4, vp10, blendMaskFirst); //get 1 right from x and 3 left from y
	    vp13 = _mm256_permute4x64_pd(vp13, permuteMask255_192to63_0);

        __m256d vp104 = _mm256_load_pd(&(XYgridCurr[fidx11 + 4])); //current level, next step (step = 4)
        __m256d vp14 = _mm256_blend_pd(vp104, vp10, blendMaskLast); //get 1 left from x and 3 right from y
        vp14 = _mm256_permute4x64_pd(vp14, permuteMask0_63to255_192);
	    //!load left and right vectors in an aligned manner -----------------------------------------------------------^

        __m256d vsum4 = _mm256_fmadd_pd(vpcoef1,vp10,
                                        _mm256_mul_pd(vpcoef2,
                                                         _mm256_add_pd(_mm256_add_pd(vp11,vp12),
                                                                          _mm256_add_pd(vp13,vp14))));
        /*summand 4---------------------------------------------------^*/
        __m256d vresult = _mm256_mul_pd(vgeneralCoef,
                                        _mm256_add_pd(vsum123,vsum4));
        __m256d resFirst = vresult;
        // re re re
        //printf("after algostep 1\n"); /*!!!*/
        __m256d blendedVector = _mm256_blend_pd(vcur, resFirst, blendMaskFirst);
        //get leftmost from X, 3 others from Y
        vXYgridNew[(fidxcur / 4)] = blendedVector; //write

        //calculate delta
        vd = _mm256_max_pd(_mm256_sub_pd(vXYgridNew[(fidxcur / 4)], vXYgridCurr[(fidxcur / 4)]),
                           _mm256_sub_pd(vXYgridCurr[(fidxcur / 4)], vXYgridNew[(fidxcur / 4)]));
        vdelta = _mm256_max_pd(vdelta, vd);
        //

        //rename vectors for use in further iterations
        VprevB = V_10;
        VprevC = V00;
        VprevT = V10;

        VcurB = V_14;
        VcurC = V04;
        VcurT = V14;

        VprevP = vp10;
        VcurP = vp104;
        //

        //printf("first elems are done!\n"); /*!!!*/
        //printf("now iterating through the rest!\n"); /*!!!*/
        for (int k = 1; k < ((sizeX / 4) - 1); k++) {
            int j = k * 4; //256 / sizeof(dtype)
            fidxcur += 4;
            vcur = VcurC; //!! moving in 4s => aligned
            // re re re
            fidx11 = convertGridCoordsToIdx(sizeX, sizeY, i, j);

            fidxm11 = convertGridCoordsToIdx(sizeX, sizeY, i - 1, j);
            fidxp11 = convertGridCoordsToIdx(sizeX, sizeY, i + 1, j);

            /*summands 1,2,3---------------------------------------------------v*/
            //!load middle vectors in an aligned manner -------------------------------------------------------------------v
            V_10 = VcurB;   //!! moving in 4s => aligned
            V00 = vcur;     //!! moving in 4s => aligned
            V10 = VcurT;    //!! moving in 4s => aligned
            //!load middle vectors in an aligned manner -------------------------------------------------------------------^

            //!load left vectors in an aligned manner ---------------------------------------------------------------------v
            V_1_4 = VprevB; //below current, prev step (step = 4)
            V_1_1 = _mm256_blend_pd(V_1_4, V_10, blendMaskFirst); //get 1 right from x and 3 left from y
	        V_1_1 = _mm256_permute4x64_pd(V_1_1, permuteMask255_192to63_0);

            V0_4 = VprevC; //current level, prev step (step = 4)
            V0_1 = _mm256_blend_pd(V0_4, V00, blendMaskFirst); //get 1 right from x and 3 left from y
	        V0_1 = _mm256_permute4x64_pd(V0_1, permuteMask255_192to63_0);

            V1_4 = VprevT; //above current, prev step (step = 4)
            V1_1 = _mm256_blend_pd(V1_4, V10, blendMaskFirst); //get 1 right from x and 3 left from y
            V1_1 = _mm256_permute4x64_pd(V1_1, permuteMask255_192to63_0);
	        //!load left vectors in an aligned manner ---------------------------------------------------------------------^

            //!load right vectors in an aligned manner --------------------------------------------------------------------v
            V_14 = _mm256_load_pd(&(XYgridCurr[fidxm11 + 4])); //below current, next step (step = 4)
            V_11 = _mm256_blend_pd(V_14, V_10, blendMaskLast); //get 1 left from x and 3 right from y
	        V_11 = _mm256_permute4x64_pd(V_11, permuteMask0_63to255_192);

            V04 = _mm256_load_pd(&(XYgridCurr[fidx11 + 4])); //current level, next step (step = 4)
            V01 = _mm256_blend_pd(V04, V00, blendMaskLast); //get 1 left from x and 3 right from y
	        V01 = _mm256_permute4x64_pd(V01, permuteMask0_63to255_192);

            V14 = _mm256_load_pd(&(XYgridCurr[fidxp11 + 4])); //above current, next step (step = 4)
            V11 = _mm256_blend_pd(V14, V10, blendMaskLast); //get 1 left from x and 3 right from y
            V11 = _mm256_permute4x64_pd(V11, permuteMask0_63to255_192);
	        //!load right vectors in an aligned manner --------------------------------------------------------------------^

            vsum1 = _mm256_add_pd(V0_1, V01);
            vsum2 = _mm256_add_pd(V10, V_10);
            vsum3 = _mm256_add_pd(_mm256_add_pd(V_1_1,V_11),_mm256_add_pd(V1_1,V11));

            vsum123 = _mm256_fmadd_pd(vcoef3,vsum3,
                                              _mm256_fmadd_pd(vcoef2,vsum2,
                                                              _mm256_mul_pd(vcoef1,vsum1)));
            /*summands 1,2,3---------------------------------------------------^*/
            /*summand 4---------------------------------------------------v*/
            vp10 = VcurP;  //!! moving in 4s => aligned (current level)
            vp11 = _mm256_load_pd(&(pgrid[fidxm11])); //!! moving in 4s => aligned (below current)
            vp12 = _mm256_load_pd(&(pgrid[fidxp11])); //!! moving in 4s => aligned (above current)
            //!load left and right vectors in an aligned manner -----------------------------------------------------------v
            vp10_4 = VprevP; //current level, prev step (step = 4)
            vp13 = _mm256_blend_pd(vp10_4, vp10, blendMaskFirst); //get 1 right from x and 3 left from y
	        vp13 = _mm256_permute4x64_pd(vp13, permuteMask255_192to63_0);

            vp104 = _mm256_load_pd(&(XYgridCurr[fidx11 + 4])); //current level, next step (step = 4)
            vp14 = _mm256_blend_pd(vp104, vp10, blendMaskLast); //get 1 left from x and 3 right from y
            vp14 = _mm256_permute4x64_pd(vp14, permuteMask0_63to255_192);
	        //!load left and right vectors in an aligned manner -----------------------------------------------------------^

            vsum4 = _mm256_fmadd_pd(vpcoef1,vp10,
                                            _mm256_mul_pd(vpcoef2,
                                                          _mm256_add_pd(_mm256_add_pd(vp11,vp12),
                                                                        _mm256_add_pd(vp13,vp14))));
            /*summand 4---------------------------------------------------^*/
            vresult = _mm256_mul_pd(vgeneralCoef,
                                    _mm256_add_pd(vsum123,vsum4));
            vXYgridNew[(fidxcur / 4)] = vresult;
            // re re re
            //calculate delta
            vd = _mm256_max_pd(_mm256_sub_pd(vXYgridNew[(fidxcur / 4)], vXYgridCurr[(fidxcur / 4)]),
                               _mm256_sub_pd(vXYgridCurr[(fidxcur / 4)], vXYgridNew[(fidxcur / 4)]));
            vdelta = _mm256_max_pd(vdelta, vd);
            //

            //rename vectors for use in further iterations
            VprevB = V_10;
            VprevC = V00;
            VprevT = V10;

            VcurB = V_14;
            VcurC = V04;
            VcurT = V14;

            VprevP = vp10;
            VcurP = vp104;
            //
        }
        //printf("all elems except last are done!\n"); /*!!!*/
        //last step
        fidxcur += 4;
        vcur = VcurC; //!! moving in 4s => aligned
        // re re re
        //printf("in algostep last\n"); /*!!!*/
        fidx11 = convertGridCoordsToIdx(sizeX, sizeY, i, sizeX - 4);

        fidxm11 = convertGridCoordsToIdx(sizeX, sizeY, i - 1, sizeX - 4);
        fidxp11 = convertGridCoordsToIdx(sizeX, sizeY, i + 1, sizeX - 4);

        /*summands 1,2,3---------------------------------------------------v*/
        //!load middle vectors in an aligned manner -------------------------------------------------------------------v
        V_10 = VcurB;   //!! moving in 4s => aligned
        V00 = vcur;     //!! moving in 4s => aligned
        V10 = VcurT;    //!! moving in 4s => aligned
        //!load middle vectors in an aligned manner -------------------------------------------------------------------^

        //!load left vectors in an aligned manner ---------------------------------------------------------------------v
        V_1_4 = VprevB; //below current, prev step (step = 4)
        V_1_1 = _mm256_blend_pd(V_1_4, V_10, blendMaskFirst); //get 1 right from x and 3 left from y
	    V_1_1 = _mm256_permute4x64_pd(V_1_1, permuteMask255_192to63_0);

	    V0_4 = VprevC; //current level, prev step (step = 4)
        V0_1 = _mm256_blend_pd(V0_4, V00, blendMaskFirst); //get 1 right from x and 3 left from y
	    V0_1 = _mm256_permute4x64_pd(V0_1, permuteMask255_192to63_0);

        V1_4 = VprevT; //above current, prev step (step = 4)
        V1_1 = _mm256_blend_pd(V1_4, V10, blendMaskFirst); //get 1 right from x and 3 left from y
        V1_1 = _mm256_permute4x64_pd(V1_1, permuteMask255_192to63_0);
	    //!load left vectors in an aligned manner ---------------------------------------------------------------------^

        //!load right vectors in an aligned manner --------------------------------------------------------------------v
        V_14 = zeroVector; //below current, next step (step = 4) //imitation
        V_11 = _mm256_blend_pd(V_14, V_10, blendMaskLast); //get 1 left from x and 3 right from y
	    V_11 = _mm256_permute4x64_pd(V_11, permuteMask0_63to255_192);

        V04 = zeroVector; //current level, next step (step = 4) //imitation
        V01 = _mm256_blend_pd(V04, V00, blendMaskLast); //get 1 left from x and 3 right from y
	    V01 = _mm256_permute4x64_pd(V01, permuteMask0_63to255_192);

        V14 = zeroVector; //above current, next step (step = 4) //imitation
        V11 = _mm256_blend_pd(V14, V10, blendMaskLast); //get 1 left from x and 3 right from y
        V11 = _mm256_permute4x64_pd(V11, permuteMask0_63to255_192);
	    //!load right vectors in an aligned manner --------------------------------------------------------------------^

        vsum1 = _mm256_add_pd(V0_1, V01);
        vsum2 = _mm256_add_pd(V10, V_10);
        vsum3 = _mm256_add_pd(_mm256_add_pd(V_1_1,V_11),_mm256_add_pd(V1_1,V11));

        vsum123 = _mm256_fmadd_pd(vcoef3,vsum3,
                                          _mm256_fmadd_pd(vcoef2,vsum2,
                                                          _mm256_mul_pd(vcoef1,vsum1)));
        /*summands 1,2,3---------------------------------------------------^*/
        /*summand 4---------------------------------------------------v*/
        vp10 = VcurP;  //!! moving in 4s => aligned (current level)
        vp11 = _mm256_load_pd(&(pgrid[fidxm11])); //!! moving in 4s => aligned (below current)
        vp12 = _mm256_load_pd(&(pgrid[fidxp11])); //!! moving in 4s => aligned (above current)
        //!load left and right vectors in an aligned manner -----------------------------------------------------------v
        vp10_4 = VprevP; //current level, prev step (step = 4)
        vp13 = _mm256_blend_pd(vp10_4, vp10, blendMaskFirst); //get 1 right from x and 3 left from y
	    vp13 = _mm256_permute4x64_pd(vp13, permuteMask255_192to63_0);

        vp104 = zeroVector; //current level, next step (step = 4) //imitation
        vp14 = _mm256_blend_pd(vp104, vp10, blendMaskLast); //get 1 left from x and 3 right from y
        vp14 = _mm256_permute4x64_pd(vp14, permuteMask0_63to255_192);
	    //!load left and right vectors in an aligned manner -----------------------------------------------------------^

        vsum4 = _mm256_fmadd_pd(vpcoef1,vp10,
                                        _mm256_mul_pd(vpcoef2,
                                                      _mm256_add_pd(_mm256_add_pd(vp11,vp12),
                                                                    _mm256_add_pd(vp13,vp14))));
        /*summand 4---------------------------------------------------^*/
        vresult = _mm256_mul_pd(vgeneralCoef,
                                _mm256_add_pd(vsum123,vsum4));
        //printf("out algostep last\n"); /*!!!*/
        __m256d resLast = vresult;
        // re re re
        blendedVector = _mm256_blend_pd(vcur, resLast, blendMaskLast);
        //get rightmost from X, 3 others from Y
        vXYgridNew[(fidxcur / 4)] = blendedVector; //write

        //calculate delta
        vd = _mm256_max_pd(_mm256_sub_pd(vXYgridNew[(fidxcur / 4)], vXYgridCurr[(fidxcur / 4)]),
                           _mm256_sub_pd(vXYgridCurr[(fidxcur / 4)], vXYgridNew[(fidxcur / 4)]));
        vdelta = _mm256_max_pd(vdelta, vd);
        //printf("last elems are done!\n"); /*!!!*/
        //
    }
    /*main loop --------------------------------------------------------------------^*/

    //find vectorised total delta after iterating through entire matrix (4 dif numbers)
    const int imm1 = 0b01001110;
    vd = _mm256_permute4x64_pd(vdelta, imm1);
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

void* myAlignedAlloc(int alignment, size_t size, void* forFree) {
    void *mem = malloc(size + (alignment -1));
    if (NULL == mem) {
        return NULL;
    }

    char* temp = (char*)mem;
    size_t rest = (size_t)temp % alignment;
    if (0 != rest) {
        temp += (alignment - rest);
    }
    forFree = mem;
    return temp;
}
//returns ptr to aligned memory
//puts pointer to allocated memory into forFree

int main(int argc, char** argv) {
    /*int Nx = atoi(argv[1]);*/ /*j >*/
    /*int Ny = atoi(argv[2]); */ /*i ^*/
    /*int Nt = atoi(argv[3]); */

    int Nx = 100; /*j >*/
    int Ny = 100; /*i ^*/
    int Nt = 1;

    dtype hx = (XB - XA) / (Nx - 1);
    dtype hy = (YB - YA) / (Ny - 1);

    /*Xj = X1 + j*hx  */
    /*Yi = Y1 + i*hy  */

    /*n = 0,1,...Nt  */

    dtype* XYgrid1; //this pointer is the one that shall be freed
    void* forFreeXY;
    XYgrid1 = (double*)myAlignedAlloc(32, 2 * Nx * Ny * sizeof(double), forFreeXY); //aligned
    if (XYgrid1 == NULL) {
        exit(1);
    }
    dtype* XYgrid2 = XYgrid1 + (Nx * Ny); //do not free with this pointer

    initXYgrid(XYgrid1, Nx, Ny);
    initXYgrid(XYgrid2, Nx, Ny);

    dtype* pgrid;
    void* forFreeP;
    pgrid = (double*)myAlignedAlloc(32, Nx * Ny * sizeof(double), forFreeP); //aligned
    if (pgrid == NULL) {
        free(forFreeXY);
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

    free(forFreeP);
    free(forFreeXY);

    printf("Deallocation complete!\n"); /*-------------------------------------*/
}
