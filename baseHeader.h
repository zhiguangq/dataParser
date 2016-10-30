#ifndef BASEHEADER_H
#define BASEHEADER_H
#include <string>

#include "Poco/Notification.h"
#include "Poco/NotificationQueue.h"

#include <boost/shared_ptr.hpp>

class MyData{
public:
    MyData()
        :m_data(NULL){
        m_data = (char*)malloc(4096);

    }
    ~MyData(){
        if (m_data){
            free(m_data);
        }
    }
    char* getData(void){
        return m_data;
    }
private:
    char* m_data;
};

class TaskNotification : public Poco::Notification
{
public:
    TaskNotification(boost::shared_ptr<MyData> line) : m_stringLine(line) {}
    boost::shared_ptr<MyData> stringLine() const
    {
        return m_stringLine;
    }
private:
    boost::shared_ptr<MyData> m_stringLine;
};

class ResultNotification : public Poco::Notification
{
public:
    ResultNotification(std::string imsi) : m_imsi(imsi) {}
    std::string imsi() const
    {
        return m_imsi;
    }
private:
    std::string m_imsi;
};

class CopyFileTaskNotification : public Poco::Notification
{
public:
    CopyFileTaskNotification(std::string srcfile, std::string destfile) : m_srcfile(srcfile), m_destfile(destfile){}
    std::string getSrcFile() const
    {
        return m_srcfile;
    }
    std::string getDestFile() const
    {
        return m_destfile;
    }
private:
    std::string m_srcfile;
    std::string m_destfile;
};

class CopyFileResultNotification : public Poco::Notification
{
public:
    CopyFileResultNotification(std::string srcfile) : m_srcfile(srcfile) {}
    std::string getSrcFile() const
    {
        return m_srcfile;
    }
private:
    std::string m_srcfile;
};

#endif
