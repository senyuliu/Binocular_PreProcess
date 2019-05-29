#include "StereoRectify.h"

IBD_SD::CStereoRectify::CStereoRectify(void)
{

    mx1 = cvCreateMat(ROWS, COLS, CV_32F);
    my1 = cvCreateMat(ROWS, COLS, CV_32F);
    mx2 = cvCreateMat(ROWS, COLS, CV_32F);
    my2 = cvCreateMat(ROWS, COLS, CV_32F);

}

IBD_SD::CStereoRectify::~CStereoRectify(void)
{

}

bool IBD_SD::CStereoRectify::initRemap(CvMat* _Q,std::string& vin)
{
    std::string common_path="./cameras_params/"+vin + "/";
    std::cout<< "parameters: " << common_path << std::endl; 
    std::string M1_path=common_path+"/_M1.xml";
    std::cout<<M1_path<<std::endl;
    CvMat* _M1 = (CvMat*)cvLoad(M1_path.c_str());

    std::string M2_path=common_path+"/_M2.xml";
    CvMat* _M2 = (CvMat*)cvLoad(M2_path.c_str());

    std::string D1_path=common_path+"/_D1.xml";
    CvMat* _D1 = (CvMat*)cvLoad(D1_path.c_str());

    std::string D2_path=common_path+"/_D2.xml";
    CvMat* _D2 = (CvMat*)cvLoad(D2_path.c_str());

    std::string R_path=common_path+"/_R.xml";
    CvMat* _R = (CvMat*)cvLoad(R_path.c_str());

    std::string T_path=common_path+"/_T.xml";
    CvMat* _T = (CvMat*)cvLoad(T_path.c_str());

    if(_M1==NULL||_M2==NULL||_D1==NULL||_D2==NULL||_R==NULL||_T==NULL)
    {
        LOG(ERROR)<<"Load camera paramers failed";
        std::cout<<"Load camera paramers failed"<<std::endl;
        return false;
    }

    LOG(INFO)<<"Load camera paramers end";
    std::cout<<"Load camera paramers end"<<std::endl;

    double R1[3][3], R2[3][3], P1[3][4], P2[3][4], NewcameraMatrix[3][3];
    CvMat _R1 = cvMat(3, 3, CV_64F, R1);
    CvMat _R2 = cvMat(3, 3, CV_64F, R2);
    CvMat _P1 = cvMat(3, 4, CV_64F, P1);
    CvMat _P2 = cvMat(3, 4, CV_64F, P2);

    cvStereoRectify(_M1, _M2, _D1, _D2, cvSize(COLS, ROWS), _R, _T,
        &_R1, &_R2, &_P1, &_P2, _Q, 1024, -1);
    cvInitUndistortRectifyMap(_M1, _D1, &_R1, &_P1, mx1, my1);
    cvInitUndistortRectifyMap(_M2, _D2, &_R2, &_P2, mx2, my2);

    LOG(INFO)<<"Compute _Q mx my end";
    std::cout<<"Compute _Q mx my end"<<std::endl;
    return true;
}
bool IBD_SD::CStereoRectify::Rectify_main(std::string& project_path,std::string& vin)
{
    double q[4][4];
    CvMat _Q = cvMat(4, 4, CV_64F, q);
    if(!initRemap(&_Q,vin))
        return false;

    std::string Q_path=project_path+"/ImageProcess/RectifyImage/Q/_Q.xml";
    std::cout<<Q_path<<std::endl;
    cvSave(Q_path.c_str(),&_Q);

    std::string cmd_ls="cd "+project_path+"Rawdata/image/; /bin/ls *.jpg";
    std::vector<std::string> LeftRightImglist;

    IBD_SD::CShellCommand ShellCommandObj;
    ShellCommandObj.getformatList(cmd_ls,LeftRightImglist);

    LOG(INFO)<<"Rectify image num="<<LeftRightImglist.size()<<std::endl;

    std::cout<<"Rectifing ...."<<std::endl;
    for(int i=0;i<LeftRightImglist.size();i++)
    {
        std::string  srcImg_path=project_path+"Rawdata/image/"+LeftRightImglist.at(i);
        IplImage* srcImg = cvLoadImage(srcImg_path.c_str(), 1);
        if(srcImg==NULL)
        {
            std::cout<<LeftRightImglist.at(i)<<" image error"<<std::endl;
            LOG(ERROR)<<LeftRightImglist.at(i)<<" image error";
            continue;
        }
        //--------------------------------imgage cut--------------
        IplImage *left_origin,*right_origin;
        cvSetImageROI(srcImg, cvRect(0, 0, COLS, ROWS));
        left_origin = cvCreateImage(cvGetSize(srcImg), srcImg->depth, srcImg->nChannels);
        cvCopy(srcImg, left_origin, 0);
        cvResetImageROI(srcImg);

        cvSetImageROI(srcImg, cvRect(COLS, 0, COLS, ROWS));
        right_origin = cvCreateImage(cvGetSize(srcImg), srcImg->depth, srcImg->nChannels);
        cvCopy(srcImg, right_origin, 0);
        cvResetImageROI(srcImg);
        //------------------------end-------------------------------
        //---------------------------------iamge rectify----------------------
        IplImage* left_rectify = cvCreateImage(cvGetSize(left_origin), left_origin->depth, left_origin->nChannels);
        IplImage* right_rectify = cvCreateImage(cvGetSize(right_origin), right_origin->depth, right_origin->nChannels);
        cvRemap(left_origin, left_rectify, mx1, my1);
        cvRemap(right_origin, right_rectify, mx2, my2);

        string left_result_path = project_path + "/ImageProcess/RectifyImage/left/"+LeftRightImglist.at(i);
        string right_result_path = project_path + "/ImageProcess/RectifyImage/right/"+LeftRightImglist.at(i);
        Mat left = cvarrToMat(left_rectify);
        Mat right = cvarrToMat(right_rectify);
        imwrite(left_result_path, left);
        imwrite(right_result_path, right);

        cvReleaseImage(&srcImg);
        cvReleaseImage(&left_origin);
        cvReleaseImage(&right_origin);
        cvReleaseImage(&left_rectify);
        cvReleaseImage(&right_rectify);
        //--------------------------------------end---------------------------------
        LOG(INFO)<<LeftRightImglist.at(i)<<" image rectify and save end";
    }

    return true;
}
bool IBD_SD::CStereoRectify::Rectify_main_org(std::string& project_path,std::string& vin)
{

    std::string cmd_ls="cd "+project_path+"Rawdata/image/; /bin/ls *.jpg";
    std::vector<std::string> LeftRightImglist;

    IBD_SD::CShellCommand ShellCommandObj;
    ShellCommandObj.getformatList(cmd_ls,LeftRightImglist);

    LOG(INFO)<<"Rectify image num="<<LeftRightImglist.size()<<std::endl;

    std::cout<<"Rectifing ...."<<std::endl;
    for(int i=0;i<LeftRightImglist.size();i++)
    {
        std::string  srcImg_path=project_path+"Rawdata/image/"+LeftRightImglist.at(i);
        IplImage* srcImg = cvLoadImage(srcImg_path.c_str(), 1);
        if(srcImg==NULL)
        {
            std::cout<<LeftRightImglist.at(i)<<" image error"<<std::endl;
            LOG(ERROR)<<LeftRightImglist.at(i)<<" image error";
            continue;
        }
        //--------------------------------imgage cut--------------
        IplImage *left_origin,*right_origin;
        cvSetImageROI(srcImg, cvRect(0, 0, COLS, ROWS));
        left_origin = cvCreateImage(cvGetSize(srcImg), srcImg->depth, srcImg->nChannels);
        cvCopy(srcImg, left_origin, 0);
        cvResetImageROI(srcImg);

        cvSetImageROI(srcImg, cvRect(COLS, 0, COLS, ROWS));
        right_origin = cvCreateImage(cvGetSize(srcImg), srcImg->depth, srcImg->nChannels);
        cvCopy(srcImg, right_origin, 0);
        cvResetImageROI(srcImg);
        //------------------------end-------------------------------
        //---------------------------------iamge rectify----------------------

        string left_result_path = project_path + "/ImageProcess/RectifyImage/left/"+LeftRightImglist.at(i);
        string right_result_path = project_path + "/ImageProcess/RectifyImage/right/"+LeftRightImglist.at(i);
        Mat left = cvarrToMat(left_origin);
        Mat right = cvarrToMat(right_origin);
        imwrite(left_result_path, left);
        imwrite(right_result_path, right);

        cvReleaseImage(&srcImg);
        cvReleaseImage(&left_origin);
        cvReleaseImage(&right_origin);

        //--------------------------------------end---------------------------------
        LOG(INFO)<<LeftRightImglist.at(i)<<" image rectify and save end";
    }

    return true;
}
