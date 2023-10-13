#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <immintrin.h>

//-------------------------------------------------------------------
#include <thread>
#include <vector>
#include <atomic>

#define N_O_THREADS (1)
//#define N_O_THREADS (2)
//#define N_O_THREADS (3)
//#define N_O_THREADS (4)
//#define N_O_THREADS (5)
//#define N_O_THREADS (6)
//#define N_O_THREADS (7)
//#define N_O_THREADS (8)

#define BARRIER_LIMIT (2)

//match (N_O_THREADS - 1)
std::atomic<int> barrier0{ 0 }; //0
std::atomic<int> barrier1{ 0 }; //1
std::atomic<int> barrier2{ 0 }; //2
std::atomic<int> barrier3{ 0 }; //3
std::atomic<int> barrier4{ 0 }; //4
std::atomic<int> barrier5{ 0 }; //5
std::atomic<int> barrier6{ 0 }; //6
std::atomic<int> barrier7{ 0 }; //7


std::vector<std::atomic_int*> barriers;
//-------------------------------------------------------------------

/*Nx = Ny ~ 8000-10000*/
/*Nt ~ 100-120*/

#define XB (4.0)
#define XA (0.0)
#define YB (4.0)
#define YA (0.0)

#define dtype double

typedef struct threadTask {
    int myId;

    void* ptrToStartOld; //cast to double
    void* ptrToActStartOld; //actual start (after 0 "frame")
    void* ptrToStartNew;
    void* ptrToActStartNew;

    void* pgrid; //same among all threads

    int mySizeX;
    int mySizeY; //should be -1 for threads that work w/ first and last rows of matrix

    int NOiters;
} threadTask;

void standOnSpinlockBarrier(std::atomic_int* barrier, int id, int myIter) {
    barrier->fetch_add(1);
    while (true) {
        if (barrier->load() >= (myIter * BARRIER_LIMIT)) {
            return;
        }
    }
}

void initOwnedPortion(double* XYgridPortion, int sizeX, int sizeY, int myId) {
    int size = sizeX * sizeY;
    for (int k = 0; k < size; k++) {
        XYgridPortion[k] = 0.0;
    }
}

int convertGridCoordsToIdx(int sizeX, int sizeY, int i, int j) {
    return (j + sizeX * i);
}

double algoStepsVectorisedForThread(double* XYgridCurr, double* XYgridNew, double const* pgrid,
                                    double hxsq, double hysq, int sizeX, int sizeY) {
    const double revsqsum = ((1.0 / hxsq) + (1.0 / hysq));
    const double generalCoef = 1.0 / (5.0 * revsqsum);
    __m256d vgeneralCoef = _mm256_set1_pd(generalCoef);

    const double coef1 = (1.0 / 2.0) * ((5.0 / hxsq) - (1.0 / hysq));
    const double coef2 = (1.0 / 2.0) * ((5.0 / hysq) - (1.0 / hxsq));
    const double coef3 = (1.0 / 4.0) * revsqsum;

    __m256d vcoef1 = _mm256_set1_pd(coef1);
    __m256d vcoef2 = _mm256_set1_pd(coef2);
    __m256d vcoef3 = _mm256_set1_pd(coef3);

    __m256d vpcoef1 = _mm256_set1_pd(2.0);
    __m256d vpcoef2 = _mm256_set1_pd(0.25);

    double delta = 0.0;

    __m256d vdelta = _mm256_set1_pd(0.0);
    __m256d vd = _mm256_set1_pd(0.0);

    const int blendMaskFirst = 0b0111; //get 1 left from a and 3 right from b
    const int blendMaskLast = 0b1110; //get 3 left from b and 1 right from a

    /*main loop --------------------------------------------------------------------v*/
    for (int i = 0; i < sizeY; i++) {
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
        __m256d vsum3 = _mm256_add_pd(_mm256_add_pd(V_1_1, V_11), _mm256_add_pd(V1_1, V11));

        __m256d vsum123 = _mm256_fmadd_pd(vcoef3, vsum3, _mm256_fmadd_pd(vcoef2, vsum2, _mm256_mul_pd(vcoef1, vsum1)));
        /*summands 1,2,3---------------------------------------------------^*/
        /*summand 4---------------------------------------------------v*/
        __m256d vp10 = _mm256_loadu_pd(&(pgrid[fidx11]));
        __m256d vp11 = _mm256_loadu_pd(&(pgrid[fidxm11]));
        __m256d vp12 = _mm256_loadu_pd(&(pgrid[fidxp11]));
        __m256d vp13 = _mm256_loadu_pd(&(pgrid[fidx1m1]));
        __m256d vp14 = _mm256_loadu_pd(&(pgrid[fidx1p1]));

        __m256d vsum4 = _mm256_fmadd_pd(vpcoef1, vp10, _mm256_mul_pd(vpcoef2, _mm256_add_pd(_mm256_add_pd(vp11, vp12), _mm256_add_pd(vp13, vp14))));
        /*summand 4---------------------------------------------------^*/
        __m256d vresult = _mm256_mul_pd(vgeneralCoef, _mm256_add_pd(vsum123, vsum4));
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
            vsum3 = _mm256_add_pd(_mm256_add_pd(V_1_1, V_11), _mm256_add_pd(V1_1, V11));

            vsum123 = _mm256_fmadd_pd(vcoef3, vsum3, _mm256_fmadd_pd(vcoef2, vsum2, _mm256_mul_pd(vcoef1, vsum1)));

            /*summands 1,2,3---------------------------------------------------^*/
            /*summand 4---------------------------------------------------v*/
            vp10 = _mm256_loadu_pd(&(pgrid[fidx11]));
            vp11 = _mm256_loadu_pd(&(pgrid[fidxm11]));
            vp12 = _mm256_loadu_pd(&(pgrid[fidxp11]));
            vp13 = _mm256_loadu_pd(&(pgrid[fidx1m1]));
            vp14 = _mm256_loadu_pd(&(pgrid[fidx1p1]));

            vsum4 = _mm256_fmadd_pd(vpcoef1, vp10, _mm256_mul_pd(vpcoef2, _mm256_add_pd(_mm256_add_pd(vp11, vp12), _mm256_add_pd(vp13, vp14))));
            /*summand 4---------------------------------------------------^*/
            vresult = _mm256_mul_pd(vgeneralCoef, _mm256_add_pd(vsum123, vsum4));

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
        vsum3 = _mm256_add_pd(_mm256_add_pd(V_1_1, V_11), _mm256_add_pd(V1_1, V11));

        vsum123 = _mm256_fmadd_pd(vcoef3, vsum3, _mm256_fmadd_pd(vcoef2, vsum2, _mm256_mul_pd(vcoef1, vsum1)));
        /*summands 1,2,3---------------------------------------------------^*/
        /*summand 4---------------------------------------------------v*/
        vp10 = _mm256_loadu_pd(&(pgrid[fidx11]));
        vp11 = _mm256_loadu_pd(&(pgrid[fidxm11]));
        vp12 = _mm256_loadu_pd(&(pgrid[fidxp11]));
        vp13 = _mm256_loadu_pd(&(pgrid[fidx1m1]));
        vp14 = _mm256_loadu_pd(&(pgrid[fidx1p1]));

        vsum4 = _mm256_fmadd_pd(vpcoef1, vp10, _mm256_mul_pd(vpcoef2, _mm256_add_pd(_mm256_add_pd(vp11, vp12), _mm256_add_pd(vp13, vp14))));
        /*summand 4---------------------------------------------------^*/
        vresult = _mm256_mul_pd(vgeneralCoef, _mm256_add_pd(vsum123, vsum4));

        __m256d resLast = vresult;
        blendedVector = _mm256_blend_pd(vcur, resLast, blendMaskLast);
        //get rightmost from X, 3 others from Y
        //vXYgridNew[(fidxcur / 4)] = blendedVector; //write
        _mm256_storeu_pd(&(XYgridNew[fidxcur]), blendedVector);

        //calculate delta
        vd = _mm256_max_pd(_mm256_sub_pd(blendedVector, vcur),
            _mm256_sub_pd(vcur, blendedVector));
        vdelta = _mm256_max_pd(vdelta, vd);
    }
    /*main loop --------------------------------------------------------------------^*/

    //find vectorised total delta after iterating through entire matrix (4 dif numbers)
    const int imm1 = 0b01001110;
    vd = _mm256_permute4x64_pd(vdelta, imm1); //value of imm8 should be used here
    vdelta = _mm256_max_pd(vdelta, vd);

    const int imm2 = 0b1111;
    vd = _mm256_shuffle_pd(vdelta, vdelta, imm2); //has lower latency than permute4x64
    vdelta = _mm256_max_pd(vd, vdelta);
    //now vdelta has 4 equal 64bit values, we need one of them

    double delta4[4] = { 0, 0, 0, 0 };
    _mm256_storeu_pd((double*)&delta4, vdelta);
    //storeu and store do not differ in latency and throughput on processor this programme is meant to be run on
    delta = delta4[0];
    //now delta has required delta from entire matrix iteration

    return delta;
}

void swapPointersV2(threadTask task) {
    void* ptemp = task.ptrToStartOld;
    task.ptrToStartOld = task.ptrToStartNew;
    task.ptrToStartNew = ptemp;

    ptemp = task.ptrToActStartOld;
    task.ptrToActStartOld = task.ptrToActStartNew;
    task.ptrToActStartNew = ptemp;
}

void threadFunction(threadTask task, double hxsq, double hysq) {

    //printf("ID %d: thread starting to execute it's function...\n", task.myId);
    //first, load portion of matrix to thread's corresponding memory unit
    //by initialising it
    if ((task.myId == 0) || (task.myId == (N_O_THREADS - 1))) {
        //in these 2 (or 1) cases mySizeY for first and last threads doesn't count
        //the first row of 0s and the last row of 0s respectively
        //yet both of them need to be initialised with 0s
        if (1 == N_O_THREADS) {
            initOwnedPortion((double*)task.ptrToStartOld, task.mySizeX, task.mySizeY + 2, task.myId);
            initOwnedPortion((double*)task.ptrToStartNew, task.mySizeX, task.mySizeY + 2, task.myId);
        } else {
            initOwnedPortion((double*)task.ptrToStartOld, task.mySizeX, task.mySizeY + 1, task.myId);
            initOwnedPortion((double*)task.ptrToStartNew, task.mySizeX, task.mySizeY + 1, task.myId);
        }
    }
    else {
        initOwnedPortion((double*)task.ptrToStartOld, task.mySizeX, task.mySizeY, task.myId);
        initOwnedPortion((double*)task.ptrToStartNew, task.mySizeX, task.mySizeY, task.myId);
    }
    //printf("ID %d: thread portion initialized...\n", task.myId);

    double deltaOld = 1000.0;
    double deltaNew = 999.0;

    //work your portion of data
    for (int k = 1; k < task.NOiters; k++) {
        //printf("ID %d: iteration %d started\n", task.myId, k);

        if (task.myId != 0) {
            standOnSpinlockBarrier(barriers.at(task.myId - 1), task.myId, k);  //stand on lower barrier
        }
        if (task.myId != (N_O_THREADS - 1)) {
            standOnSpinlockBarrier(barriers.at(task.myId), task.myId, k);  //stand on upper barrier
        }

        //-------------------------------------------------
//        int redConst = 25;
//        if (k % redConst == redConst - 1) {
//            printf("ID %d: %d iterations complete!\n", task.myId, redConst);
//        }
        //-------------------------------------------------

        //step
        deltaNew = algoStepsVectorisedForThread((double*)task.ptrToActStartOld, (double*)task.ptrToActStartNew, (double*)task.pgrid, hxsq, hysq, task.mySizeX, task.mySizeY);

        //check if new < old
        if (deltaNew >= deltaOld) {
            printf("ID %d: something is going wrong, delta increasing\n", task.myId);
        }
        else {
            //nothing
        } //surely compiler will cut "else{}" part out

        swapPointersV2(task);
    }

} //assume working!!!!!!!!!!!!!!!!!!!!!!!!!!

void createAndLaunchThreads(std::thread(*threads)[N_O_THREADS], threadTask threadTasks[], double hxsq, double hysq) {
    //printf("Starting thread creation...\n");
    for (int i = 0; i < N_O_THREADS; i++) {
        (*threads)[i] = std::thread(threadFunction, threadTasks[i], hxsq, hysq);
    }
    //printf("Thread creation complete\n");
}


//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------

void mainFuncForThreads(double* gridOld, double* gridNew, double* gridp,
                        int sizeX, int sizeY, int Nt, double hx, double hy,
                        std::thread(*threads)[N_O_THREADS]) {
    //create threadTasks
    //(also separate Nx*Ny space between them equally)
    //printf("Main func for threads starting...\n");
    int rowsPerThread = sizeY / N_O_THREADS;
    int rowsPerLastThread = sizeY - (rowsPerThread * (N_O_THREADS - 1));
    //printf("All threads need to work %d rows\n", rowsPerThread);
    //printf("Last thread need to work %d rows\n", rowsPerLastThread);

    threadTask arrOfThreadTasks[N_O_THREADS];
    for (int i = 0; i < N_O_THREADS; i++) {
        arrOfThreadTasks[i].myId = i;

        arrOfThreadTasks[i].ptrToStartOld = (void*)(gridOld + ((rowsPerThread * i) * sizeX));
        arrOfThreadTasks[i].ptrToStartNew = (void*)(gridNew + ((rowsPerThread * i) * sizeX));

        if (i != 0) {
            arrOfThreadTasks[i].ptrToActStartOld = arrOfThreadTasks[i].ptrToStartOld;
            arrOfThreadTasks[i].ptrToActStartNew = arrOfThreadTasks[i].ptrToStartNew;
        }
        else {
            arrOfThreadTasks[i].ptrToActStartOld = (void*)(gridOld + sizeX);
            arrOfThreadTasks[i].ptrToActStartNew = (void*)(gridNew + sizeX);
        }

        if (i != 0) {
            arrOfThreadTasks[i].pgrid = (void*)(gridp + ((rowsPerThread * i) * sizeX));
        } else {
            arrOfThreadTasks[i].pgrid = (void*)(gridp + sizeX);
        }

        arrOfThreadTasks[i].mySizeX = sizeX;
        if (i == (N_O_THREADS - 1)) {
            arrOfThreadTasks[i].mySizeY = rowsPerLastThread;
        }
        else {
            arrOfThreadTasks[i].mySizeY = rowsPerThread;
        }

        if (i == 0) {
            arrOfThreadTasks[i].mySizeY -= 1;
        }
        if (i == (N_O_THREADS - 1)) {
            arrOfThreadTasks[i].mySizeY -= 1;
        }

        arrOfThreadTasks[i].NOiters = Nt;
    }

    double hxsq = (hx * hx);
    double hysq = (hy * hy);

    createAndLaunchThreads(threads, arrOfThreadTasks, hxsq, hysq);

    //printf("Main func for threads over!\n");
}

void joinAllThreads(std::thread(*threads)[N_O_THREADS]) {
    for (int i = 0; i < N_O_THREADS; i++) {
        (*threads)[i].join();
    }
}

void initBarrierVector() {
    //printf("Starting barrier initialisation...\n");
    barriers.push_back(&barrier0);
    barriers.push_back(&barrier1);
    barriers.push_back(&barrier2);
    barriers.push_back(&barrier3);
    barriers.push_back(&barrier4);
    barriers.push_back(&barrier5);
    barriers.push_back(&barrier6);
    barriers.push_back(&barrier7);
    //printf("Starting barrier initialisation...\n");
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

    const int padding = 2;

    dtype* XYgrid1; //this pointer is the one that shall be freed
    XYgrid1 = (double*)malloc((2 * Nx * Ny + padding) * sizeof(double));
    if (XYgrid1 == NULL) {
        exit(1);
    }
    XYgrid1 += 1; //!!!dont forget to -= 1 to free!!!
    dtype* XYgrid2 = XYgrid1 + (Nx * Ny); //do not free with this pointer

    //initXYgrid(XYgrid1, Nx, Ny);
    //initXYgrid(XYgrid2, Nx, Ny);

    //dtype* pgrid = (dtype*)malloc(Nx * Ny * sizeof(dtype));
    dtype* pgrid;
    pgrid = (double*)malloc(Nx * Ny * sizeof(double));
    if (pgrid == NULL) {
        free(XYgrid1 - 1);
        exit(1);
    }
    initPgrid(pgrid, Nx, Ny, hx, hy);

    initBarrierVector();

    printf("Allocation and initialization complete!\n"); /*-------------------------------------*/

    double* newGrid = NULL;
    std::thread arrOfThreads[N_O_THREADS];
    mainFuncForThreads(XYgrid1, XYgrid2, pgrid, Nx, Ny, Nt, hx, hy, &arrOfThreads);
    if (Nt % 2 == 1) {
        newGrid = XYgrid2;
    }
    else {
        newGrid = XYgrid1;
    }
    //dtype* newGrid = mainFunc(XYgrid1, XYgrid2, pgrid, Nx, Ny, Nt, hx, hy);

    joinAllThreads(&arrOfThreads);

    printf("Calculation complete!\n"); /*-------------------------------------*/

    FILE* fileForOutput = fopen("outputFile1.txt", "wb");
    fwrite(&(newGrid[0]), sizeof(dtype), Nx * Ny, fileForOutput);

    printf("Output to file complete!\n"); /*-------------------------------------*/

    fclose(fileForOutput);

    free(pgrid);
    free(XYgrid1 - 1);
    //free(XYgrid2);

    printf("Deallocation complete!\n"); /*-------------------------------------*/
}
