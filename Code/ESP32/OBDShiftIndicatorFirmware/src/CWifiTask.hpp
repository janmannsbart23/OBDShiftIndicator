#include "ITask.hpp"
#include <WiFi.h>


class CWifiTask : public ITask
{

public:
    CWifiTask();
    virtual ~CWifiTask();

    void StartTask();

private:
    void ReconnectWifi();
    void Task();
    static void StartTaskImpl(void *_this);
};