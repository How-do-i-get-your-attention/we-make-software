#define SystemApplicationProgrammingInterface
#include ".h"
void System::Task::_Start()
{
    std::lock_guard<std::mutex> lock(*Mutex);
    if (Counter->empty()) {
        Counter->push_back(true);
        return;
    }
    bool carry = true;
    for (auto it = Counter->rbegin(); it != Counter->rend(); ++it) {
        bool sum = *it ^ carry;
        carry = *it && carry;
        *it = sum;
        if (!carry)break;
    }
    if (carry)Counter->insert(Counter->begin(), true);
    while (!Counter->empty() && !Counter->front())
        Counter->erase(Counter->begin());
}
void System::Task::_End()
{
    std::lock_guard<std::mutex> lock(*Mutex);
    if (Counter->empty())
        return;
    bool borrow = true;
    for (auto it = Counter->rbegin(); it != Counter->rend(); ++it) {
        bool diff = *it ^ borrow;
        borrow = !*it && borrow;
        *it = diff;
        if (!borrow)
            break;
    }
    while (!Counter->empty() && !Counter->front())
        Counter->erase(Counter->begin());
}
void System::Task::Start()
{
    std::thread(&System::Task::_Start, this).detach();
}
void System::Task::End()
{
    std::thread(&System::Task::_End, this).detach();
}
bool System::Task::IsRunning()
{
    return !Counter->empty();
}
void System::Task::WaitToFinish()
{
    while (IsRunning())
        Sleep(100);
}
System::Task::Task() : Counter(new std::vector<bool>()), Mutex(new std::mutex)
{
}
System::Task::~Task()
{
    delete Counter;
    delete Mutex;
}
HMODULE System::Libraries::Get(std::wstring name)
{
    for (const auto& module : System::Libraries::Modules)
        if (module.first == name)
            return module.second;
    return nullptr;
}
bool System::Libraries::Wait(std::wstring name, System::Status status)
{
    while (auto get = Get(name)) {
        if (System::Status* FileStatus = (System::Status*)GetProcAddress(get, "Status"); !FileStatus || *FileStatus == status)
            return !FileStatus ? false : true;
        Sleep(100);
        continue;
    }
    return false;
}
bool System::Libraries::WaitForModule(std::wstring name)
{
    while (true) {
        for (const auto& module : System::Libraries::Modules)
            if (module.first == name)
                return true;
        Sleep(100);
    }
}
bool System::Libraries::WaitForModules(std::initializer_list<std::wstring> names)
{
    std::vector<std::wstring> namesVec(names);
    while (!namesVec.empty()) {
        for (auto it = namesVec.begin(); it != namesVec.end(); ) {
            bool found = false;
            for (const auto& module : System::Libraries::Modules) {
                if (module.first == *it) {
                    found = true;
                    break;
                }
            }
            if (found)
                it = namesVec.erase(it);
            else
                ++it;
            continue;
        }
        Sleep(100);
    }
    return true;
}
bool System::Libraries::WaitForModules(std::initializer_list<std::wstring> names, System::Status status)
{
    if (!WaitForModules(names)) return false;
    std::vector<std::wstring> namesVec(names);
    while (!namesVec.empty()) {
        if (Wait(namesVec.front(), status))
            namesVec.erase(namesVec.begin());
        else return false;
    }
    return true;
}
void Main() {
    wchar_t buffer[MAX_PATH];
    (void)GetModuleFileNameW(NULL, buffer, MAX_PATH);
    std::filesystem::path exePath = std::filesystem::path(buffer).parent_path();
    std::filesystem::path workerPath = exePath / "worker";
    if (std::filesystem::exists(workerPath))
        std::filesystem::remove_all(workerPath);
    std::filesystem::create_directory(workerPath);
    SetDllDirectory(workerPath.c_str());
    for (const auto& entry : std::filesystem::directory_iterator(std::filesystem::path(buffer).parent_path().wstring()))
        if (entry.path().extension() == ".dll" && entry.path().filename() != "System.dll") {
            std::filesystem::path destination = workerPath / entry.path().filename();
            std::filesystem::copy(entry.path(), destination, std::filesystem::copy_options::overwrite_existing);
            if (HMODULE hModule = LoadLibraryW(destination.c_str()))
               System::Libraries::Modules.push_back(std::make_pair(destination.filename().wstring(), hModule));
            std::vector<std::string> extensions = { ".lib", ".pdb", ".exp" };
            for (const auto& ext : extensions) {
                std::filesystem::path filePath = entry.path();
                filePath.replace_extension(ext);
                if (std::filesystem::exists(filePath)) {
                    std::filesystem::path fileDestination = workerPath / filePath.filename();
                    std::filesystem::copy(filePath, fileDestination, std::filesystem::copy_options::overwrite_existing);
                }
            }
        }
    for (auto& module : System::Libraries::Modules) {
        if (System::Status* status = (System::Status*)GetProcAddress(module.second, "Status")) continue;
        FreeLibrary(module.second);
        System::Libraries::Modules.erase(std::remove_if(System::Libraries::Modules.begin(), System::Libraries::Modules.end(), [&](const std::pair<std::wstring, HMODULE>& pair) { return pair.second == module.second; }), System::Libraries::Modules.end());
    }
    for (auto& module : System::Libraries::Modules)
        if (auto StartFunc = (void (*)())GetProcAddress(module.second, "Start"))
            std::thread([StartFunc] { StartFunc(); }).detach();
}
void Exit() {
    for (auto& module : System::Libraries::Modules)
        if (auto stopFunc = (void (*)(void))GetProcAddress(module.second, "Stop"))std::thread([stopFunc] {stopFunc(); }).detach();
    while (!System::Libraries::Modules.empty())
    {
        for (auto& module : System::Libraries::Modules) {
            if (System::Status* FileStatus = (System::Status*)GetProcAddress(module.second, "Status"); !FileStatus || *FileStatus != System::Stopped) continue;
            FreeLibrary(module.second);
            System::Libraries::Modules.erase(std::remove_if(System::Libraries::Modules.begin(), System::Libraries::Modules.end(), [&](std::pair<std::wstring, HMODULE>& pair) { return pair.second == module.second; }), System::Libraries::Modules.end());
        }
        if (System::Libraries::Modules.empty())
            break;
        else
            Sleep(100);
    }
    System::Libraries::Modules.clear();
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    std::filesystem::path exePath = std::filesystem::path(buffer).parent_path();
    std::filesystem::path workerPath = exePath / "worker";
    std::filesystem::remove_all(workerPath);
    if (IsDebuggerPresent()) {
        std::cout << "Sleeping for 5 minutes before exit..." << std::endl;
        Sleep(300000);
        exit(0);
    }
    else
        system("shutdown /r /f /t 0");
}
StartDLL(Main)
StopDLL(Exit)