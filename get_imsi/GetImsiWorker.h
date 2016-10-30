#ifndef GETIMSIWORKER_H
#define GETIMSIWORKER_H

#include "../baseHeader.h"
#include "Poco/Runnable.h"

class GetImsiWorker : public Poco::Runnable
{
public:
    GetImsiWorker(Poco::NotificationQueue& taskQueue, Poco::NotificationQueue& resultQueue)
        : m_taskQueue(taskQueue)
        , m_resultQueue(resultQueue){}

    std::string getColumn(std::string line, int clo);

    void run();
private:
    Poco::NotificationQueue& m_taskQueue;
    Poco::NotificationQueue& m_resultQueue;
};

#endif
