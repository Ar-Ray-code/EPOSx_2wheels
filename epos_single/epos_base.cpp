#include "epos_base.hpp"

epos_base::epos_base(std::string load_name, std::string sub_txt, std::string configure_file_path)
{
	txt_path = sub_txt;
	{
		
		try{
			std::cout << load_name.c_str() << std::endl;
			YAML::Node config = YAML::LoadFile(configure_file_path)[load_name.c_str()];
			
			int KeyHandle_int			= config["keyhandle"].as<int>();

			epos.g_usNodeId 			= config["NodeID"].as<unsigned short>();
			epos.g_deviceName 			= config["DeviceName"].as<std::string>();
			epos.g_protocolStackName 	= config["Protocol"].as<std::string>();
			epos.g_interfaceName 		= config["Interface"].as<std::string>();
    		epos.g_portName 			= config["PortName"].as<std::string>();
    		epos.g_baudrate 			= config["Baudrate"].as<int>();

			epos.g_pKeyHandle = (HANDLE)KeyHandle_int;

			PrintSettings();
		}
		catch(YAML::Exception& e)
		{
			std::cerr << e.what() << std::endl;
			exit(1);
		}
	}

    OpenDevice(&ulErrorCode);
	PrepareDemo(&ulErrorCode);

	std::thread epos_thread(&epos_base::VelocityMode,this);
	epos_thread.detach();
}

epos_base::~epos_base()
{
	CloseDevice(&ulErrorCode);
}

void epos_base::LogError(std::string functionName, int p_lResult, unsigned int p_ulErrorCode)
{
	std::cerr << functionName << " failed (result=" << p_lResult << ", errorCode=0x" << std::hex << p_ulErrorCode << ")"<< std::endl;
}

void epos_base::LogInfo(std::string message)
{
	std::cout << message << std::endl;
}

void epos_base::SeparatorLine()
{
	const int lineLength = 65;
	for(int i=0; i<lineLength; i++)
	{
		std::cout << "-";
	}
	std::cout << std::endl;
}

void epos_base::PrintSettings()
{
	std::stringstream msg;

	msg << "default settings:" << std::endl;
	msg << "node id             = " << epos.g_usNodeId << std::endl;
	msg << "device name         = '" << epos.g_deviceName << "'" << std::endl;
	msg << "protocal stack name = '" << epos.g_protocolStackName << "'" << std::endl;
	msg << "interface name      = '" << epos.g_interfaceName << "'" << std::endl;
	msg << "port name           = '" << epos.g_portName << "'"<< std::endl;
	msg << "baudrate            = " << epos.g_baudrate;

	LogInfo(msg.str());

	SeparatorLine();
}

void epos_base::OpenDevice(unsigned int* p_pErrorCode)
{
	int lResult = MMC_FAILED;

	char* pDeviceName = new char[255];
	char* pProtocolStackName = new char[255];
	char* pInterfaceName = new char[255];
	char* pPortName = new char[255];

	strcpy(pDeviceName, 		epos.g_deviceName.c_str());
	strcpy(pProtocolStackName, 	epos.g_protocolStackName.c_str());
	strcpy(pInterfaceName, 		epos.g_interfaceName.c_str());
	strcpy(pPortName, 			epos.g_portName.c_str());

	LogInfo("Open device...");

	epos.g_pKeyHandle = VCS_OpenDevice(pDeviceName, pProtocolStackName, pInterfaceName, pPortName, p_pErrorCode);

	if(epos.g_pKeyHandle!=0 && *p_pErrorCode == 0)
	{
		unsigned int lBaudrate = 0;
		unsigned int lTimeout = 0;

		if(VCS_GetProtocolStackSettings(epos.g_pKeyHandle, &lBaudrate, &lTimeout, p_pErrorCode)!=0)
		{
			if(VCS_SetProtocolStackSettings(epos.g_pKeyHandle, epos.g_baudrate, lTimeout, p_pErrorCode)!=0)
			{
				if(VCS_GetProtocolStackSettings(epos.g_pKeyHandle, &lBaudrate, &lTimeout, p_pErrorCode)!=0)
				{
					if(epos.g_baudrate==(int)lBaudrate)
					{
						lResult = MMC_SUCCESS;
					}
				}
			}
		}
	}
	else
	{
		epos.g_pKeyHandle = 0;
	}

	delete []pDeviceName;
	delete []pProtocolStackName;
	delete []pInterfaceName;
	delete []pPortName;
}

void epos_base::CloseDevice(unsigned int* p_pErrorCode)
{
	int lResult = MMC_FAILED;

	*p_pErrorCode = 0;

	LogInfo("Close device");

	if(VCS_CloseDevice(epos.g_pKeyHandle, p_pErrorCode)!=0 && *p_pErrorCode == 0)
	{
		lResult = MMC_SUCCESS;
	}
}

void epos_base::VelocityMode()
{
	int lResult = MMC_SUCCESS;
	std::stringstream msg;
	char number_str[10] = "0";
	int number = 0;

	std::list<long> velocityList;
	std::string txt;

	int state;
	int ret;

	msg << "set profile velocity mode, node = " << epos.g_usNodeId;

	LogInfo(msg.str());
	VCS_ActivateProfileVelocityMode(epos.g_pKeyHandle, epos.g_usNodeId, &ulErrorCode);

	while(1)
	{
		std::ifstream ifs(txt_path);
		if(ifs)
		{
			std::getline(ifs, txt);
			number = atoi(txt.c_str());
			std::cout << "spd: " << number << std::endl;

			VCS_MoveWithVelocity(epos.g_pKeyHandle, epos.g_usNodeId, number, &ulErrorCode);
			usleep(10000);
		}
		ifs.close();
	}
}

void epos_base::PrepareDemo(unsigned int* p_pErrorCode)
{
	int lResult = MMC_SUCCESS;
	BOOL oIsFault = 0;

	if(VCS_GetFaultState(epos.g_pKeyHandle, epos.g_usNodeId, &oIsFault, p_pErrorCode ) == 0)
	{
		LogError("VCS_GetFaultState", lResult, *p_pErrorCode);
		lResult = MMC_FAILED;
	}

	if(lResult==0)
	{
		if(oIsFault)
		{
			std::stringstream msg;
			msg << "clear fault, node = '" << epos.g_usNodeId << "'";
			LogInfo(msg.str());

			if(VCS_ClearFault(epos.g_pKeyHandle, epos.g_usNodeId, p_pErrorCode) == 0)
			{
				LogError("VCS_ClearFault", lResult, *p_pErrorCode);
				lResult = MMC_FAILED;
			}
		}

		if(lResult==0)
		{
			BOOL oIsEnabled = 0;

			if(VCS_GetEnableState(epos.g_pKeyHandle, epos.g_usNodeId, &oIsEnabled, p_pErrorCode) == 0)
			{
				LogError("VCS_GetEnableState", lResult, *p_pErrorCode);
				lResult = MMC_FAILED;
			}

			if(lResult==0)
			{
				if(!oIsEnabled)
				{
					if(VCS_SetEnableState(epos.g_pKeyHandle, epos.g_usNodeId, p_pErrorCode) == 0)
					{
						LogError("VCS_SetEnableState", lResult, *p_pErrorCode);
						lResult = MMC_FAILED;
					}
				}
			}
		}
	}
}

void epos_base::PrintHeader()
{
	SeparatorLine();

	LogInfo("Epos Command Library Example Program, (c) maxonmotor ag 2014-2019");

	SeparatorLine();
}

void epos_base::PrintAvailablePorts(char* p_pInterfaceNameSel)
{
	int lResult = MMC_FAILED;
	int lStartOfSelection = 1;
	int lMaxStrSize = 255;
	char* pPortNameSel = new char[lMaxStrSize];
	int lEndOfSelection = 0;
	unsigned int ulErrorCode = 0;

	do
	{
		if(!VCS_GetPortNameSelection((char*)epos.g_deviceName.c_str(), (char*)epos.g_protocolStackName.c_str(), p_pInterfaceNameSel, lStartOfSelection, pPortNameSel, lMaxStrSize, &lEndOfSelection, &ulErrorCode))
		{
			lResult = MMC_FAILED;
			LogError("GetPortNameSelection", lResult, ulErrorCode);
			break;
		}
		else
		{
			lResult = MMC_SUCCESS;
			printf("            port = %s\n", pPortNameSel);
		}

		lStartOfSelection = 0;
	}
	while(lEndOfSelection == 0);
}

void epos_base::PrintAvailableInterfaces()
{
	int lResult = MMC_FAILED;
	int lStartOfSelection = 1;
	int lMaxStrSize = 255;
	char* pInterfaceNameSel = new char[lMaxStrSize];
	int lEndOfSelection = 0;
	unsigned int ulErrorCode = 0;

	do
	{
		if(!VCS_GetInterfaceNameSelection((char*)epos.g_deviceName.c_str(), (char*)epos.g_protocolStackName.c_str(), lStartOfSelection, pInterfaceNameSel, lMaxStrSize, &lEndOfSelection, &ulErrorCode))
		{
			lResult = MMC_FAILED;
			LogError("GetInterfaceNameSelection", lResult, ulErrorCode);
			break;
		}
		else
		{
			lResult = MMC_SUCCESS;

			printf("interface = %s\n", pInterfaceNameSel);

			PrintAvailablePorts(pInterfaceNameSel);
		}

		lStartOfSelection = 0;
	}
	while(lEndOfSelection == 0);

	SeparatorLine();

	delete[] pInterfaceNameSel;
}

void epos_base::PrintAvailableProtocols()
{
	int lResult = MMC_FAILED;
	int lStartOfSelection = 1;
	int lMaxStrSize = 255;
	char* pProtocolNameSel = new char[lMaxStrSize];
	int lEndOfSelection = 0;
	unsigned int ulErrorCode = 0;

	do
	{
		if(!VCS_GetProtocolStackNameSelection((char*)epos.g_deviceName.c_str(), lStartOfSelection, pProtocolNameSel, lMaxStrSize, &lEndOfSelection, &ulErrorCode))
		{
			lResult = MMC_FAILED;
			LogError("GetProtocolStackNameSelection", lResult, ulErrorCode);
			break;
		}
		else
		{
			lResult = MMC_SUCCESS;

			printf("protocol stack name = %s\n", pProtocolNameSel);
		}

		lStartOfSelection = 0;
	}
	while(lEndOfSelection == 0);

	SeparatorLine();

	delete[] pProtocolNameSel;
}