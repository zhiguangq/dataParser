/**************************************************************************

Copyright:

Author: qiuzhiguang

Date:2016-09-25

Description: 文件处理扫描、移动、写日志文件

**************************************************************************/

#include "SourceFile.h"
#include <iostream>
#include <boost/regex.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/format.hpp>  
#include <boost/lexical_cast.hpp>       
#include <boost/tokenizer.hpp>  
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/searching/boyer_moore.hpp>
#include <boost/algorithm/searching/knuth_morris_pratt.hpp>

#include "Poco/File.h"
#include "Poco/Path.h"
#include "Poco/FileStream.h"
#include "Poco/String.h"
#include "Poco/DateTime.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/DateTimeFormatter.h"

long long FileInfo::static_id = 1000000;
std::string m_directory[TYPENUMBER + 1] = { "UU", "X2", "UE_MR", "CELL_MR", "S1-MME", "S1-U", "ERROR"};

SourceFile::SourceFile()
{
    m_totalFile = 0;
}

SourceFile::~SourceFile()
{
}

INTERFACE SourceFile::getFileType(std::string str){
    INTERFACE allType[TYPENUMBER] = { UU, X2, UE_MR, CELL_MR, S1_MME, S1_U };
    std::string typeName[TYPENUMBER] = { "095", "099", "098", "092", "096", "103" };

    for (int i = 0; i < TYPENUMBER; i++){
        if (Poco::icompare(typeName[i], str) == 0){
            return allType[i];
        }
    }
    return ErrorType;
}

std::string SourceFile::getFileTypeString(INTERFACE type){
    INTERFACE allType[TYPENUMBER] = { UU, X2, UE_MR, CELL_MR, S1_MME, S1_U };
    std::string typeName[TYPENUMBER] = { "UU", "X2", "UE_MR", "CELL_MR", "S1-MME", "S1-U" };
    for (int i = 0; i < TYPENUMBER; i++){
        if (allType[i] == type){
            return typeName[i];
        }
    }
    return "NotDefineInterface";
}

int SourceFile::getTimestampColumn(INTERFACE type){
    //    Interface = 1   UU  第10个字段
    //    Interface = 2   X2  第10个字段
    //    Interface = 3   UE_MR  第14个字段
    //    Interface = 4   CELL_MR  第11个字段
    //    Interface = 5   S1 - MME  第10个字段
    //    Interface = 11  S1 - U  第20个字段
    switch (type){
    case UU:
    case X2:
    case S1_MME:
        return 10;
    case  UE_MR:
        return 14;
    case CELL_MR:
        return 11;
    case S1_U:
        return 20;
    default:
        return -1;
    }
    return -1;
}

void SourceFile::printFileInfo(FileInfo& fi){
    std::cout << "id :" << fi.id << std::endl;
    std::cout << "name :" << fi.name << std::endl;
    std::cout << "directory :" << fi.directory << std::endl;
    std::cout << "type :" << fi.type << std::endl;
    std::cout << "fileSize : " << fi.fileSize << std::endl;
    std::cout << "nameInlineTime : " << fi.nameInlineTime << std::endl;
    std::cout << "firstLineTime : " << fi.firstLineTime << std::endl;
    std::cout << "lastLineTime : " << fi.lastLineTime << std::endl;
    std::cout << "lineNumber : " << fi.lineNumber << std::endl;
    std::cout << "gapTime : " << fi.gapTime << std::endl;
}

// 扫描出所有文件
bool SourceFile::scanFiles(std::string path){
    if (boost::filesystem::exists(path)){   // 如果目录存在
        for (boost::filesystem::directory_iterator it(path); it != boost::filesystem::directory_iterator(); ++it){  // 查找目录里的所有文件和子目录
            if (boost::filesystem::is_directory(*it))   // 如果是子目录
            {                
                this->scanFiles(it->path().string());   // 递归扫描子目录
            }
            else
            {
                // 扫描到文件
                std::string fileName = it->path().filename().string();

                boost::cmatch res;
                boost::regex reg1("(\\d+)_(\\d+)_(\\d+).(\\w+)");           // 第一种文件名正则
                boost::regex reg2("(\\w+)_(\\d{3})(\\d+)_(\\d+).(\\w+)");   // 第二种文件名正则
                if (boost::regex_match(fileName.c_str(), res, reg1)){       // 符合第一种文件名正则
                    if (res.size() == 5 && getFileType(res[1]) != ErrorType){
                        FileInfo fi;
                        fi.name = res[0];                                   // 文件名
                        fi.directory = it->path().parent_path().string();   // 文件目录
                        fi.interface = getFileType(res[1]);                 // Interface 枚举类型
                        fi.type = getFileTypeString(fi.interface);          // Interface 类型字条串
                        fi.nameInlineTime = res[2];                         // 文件名里的时间
                        fi.fileSize = boost::filesystem::file_size(it->path().string());            // 文件大小
                        //printFileInfo(fi);
                        m_interfaceFiles[fi.interface].push_back(fi);       // 放入vecoter
                        m_totalFile++;
                        continue;
                    }
                }
                else if (boost::regex_match(fileName.c_str(), res, reg2)){
                    if (res.size() == 6){
                        FileInfo fi;
                        fi.name = res[0];                                   // 文件名
                        fi.directory = it->path().parent_path().string();   // 文件目录
                        fi.interface = getFileType(res[2]);                 // Interface 枚举类型
                        fi.type = getFileTypeString(fi.interface);          // Interface 类型字条串
                        fi.nameInlineTime = res[4];                         // 文件名里的时间
                        fi.fileSize = boost::filesystem::file_size(it->path().string());            // 文件大小
                        //printFileInfo(fi);
                        m_interfaceFiles[fi.interface].push_back(fi);       // 放入vecoter
                        m_totalFile++;
                        continue;
                    }
                }
                else{
                    // 匹配失败 ，放入ERROR目录
                    FileInfo fi;
                    fi.name = it->path().filename().string();               // 文件名
                    fi.directory = it->path().parent_path().string();       // 文件目录
                    m_interfaceFiles[TYPENUMBER].push_back(fi);             // 放入vecoter
                    std::cout << "File is not support : " << it->path().string() << std::endl;
                }
            }
        }
    }
    else{
        // 找不到目录
        std::cout << "Input Direction can not found : " << path << std::endl;
        return false;
    }

    return true;
}

bool SourceFile::CompTime(const FileInfo& pl, const FileInfo& pr)
{
    return atoll(pl.nameInlineTime.c_str()) < atoll(pr.nameInlineTime.c_str());
}

bool SourceFile::sortFiles(void){
    for (int i = 0; i < TYPENUMBER; i++){
        // 先把同一类型的文件安文件名里的时间从小到大排序
        sort(m_interfaceFiles[i].begin(), m_interfaceFiles[i].end(), SourceFile::CompTime);
    }
    return true;
}

bool SourceFile::getIMSI(std::string path){
    boost::unordered_map<std::string, int> s1u_imsiMap;
    
    std::cout << "-----------parse S1_U file (" << m_interfaceFiles[S1_U].size() << "), then add to map1------------------" << std::endl;
    int file_number = 0;
    for (std::vector<class FileInfo>::iterator it = m_interfaceFiles[S1_U].begin(); it != m_interfaceFiles[S1_U].end(); it++){  // S1_U
        boost::filesystem::path oldFile = it->directory + "\\" + it->name;
        std::cout << "parse file (" << m_interfaceFiles[S1_U].size() - file_number++ << ") : " << oldFile << std::endl;
        Poco::FileInputStream fis(oldFile.string());
        char str[4096];

        while (fis.getline(str, 4096)){
            std::string uri = getTimestamp(str, 60);
            std::string::const_iterator it1 = boost::algorithm::boyer_moore_search<std::string, std::string>(uri, "lat");
            std::string::const_iterator it2 = boost::algorithm::boyer_moore_search<std::string, std::string>(uri, "lon");
            if (it1 != uri.end() && it2 != uri.end()){
                // 如果存在lat 和 lon 后，把imsi字段加入进表
                std::string imsi = getTimestamp(str, 6);
                if (s1u_imsiMap.find(imsi) == s1u_imsiMap.end()){    // 如果imsi是第一次出现，就进入map
                    s1u_imsiMap[imsi] = 1;
                }
            }             
        }
        std::cout << "Now map1 size = " << s1u_imsiMap.size() << std::endl;
    }

    std::cout << "-----------parse UE_MR file (" << m_interfaceFiles[UE_MR].size() << "), then \"Logic and \" whth map1 to map3 ------------------" << std::endl;
    file_number = 0;
    for (std::vector<class FileInfo>::iterator it = m_interfaceFiles[UE_MR].begin(); it != m_interfaceFiles[UE_MR].end(); it++){  // UE_MR
        boost::filesystem::path oldFile = it->directory + "\\" + it->name;
        std::cout << "parse file (" << m_interfaceFiles[UE_MR].size() - file_number++ << ") : " << oldFile << std::endl;
        Poco::FileInputStream fis(oldFile.string());
        char str[4096];

        while (fis.getline(str, 4096)){
            std::string imsi = getTimestamp(str, 6);
            if (s1u_imsiMap.find(imsi) != s1u_imsiMap.end()){
                m_imsiMap[imsi] = 1;
            }
        }
        std::cout << "Now map3 size = " << m_imsiMap.size() << std::endl;
    }

    Poco::FileOutputStream fos(path + "\\" + "imsi.txt");
    for (boost::unordered_map<std::string, int>::iterator it = m_imsiMap.begin(); it != m_imsiMap.end(); it++){
        std::string outLine = it->first + "\r\n";
        fos.write(outLine.c_str(), outLine.length());       // 写日志文件
    }

    return true;
}

bool SourceFile::getIMSIMap(std::string path){
    try{
        Poco::FileInputStream fis(path);
        char str[4096];
        while (fis.getline(str, 4096)){
            m_imsiMap[std::string(str,15)] = 1;
        }
    }
    catch (...){
        return false;                                        
    }    

    return true;
}

std::string SourceFile::utcToStream(std::string timestampStr){
    long long timestamp = atoll(timestampStr.c_str());
    Poco::Timestamp  ts = Poco::Timestamp::fromEpochTime(timestamp / 1000 + 28800);     // 东八区+8*60*60秒
    Poco::DateTime dt(ts);
    return  Poco::DateTimeFormatter::format(dt, Poco::DateTimeFormat::SORTABLE_FORMAT);  // 返回可读时间
}

std::string SourceFile::getTimestamp(std::string line, int clo){
    boost::char_separator<char> sep("|");       // “|” 作为每列分隔
    typedef boost::tokenizer<boost::char_separator<char> > CustonTokenizer;
    CustonTokenizer tok(line, sep);

    int index = 0;
    std::string utcTime;
    for (CustonTokenizer::iterator beg = tok.begin(); beg != tok.end() && index != clo; ++beg)
    {
        index++;
        if (index == clo){
            utcTime = *beg;     // 找到相应列UTC时间
            break;
        }
    }

    return utcTime;
}

bool SourceFile::parseFile(std::string file, FileInfo& fi, long long &lastFileTime){
    Poco::FileInputStream fis(file);
    char str1[2048];
    char str2[2048];
    int lineCount = 0;

    // 解释出第一行的时间
    if (fis.getline(str1, 2048)){
        ++lineCount;
        fi.firstLineTime = utcToStream(getTimestamp(str1, getTimestampColumn(fi.interface)));       // 找出第一行时间戳
        if (lastFileTime > 0){
            fi.gapTime = atoll(getTimestamp(str1, getTimestampColumn(fi.interface)).c_str()) - lastFileTime;    // 与上一个文件时间戳gap
        }
        
    }
    
    // 循环所有行，得出行数
    while (fis.getline(str1, 2048)){
        ++lineCount;
        if (!fis.getline(str2, 2048)){  // 主要是源文件可能最后一行为空的，这样做是为了循环出来时能有最后一行数据
            break;
        }
        ++lineCount;
    }

    // 解释出最后一行的时间，主要是源文件可能最后一行为空的
    if (lineCount > 0){
        std::string time1 = getTimestamp(str1, getTimestampColumn(fi.interface));
        std::string time2 = getTimestamp(str2, getTimestampColumn(fi.interface));
        fi.lastLineTime = utcToStream((std::strcmp(time1.c_str(), time2.c_str()) > 0) ? time1 : time2); // 最后一行和倒数第二行时间比较
        lastFileTime = atoll((std::strcmp(time1.c_str(), time2.c_str()) > 0) ? time1.c_str() : time2.c_str()); // 修改lastFileTime
    }

    fi.lineNumber = lineCount;  // 文件行数

    return true;
}

bool SourceFile::copyFiles(std::string srcfile, std::string destfile, FileInfo& fi, long long &lastFileTime){
    Poco::FileInputStream fis(srcfile);
    Poco::FileOutputStream fos(destfile);

    std::cout << "(" << m_totalFile-- << ") filtering file : " << srcfile << std::endl;
    char str[4096];
    while (fis.getline(str, 4096)){
        std::string imsi = getTimestamp(str, 6);
        if (m_imsiMap.find(imsi) != m_imsiMap.end()){
            fos.write(str, std::strlen(str));       // 写新文件
        }
    }
    fos.close();

    return true;
}


// 移动文件到相应目录
bool SourceFile::moveFiles(std::string path){
    if (boost::filesystem::exists(path)){
        try{
            // 移动所有Interface的文件到相应目录, error文件放入ERROR目录
            for (int i = 0; i < TYPENUMBER ; i++){           
                long long lastFileTime = 0;
                // 循环每个Interface类型的所有文件
                for (std::vector<class FileInfo>::iterator it = m_interfaceFiles[i].begin(); it != m_interfaceFiles[i].end(); it++){
                    // 移动某个文件
                    boost::filesystem::path oldFile = it->directory + "\\" + it->name;

                    boost::filesystem::path newFile;

                    // 创建不存在的时间目录
                    if (!boost::filesystem::exists(path + "\\" + it->nameInlineTime.substr(0,8))){
                        boost::filesystem::create_directory(path + "\\" + it->nameInlineTime.substr(0, 8));
                    }

                    // 创建所有Interface的子目录及error目录
                    if (!boost::filesystem::exists(path + "\\" + it->nameInlineTime.substr(0, 8) + "\\" + m_directory[i]))  {
                        boost::filesystem::create_directory(path + "\\" + it->nameInlineTime.substr(0, 8) + "\\" + m_directory[i]);
                    }                        

                    newFile = path + "\\" + it->nameInlineTime.substr(0, 8) + "\\" + m_directory[i] + "\\" + oldFile.filename().string();

                    //boost::filesystem::path newFile = path + "\\" + m_directory[i] + "\\" + oldFile.filename().string();
                    boost::filesystem::path renameNewFile = newFile;
                    for (char c = 'a'; boost::filesystem::exists(renameNewFile); c++){
                        // 如果移动目标目录存在相同文件，就重命名文件_a _b，依此类推
                        renameNewFile = boost::filesystem::path(newFile).replace_extension().string() + "_" + c + newFile.extension().string();
                    }

                    // Interface 类型类型文件写log文件
                    //Poco::FileOutputStream fos(path + "\\" + it->nameInlineTime.substr(0, 8) + "\\" + m_directory[i] + "\\log.csv", std::ios_base::app);
                    // 解释文件
                    //parseFile(oldFile.string(), *it, lastFileTime); //解释文件，主要是取得文件行数和gap时间
                    copyFiles(oldFile.string(), renameNewFile.string(), *it, lastFileTime);
                    it->name = renameNewFile.filename().string();

                    //printFileInfo(*it);
                    /*
                    std::string outLine = boost::lexical_cast<std::string>(it->id) + "|" + \
                                            it->directory + "|" + \
                                            it->name + "|" + \
                                            boost::lexical_cast<std::string>(it->fileSize) + "|" + \
                                            it->type + "|" + \
                                            boost::lexical_cast<std::string>(it->lineNumber) + "|" + \
                                            it->nameInlineTime + "|" + \
                                            it->firstLineTime + "|" + \
                                            it->lastLineTime + "|" + \
                                            boost::lexical_cast<std::string>(it->gapTime) + "\r\n";

                    std::cout << outLine << std::endl;
                    fos.write(outLine.c_str(), outLine.length());       // 写日志文件   
                    */
                }
            }
        }
        catch (...){
            std::cout << "exection : " << std::endl;
        }
    }
    else{
        // 找不到目录
        std::cout << "Output Direction can not found : " << path << std::endl;
        return false;
    }
    return true;
}
