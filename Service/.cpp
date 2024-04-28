#include ".h"
HMODULE* System = nullptr;
void Start() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    ((void (*)())GetProcAddress(*(System = new HMODULE(LoadLibraryW(L"System.dll"))), "Start"))();
}
void Stop() {
    ((void (*)(void))GetProcAddress(*System, "Stop"))();
    FreeLibrary(*System);
    WSACleanup();
    delete System;
    System = nullptr;
}
SERVICE_STATUS serviceStatus;
SERVICE_STATUS serviceStatusHandle;
SERVICE_STATUS_HANDLE Handle;
void __stdcall Action(DWORD dwControl) {
    if (IsDebuggerPresent()) {
        Stop();
        return;
    }
    if (dwControl == SERVICE_CONTROL_STOP || dwControl == SERVICE_CONTROL_SHUTDOWN) {
        serviceStatusHandle.dwCurrentState = SERVICE_STOP_PENDING;
        SetServiceStatus(Handle, &serviceStatusHandle);
        serviceStatusHandle.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(Handle, &serviceStatusHandle);
        Stop();

        return;
    }
}
void __stdcall ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv) {
    if (IsDebuggerPresent()) {
        Start();
        return;
    }
    struct NullBuffer : std::streambuf { int overflow(int c) override { return c; } };
    NullBuffer null_buffer;
    std::cout.rdbuf(&null_buffer);
    struct wNullBuffer : std::wstreambuf { int_type overflow(int_type c) override { return c; } };
    wNullBuffer wnull_buffer;
    std::wcout.rdbuf(&wnull_buffer);
    SetServiceStatus((Handle = RegisterServiceCtrlHandlerW(L"we-make-software", Action)), &(serviceStatusHandle = { .dwServiceType = SERVICE_WIN32_OWN_PROCESS,.dwCurrentState = SERVICE_RUNNING,.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN ,.dwWin32ExitCode = NO_ERROR,.dwServiceSpecificExitCode = 0,.dwCheckPoint = 0,.dwWaitHint = 0 }));
    Start();
}
int main() {
    TRY_CATCH_ERROR
        if ([]()->bool {
            HANDLE tokenHandle = NULL;
            if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &tokenHandle))
                throw std::runtime_error("Failed to open process token.");
                DWORD bufferSize = 0;
                GetTokenInformation(tokenHandle, TokenUser, NULL, 0, &bufferSize);
                if (bufferSize == 0) {
                    std::cerr << "Failed to get the buffer size for token information.\n";
                    CloseHandle(tokenHandle);
                    throw std::runtime_error("Failed to get the buffer size for token information.");
                }
            PTOKEN_USER tokenUser = (PTOKEN_USER)malloc(bufferSize);
            if (tokenUser == NULL) {
                CloseHandle(tokenHandle);
                throw std::runtime_error("Failed to allocate memory for token user.");
            }
            if (!GetTokenInformation(tokenHandle, TokenUser, tokenUser, bufferSize, &bufferSize)) {
                free(tokenUser);
                CloseHandle(tokenHandle);
                throw std::runtime_error("Failed to get token information.");
            }
            LPWSTR stringSid = NULL;
            if (!ConvertSidToStringSid(tokenUser->User.Sid, &stringSid)) {
                free(tokenUser);
                CloseHandle(tokenHandle);
                throw std::runtime_error("Failed to convert SID to string.");
            }
            bool isSystem = (wcscmp(stringSid, L"S-1-5-18") == 0);
            LocalFree(stringSid);
            free(tokenUser);
            CloseHandle(tokenHandle);
            return isSystem;
            }()) {
            SERVICE_TABLE_ENTRYW Table[] =
            {
                { const_cast<LPWSTR>(L"we-make-software"), reinterpret_cast<LPSERVICE_MAIN_FUNCTIONW>(ServiceMain) },
                { NULL, NULL }
            };
            StartServiceCtrlDispatcherW(Table);
            return 0;
        }
    BOOL isAdmin = FALSE;
    PSID adminGroupSID = NULL;
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
    if (!AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroupSID)) {
        throw std::runtime_error("AllocateAndInitializeSid failed: " + std::to_string(GetLastError()));
    }
    if (!CheckTokenMembership(NULL, adminGroupSID, &isAdmin)) {
        FreeSid(adminGroupSID); // Ensure SID is freed to prevent memory leaks
        throw std::runtime_error("CheckTokenMembership failed: " + std::to_string(GetLastError()));
    }
    FreeSid(adminGroupSID); // Free SID after use
    if (!isAdmin) {
        throw std::runtime_error("Not running as System or Administrator?");
    }
    wchar_t path[MAX_PATH] = { 0 };
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    if (IsDebuggerPresent()) {
        std::cout << "Press any key to Start the service." << std::endl;
        (void)_getch();
        std::cout << "Calling ServiceMain, press any key to go to Action Stop." << std::endl;
        Start();
        (void)_getch();
        std::cout << "Calling Action with parameter SERVICE_CONTROL_STOP, press any key to go to Action Shutdown." << std::endl;
        Stop();
        (void)_getch();
        main();
        return 0;
    }
    SC_HANDLE schSCManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
    SC_HANDLE schService = OpenService(schSCManager, L"we-make-software", SERVICE_ALL_ACCESS);
    SERVICE_STATUS_PROCESS ssp;
    if (schService && QueryServiceStatusEx(schService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), nullptr) && ssp.dwCurrentState == SERVICE_RUNNING) {
        ControlService(schService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&ssp);
        while (QueryServiceStatusEx(schService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(ssp), nullptr) && ssp.dwCurrentState != SERVICE_STOPPED)
            Sleep(2000);
    }
    if (schService)
        DeleteService(schService);
    schService = CreateService(schSCManager, L"we-make-software", L"We Make Software Service", SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_IGNORE, path, nullptr, nullptr, nullptr, nullptr, nullptr);
    StartService(schService, 0, nullptr);
    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
    END_TRY_CATCH
        return -1;
}