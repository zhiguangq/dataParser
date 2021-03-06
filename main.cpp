/**************************************************************************

Copyright:

Author: qiuzhiguang

Date:2016-09-25

Description: 文件处理扫描、移动、写日志文件

**************************************************************************/

#include <iostream>
#include <fstream>

#include "SourceFile.h"

int main(int argc, char* argv[]){
#if 0
    SourceFile sf;
	
    // 二个输入参数，分别是输入目录路径、输出目录路径，暂时不支持中文
    if (argc != 4){
        std::cout << "Usage : data_filter.exe F:\\data\\imsi.txt F:\\data\\input F:\\data\\output" << std::endl;
        return 1;
    }

#ifdef _DEBUG
    if (!sf.getIMSIMap("F:\\data\\output\\imsi.txt")){
#else
    if (!sf.getIMSIMap(argv[1])){
#endif     
        std::cout << "Can not find imsi file : " << argv[1] << std::endl;
        return false;
    }


    // 扫描目录下所有文件，如果有不符合规则的，停止并返回
#ifdef _DEBUG
    if (!sf.scanFiles("F:\\data\\input")){
#else
    if (!sf.scanFiles(argv[2])){
#endif
        return 1;
    }
                               
    // 移动输入目录里所有文件到输出目录，如果相同类型目录里存在相同文件，以_a _b递加作后缀名
#ifdef _DEBUG
    if (!sf.moveFiles("F:\\data\\output")){
#else
    if (!sf.moveFiles(argv[3])){
#endif
        return 1;
    }

    std::cout << "------------------------- end --------------------------------" << std::endl;
#endif
    return 0;
}
