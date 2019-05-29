#ifndef __IMAGEPROCESS_H__
#define __IMAGEPROCESS_H__

#include "iostream"
#include "stdlib"
#include "io.h"
#include "vector"

using namespace IBD_SD
{

CImageReName
{
public: 
    CImageReName(void); 
   
    ~CImageReName(void); 

public: 

    void renameImg(std::string project_path,std::vector<std::string>& imglist);
}

}
#endif 
