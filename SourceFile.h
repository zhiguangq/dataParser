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
    bool scanFiles(std::string path);       // 扫描出所有文件
    bool sortFiles(void);
    bool moveFiles(std::string path);       // 移动文件到相应目录
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
