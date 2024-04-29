#pragma once
#include "../Firewall/.h"
#ifdef NetworkApplicationProgrammingInterface
#define NetworkApplicationProgrammingInterface _declspec(dllexport)  
#else
#pragma comment(lib, "Network.lib")
#define NetworkApplicationProgrammingInterface  _declspec(dllimport)  
#endif