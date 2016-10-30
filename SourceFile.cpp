/**************************************************************************

Copyright:

Author: qiuzhiguang

Date:2016-09-25

Description: �ļ�����ɨ�衢�ƶ���д��־�ļ�

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
    //    Interface = 1   UU  ��10���ֶ�
    //    Interface = 2   X2  ��10���ֶ�
    //    Interface = 3   UE_MR  ��14���ֶ�
    //    Interface = 4   CELL_MR  ��11���ֶ�
    //    Interface = 5   S1 - MME  ��10���ֶ�
    //    Interface = 11  S1 - U  ��20���ֶ�
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

// ɨ��������ļ�
bool SourceFile::scanFiles(std::string path){
    if (boost::filesystem::exists(path)){   // ���Ŀ¼����
        for (boost::filesystem::directory_iterator it(path); it != boost::filesystem::directory_iterator(); ++it){  // ����Ŀ¼��������ļ�����Ŀ¼
            if (boost::filesystem::is_directory(*it))   // �������Ŀ¼
            {                
                this->scanFiles(it->path().string());   // �ݹ�ɨ����Ŀ¼
            }
            else
            {
                // ɨ�赽�ļ�
                std::string fileName = it->path().filename().string();

                boost::cmatch res;
                boost::regex reg1("(\\d+)_(\\d+)_(\\d+).(\\w+)");           // ��һ���ļ�������
                boost::regex reg2("(\\w+)_(\\d{3})(\\d+)_(\\d+).(\\w+)");   // �ڶ����ļ�������
                if (boost::regex_match(fileName.c_str(), res, reg1)){       // ���ϵ�һ���ļ�������
                    if (res.size() == 5 && getFileType(res[1]) != ErrorType){
                        FileInfo fi;
                        fi.name = res[0];                                   // �ļ���
                        fi.directory = it->path().parent_path().string();   // �ļ�Ŀ¼
                        fi.interface = getFileType(res[1]);                 // Interface ö������
                        fi.type = getFileTypeString(fi.interface);          // Interface ����������
                        fi.nameInlineTime = res[2];                         // �ļ������ʱ��
                        fi.fileSize = boost::filesystem::file_size(it->path().string());            // �ļ���С
                        //printFileInfo(fi);
                        m_interfaceFiles[fi.interface].push_back(fi);       // ����vecoter
                        m_totalFile++;
                        continue;
                    }
                }
                else if (boost::regex_match(fileName.c_str(), res, reg2)){
                    if (res.size() == 6){
                        FileInfo fi;
                        fi.name = res[0];                                   // �ļ���
                        fi.directory = it->path().parent_path().string();   // �ļ�Ŀ¼
                        fi.interface = getFileType(res[2]);                 // Interface ö������
                        fi.type = getFileTypeString(fi.interface);          // Interface ����������
                        fi.nameInlineTime = res[4];                         // �ļ������ʱ��
                        fi.fileSize = boost::filesystem::file_size(it->path().string());            // �ļ���С
                        //printFileInfo(fi);
                        m_interfaceFiles[fi.interface].push_back(fi);       // ����vecoter
                        m_totalFile++;
                        continue;
                    }
                }
                else{
                    // ƥ��ʧ�� ������ERRORĿ¼
                    FileInfo fi;
                    fi.name = it->path().filename().string();               // �ļ���
                    fi.directory = it->path().parent_path().string();       // �ļ�Ŀ¼
                    m_interfaceFiles[TYPENUMBER].push_back(fi);             // ����vecoter
                    std::cout << "File is not support : " << it->path().string() << std::endl;
                }
            }
        }
    }
    else{
        // �Ҳ���Ŀ¼
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
        // �Ȱ�ͬһ���͵��ļ����ļ������ʱ���С��������
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
                // �������lat �� lon �󣬰�imsi�ֶμ������
                std::string imsi = getTimestamp(str, 6);
                if (s1u_imsiMap.find(imsi) == s1u_imsiMap.end()){    // ���imsi�ǵ�һ�γ��֣��ͽ���map
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
        fos.write(outLine.c_str(), outLine.length());       // д��־�ļ�
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
    Poco::Timestamp  ts = Poco::Timestamp::fromEpochTime(timestamp / 1000 + 28800);     // ������+8*60*60��
    Poco::DateTime dt(ts);
    return  Poco::DateTimeFormatter::format(dt, Poco::DateTimeFormat::SORTABLE_FORMAT);  // ���ؿɶ�ʱ��
}

std::string SourceFile::getTimestamp(std::string line, int clo){
    boost::char_separator<char> sep("|");       // ��|�� ��Ϊÿ�зָ�
    typedef boost::tokenizer<boost::char_separator<char> > CustonTokenizer;
    CustonTokenizer tok(line, sep);

    int index = 0;
    std::string utcTime;
    for (CustonTokenizer::iterator beg = tok.begin(); beg != tok.end() && index != clo; ++beg)
    {
        index++;
        if (index == clo){
            utcTime = *beg;     // �ҵ���Ӧ��UTCʱ��
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

    // ���ͳ���һ�е�ʱ��
    if (fis.getline(str1, 2048)){
        ++lineCount;
        fi.firstLineTime = utcToStream(getTimestamp(str1, getTimestampColumn(fi.interface)));       // �ҳ���һ��ʱ���
        if (lastFileTime > 0){
            fi.gapTime = atoll(getTimestamp(str1, getTimestampColumn(fi.interface)).c_str()) - lastFileTime;    // ����һ���ļ�ʱ���gap
        }
        
    }
    
    // ѭ�������У��ó�����
    while (fis.getline(str1, 2048)){
        ++lineCount;
        if (!fis.getline(str2, 2048)){  // ��Ҫ��Դ�ļ��������һ��Ϊ�յģ���������Ϊ��ѭ������ʱ�������һ������
            break;
        }
        ++lineCount;
    }

    // ���ͳ����һ�е�ʱ�䣬��Ҫ��Դ�ļ��������һ��Ϊ�յ�
    if (lineCount > 0){
        std::string time1 = getTimestamp(str1, getTimestampColumn(fi.interface));
        std::string time2 = getTimestamp(str2, getTimestampColumn(fi.interface));
        fi.lastLineTime = utcToStream((std::strcmp(time1.c_str(), time2.c_str()) > 0) ? time1 : time2); // ���һ�к͵����ڶ���ʱ��Ƚ�
        lastFileTime = atoll((std::strcmp(time1.c_str(), time2.c_str()) > 0) ? time1.c_str() : time2.c_str()); // �޸�lastFileTime
    }

    fi.lineNumber = lineCount;  // �ļ�����

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
            fos.write(str, std::strlen(str));       // д���ļ�
        }
    }
    fos.close();

    return true;
}


// �ƶ��ļ�����ӦĿ¼
bool SourceFile::moveFiles(std::string path){
    if (boost::filesystem::exists(path)){
        try{
            // �ƶ�����Interface���ļ�����ӦĿ¼, error�ļ�����ERRORĿ¼
            for (int i = 0; i < TYPENUMBER ; i++){           
                long long lastFileTime = 0;
                // ѭ��ÿ��Interface���͵������ļ�
                for (std::vector<class FileInfo>::iterator it = m_interfaceFiles[i].begin(); it != m_interfaceFiles[i].end(); it++){
                    // �ƶ�ĳ���ļ�
                    boost::filesystem::path oldFile = it->directory + "\\" + it->name;

                    boost::filesystem::path newFile;

                    // ���������ڵ�ʱ��Ŀ¼
                    if (!boost::filesystem::exists(path + "\\" + it->nameInlineTime.substr(0,8))){
                        boost::filesystem::create_directory(path + "\\" + it->nameInlineTime.substr(0, 8));
                    }

                    // ��������Interface����Ŀ¼��errorĿ¼
                    if (!boost::filesystem::exists(path + "\\" + it->nameInlineTime.substr(0, 8) + "\\" + m_directory[i]))  {
                        boost::filesystem::create_directory(path + "\\" + it->nameInlineTime.substr(0, 8) + "\\" + m_directory[i]);
                    }                        

                    newFile = path + "\\" + it->nameInlineTime.substr(0, 8) + "\\" + m_directory[i] + "\\" + oldFile.filename().string();

                    //boost::filesystem::path newFile = path + "\\" + m_directory[i] + "\\" + oldFile.filename().string();
                    boost::filesystem::path renameNewFile = newFile;
                    for (char c = 'a'; boost::filesystem::exists(renameNewFile); c++){
                        // ����ƶ�Ŀ��Ŀ¼������ͬ�ļ������������ļ�_a _b����������
                        renameNewFile = boost::filesystem::path(newFile).replace_extension().string() + "_" + c + newFile.extension().string();
                    }

                    // Interface ���������ļ�дlog�ļ�
                    //Poco::FileOutputStream fos(path + "\\" + it->nameInlineTime.substr(0, 8) + "\\" + m_directory[i] + "\\log.csv", std::ios_base::app);
                    // �����ļ�
                    //parseFile(oldFile.string(), *it, lastFileTime); //�����ļ�����Ҫ��ȡ���ļ�������gapʱ��
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
                    fos.write(outLine.c_str(), outLine.length());       // д��־�ļ�   
                    */
                }
            }
        }
        catch (...){
            std::cout << "exection : " << std::endl;
        }
    }
    else{
        // �Ҳ���Ŀ¼
        std::cout << "Output Direction can not found : " << path << std::endl;
        return false;
    }
    return true;
}
