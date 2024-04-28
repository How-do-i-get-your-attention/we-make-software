#pragma once
#include "../Service/.h"
#ifdef SystemApplicationProgrammingInterface
#define SystemApplicationProgrammingInterface _declspec(dllexport)  
#else
#pragma comment(lib, "System.lib")
#define SystemApplicationProgrammingInterface  _declspec(dllimport)  
#endif
namespace System {
    enum Status
    {
        Initialization, Started, Paused, Stopped
    };
    struct SystemApplicationProgrammingInterface Task
    {
        Task();
        ~Task();
        void WaitToFinish();
        bool IsRunning();
        void Start();
        void End();
    private:
        std::vector<bool>* Counter;
        std::mutex* Mutex;
        void _Start();
        void _End();
    };
    namespace Libraries {
        SystemApplicationProgrammingInterface void Restart();
        SystemApplicationProgrammingInterface std::vector<std::pair<std::wstring, HMODULE>> Modules;
        SystemApplicationProgrammingInterface HMODULE Get(std::wstring);
        SystemApplicationProgrammingInterface bool Wait(std::wstring, System::Status);
        SystemApplicationProgrammingInterface bool WaitForModules(std::initializer_list<std::wstring>, System::Status);
        SystemApplicationProgrammingInterface bool WaitForModule(std::wstring);
        SystemApplicationProgrammingInterface bool WaitForModules(std::initializer_list<std::wstring>);
    }
}
/// <summary>
/// An instance of the Task structure. This object can be used to manage a task in the DLL.
/// </summary>
System::Task Task;
/// <summary>
/// The current status of the DLL.
/// </summary>
extern "C" _declspec(dllexport)System::Status Status = System::Status::Initialization;
/// <summary>
/// Starts the DLL. The DLL status is set to Initialization, then the provided function is executed, and finally the status is set to Started.
/// </summary>
/// <param name="Function">The function to execute when the DLL starts.</param>
#define StartDLL(Function) extern "C" _declspec(dllexport) void Start() { \
    Task.Start(); \
    Status = System::Status::Initialization; \
    Function(); \
    Status = System::Status::Started; \
    Task.End(); \
}
/// <summary>
/// Stops the DLL. The DLL status is set to Paused, then the provided function is executed, and finally the status is set to Stopped.
/// </summary>
/// <param name="Function">The function to execute when the DLL stops.</param>
#define StopDLL(Function)extern "C" _declspec(dllexport) void Stop() { \
    Status = System::Status::Paused; \
    Task.WaitToFinish(); \
    Function(); \
    Status = System::Status::Stopped; \
}
#ifndef StopDLL
extern "C" _declspec(dllexport) void Stop() {Status = System::Status::Paused; Task.WaitToFinish(); Status = System::Status::Stopped; }
#endif 

