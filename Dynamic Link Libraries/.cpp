#define EnableCodeBehindDynamicLinkLibraries
#include ".h"
HMODULE* DynamicLinkLibrariesModule = nullptr;
//DynamicLinkLibraries* DynamicLinkLibrariesStruct = nullptr;
//std::vector<std::wstring*, HMODULE*> Modules;
//
//DynamicLinkLibraries::DynamicLinkLibraries() {
//    // Constructor implementation
//}
//
//DynamicLinkLibraries::~DynamicLinkLibraries() {
//
//    //for (auto& module : Modules) {
//    //    HMODULE hModule = std::get<HMODULE>(module);
//    //    if (hModule != nullptr)
//    //        FreeLibrary(hModule);
//    //}
//    //Modules.clear();
//}
//void DynamicLinkLibraries::Add(std::wstring*& DisplayName)
//{
//    std::wcout << L"DisplayName: " << *DisplayName << std::endl;
//}
//HMODULE DynamicLinkLibraries::Get(std::wstring)
//{
//    return HMODULE();
//}
extern "C" {
    __declspec(dllexport) void Start(HMODULE*& module) {
        std::wcout << L"start: "<< std::endl;
        //DynamicLinkLibrariesModule = module;
        //DynamicLinkLibrariesStruct = new DynamicLinkLibraries();
        //wchar_t buffer[MAX_PATH];
        //GetModuleFileNameW(NULL, buffer, MAX_PATH);
        //std::wstring exePath(buffer);
        //WIN32_FIND_DATA findFileData;
        //HANDLE hFind = FindFirstFile((exePath.substr(0, exePath.find_last_of(L"\\/") + 1) + L"*.dll").c_str(), &findFileData);
        //do {
        //    std::wstring fullFileName = findFileData.cFileName;
        //    std::wstring* displayName = new std::wstring(fullFileName);
        //    if (fullFileName != L"Dynamic Link Libraries.dll") {
        //        size_t extPos = displayName->rfind(L".dll");
        //        if (extPos != std::wstring::npos)
        //            displayName->erase(extPos, 4);
        //        DynamicLinkLibrariesStruct->Add(displayName);
        //    }
        //    delete displayName;
        //} while (FindNextFile(hFind, &findFileData) != 0);
        //FindClose(hFind);
    }
    __declspec(dllexport) void Stop() {
   /*     delete DynamicLinkLibrariesStruct;
        DynamicLinkLibrariesStruct = nullptr;*/
    }
}