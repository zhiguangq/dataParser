#include <iostream>
#include <fstream>

#include "SourceFile.h"

int main(int argc, char* argv[]){
    SourceFile sf;
    if (argc != 3){
        std::cout << "Usage : dataParser.exe F:\\data\\input F:\\data\\output" << std::endl;
        return 1;
    }


    // 扫描目录下所有文件，如果有不符合规则的，停止并返回
    if (!sf.scanFiles(argv[1])){
        return 1;
    }

    // 移动输入目录里所有文件到输出目录，如果相同类型目录里存在相同文件，以_a _b递加作后缀名
    if (!sf.moveFiles(argv[2])){
        return 1;
    }

    return 0;
}
