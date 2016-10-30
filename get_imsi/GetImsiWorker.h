#include "Poco/Notification.h"
#include "Poco/NotificationQueue.h"
#include "Poco/ThreadPool.h"
#include "Poco/Runnable.h"
#include "Poco/AutoPtr.h"

#include <boost/shared_ptr.hpp>
#include <boost/tokenizer.hpp>  
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/searching/boyer_moore.hpp>
#include <boost/algorithm/searching/knuth_morris_pratt.hpp>
#include <iostream>

#include "../baseHeader.h"

class GetImsiWorker : public Poco::Runnable
{
public:
    GetImsiWorker(Poco::NotificationQueue& taskQueue, Poco::NotificationQueue& resultQueue)
        : m_taskQueue(taskQueue)
        , m_resultQueue(resultQueue){}

    std::string getColumn(std::string line, int clo){   // clo越大，这个函数的性能起差
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
    void run()
    {
        Poco::AutoPtr<Poco::Notification> pNf(m_taskQueue.waitDequeueNotification());
        while (pNf)
        {
            TaskNotification* pWorkNf = dynamic_cast<TaskNotification*>(pNf.get());
            if (pWorkNf)
            {
                std::string uri = getColumn(pWorkNf->stringLine()->getData(), 60);
                std::string::const_iterator it1 = boost::algorithm::boyer_moore_search<std::string, std::string>(uri, "lat");
                std::string::const_iterator it2 = boost::algorithm::boyer_moore_search<std::string, std::string>(uri, "lon");
                std::string imsi;
                if (it1 != uri.end() && it2 != uri.end()){
                    imsi = getColumn(pWorkNf->stringLine()->getData(), 6);
                }
                m_resultQueue.enqueueNotification(new ResultNotification(imsi));
            }
            pNf = m_taskQueue.waitDequeueNotification();
        }
    }
private:
    Poco::NotificationQueue& m_taskQueue;
    Poco::NotificationQueue& m_resultQueue;
};