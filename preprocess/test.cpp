#include <stdio.h>

#include <stdlib.h>

#include <string>

#include <fstream>

#include<opencv2/core.hpp>

#include<opencv2/highgui.hpp>

#include<opencv2/imgproc.hpp>

#include <iostream>

#include <iomanip>
#include "shellCommand.h"
#include "cJSON.h"
#include "GPS.h"
//#include <glog/logging.h>
using namespace cv;

using namespace std;

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


std::string out_dir="";
std::string project_path;
std::string BeijingDate="";

void TraceFiltering(std::vector<IMUData>& in_IMUDataList,std::vector<IMUData>& out_IMUDataList)
{
    out_IMUDataList.push_back(in_IMUDataList.at(0));
    int old_index=0;
    for(int i=1;i<in_IMUDataList.size();i++)
    {
	if(abs(in_IMUDataList.at(i).TimeStampe-in_IMUDataList.at(i-1).TimeStampe)<0.0001)//抑制数据重复
	    continue;
	if(abs(in_IMUDataList.at(i).Lat-in_IMUDataList.at(i-old_index).Lat)>1||abs(in_IMUDataList.at(i).Lat-in_IMUDataList.at(i-old_index).Lat)>1)
	 {
	    continue;//抑制位置大幅度漂移
	}
	if(in_IMUDataList.at(i).Speed<0.1)//speed = 0
	    continue;
	old_index=i;
	out_IMUDataList.push_back(in_IMUDataList.at(i));
    }

}

bool ImageFilteringProcess(std::string imgtime,std::vector<IMUData>& out_IMUDataList)
{
    double ImgGPStime=atol(imgtime.c_str());
    double diff=0;
    for(int i=0;i<out_IMUDataList.size();i++)
    {
         diff=abs(ImgGPStime-out_IMUDataList.at(i).TimeStampe);
      //  cout<<"diff:"<<diff<<endl;
	if(diff<1)
	    return true;
    }
 //   cout<<"diff:"<<diff<<endl;
    return false;
}
void ImageFiltering(std::vector<std::string>& Imglist,std::vector<IMUData>& out_IMUDataList)
{

    for(int i=0;i<Imglist.size();i++)
    {
	std::string imgtime=Imglist.at(i).substr(0,Imglist.at(i).length()-4);
        bool flag=ImageFilteringProcess(imgtime,out_IMUDataList);
	if(!flag)
	{
	    //remove to discardImg
	    std::string cmd_cp_img="cd "+project_path+"/Rawdata/image/;cp "+Imglist.at(i)+" ./discardImg/";
	    FILE* fpin=popen(cmd_cp_img.c_str(),"r");
	    if(fpin==NULL)
		break;
	     if(0!=pclose(fpin))
		break;
	    std::string img=project_path+"/Rawdata/image/"+Imglist.at(i);
	     remove(img.c_str());
	  //  cout<<Imglist.at(i)<<endl;
	
	}
     
    }
}


//二分法排序
void sortIMUData(vector<IMUData>& IMUDataList)
{

for (int i = 1; i < IMUDataList.size(); i++) 
{ 
    IMUData temp = IMUDataList.at(i); 
    //要插入的第i个元素 
    int low = 0; 
    int high = i - 1; 
    //插入目标元素的前 i-1 个元素
     int mid = -1; 
    while (low <= high) 
    { 
	mid = low + (high - low) / 2; 
	if (IMUDataList.at(mid).TimeStampe > temp.TimeStampe) 
	{ 
	    high = mid - 1; 
	} 
	else 
	{ 
    	    // 元素相同时，也插入在后面的位置 
	    low = mid + 1;
 	} 
    } 
    // 目标位置 之后的元素 整体移动一位 
    for(int j = i - 1; j >= low; j--) 
    { 
	IMUDataList.at(j + 1) = IMUDataList.at(j); 
    } 
    IMUDataList.at(low) = temp;
 }

}


void readLogToString(std::string file_path,std::string &out_json)
{
    ifstream ifn(file_path.c_str());
    if(!ifn)
    {
	std::cout<<"fail to open file"<<std::endl;
	return;
    }

    while(!ifn.eof())
    {
	std::string temp;
	getline(ifn,temp);
	out_json+=temp;
    }

    ifn.close();
}
std::string ConverGPSTimeToBeijingTime(double GPSTime)
{
cout<<"here::"<<GPSTime<<endl;
   std::string BeijingTime="";
   int hour=int(GPSTime/3600)+8;
cout<<"hour:"<<hour<<endl;
   int mini=int((int(int(GPSTime)%3600))/60);
cout<<"mini:"<<mini<<endl;
   int sec= int(GPSTime)%60;
cout<<sec<<endl;
   int subsec=int((GPSTime-int(GPSTime))*10+0.5);
cout<<subsec<<endl;
   
    BeijingTime=to_string(hour)+":"+to_string(mini)+":"+to_string(sec)+"."+to_string(subsec);
    return BeijingTime;
}

bool ParseProcess(string logstr,IMUData& m_IMUData)
{
 
     if(logstr == ""||logstr.at(0)!='#')
	return false;

    int count=0;
    for(int i=0;i<logstr.length();i++)
    {
	
        if(logstr.at(i)==',')
	    count++;

    }

 //  std::cout<<"cout::"<<count<<endl;
   if(count!=48)
       return false;

    int index = logstr.find_first_of(';', 0);
    string headerstr=logstr.substr(0,index+1);
    for(int i=0;i<7;i++)
    {
	int index2=headerstr.find_first_of(",",0);
	string temp = headerstr.substr(0,index2);
//cout<<temp<<" "<<i<<endl;

	headerstr=headerstr.substr(index2+1,logstr.length()-index2);
	if(i==5)
	{
	    m_IMUData.Weeks=temp; 

	    continue;
	}
	if(i==6)
	{
    	    double weeksecond=atof(temp.c_str());
	    m_IMUData.weekseconds=weeksecond;
    	    m_IMUData.TimeStampe=weeksecond=(int)weeksecond%(3600*24)+(weeksecond-int(weeksecond));
//cout<<m_IMUData.TimeStampe<<endl;
//	    m_IMUData.BeijingTime=ConverGPSTimeToBeijingTime(m_IMUData.TimeStampe);
	    continue;

	}

    }


    logstr = logstr.substr(index+1,logstr.length()-index);
    double ve=0;
    double vn=0;
    for(int i = 0 ;i<25;i++)
    {
	int index2=logstr.find_first_of(",",0);
	string temp = logstr.substr(0,index2);

	logstr=logstr.substr(index2+1,logstr.length()-index2);

	long double lat,lon;
	if(i==9)
	{
	    m_IMUData.Lat=atof(temp.c_str()); 
	    lat=atol(temp.c_str());
	}
	if(i==10)
	{
	    m_IMUData.Lon=atof(temp.c_str());
	    lon=atol(temp.c_str());
	}
	if(i==11)
	    m_IMUData.Hgt=atof(temp.c_str());

	if(i==16)
	    ve=atof(temp.c_str());
	if(i==17)
	    vn=atof(temp.c_str());
	if(i==22)
	    m_IMUData.Heading=atof(temp.c_str());
	if(i==23)
	    m_IMUData.Pitch=atof(temp.c_str());
	if(i==24)
	    m_IMUData.Roll=atof(temp.c_str());

	if(lat<-90||lat>90)
    	    return false;
	if(lon<0||lon>180)
    	    return false;
	if(lon==0||lat==0)
    	    return false;

    }
    m_IMUData.Speed=sqrt(ve*ve+vn*vn);

   return true;
}
void SaveLogPost1(string saveDir,vector<IMUData>& IMUresult)
{

    ofstream ofn(saveDir.c_str());
    ofn<<"GPStimestamp  northing       easting            Lat               Lon             Hgt       speed       Heading       Roll        Pitch"<<endl;
    for(int i =0;i<IMUresult.size();i++)
    {

	PCRDCARTESIAN s_x_y;
  	PCRDGEODETIC s_l_l;
	s_l_l.latitude =IMUresult.at(i).Lat;	
  	s_l_l.longitude = IMUresult.at(i).Lon;
	s_l_l.height=IMUresult.at(i).Hgt;
	GeodeticToCartesian(s_x_y, s_l_l);

        
        ofn<<std::setprecision(6) <<IMUresult.at(i).TimeStampe<<"    "
      //  <<std::setprecision(6) <<IMUresult.at(i).BeijingTime<<"    "
	<<std::setprecision(13)<<s_x_y.x<<"    "
	<<std::setprecision(13)<<s_x_y.y<<"    "

	<<std::setprecision(13)<<IMUresult.at(i).Lat<<"    "
	<<std::setprecision(13)<<IMUresult.at(i).Lon<<"    "
	<<std::setprecision(6)<<IMUresult.at(i).Hgt<<"    "
	<<std::setprecision(6)<<IMUresult.at(i).Speed<<"    "

	<<std::setprecision(8)<<IMUresult.at(i).Heading<<"    "
	<<std::setprecision(8)<<IMUresult.at(i).Roll<<"    "
	<<std::setprecision(8)<<IMUresult.at(i).Pitch<<endl;
    }

}

void SaveLogPost(string saveDir,vector<IMUData>& IMUresult)
{

    ofstream ofn(saveDir.c_str());
    ofn<<"timestamp    Lat                 Lon          Hgt       speed      Heading       Roll        Pitch"<<endl;
    for(int i =0;i<IMUresult.size();i++)
    {
        ofn<<std::setprecision(6) <<IMUresult.at(i).TimeStampe<<"    "
	<<std::setprecision(13)<<IMUresult.at(i).Lat<<"    "
	<<std::setprecision(13)<<IMUresult.at(i).Lon<<"    "
	<<std::setprecision(6)<<IMUresult.at(i).Hgt<<"    "
	<<std::setprecision(6)<<IMUresult.at(i).Speed<<"    "
	<<std::setprecision(8)<<IMUresult.at(i).Heading<<"    "
	<<std::setprecision(8)<<IMUresult.at(i).Roll<<"    "
	<<std::setprecision(8)<<IMUresult.at(i).Pitch<<endl;
    }

}


void ParseJsonstr(std::string cJSON_str,vector<IMUData>& IMUresult,int& vin)
{
    cJSON *root=cJSON_Parse(cJSON_str.c_str());

    cJSON * item = NULL;
    item= cJSON_GetObjectItem(root,"vin");
    vin=item->valueint;

    cJSON *logArray=cJSON_GetObjectItem(root,"log");
    int arraysize=cJSON_GetArraySize(logArray);

    std::string src_log=project_path+"/IMUProcess/"+to_string(vin)+"-"+BeijingDate+"-Parsedlog.txt";

    ofstream ofn(src_log.c_str());
    for(int i=0;i<arraysize;i++)
    {
	IMUData m_IMUData;
	item=cJSON_GetArrayItem(logArray,i);
	std::string log_str=item->valuestring;
	ofn<<log_str<<endl;
	ParseProcess(log_str,m_IMUData);
	IMUresult.push_back(m_IMUData);
    }
    ofn.close();
}


void ParseIMUData(string pathDir,vector<IMUData>& IMUDataList)
{

    vector<string> fileList;
    string cmd="/bin/ls "+pathDir+"/";

    getformatList(cmd,fileList);

  //  ListSort(fileList);
  //  ListSort1(fileList);

    if(fileList.size()==0)
    {
	cout<<"fileList is empty"<<endl;
	return;
    }

    //cout<<fileList.size()<<endl;
    for(int i=0;i<fileList.size();i++)
    {
	string fileDir = pathDir+fileList.at(i);

	ifstream ifn(fileDir.c_str());
        if(!ifn)
        {
	    cout<<"ifn is null"<<endl;
	    continue;
        }

	while(!ifn.eof())
	{
	    string logstr="";
	    ifn>>logstr;
	 
	   IMUData m_IMUData;
	   bool flagresult= ParseProcess(logstr,m_IMUData);
	   if(flagresult)
	       IMUDataList.push_back(m_IMUData);

	}

	ifn.close();
    }




}

bool GetDeviceNum(string src_dir, string& DeviceNum)
{
    if(src_dir.length()<13)
	return 0;


   if(src_dir[src_dir.length()-1]=='/')
	DeviceNum=src_dir.substr(src_dir.length()-6,5);
   if(src_dir[src_dir.length()-1]!='/')
	DeviceNum=src_dir.substr(src_dir.length()-5,5);
    
cout<<"NUm:"<<DeviceNum<<endl;
    return 1;
}

void renameImg(std::string project_path,std::vector<std::string>& imglist)
{

    for(int i=0;i<imglist.size();i++)
    {
	std::string temp=imglist.at(i);
	temp=temp.substr(0,temp.length()-4);
	
	double gpstime=atof(temp.c_str())+18;
	std::string new_name=project_path+"/Rawdata/image/"+to_string(gpstime)+".jpg";
	std::string old_name=project_path+"/Rawdata/image/"+imglist.at(i);
	rename(old_name.c_str(),new_name.c_str());
cout<<"old:"<<old_name<<endl;
cout<<"new:"<<new_name<<endl;

    }
}
int main(int argc,char** argv)
{

    if(argc!=3)
    {
	std::cout<<"intput param is error"<<std::endl;
	return 0;
    }

    std::string Rawdata_path=argv[1];
    std::string Project_dir=argv[2];

    std::string log_dir=Rawdata_path+"/Json_log/";


//--------parse log and process --------
    vector<IMUData> IMUresult;
    ParseIMUData(log_dir,IMUresult);
    sortIMUData(IMUresult);//sort
    std::vector<IMUData> out_IMUresult; //过滤
    TraceFiltering(IMUresult,out_IMUresult);
//---------------end----------------------
//----------create binocular project---------------
    string vin="";
    GetDeviceNum(Rawdata_path,vin);
    std::string weeks=out_IMUresult.at(0).Weeks;
    std::string subseconds=to_string(int(out_IMUresult.at(0).weekseconds));
    project_path=Project_dir+"/"+vin+"-"+weeks+"-"+subseconds+"/";
    CreateProject(project_path);
    CopyRawdata(Rawdata_path,project_path);
//---end-----
//----save post trace----
    std::string image_trace_xyz=project_path+"/IMUProcess/"+vin+"-"+weeks+"-"+subseconds+"-IMUTrace_xyz.txt";
    SaveLogPost1(image_trace_xyz,out_IMUresult);
//--------end--------------

//-------process image-----------
    std::string mkdirpath=project_path+"/Rawdata/image/discardImg/";
    if(!MakeDirection(mkdirpath));
	MakeDirection(mkdirpath);
	
    std::vector<std::string> imglist;
    std::string cmd_ls_img="cd "+project_path+"/Rawdata/image/;/bin/ls *.jpg";
    getformatList(cmd_ls_img,imglist);
//--rename time+18
    renameImg(project_path,imglist);
    imglist.clear();
    std::vector<std::string> (imglist).swap(imglist);
    getformatList(cmd_ls_img,imglist);
    ImageFiltering(imglist,out_IMUresult);

    IMUresult.clear();
    std::vector<IMUData>(IMUresult).swap(IMUresult);


//-----------------------------

}



