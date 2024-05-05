#pragma once
#include "../Service/.h"
#include <cwctype>
#ifdef SystemApplicationProgrammingInterface
#define SystemApplicationProgrammingInterface _declspec(dllexport)  
#else
#pragma comment(lib, "System.lib")
#define SystemApplicationProgrammingInterface  _declspec(dllimport)  
#endif
namespace System {
    SystemApplicationProgrammingInterface std::string Name;
    enum Status
    {
      Initialization, Started, Paused, Updating, Stopped
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
        SystemApplicationProgrammingInterface void Update(std::wstring);
        SystemApplicationProgrammingInterface void Delete(std::wstring);
        SystemApplicationProgrammingInterface std::vector<std::pair<std::wstring, HMODULE>> Modules;
        SystemApplicationProgrammingInterface HMODULE Get(std::wstring);
        SystemApplicationProgrammingInterface bool Wait(std::wstring, System::Status);
        SystemApplicationProgrammingInterface bool If(std::wstring, System::Status);
        SystemApplicationProgrammingInterface bool WaitForModules(std::initializer_list<std::wstring>, System::Status);
        SystemApplicationProgrammingInterface bool WaitForModule(std::wstring);
        SystemApplicationProgrammingInterface bool WaitForModules(std::initializer_list<std::wstring>);
    }
}
System::Task Task;
extern "C" _declspec(dllexport)System::Status Status=System::Status::Initialization;
#ifdef StartDLL
extern void Main(); extern "C" _declspec(dllexport)void Start(){Task.Start();Status=System::Status::Initialization;Main();Status=System::Status::Started;Task.End();}
#else 
 extern "C" _declspec(dllexport) void Start(){Task.Start();Status=System::Status::Initialization;Status=System::Status::Started;Task.End();}
#endif 
#ifdef StopDLL
extern void Exit();extern "C" _declspec(dllexport)void Stop(){Status=System::Status::Paused;Task.WaitToFinish();Exit();Status=System::Status::Stopped;}
#else
extern "C" _declspec(dllexport)void Stop() {Status=System::Status::Paused;Task.WaitToFinish();Status=System::Status::Stopped; }
#endif 
#ifdef UnmountDLL
extern void Unmount();extern "C" _declspec(dllexport)void Unmounting(){Status=System::Status::Updating;Task.WaitToFinish();Unmount();}
#else
extern "C" _declspec(dllexport)void Unmounting() {Status=System::Status::Updating;Task.WaitToFinish();}
#endif