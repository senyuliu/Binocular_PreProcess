#ifndef __GPS_H__
#define __GPS_H__

#include <iostream>
#include <math.h>

namespace IBD_SD
{
//笛卡尔坐标系
typedef struct tagCRDCARTESIAN{
double x;
double y;
double z;
}PCRDCARTESIAN;
 
//大地坐标系
typedef struct tagCRDGEODETIC{
double longitude; //经度
double latitude;  //纬度 
double height;    //大地高,可设为0
}PCRDGEODETIC;

class CGpsCoord
{
public: 
    CGpsCoord(void); 
    ~CGpsCoord(void);

public: 
    void GeodeticToCartesian (PCRDCARTESIAN &pcc, PCRDGEODETIC &pcg);
};

}

#endif
