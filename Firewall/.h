#pragma once
#include "../International Organization Standardization/.h"
#ifdef FirewallApplicationProgrammingInterface
#define FirewallApplicationProgrammingInterface _declspec(dllexport)  
#else
#pragma comment(lib, "Firewall.lib")
#define FirewallApplicationProgrammingInterface  _declspec(dllimport)  
#endif
namespace Firewall {
	enum  Protocol
	{
		UDP, TCP
	};
	FirewallApplicationProgrammingInterface void Add(Protocol, int);
	FirewallApplicationProgrammingInterface void Remove(Protocol, int);
}