/**************************************************************************

Copyright:

Author: qiuzhiguang

Date:2016-09-25

Description: 文件处理扫描、移动、写日志文件

**************************************************************************/

#include <iostream>
#include <fstream>

#include "..\SourceFile.h"
#include "GetImsiWorker.h"

#include "Poco/NotificationQueue.h"
#include "Poco/ThreadPool.h"
#include "Poco/AutoPtr.h" 

int main(int argc, char* argv[]){
    SourceFile sf;

    // 二个输入参数，分别是输入目录路径、输出目录路径，暂时不支持中文
    if (argc != 4){
        std::cout << "Usage : get_msi.exe <input path> <out path> <thread number>" << std::endl;
        std::cout << "      : get_msi.exe F:\\data\\input F:\\data\\output 4" << std::endl;
        return 1;
    }

#ifdef _DEBUG
    int threadNumber = 4;
#else
    int threadNumber = atoi(argv[3]) - 1;
    if (threadNumber == 0){
        std::cout << "Please input right thread." << std::endl;
        return 1;
    }
#endif

    // 扫描目录下所有文件，如果有不符合规则的，停止并返回
#ifdef _DEBUG
    if (!sf.scanFiles("F:\\data\\input")){
#else
    if (!sf.scanFiles(argv[1])){
#endif
        return 1;
    }

    Poco::NotificationQueue taskQueue;
    Poco::NotificationQueue resultQueue;
    for (int i = 0; i < threadNumber; i++){
        GetImsiWorker* worker = new GetImsiWorker(taskQueue, resultQueue);
        Poco::ThreadPool::defaultPool().start(*worker);
    }

    for (int i = 0; i < threadNumber; i++){
        
    }

#ifdef _DEBUG
    sf.getIMSI("F:\\data\\output", taskQueue);
#else
    sf.getIMSI(argv[2], taskQueue, resultQueue);
#endif

    std::cout << "------------------------- end --------------------------------" << std::endl;
    taskQueue.wakeUpAll();
    resultQueue.wakeUpAll();
    Poco::ThreadPool::defaultPool().joinAll();


    return 0;
}


