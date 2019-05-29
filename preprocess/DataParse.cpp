#include "DataParse.h"

IBD_SD::CDataParse::CDataParse(void)
{
    project_path = ""; 
}

IBD_SD::CDataParse::~CDataParse(void)
{

}

bool IBD_SD::CDataParse::ParseProcess(string logstr,IMUData& m_IMUData)
{

    LOG(INFO) << "DataParse_CPP ParseProcess :  IN..."; 
    if(logstr == ""||logstr.at(0)!='#')
    { 
        LOG(ERROR) << "DataParse_CPP ParseProcess : LOGSTR == " " || LOGSTR.at(0)!='#'";  
        return false;
    }

    int count=0;
    for(int i=0;i<logstr.length();i++)
    {
        if(logstr.at(i)==',')
        {
           count++;
        }
    }

   if(count!=48)
   {
       LOG(ERROR) << "DataParse_CPP ParseProcess : line of imu is invalid"; 
       return false;
   }

    int index = logstr.find_first_of(';', 0);
    string headerstr=logstr.substr(0,index+1);
    for(int i=0;i<7;i++)
    {
        int index2  = headerstr.find_first_of(",",0);
        string temp = headerstr.substr(0,index2);

        headerstr=headerstr.substr(index2+1,logstr.length()-index2);
        if(i==5)
        {
            m_IMUData.Weeks=temp;

            continue;
        }
        if(i==6)
        {
            double weeksecond=atof(temp.c_str());
            m_IMUData.weekseconds =  weeksecond;
            m_IMUData.TimeStampe  =  weeksecond=(int)weeksecond%(3600*24)+(weeksecond-int(weeksecond));

//	    m_IMUData.BeijingTime=ConverGPSTimeToBeijingTime(m_IMUData.TimeStampe);
            continue;
        }

    }


    logstr = logstr.substr(index+1,logstr.length()-index);
    double ve=0;
    double vn=0;
    float  undulation = 0.0;
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
    if(i==12)
    {
        undulation   = atof(temp.c_str());
    }
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

    if(i>=24)
    {
        m_IMUData.Hgt = m_IMUData.Hgt + undulation; 
    }

    if(lat<-90||lat>90)
    {
        LOG(ERROR) << "DataParse_CPP ParseProcess : lat<-90||lat>90";
        return false;
    }
    if(lon<0||lon>180)
    {
        LOG(ERROR) << "DataParse_CPP ParseProcess : lon<0||lon>180";
        return false;
    }
    if(lon==0||lat==0)
    {
       LOG(ERROR) << "DataParse_CPP ParseProcess : lon==0||lat==0";
       return false;
    }
    }
    m_IMUData.Speed=sqrt(ve*ve+vn*vn);

   return true;
}


void IBD_SD::CDataParse::ParseIMUData(string pathDir,vector<IMUData>& IMUDataList)
{

    vector<string> fileList;
    string cmd="/bin/ls "+pathDir+"/";

    IBD_SD::CShellCommand ShellCommandObj;
    ShellCommandObj.getformatList(cmd,fileList);

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

//二分法排序
void IBD_SD::CDataParse::sortIMUData(vector<IMUData>& IMUDataList)
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


void IBD_SD::CDataParse::TraceFiltering(std::vector<IMUData>& in_IMUDataList,std::vector<IMUData>& out_IMUDataList)
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
bool IBD_SD::CDataParse::GetDeviceNum(string src_dir, string& DeviceNum)
{

    int index=0;
      for(int i=0;i<src_dir.length();i++)
      {
          if(src_dir.at(i)=='-')
              index=i;
      }

      DeviceNum=src_dir.substr(index+1,5);

      return true;
    /*
    if(src_dir.length()<13)
    return 0;


   if(src_dir[src_dir.length()-1]=='/')
    DeviceNum=src_dir.substr(src_dir.length()-6,5);
   if(src_dir[src_dir.length()-1]!='/')
    DeviceNum=src_dir.substr(src_dir.length()-5,5);

cout<<"NUm:"<<DeviceNum<<endl;
    return 1;
    */
}
bool IBD_SD::CDataParse::SaveLogPost1(string saveDir,vector<IMUData>& IMUresult)
{
    return true;  
    

    ofstream ofn(saveDir.c_str());
    if(!ofn)
        return false;
    ofn<<"GPStimestamp,northing,easting,Lat,Lon,Hgt,speed,Heading,Roll,Pitch"<<endl;
    for(int i =0;i<IMUresult.size();i++)
    {
        PCRDCARTESIAN s_x_y;
        PCRDGEODETIC s_l_l;
        s_l_l.latitude =IMUresult.at(i).Lat;
        s_l_l.longitude = IMUresult.at(i).Lon;
        s_l_l.height=IMUresult.at(i).Hgt;

        IBD_SD::CGpsCoord gpsObj; 
        gpsObj.GeodeticToCartesian(s_x_y, s_l_l);


        ofn<<std::setprecision(6) <<IMUresult.at(i).TimeStampe<<","
          //  <<std::setprecision(6) <<IMUresult.at(i).BeijingTime<<"    "
        <<std::setprecision(13)<<s_x_y.x<<","
        <<std::setprecision(13)<<s_x_y.y<<","

        <<std::setprecision(13)<<IMUresult.at(i).Lat<<","
        <<std::setprecision(13)<<IMUresult.at(i).Lon<<","
        <<std::setprecision(6)<<IMUresult.at(i).Hgt<<","
        <<std::setprecision(6)<<IMUresult.at(i).Speed<<","

        <<std::setprecision(8)<<IMUresult.at(i).Heading<<","
        <<std::setprecision(8)<<IMUresult.at(i).Roll<<","
        <<std::setprecision(8)<<IMUresult.at(i).Pitch<<endl;
    }
    return true;

}

void IBD_SD::CDataParse::renameImg(std::string project_path,std::vector<std::string>& imglist)
{

    for(int i=0;i<imglist.size();i++)
    {
        std::string temp=imglist.at(i);
        temp=temp.substr(0,temp.length()-4);

        double gpstime=atof(temp.c_str())+18;
        std::string new_name=project_path+"/Rawdata/image/"+to_string(gpstime)+".jpg";
        std::string old_name=project_path+"/Rawdata/image/"+imglist.at(i);
        rename(old_name.c_str(),new_name.c_str());
        LOG(INFO)<<"old:"<<imglist.at(i)<<" new: "<<to_string(gpstime)<<".jpg";
    }
}

bool IBD_SD::CDataParse::ImageFilteringProcess(std::string imgtime,std::vector<IMUData>& out_IMUDataList)
{
    double ImgGPStime=atol(imgtime.c_str());
    double diff=0;
    for(int i=0;i<out_IMUDataList.size();i++)
    {
        diff=abs(ImgGPStime-out_IMUDataList.at(i).TimeStampe);
        if(diff<1)
            return true;
    }
    return false;
}

void IBD_SD::CDataParse::ImageFiltering(std::vector<std::string>& Imglist,std::vector<IMUData>& out_IMUDataList,std::string &project_path)
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

void IBD_SD::CDataParse::ImageFiltering(std::vector<std::string>& Imglist,std::vector<IMUData>& out_IMUDataList)
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




