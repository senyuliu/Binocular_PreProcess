#include <stdio.h>

#include <stdlib.h>

#include <string>

#include <fstream>

#include <iostream>

#include <iomanip>
#include "shellCommand.h"
#include "cJSON.h"
#include "GPS.h"
#include <cmath>
#include <glog/logging.h>

namespace IBD_SD
{

struct IMUData{
    std::string Weeks;
    double weekseconds;
    double TimeStampe;
    std::string BeijingTime;
    double Lat;
    double Lon;
    double Hgt;
    double Speed;
    double Heading;
    double Roll;
    double Pitch;
};

class CDataParse
{
public: 
    CDataParse(void); 
    ~CDataParse(void);

public: 

    bool ParseProcess(string logstr,IMUData& m_IMUData);
    void ParseIMUData(string pathDir,vector<IMUData>& IMUDataList);

    void sortIMUData(vector<IMUData>& IMUDataList);
    void TraceFiltering(std::vector<IMUData>& in_IMUDataList,std::vector<IMUData>& out_IMUDataList);

    bool GetDeviceNum(string src_dir, string& DeviceNum);
    bool SaveLogPost1(string saveDir,vector<IMUData>& IMUresult);

    void renameImg(std::string project_path,std::vector<std::string>& imglist);

    bool ImageFilteringProcess(std::string imgtime,std::vector<IMUData>& out_IMUDataList);
    void ImageFiltering(std::vector<std::string>& Imglist,std::vector<IMUData>& out_IMUDataList,std::string &project_path);

    void ImageFiltering(std::vector<std::string>& Imglist,std::vector<IMUData>& out_IMUDataList);
public: 
    string project_path; 
};

}
