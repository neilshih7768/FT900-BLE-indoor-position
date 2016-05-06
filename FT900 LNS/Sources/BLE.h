#ifndef BLE_H
#define BLE_H


#define iFileSize     18
#define iSampleHeader 8      // How many headers in data
#define iSampleRate   300
#define dReference	  200.0


typedef struct _BLEData {
    int iDistance;              // CM
    int iData[iSampleRate];     // RSSIs
    double dMean;               // RSSIs mean
    double dD;
} BLEData;


static double dP0, dN;


void SetBLEData();
void GetLNSData(double dRef, double *dP0, double *dN);
void PrintBLEMean();
double ConvertRSSI2toDist(double rssi);

#endif
