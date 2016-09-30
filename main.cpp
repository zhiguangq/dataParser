#include <iostream>
#include <fstream>

#include "SourceFile.h"

int main(int argc, char* argv[]){
    SourceFile sf;
    if (argc != 3){
        std::cout << "Usage : dataParser.exe F:\\data\\input F:\\data\\output" << std::endl;
        return 1;
    }


    // ɨ��Ŀ¼�������ļ�������в����Ϲ���ģ�ֹͣ������
#ifdef _DEBUG
    if (!sf.scanFiles("F:\\data\\input")){
#else
    if (!sf.scanFiles(argv[1])){
#endif
        return 1;
    }
    
    // ����
    sf.sortFiles();

    // �ƶ�����Ŀ¼�������ļ������Ŀ¼�������ͬ����Ŀ¼�������ͬ�ļ�����_a _b�ݼ�����׺��
#ifdef _DEBUG
    if (!sf.moveFiles("F:\\data\\output")){
#else
    if (!sf.moveFiles(argv[2])){
#endif
        return 1;
    }

    return 0;
}
