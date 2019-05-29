#include "GPS.h"
 
IBD_SD::CGpsCoord::CGpsCoord(void)
{

}

IBD_SD::CGpsCoord::~CGpsCoord(void)
{

}

void IBD_SD::CGpsCoord::GeodeticToCartesian (IBD_SD::PCRDCARTESIAN &pcc, IBD_SD::PCRDGEODETIC &pcg)
{
        double B;        //纬度度数
        double L;        //经度度数
        double L0;        //中央经线度数
        double l;        //L-L0
        double t;        //tanB
        double m;        //ltanB
        double N;        //卯酉圈曲率半径 
        double q2;
        double x;        //高斯平面纵坐标
        double y;        //高斯平面横坐标
        double s;        //赤道至纬度B的经线弧长
        double f;        //参考椭球体扁率
        double e2;        //椭球第一偏心率
        double a;        //参考椭球体长半轴
        //double b;        //参考椭球体短半轴
        double a1;
        double a2;
        double a3;
        double a4;
        double b1;
        double b2;
	double b3;
	double b4;
        double c0;
        double c1;
        double c2;
        double c3;
        int Datum=84;        //投影基准面类型：北京54基准面为54，西安80基准面为80，WGS84基准面为84
        int prjno=0;        //投影带号
        int zonewide=3;        
        double IPI=0.0174532925199433333333;        //3.1415926535898/180.0
        B=pcg.latitude ; //纬度
        L=pcg.longitude ; //经度
        if (zonewide==6) 
        {
                 prjno=(int)(L/zonewide)+1;
                 L0=prjno*zonewide-3;
        }
        else
        {
                prjno=(int)((L-1.5)/3)+1;
                L0=prjno*3;
        }
        
        if(Datum==54)
        {
                 a=6378245;
                 f=1/298.3;
        }        
        else if(Datum==84)
        {
                a=6378137;
                f=1/298.257223563;
        }

        L0=L0*IPI;
        L=L*IPI;
        B=B*IPI;
 
        e2=2*f-f*f;//(a*a-b*b)/(a*a);
        l=L-L0;
        t=tan(B);
        m=l * cos(B);
        N=a/sqrt(1-e2* sin(B) * sin(B));
        q2=e2/(1-e2)* cos(B)* cos(B);
        a1=1+(double)3/4*e2+(double)45/64*e2*e2+(double)175/256*e2*e2*e2+(double)11025/16384*e2*e2*e2*e2+(double)43659/65536*e2*e2*e2*e2*e2;
        a2=(double)3/4*e2+(double)15/16*e2*e2+(double)525/512*e2*e2*e2+(double)2205/2048*e2*e2*e2*e2+(double)72765/65536*e2*e2*e2*e2*e2;
        a3=(double)15/64*e2*e2+(double)105/256*e2*e2*e2+(double)2205/4096*e2*e2*e2*e2+(double)10359/16384*e2*e2*e2*e2*e2;
        a4=(double)35/512*e2*e2*e2+(double)315/2048*e2*e2*e2*e2+(double)31185/13072*e2*e2*e2*e2*e2;
        b1=a1*a*(1-e2);
        b2=(double)-1/2*a2*a*(1-e2);
        b3=(double)1/4*a3*a*(1-e2);
        b4=(double)-1/6*a4*a*(1-e2);
        c0=b1;
        c1=2*b2+4*b3+6*b4;
        c2=-(8*b3+32*b4);
        c3=32*b4;
        s=c0*B+cos(B)*(c1*sin(B)+c2*sin(B)*sin(B)*sin(B)+c3*sin(B)*sin(B)*sin(B)*sin(B)*sin(B));
        x=s+(double)1/2*N*t*m*m+(double)1/24*(5-t*t+9*q2+4*q2*q2)*N*t*m*m*m*m+(double)1/720*(61-58*t*t+t*t*t*t)*N*t*m*m*m*m*m*m;
        y=N*m+(double)1/6*(1-t*t+q2)*N*m*m*m+(double)1/120*(5-18*t*t+t*t*t*t-14*q2-58*q2*t*t)*N*m*m*m*m*m;
        y=y+500000;//y=y+1000000*prjno+500000;
        pcc.x=x;
        pcc.y=y;//pcc.y=y-38000000;
        pcc.z=pcg.height;
 
}
