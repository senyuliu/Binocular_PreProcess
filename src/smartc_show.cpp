#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include"./lock.h"
#include<stdio.h>
#include<iostream>
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <string>

//--rviz-test
#include "myviz.h"
#include <QApplication>
#include <QWidget>
#include <QDialog>
#include "Dialog.h"

#define DEBUG
//#undef NDEBUG
#include <glog/logging.h>

//preprocess
#include "shellCommand.h"
#include "cJSON.h"
#include "GPS.h"
#include "DataParse.h"
#include "StereoRectify.h"

//#include "preprocess.h"


using namespace std;
using namespace cv;

std::string out_dir      ="";
std::string project_path    ;
std::string BeijingDate  ="";

struct g_rviz_st
{
    MyViz*          myviz;

    QApplication*   app;

} global_param;

int PreProcess(int argc, string pathin, string pathout, MyViz* mviz);

void *thread_msg(void* ptr)
{
   DLOG(INFO)<<"smartc_show: thread_msg :  Entrances ";
   
   g_rviz_st * global_param = (g_rviz_st *) ptr;
   DLOG(INFO)<<"smartc_show: thread_msg :  global_param_OVER ";

   MyViz*        viz_th     = global_param->myviz;
   DLOG(INFO)<<"smartc_show: thread_msg :  Myviz_viz_th_OVER ";      

   QApplication* app_th     = global_param->app;
   DLOG(INFO)<<"smartc_show: thread_msg :  QApplication_app_th_OVER "; 



   while(1)
   {
       DLOG(INFO)<<"smartc_show: thread_msg :  while_Entrances ";

       sleep(1);

       LOG_FIRST_N(INFO , 1) << "smartc_show :  while in " ; 
       
       if(viz_th->start == true)
       {
         DLOG(INFO)<<"smartc_show: thread_msg :  viz_th->start_true_OVER ";
         int     argc     = 3; 
         string     pathin   = viz_th->Path4In;
         string     pathout  = viz_th->Path4Out;
         DLOG(INFO)<<"smartc_show: thread_msg :  viz_th->start_true_OVER ";

         PreProcess(argc, pathin, pathout, viz_th);
         DLOG(INFO)<<"smartc_show: thread_msg :  PreProcess_OVER "; 
       }

       viz_th->start = false; 
       DLOG(INFO)<<"smartc_show: thread_msg :  viz_th->start_false_OVER ";
       DLOG(INFO)<<"smartc_show: thread_msg :  while_OVER ";
   }

   DLOG(INFO)<<"smartc_show: thread_msg :  GameOver ";

}


int main(int argc, char** argv)
{
    //DIR_LOG 
    FLAGS_log_dir = "../log";
 
    //[-1]rviz test
    google::InitGoogleLogging(argv[0]);

    DLOG(INFO)<<"smartc_show: main :  Entrances ";
   
    //[0]thread of myviz
    QApplication app(argc,argv); 
    DLOG(INFO)<<"smartc_show: main :  app_OVER ";

    MyViz* myviz = new MyViz();
    DLOG(INFO)<<"smartc_show: main :  myviz_OVER ";

    myviz->show();
    DLOG(INFO)<<"smartc_show: main :  myviz->show_OVER ";

    global_param.myviz = myviz; 
    global_param.app   = &app  ; 
    DLOG(INFO)<<"smartc_show: main :  global_param_OVER ";

    //myviz->setDisabled(true);
    pthread_t id_msg;  
    DLOG(INFO)<<"smartc_show: main :  thread_param_id_msg_OVER ";  

    int ret_msg = pthread_create(&id_msg, NULL, thread_msg ,&global_param);
    DLOG(INFO)<<"smartc_show: main :  thread_create_id_msg_OVER ";

    if(ret_msg)
    {
        LOG(INFO)<<"smartc_show: main: create_pthread_msg error!" <<endl;
        return 1;
    }

    //[2] sleep to show
    
    //exec
    app.exec();

    //cout<<"app go to next !"<<endl;
    DLOG(INFO)<<"smartc_show: main :  GAMEOVER ";
    google::ShutdownGoogleLogging();

    return 0;
}


int PreProcess(int argc, string pathin, string pathout, MyViz* mviz)
{
    DLOG(INFO)<<"smartc_show: PreProcess :  ENTRANCE ";
    LOG(INFO) << "PreProcess: pathin is : " << pathin; 
    LOG(INFO) << "PreProcess: pathout is : "<< pathout; 

    LOG_IF(INFO, NULL == mviz) << "PreProcess: mviz is : NULL"; 

    if(argc!=3)
    {
        DLOG(ERROR)<<"smartc_show: PreProcess :  argc is not equal to three ";
    }
    if(pathin.size()<3 || pathout.size()<3)
    {
        DLOG(ERROR)<<"smartc_show: PreProcess :  pathin or pathout is invalid ";
        return 0; 
    }

    int progress_value = 1; 
    mviz->showprogress(progress_value);  //设置DISPLAY
    DLOG(INFO)<<"smartc_show: PreProcess :   mviz->showprogress_OVER";

    std::string Rawdata_path= pathin ;
    std::string Project_dir = pathout;

    std::string log_dir=Rawdata_path+"/json_log/";
    DLOG(INFO)<<"smartc_show: PreProcess :   pathin_pathout_log_dir_OVER";

    LOG(INFO) <<"smartc_show: PreProcess :   Rawdata_path : "<<Rawdata_path; 
    LOG(INFO) <<"smartc_show: PreProcess :   Project_dir  : "<<Project_dir;      
    
//[0]--------parse log and sort tracefilter --------
    vector<IBD_SD::IMUData> IMUresult;

    LOG(INFO) <<"smartc_show: PreProcess :   log_dir  : "    <<log_dir; 
    IBD_SD::CDataParse pDataParseObj; 
    pDataParseObj.ParseIMUData(log_dir,IMUresult);
    
    LOG(INFO) <<"smartc_show: PreProcess :   IMUresult size  : "<<IMUresult.size(); 
    DLOG(INFO)<<"smartc_show: PreProcess :   ParseIMUData_OVER";

    pDataParseObj.sortIMUData(IMUresult);//sort
    DLOG(INFO)<<"smartc_show: PreProcess :   sortIMUData_OVER";

    std::vector<IBD_SD::IMUData> out_IMUresult; //过滤
    pDataParseObj.TraceFiltering(IMUresult,out_IMUresult);

    LOG(INFO) <<"smartc_show: PreProcess :   TraceFiltering : out_IMUresult.size() " << out_IMUresult.size();
    DLOG(INFO)<<"smartc_show: PreProcess :   TraceFiltering_OVER";

//---------------end----------------------
    progress_value = 11; 
    mviz->showprogress(progress_value);  //设置当前的运行值
    DLOG(INFO)<<"smartc_show: PreProcess :   mviz->showprogress11_OVER";
//[1]----------get vin && create project && CopyRawdata-------------
    string vin="";
    pDataParseObj.GetDeviceNum(Rawdata_path,vin);
    LOG(INFO) <<"smartc_show: PreProcess :   GetDeviceNum vin : " << vin;
    DLOG(INFO)<<"smartc_show: PreProcess :   GetDeviceNum_OVER";   

    std::string weeks     =  out_IMUresult.at(0).Weeks;
    std::string subseconds=  to_string(int(out_IMUresult.at(0).weekseconds));

    project_path=Project_dir+"/"+vin+"-"+weeks+"-"+subseconds+"/";

    IBD_SD::CShellCommand ShellCommandObj; 
    ShellCommandObj.CreateProject(project_path);

    LOG(INFO) <<"smartc_show: PreProcess :   CreateProject: project_path : " << project_path;  
    DLOG(INFO)<<"smartc_show: PreProcess :   CreateProject_OVER";

    ShellCommandObj.CopyRawdata(Rawdata_path,project_path);
    DLOG(INFO)<<"smartc_show: PreProcess :   CopyRawdata_OVER";
//---end-----
    progress_value = 30; 
    mviz->showprogress(progress_value);  //设置当前的运行值
    DLOG(INFO)<<"smartc_show: PreProcess :   mviz->showprogress30_OVER";
//[2]----save post trace----
    std::string image_trace_xyz=project_path+"/IMUProcess/"+vin+"-"+weeks+"-"+subseconds+"-IMUTrace_xyz.txt";
    pDataParseObj.SaveLogPost1(image_trace_xyz,out_IMUresult);

    LOG(INFO) <<"smartc_show: SaveLogPost1:  image_trace_xyz path: " << image_trace_xyz; 
    DLOG(INFO)<<"smartc_show: PreProcess  :   SaveLogPost1_OVER";
//--------end--------------
    progress_value = 60; 
    mviz->showprogress(progress_value);  //设置当前的运行值
    DLOG(INFO)<<"smartc_show: PreProcess :   mviz->showprogress60_OVER";
//[3]-------process image-----------
    std::string mkdirpath=project_path+"/Rawdata/image/discardImg/";
    if(!ShellCommandObj.MakeDirection(mkdirpath))
    {
	ShellCommandObj.MakeDirection(mkdirpath);
    } 	
    LOG(INFO) <<"smartc_show: PreProcess : discardImg Path : " << mkdirpath; 
    DLOG(INFO)<<"smartc_show: PreProcess :   MakeDirection_OVER";

    std::vector<std::string> imglist;
    std::string cmd_ls_img="cd "+project_path+"/Rawdata/image/;/bin/ls *.jpg";
    ShellCommandObj.getformatList(cmd_ls_img,imglist);

    LOG(INFO) <<"smartc_show: PreProcess : Rawdata image list cmd : " << cmd_ls_img;  
    DLOG(INFO)<<"smartc_show: PreProcess :   getformatList_OVER";

    progress_value = 80; 
    mviz->showprogress(progress_value);  //设置当前的运行值
    DLOG(INFO)<<"smartc_show: PreProcess :   mviz->showprogress80_OVER";
//[4]--rename time+18
    pDataParseObj.renameImg(project_path,imglist);

    LOG(INFO) <<"smartc_show: PreProcess :   renameImg : imglist.size() : " << imglist.size() ;
    DLOG(INFO)<<"smartc_show: PreProcess :   renameImg_OVER";

    imglist.clear();
    std::vector<std::string> (imglist).swap(imglist);

    ShellCommandObj.getformatList(cmd_ls_img,imglist); 
    DLOG(INFO)<<"smartc_show: PreProcess :   getformatList_OVER";   

    pDataParseObj.ImageFiltering(imglist,out_IMUresult);

    LOG(INFO) <<"smartc_show: PreProcess :   ImageFiltering : imglist.size() : " << imglist.size();  
    DLOG(INFO)<<"smartc_show: PreProcess :   ImageFiltering_OVER";

    IMUresult.clear();
    std::vector<IBD_SD::IMUData>(IMUresult).swap(IMUresult);
//-----------------------------

//[5]----------------------- rectify-----------------------
    IBD_SD::CStereoRectify pStereoRectifyObj; 
    if(!pStereoRectifyObj.Rectify_main(project_path,vin))  //_org
    {
        DLOG(ERROR)<<"smartc_show: PreProcess :   Rectify_main_failed! vin : " << vin;
        return 0;
    }
    std::cout<<"Image rectify end"<<std::endl;
//------------------------------end------------------

    std::cout<<"sucessfully"<<std::endl;
    progress_value = 100; 
    mviz->showprogress(progress_value);  //设置当前的运行值
    DLOG(INFO)<<"smartc_show: PreProcess :   mviz->showprogress100_OVER";
    DLOG(INFO)<<"smartc_show: PreProcess :  GAME_OVER";
}



