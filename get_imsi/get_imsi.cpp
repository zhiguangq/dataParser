/**************************************************************************

Copyright:

Author: qiuzhiguang

Date:2016-09-25

Description: �ļ�����ɨ�衢�ƶ���д��־�ļ�

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

    // ��������������ֱ�������Ŀ¼·�������Ŀ¼·������ʱ��֧������
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

    // ɨ��Ŀ¼�������ļ�������в����Ϲ���ģ�ֹͣ������
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


