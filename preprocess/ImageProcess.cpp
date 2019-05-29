#include "ImageProcess.h"

IBD_SD::CImageReName::CImageReName(void)
{

}

IBD_SD::CImageReName::~CImageReName(void) 
{

}

void IBD_SD::CImageReName::renameImg(std::string project_path,std::vector<std::string>& imglist)
{

    for(int i=0;i<imglist.size();i++)
    {
	std::string temp=imglist.at(i);
	temp=temp.substr(0,temp.length()-4);
	
	double gpstime=atof(temp.c_str())+18;
	std::string new_name=project_path+"/Rawdata/image/"+to_string(gpstime)+".jpg";
	std::string old_name=project_path+"/Rawdata/image/"+imglist.at(i);
	rename(old_name.c_str(),new_name.c_str());
LOG(INFO)<<"ImageProcess_cpp : old_name : "<<<<endl;
LOG(INFO)<<"ImageProcess_cpp : new_name:"<<new_name<<endl;

    }
}

