#include <stdio.h>
#include <stdlib.h>
#include "BLE.h"

int main(void)
{
	SetBLEData();

	GetLNSData(dReference, &dP0, &dN);

	printf("P0 = %.3f  N = %.3f  dRef = %.3f \n", dP0, dN, dReference);




	while (1);
	return 0;
}
