#ifndef __STEREORECTIRY__H__
#define __STEREORECTIRY__H__

#include <iostream>

#include <opencv2/opencv.hpp>
#include "opencv2/calib3d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <glog/logging.h>

#include "shellCommand.h"


#define COLS 1280
#define ROWS 720

using namespace cv;
namespace IBD_SD
{

class CStereoRectify
{

public: 

    CStereoRectify(void); 
    ~CStereoRectify(void);

public: 
    //-----------------------------校正参数--------------------------
    bool Rectify_main(std::string& project_path,std::string& vin);

    bool Rectify_main_org(std::string& project_path,std::string& vin);

    bool initRemap(CvMat* _Q,std::string& vin);

public: 

    CvMat* mx1 ;
    CvMat* my1 ;
    CvMat* mx2 ;
    CvMat* my2 ;

}; 

}
#endif 
