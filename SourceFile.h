/**************************************************************************

Copyright:

Author: qiuzhiguang

Date:2016-09-25

Description: �ļ�����ɨ�衢�ƶ���д��־�ļ�

**************************************************************************/

#ifndef SOURCEFILE_H
#define SOURCEFILE_H
#include <string>
#include <vector>
#include <map>
#include<boost/unordered_map.hpp>
#include "Poco/NotificationQueue.h"

#define TYPENUMBER 6

// �������ֲ�ͬ��Interfaceö������
enum INTERFACE { UU = 0, X2 = 1, UE_MR = 2, CELL_MR = 3, S1_MME = 4, S1_U = 5, ErrorType = 1000 };


// �ļ���Ϣ�ṹ��
class FileInfo{
public:
    FileInfo(){ interface = ErrorType; id = static_id++; lineNumber = 0; fileSize = 0; gapTime = 0; }
    INTERFACE       interface;      // enum interface type

    long long       id;             // ID
    std::string     name;           // �ļ���
    std::string     directory;      // �ļ�����Ŀ¼
    std::string     type;           // �ļ�interface����
    std::string     nameInlineTime; // �ļ���ʱ��
    std::string     firstLineTime;  // ����ʱ��
    std::string     lastLineTime;   // ĩ��ʱ��
    long long       lineNumber;     // ����Ŀ��
    long long       fileSize;       // �ļ���С
    long long       gapTime;        // ��ǰ�ļ����к���һ�ļ�ĩ�е�ʱ���
private:
    static long long static_id;
};


class SourceFile
{
public:
    /*
    *Summary:    ���캯��
    *Parameters:
    * 
    *Return : 
    */
    SourceFile();
	
    /*
    *Summary:    ��������
    *Parameters:
    * 
    *Return : 
    */
    ~SourceFile();
	
    /*
    *Summary:    �ݹ�ɨ���Ŀ¼�������ļ���������Ŀ¼
    *Parameters:
    * path:      ����Ŀ¼
    *Return : 	 true ɨ��ɹ�
    */
    bool scanFiles(std::string path);
    
    /*
    *Summary:    �������ļ����ļ������ʱ���������
    *Parameters:
    * 
    *Return : 	 true ����ɹ�
    */
    bool sortFiles(void);

    /*
    *Summary: ��ȡIMSI�б�
    *         S1U�ļ���URI�ֶ����������lat��lon�ؼ��ֵ��У� ȡ��Щ�е�IMSI�ֶ�ȥ�ظ�����Ϊ��1
    *         UMER�ļ����棬ȡ�����е�IMSI�ֶβ�ȥ�ظ�����Ϊ��2
    *         ��1�ͱ�2�󽻼����õ����ձ�3
    *Return : true ��ȡimsi��ɹ�         
    */    
    bool getIMSI(std::string path, Poco::NotificationQueue& taskQueue, Poco::NotificationQueue& resultQueue);
    
    /*
    *Summary:    �ƶ��ļ������Ŀ¼
    *Parameters:
    * path:      ���Ŀ¼
    *Return : 	 true ����ɹ�
    */
    bool moveFiles(std::string path);

    bool getIMSIMap(std::string path);
private:
    /*
    *Summary:    ����Interface�ı�����ȡInterface��ö������
    *Parameters:
    * str:       Interface�ı�    "095" = UU; "099" = X2; "098" = UE_MR; "092" = CELL_MR; "096" = S1 - MME; "103" = S1 - U
    *Return : 	 INTERFACE ��ö������
    */
    INTERFACE getFileType(std::string str);
    
    /*
    *Summary:    ��ȡinterface���͵����� "UU", "X2", "UE_MR", "CELL_MR", "S1-MME", "S1-U"
    *Parameters:
    * path:      INTERFACE ��ö������
    *Return : 	 string interface���������ַ���
    */
    std::string getFileTypeString(INTERFACE type);
    
    /*
    *Summary:    ����Interface���ͣ���ȡ������ʱ�����Ӧ��һ��
    *Parameters:
    * path:      INTERFACE ��ö������
    * Interface = 1   UU  ��10���ֶ�
    * Interface = 2   X2  ��10���ֶ�
    * Interface = 3   UE_MR  ��14���ֶ�
    * Interface = 4   CELL_MR  ��11���ֶ�
    * Interface = 5   S1 - MME  ��10���ֶ�
    * Interface = 11  S1 - U  ��20���ֶ�
    *Return : 	 int ��
    */
    int getTimestampColumn(INTERFACE type);
    
    /*
    *Summary:    ���ļ���Ϣ������debug
    *Parameters:
    * fi :       �ļ��ṹ��
    *Return :
    */
    void printFileInfo(FileInfo& fi);

    /*
    *Summary:    �Ƚ������ļ�ʱ���С�����������㷨
    *Parameters:
    * pl :       �ļ�1
    * pr :       �ļ�2
    *Return :    true �ļ�1��ʱ��С���ļ�2��ʱ��
    */
    static bool CompTime(const FileInfo& pl, const FileInfo& pr);

    /*
    *Summary:    ��utcʱ��ת�ɿɶ���ʱ���ʽ
    *Parameters:
    *  timestampStr: utcʱ��
    *Return :  string �ɶ�ʱ���ʽ
    */
    std::string utcToStream(std::string timestampStr);
    
    /*
    *Summary:    ��һ�������ж���ʱ���
    *Parameters:
    * line :     һ������
    * clo  :     ʱ������ڵ���
    *Return :    utcʱ��
    */
    std::string getTimestamp(std::string line, int clo);
    
    /*
    *Summary:    �����ļ��������ļ���Ӧ��Ϣд��FileInfo�ṹ��
    *Parameters:
    * file :     �ļ���
    * fi   :     FileInfo�ṹ������
    * lastFileTime �� ��һ���ļ�����ʱ���
    *Return :  true ���ͳɹ�
    */
    bool parseFile(std::string file, FileInfo& fi, long long &lastFileTime);

    bool copyFiles(std::string srcfile, std::string destfile, FileInfo& fi, long long &lastFileTime);

    std::vector<class FileInfo> m_interfaceFiles[TYPENUMBER + 1];   // ÿ��Interface�ļ�������ݽṹ��Verctor
    boost::unordered_map<std::string, int> m_imsiMap;
    int m_totalFile;
};

#endif
