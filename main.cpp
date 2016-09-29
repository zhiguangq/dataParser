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
    if (!sf.scanFiles(argv[1])){
        return 1;
    }

    // �ƶ�����Ŀ¼�������ļ������Ŀ¼�������ͬ����Ŀ¼�������ͬ�ļ�����_a _b�ݼ�����׺��
    if (!sf.moveFiles(argv[2])){
        return 1;
    }

    return 0;
}
