/**************************************************************************

Copyright:

Author: qiuzhiguang

Date:2016-09-25

Description: �ļ�����ɨ�衢�ƶ���д��־�ļ�

**************************************************************************/

#include <iostream>
#include <fstream>

#include "..\SourceFile.h"

int main(int argc, char* argv[]){
    SourceFile sf;

    // ��������������ֱ�������Ŀ¼·�������Ŀ¼·������ʱ��֧������
    if (argc != 5){
        std::cout << "Usage : data_filter.exe <imsi file> <input path> <out path> <thread number>" << std::endl;
        std::cout << "Usage : data_filter.exe F:\\data\\imsi.txt F:\\data\\input F:\\data\\output 4" << std::endl;
        return 1;
    }

#ifdef _DEBUG
    int threadNumber = 4;
#else
    int threadNumber = atoi(argv[4]) - 1;
    if (threadNumber <= 0){
        std::cout << "Please input right thread." << std::endl;
        return 1;
    }
#endif

#ifdef _DEBUG
    if (!sf.getIMSIMap("F:\\data\\output\\imsi.txt")){
#else
    if (!sf.getIMSIMap(argv[1])){
#endif     
        std::cout << "Can not find imsi file : " << argv[1] << std::endl;
        return false;
    }


    // ɨ��Ŀ¼�������ļ�������в����Ϲ���ģ�ֹͣ������
#ifdef _DEBUG
    if (!sf.scanFiles("F:\\data\\input")){
#else
    if (!sf.scanFiles(argv[2])){
#endif
        return 1;
    }

    // �ƶ�����Ŀ¼�������ļ������Ŀ¼�������ͬ����Ŀ¼�������ͬ�ļ�����_a _b�ݼ�����׺��
#ifdef _DEBUG
    if (!sf.moveFiles("F:\\data\\output")){
#else
    if (!sf.moveFiles(argv[3])){
#endif
        return 1;
    }

    std::cout << "------------------------- end --------------------------------" << std::endl;

    return 0;
}
