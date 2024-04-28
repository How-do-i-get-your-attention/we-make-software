#pragma once
#define WIN32_LEAN_AND_MEAN
#define SECURITY_WIN32
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <windows.h> 
#include <sddl.h> 
#include <iostream>
#include <stdexcept>
#include <any>
#include <vector>
#include <conio.h> 
#include <string>
#include <chrono>
#include <ctime>
#include <sstream>
#include <thread>
#include <stdio.h>
#include <netfw.h>
#include <comutil.h>
#include <filesystem>
#include <mutex>
#include <schannel.h>
#include <sspi.h>
#include <functional>
#include <netfw.h>
#include <comutil.h>
#include <bitset>
#include <variant>
#pragma comment(lib, "comsuppw.lib")
#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "OleAut32.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "secur32.lib")
#pragma warning(disable : 6387 4551 6001 6031 26495 4018 28193 4251)
#define TRY_CATCH_ERROR try {
#define END_TRY_CATCH } catch (const std::runtime_error& e) { if (IsDebuggerPresent()) { std::cerr << "Error: " << e.what() << std::endl; } }