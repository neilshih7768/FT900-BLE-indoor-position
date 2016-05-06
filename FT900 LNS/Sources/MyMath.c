#include "MyMath.h"
#include <stdio.h>


double Mean(int *iData, int iSize)
{
    int i = 0, sum = 0;
    
    for(i = 0; i < iSize; i++) {
        sum += iData[i];
    }

    return (double)sum / (double)iSize;
}