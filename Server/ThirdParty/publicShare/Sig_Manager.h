#pragma once

#ifdef __linux__

#include <vector>

using namespace std;

class SigManager
{
public:
    static SigManager *Instance()
    {
        static SigManager *m_Instance = new SigManager();
        return m_Instance;
    }

private:
    SigManager(){};

public:
    vector<int> &GetPipes();
    void AddPipe(int pipe);
    void DelPipe(int pipe);

private:
    vector<int> Pipes;
};

#endif