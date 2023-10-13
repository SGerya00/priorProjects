#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

int convertIdxToX(int sizeX, int sizeY, int idx) {
    return (idx % sizeX);
}

int convertIdxToY(int sizeX, int sizeY, int idx) {
    return (idx / sizeX);
}

void algoStepsV2(dtype* XYgridCurr, dtype* XYgridNew, dtype const* pgrid,
                 dtype hxsq, dtype hysq, int sizeX, int sizeY) {
    const dtype revsqsum = ((1.0 / hxsq) + (1.0 / hysq));
    const dtype generalCoef = 1.0 / (5.0 * revsqsum);

    const dtype coef1 = (1.0 / 2.0) * ((5.0 / hxsq) - (1.0 / hysq));
    const dtype coef2 = (1.0 / 2.0) * ((5.0 / hysq) - (1.0 / hxsq));
    const dtype coef3 = (1.0 / 4.0) * revsqsum;

    /*main loop --------------------------------------------------------------------v*/
    for (int i = 1; i < (sizeY - 1); i++) {
        for (int j = 1; j < (sizeX - 1); j++) {
            const int fidx11 = convertGridCoordsToIdx(sizeX, sizeY, i, j);

            const int fidx1m1 = convertGridCoordsToIdx(sizeX, sizeY, i, j - 1);
            const int fidx1p1 = convertGridCoordsToIdx(sizeX, sizeY, i, j + 1);

            const int fidxm11 = convertGridCoordsToIdx(sizeX, sizeY, i - 1, j);
            const int fidxp11 = convertGridCoordsToIdx(sizeX, sizeY, i + 1, j);

            const int fidxm1m1 = convertGridCoordsToIdx(sizeX, sizeY, i - 1, j - 1);
            const int fidxm1p1 = convertGridCoordsToIdx(sizeX, sizeY, i - 1, j + 1);
            const int fidxp1m1 = convertGridCoordsToIdx(sizeX, sizeY, i + 1, j - 1);
            const int fidxp1p1 = convertGridCoordsToIdx(sizeX, sizeY, i + 1, j + 1);

            /*summands 1,2,3---------------------------------------------------v*/
            const dtype F11 = XYgridCurr[fidx1m1];
            const dtype F12 = XYgridCurr[fidx1p1];

            const dtype F21 = XYgridCurr[fidxm11];
            const dtype F22 = XYgridCurr[fidxp11];

            const dtype F31 = XYgridCurr[fidxm1m1];
            const dtype F32 = XYgridCurr[fidxm1p1];
            const dtype F33 = XYgridCurr[fidxp1m1];
            const dtype F34 = XYgridCurr[fidxp1p1];

            const dtype summands123 = coef1 * (F11 + F12) +
                                coef2 * (F21 + F22) +
                                coef3 * (F31 + F32 + F33 + F34);
            /*summands 1,2,3---------------------------------------------------^*/
            /*summand 4---------------------------------------------------v*/
            const dtype p10 = pgrid[fidx11];
            const dtype p11 = pgrid[fidxm11];
            const dtype p12 = pgrid[fidxp11];
            const dtype p13 = pgrid[fidx1m1];
            const dtype p14 = pgrid[fidx1p1];

            const dtype summand4 = 2.0 * p10 + (1.0 / 4.0) * (p11 + p12 + p13 + p14);
            /*summand 4---------------------------------------------------^*/
            XYgridNew[fidx11] = (generalCoef * (summands123 + summand4));
        }
    }
    /*main loop --------------------------------------------------------------------^*/
    return;
}

dtype calcNewDelta(dtype* XYgridCurr, dtype* XYgridNew,
                   int sizeX, int sizeY) {
    dtype max = -1.0;

    for (int k = 0; k < sizeX * sizeY; k++) {
        dtype newVal = XYgridNew[k];
        dtype oldVal = XYgridCurr[k];
        dtype abs = fabs(newVal - oldVal);

        if (abs > max) {
            max = abs;
        }
    }

    return max;
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
        algoStepsV2(currg, newg, gridp, hxsq, hysq, sizeX, sizeY);

        /*calculate delta*/
        deltaOld = deltaNew;
        deltaNew = calcNewDelta(currg, newg, sizeX, sizeY);

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

    dtype* XYgrid1 = (dtype*)malloc(Nx * Ny * sizeof(dtype));
    dtype* XYgrid2 = (dtype*)malloc(Nx * Ny * sizeof(dtype));
    initXYgrid(XYgrid1, Nx, Ny);
    initXYgrid(XYgrid2, Nx, Ny);

    dtype* pgrid = (dtype*)malloc(Nx * Ny * sizeof(dtype));
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
    free(XYgrid2);

    printf("Deallocation complete!\n"); /*-------------------------------------*/
}
