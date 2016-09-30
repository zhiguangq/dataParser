#ifndef SOURCEFILE_H
#define SOURCEFILE_H
#include <string>
#include <vector>
#include <map>

#define TYPENUMBER 6

enum INTERFACE { UU = 0, X2 = 1, UE_MR = 2, CELL_MR = 3, S1_MME = 4, S1_U = 5, ErrorType = 1000 };

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

class InterfaceFiles{
public:
    std::string fileName;
    std::vector<class FileInfo> m_allFiles;
};

class SourceFile
{
public:
    SourceFile();
    ~SourceFile();
    bool scanFiles(std::string path);       // ɨ��������ļ�
    bool sortFiles(void);
    bool moveFiles(std::string path);       // �ƶ��ļ�����ӦĿ¼
private:
    INTERFACE getFileType(std::string str);
    std::string getFileTypeString(INTERFACE type);
    int getTimestampColumn(INTERFACE type);
    void printFileInfo(FileInfo& fi);
    static bool CompTime(const FileInfo& pl, const FileInfo& pr);

    std::string utcToStream(std::string timestampStr);
    std::string getTimestamp(std::string line, int clo);
    bool parseFile(std::string file, FileInfo& fi, long long &lastFileTime);

    std::vector<std::string> m_errorSourceFileList;
    std::vector<class FileInfo> m_interfaceFiles[TYPENUMBER + 1];
};

#endif
