#include <stdio.h>
#include <stdlib.h>
#include "Sample.h"
#include "Matrix.h"
#include "BLE.h"


// Find a inverse matrix of a square and check it
void InverseSample()
{
    const int iSize = 3;
    double mTestInit[3][3] = { {1, 2, 0}, { -1, 1, 1}, { 1, 2, 3} };
    
    double **mTest = (double **)malloc(sizeof(double) * iSize);
    double **mInverse;
    double **mMulti;

    int i = 0, j = 0;

    for(i = 0; i < iSize; i++) {
        mTest[i] = (double *)malloc(sizeof(double) * iSize);
    }

    for(i = 0; i < iSize; i++) {
        for(j = 0; j < iSize; j++) {
            mTest[i][j] = mTestInit[i][j];
        }
    }

    printf("\n======Sample 1 ======\n");

    // Print input matrix
    printf("Test matrix A = \n");
    PrintMatrix(mTest, iSize, iSize);


    // Inverse the matrix
    mInverse = Inverse(mTest, iSize);
    printf("Inverse matrix IA = \n");
    PrintMatrix(mInverse, iSize, iSize);


    // Check the inverse matrix multiplied by input matrix is identity matrix
    mMulti = MultiMatrix(mTest, mInverse, 3, 3, 3);
    printf("Multiple matrix A * IA = \n");
    PrintMatrix(mMulti, iSize, iSize);
    
    // Free memory
    for(i = 0; i < iSize; i++) {
        free(mTest[i]);
        free(mInverse[i]);
        free(mMulti[i]);
    }

    free(mTest);
    free(mInverse);
    free(mMulti);
}


// Find a transpose matrix of a none square matrix
void TransposeSample()
{
    const int iSizeM = 2, iSizeN = 3;
    double mTestInit[2][3] = { {1, 2, 0}, { -1, 1, 1} };

    double **mTest = (double **)malloc(sizeof(double) * iSizeM);    // Important index is M
    double **mTranspose;

    int i = 0, j = 0;

    for(i = 0; i < iSizeM; i++) {                                   // Important index is M
        mTest[i] = (double *)malloc(sizeof(double) * iSizeN);       // Important index is N
    }

    for(i = 0; i < iSizeM; i++) {
        for(j = 0; j < iSizeN; j++) {
            mTest[i][j] = mTestInit[i][j];
        }
    }

    printf("\n======Sample 2 ======\n");

    // Print input matrix
    printf("Test matrix A = \n");
    PrintMatrix(mTest, iSizeM, iSizeN);


    // Transpose the matrix
    mTranspose = Transpose(mTest, iSizeM, iSizeN);
    printf("Transpose matrix TA = \n");
    PrintMatrix(mTranspose, iSizeN, iSizeM);

    for(i = 0; i < iSizeM; i++) {
        free(mTest[i]);
    }

    for(i = 0; i < iSizeN; i++) {
        free(mTranspose[i]);
    }

    free(mTest);
    free(mTranspose);
}


// Find BLE data
void LNSSample()
{
    double dP0, dN, dRef;

    dRef = 200.0;

    SetBLEData();

    GetLNSData(dRef, &dP0, &dN);

    printf("P0 = %.3f  N = %.3f  dRef = %.3f \n", dP0, dN, dRef);
}
