#include "CopyFileWorker.h"

#include "Poco/FileStream.h"
#include "Poco/Notification.h"
#include "Poco/NotificationQueue.h"
#include "Poco/ThreadPool.h"
#include "Poco/AutoPtr.h"

#include <boost/shared_ptr.hpp>
#include <boost/tokenizer.hpp>  
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/searching/boyer_moore.hpp>
#include <boost/algorithm/searching/knuth_morris_pratt.hpp>

std::string CopyFileWorker::getColumn(std::string line, int clo){   // clo越大，这个函数的性能起差
    boost::char_separator<char> sep("|");       // “|” 作为每列分隔
    typedef boost::tokenizer<boost::char_separator<char> > CustonTokenizer;
    CustonTokenizer tok(line, sep);

    int index = 0;
    std::string cloumn;
    for (CustonTokenizer::iterator beg = tok.begin(); beg != tok.end() && index != clo; ++beg)
    {
        index++;
        if (index == clo){
            cloumn = *beg;
            break;
        }
    }

    return cloumn;
}

CopyFileWorker::CopyFileWorker(Poco::NotificationQueue& taskQueue, Poco::NotificationQueue& resultQueue, std::string imsiPath)
    : m_taskQueue(taskQueue)
    , m_resultQueue(resultQueue){
    getIMSIMap(imsiPath);
}

bool CopyFileWorker::getIMSIMap(std::string path){
    try{
        Poco::FileInputStream fis(path);
        char str[4096];
        while (fis.getline(str, 4096)){
            m_imsiMap[std::string(str, 15)] = 1;
        }
    }
    catch (...){
        return false;
    }

    return true;
}

void CopyFileWorker::run()
{
    Poco::AutoPtr<Poco::Notification> pNf(m_taskQueue.waitDequeueNotification());
    while (pNf)
    {
        CopyFileTaskNotification* pWorkNf = dynamic_cast<CopyFileTaskNotification*>(pNf.get());
        if (pWorkNf)
        {
            //bool SourceFile::copyFiles(std::string srcfile, std::string destfile, FileInfo& fi, long long &lastFileTime){
            Poco::FileInputStream fis(pWorkNf->getSrcFile());
            Poco::FileOutputStream fos(pWorkNf->getDestFile());

            //std::cout << "filtering file : " << pWorkNf->getSrcFile() << std::endl;
            char str[4096];
            while (fis.getline(str, 4096)){
                std::string imsi = getColumn(str, 6);
                if (m_imsiMap.find(imsi) != m_imsiMap.end()){
                    fos.write(str, std::strlen(str));       // 写新文件
                }
            }
            fos.close();
            m_resultQueue.enqueueNotification(new CopyFileResultNotification(pWorkNf->getSrcFile()));
        }
        
        pNf = m_taskQueue.waitDequeueNotification();
    }
}
