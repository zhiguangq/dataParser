#ifndef COPYFILEWORKER_H
#define COPYFILEWORKER_H

#include <string>
#include "../baseHeader.h"
#include "Poco/Runnable.h"
#include<boost/unordered_map.hpp>

class CopyFileWorker : public Poco::Runnable
{
public:
    CopyFileWorker(Poco::NotificationQueue& taskQueue, Poco::NotificationQueue& resultQueue, std::string imsiPath);

    std::string getColumn(std::string line, int clo);

    void run();
private:
    bool getIMSIMap(std::string path);
    Poco::NotificationQueue& m_taskQueue;
    Poco::NotificationQueue& m_resultQueue;
    boost::unordered_map<std::string, int> m_imsiMap;
};

#endif
