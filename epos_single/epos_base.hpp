# ifndef EPOS_BASE_HPP
# define EPOS_BASE_HPP

#include <iostream>
#include <cstdlib>

#include "Definitions.h"
#include <string.h>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/times.h>
#include <sys/time.h>

#include <yaml-cpp/yaml.h>
#include <thread>

#define MMC_SUCCESS 0
#define MMC_FAILED 1

typedef void* HANDLE;

typedef struct {
    HANDLE g_pKeyHandle;
    unsigned short g_usNodeId;
    std::string g_deviceName;
    std::string g_protocolStackName;
    std::string g_interfaceName;
    std::string g_portName;
    int g_baudrate;
} epos_device;

class epos_base
{

private:
    typedef int BOOL;
    std::string txt_path;
    epos_device epos;
    unsigned int ulErrorCode;

public:

    epos_base(std::string load_name, std::string sub_txt, std::string configure_file_path);
    ~epos_base();

    void VelocityMode();
    void SeparatorLine();
    void LogError(std::string functionName, int p_lResult, unsigned int p_ulErrorCode);
    void LogInfo(std::string message);
    void PrintHeader();
    void PrintSettings();
    void OpenDevice(unsigned int* p_pErrorCode);
    void CloseDevice(unsigned int* p_pErrorCode);
    void PrepareDemo(unsigned int* p_pErrorCode);
    void PrintAvailableInterfaces();
    void PrintAvailablePorts(char* p_pInterfaceNameSel);
    void PrintAvailableProtocols();

};

#endif