#pragma once

#ifdef __linux__

#include <mysql/mysql.h>
#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>

#include "PublicShareExportMacro.h"

struct MySQL_Query_Result
{
    MYSQL_RES Res;
};

class PUBLICSHARE_API Mysql_Server
{
public:
    static Mysql_Server *Instance()
    {
        static Mysql_Server *m_Instance = new Mysql_Server();
        return m_Instance;
    }

public:
    bool CreateConnect(const std::string &host, int port, const std::string &database, const std::string &user, const std::string &password);

    bool Query(const std::string &command, MYSQL_RES **result, int *result_count);
    bool Query(const std::string &&command, MYSQL_RES **result, int *result_count);
    bool Update(const std::string &command, int *affected_row_num);
    bool Update(const std::string &&command, int *affected_row_num);

protected:
    void HeartBeat_Task();

private:
    Mysql_Server();
    ~Mysql_Server();

private:
    MYSQL *connection;
    bool Stop;

    std::condition_variable HeartBeat_cv;
    std::mutex HeartBeat_mutex;
    std::thread HeartBeat_thread;
};

#endif