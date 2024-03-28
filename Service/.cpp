#define CodeBehindService
#include ".h"

HANDLE  DefaultInternalProcessCommunication = NULL;


ThreadManager::ThreadManager(bool(*function)()) :Function(function), Thread(new std::thread([&] { while (Thread && (*Function)()) {

// In here check cpu set some wait or notification and contione.
}
    if (Thread)
        delete this;
        })) {
        Thread->detach();

    }
ThreadManager::~ThreadManager() {
        if (Thread)
        {
            delete Thread;
            Thread = nullptr;
        }
    }


ThreadManager* ThreadManagerDeveloper = nullptr;
bool ThreadDeveloper() {

    return false;
}
void Start() {
    TRY_CATCH_ERROR

        ThreadManagerDeveloper= new ThreadManager(&ThreadDeveloper);


    END_TRY_CATCH
}
void Stop() {
    if (DefaultInternalProcessCommunication) {
        CloseHandle(DefaultInternalProcessCommunication);
        DefaultInternalProcessCommunication = NULL;
    }
}

// Basically, start up and run the service as a developer or in a real environment.
// I don't know how you will run it.
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
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr))
        throw std::runtime_error("CoInitializeEx failed.");
    INetFwPolicy2* pNetFwPolicy2 = nullptr;
    hr = CoCreateInstance(__uuidof(NetFwPolicy2), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwPolicy2), (void**)&pNetFwPolicy2);
    if (FAILED(hr)) {
        CoUninitialize();
        throw std::runtime_error("CoCreateInstance failed.");
    }
    INetFwRules* pFwRules = nullptr;
    hr = pNetFwPolicy2->get_Rules(&pFwRules);
    if (FAILED(hr)) {
        pNetFwPolicy2->Release();
        CoUninitialize();
        throw std::runtime_error("get_Rules failed.");
    }
    long ruleCount = 0;
    pFwRules->get_Count(&ruleCount);
    INetFwRule* pFwRule = nullptr;
    hr = pFwRules->Item(_bstr_t(L"we-make-software"), &pFwRule);
    if (SUCCEEDED(hr)) {
        pFwRule->Release();
        pFwRules->Release();
        pNetFwPolicy2->Release();
        CoUninitialize();
    }
    else {
        hr = CoCreateInstance(__uuidof(NetFwRule), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwRule), (void**)&pFwRule);
        if (FAILED(hr)) {
            pFwRules->Release();
            pNetFwPolicy2->Release();
            CoUninitialize();
            throw std::runtime_error("CoCreateInstance for INetFwRule failed.");
        }
        pFwRule->put_Name(_bstr_t(L"we-make-software"));
        pFwRule->put_Description(_bstr_t(L"how-do-i-get-your-attention.com"));
        pFwRule->put_ApplicationName(_bstr_t(path));
        pFwRule->put_Protocol(NET_FW_IP_PROTOCOL_ANY);
        pFwRule->put_Direction(NET_FW_RULE_DIR_IN);
        pFwRule->put_Action(NET_FW_ACTION_ALLOW);
        pFwRule->put_Enabled(VARIANT_TRUE);
        pFwRule->put_EdgeTraversal(VARIANT_TRUE);
        hr = pFwRules->Add(pFwRule);
        if (FAILED(hr)) {
            pFwRule->Release();
            pFwRules->Release();
            pNetFwPolicy2->Release();
            CoUninitialize();
            throw std::runtime_error("Failed to add firewall rule.");
        }
        pFwRule->Release();
        pFwRules->Release();
        pNetFwPolicy2->Release();
        CoUninitialize();
    }
    if (IsDebuggerPresent()) {
        std::cout << "Press any key to Start the service." << std::endl;
        (void)_getch();
        std::cout << "Calling ServiceMain, press any key to go to Action Stop." << std::endl;
        ServiceMain(1, [&]()->LPWSTR* { static LPWSTR argv[] = { const_cast<LPWSTR>(L"we-make-software") }; return argv; }());
        (void)_getch();
        std::cout << "Calling Action with parameter SERVICE_CONTROL_STOP, press any key to go to Action Shutdown." << std::endl;
        Action(SERVICE_CONTROL_STOP);
        (void)_getch();
        FreeConsole();
        ExitProcess(1);
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