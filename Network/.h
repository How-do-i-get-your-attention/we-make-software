#pragma once
#include "../Firewall/.h"
#ifdef NetworkApplicationProgrammingInterface
#define NetworkApplicationProgrammingInterface _declspec(dllexport)  
#else
#pragma comment(lib, "Network.lib")
#define NetworkApplicationProgrammingInterface  _declspec(dllimport)  
#endif
namespace Network {
	enum  Protocol
	{
		UDP, TCP
	};
	NetworkApplicationProgrammingInterface void Add(Protocol, int, int, std::wstring);
	NetworkApplicationProgrammingInterface bool IsTCPServer(std::shared_ptr<SOCKET>);
	NetworkApplicationProgrammingInterface std::vector<unsigned char> GetTCPInterneProtocolAddress(std::shared_ptr<SOCKET> socket);
}
//#ifdef NetworkUDP
//extern "C" _declspec(dllexport)void UDP(std::shared_ptr<SOCKET>, int, std::shared_ptr<sockaddr_storage>, std::shared_ptr<std::vector<unsigned char>>);
//#define NetworkUDP(port,bufferSize,name) Network::Add(Network::Protocol::UDP, port, bufferSize, name)
//#endif
//#ifdef NetworkTCP
//extern "C" _declspec(dllexport)void TCP(std::shared_ptr<SOCKET>, int);
//#define NetworkTCP(port,bufferSize,name) Network::Add(Network::Protocol::TCP, port, bufferSize, name)
//#endif
