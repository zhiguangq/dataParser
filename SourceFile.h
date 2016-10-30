/**************************************************************************

Copyright:

Author: qiuzhiguang

Date:2016-09-25

Description: 文件处理扫描、移动、写日志文件

**************************************************************************/

#ifndef SOURCEFILE_H
#define SOURCEFILE_H
#include <string>
#include <vector>
#include <map>
#include<boost/unordered_map.hpp>
#include "Poco/NotificationQueue.h"

#define TYPENUMBER 6

// 定义六种不同的Interface枚举类型
enum INTERFACE { UU = 0, X2 = 1, UE_MR = 2, CELL_MR = 3, S1_MME = 4, S1_U = 5, ErrorType = 1000 };


// 文件信息结构体
class FileInfo{
public:
    FileInfo(){ interface = ErrorType; id = static_id++; lineNumber = 0; fileSize = 0; gapTime = 0; }
    INTERFACE       interface;      // enum interface type

    long long       id;             // ID
    std::string     name;           // 文件名
    std::string     directory;      // 文件所属目录
    std::string     type;           // 文件interface类型
    std::string     nameInlineTime; // 文件名时间
    std::string     firstLineTime;  // 首行时间
    std::string     lastLineTime;   // 末行时间
    long long       lineNumber;     // 总条目数
    long long       fileSize;       // 文件大小
    long long       gapTime;        // 当前文件首行和上一文件末行的时间差
private:
    static long long static_id;
};


class SourceFile
{
public:
    /*
    *Summary:    构造函数
    *Parameters:
    * 
    *Return : 
    */
    SourceFile();
	
    /*
    *Summary:    析构函数
    *Parameters:
    * 
    *Return : 
    */
    ~SourceFile();
	
    /*
    *Summary:    递归扫描出目录里所有文件，包括子目录
    *Parameters:
    * path:      输入目录
    *Return : 	 true 扫描成功
    */
    bool scanFiles(std::string path);
    
    /*
    *Summary:    对所有文件安文件名里的时间进行排列
    *Parameters:
    * 
    *Return : 	 true 排序成功
    */
    bool sortFiles(void);

    /*
    *Summary: 获取IMSI列表，
    *         S1U文件里URI字段里面包含有lat和lon关键字的行， 取这些行的IMSI字段去重复后作为表1
    *         UMER文件里面，取出所有的IMSI字段并去重复后作为表2
    *         表1和表2求交集，得到最终表3
    *Return : true 获取imsi表成功         
    */    
    bool getIMSI(std::string path, Poco::NotificationQueue& taskQueue, Poco::NotificationQueue& resultQueue);
    
    /*
    *Summary:    移动文件到输出目录
    *Parameters:
    * path:      输出目录
    *Return : 	 true 排序成功
    */
    bool moveFiles(std::string path);

    bool getIMSIMap(std::string path);
private:
    /*
    *Summary:    根据Interface文本，获取Interface的枚举类型
    *Parameters:
    * str:       Interface文本    "095" = UU; "099" = X2; "098" = UE_MR; "092" = CELL_MR; "096" = S1 - MME; "103" = S1 - U
    *Return : 	 INTERFACE 的枚举类型
    */
    INTERFACE getFileType(std::string str);
    
    /*
    *Summary:    获取interface类型的名称 "UU", "X2", "UE_MR", "CELL_MR", "S1-MME", "S1-U"
    *Parameters:
    * path:      INTERFACE 的枚举类型
    *Return : 	 string interface类型名称字符串
    */
    std::string getFileTypeString(INTERFACE type);
    
    /*
    *Summary:    根据Interface类型，获取此类型时间戳对应那一列
    *Parameters:
    * path:      INTERFACE 的枚举类型
    * Interface = 1   UU  第10个字段
    * Interface = 2   X2  第10个字段
    * Interface = 3   UE_MR  第14个字段
    * Interface = 4   CELL_MR  第11个字段
    * Interface = 5   S1 - MME  第10个字段
    * Interface = 11  S1 - U  第20个字段
    *Return : 	 int 列
    */
    int getTimestampColumn(INTERFACE type);
    
    /*
    *Summary:    打开文件信息，用于debug
    *Parameters:
    * fi :       文件结构体
    *Return :
    */
    void printFileInfo(FileInfo& fi);

    /*
    *Summary:    比较两个文件时间大小，用于排序算法
    *Parameters:
    * pl :       文件1
    * pr :       文件2
    *Return :    true 文件1的时间小于文件2的时间
    */
    static bool CompTime(const FileInfo& pl, const FileInfo& pr);

    /*
    *Summary:    把utc时间转成可读的时间格式
    *Parameters:
    *  timestampStr: utc时间
    *Return :  string 可读时间格式
    */
    std::string utcToStream(std::string timestampStr);
    
    /*
    *Summary:    从一行数据中读出时间戳
    *Parameters:
    * line :     一行数据
    * clo  :     时间戳所在的行
    *Return :    utc时间
    */
    std::string getTimestamp(std::string line, int clo);
    
    /*
    *Summary:    解释文件，并把文件相应信息写入FileInfo结构体
    *Parameters:
    * file :     文件名
    * fi   :     FileInfo结构体引用
    * lastFileTime ： 上一个文件最后的时间戳
    *Return :  true 解释成功
    */
    bool parseFile(std::string file, FileInfo& fi, long long &lastFileTime);

    bool copyFiles(std::string srcfile, std::string destfile, FileInfo& fi, long long &lastFileTime);

    std::vector<class FileInfo> m_interfaceFiles[TYPENUMBER + 1];   // 每个Interface文件相关数据结构体Verctor
    boost::unordered_map<std::string, int> m_imsiMap;
    int m_totalFile;
};

#endif
