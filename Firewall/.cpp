#define StopDLL
#define FirewallApplicationProgrammingInterface
#include ".h"
std::vector<std::pair<Firewall::Protocol, int>> Rules;
void Clean() {
    while (!Rules.empty()) {
		auto rule = Rules.front();
		Firewall::Protocol protocol = rule.first;
		int port = rule.second;
		Firewall::Remove(protocol, port);
	}
}
void Exit(){
    System::Libraries::Wait(L"Network.dll", System::Stopped);
    Clean();
}
std::wstring GenerateWindowsFirewallName(Firewall::Protocol protocol, int Port)
{
    static const wchar_t* protocolNames[] = { L"UDP", L"TCP" };
    std::wstringstream ss;
    ss << L"we-make-software[" << protocolNames[protocol] << L"][" << Port << L"]";
    return ss.str();
}
bool Exists(Firewall::Protocol protocol, int port)
{
    std::wstring ruleName = GenerateWindowsFirewallName(protocol, port);
    auto rule = std::make_pair(protocol, port);
    if (std::find(Rules.begin(), Rules.end(), rule) != Rules.end())
        return true;
    INetFwPolicy2* pNetFwPolicy2 = NULL;
    INetFwRules* pFwRules = NULL;
    IUnknown* pEnumerator = NULL;
    IEnumVARIANT* pVariant = NULL;
    INetFwRule* pFwRule = NULL;
    VARIANT var;
    bool ruleExists = false;
    CoInitialize(NULL);
    CoCreateInstance(__uuidof(NetFwPolicy2), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwPolicy2), (void**)&pNetFwPolicy2);
    pNetFwPolicy2->get_Rules(&pFwRules);
    pFwRules->get__NewEnum(&pEnumerator);
    pEnumerator->QueryInterface(__uuidof(IEnumVARIANT), (void**)&pVariant);
    VariantInit(&var);
    while (pVariant->Next(1, &var, NULL) == S_OK)
    {
        IUnknown* punk = V_UNKNOWN(&var);
        punk->QueryInterface(__uuidof(INetFwRule), (void**)&pFwRule);

        BSTR bstrRuleName;
        pFwRule->get_Name(&bstrRuleName);
        if (wcscmp(bstrRuleName, ruleName.c_str()) == 0)
        {
            ruleExists = true;
            SysFreeString(bstrRuleName);
            break;
        }
        SysFreeString(bstrRuleName);
        pFwRule->Release();
        VariantClear(&var);
    }
    if (pVariant != NULL) pVariant->Release();
    if (pEnumerator != NULL) pEnumerator->Release();
    if (pFwRules != NULL) pFwRules->Release();
    if (pNetFwPolicy2 != NULL) pNetFwPolicy2->Release();
    CoUninitialize();
    if (ruleExists)
        Rules.push_back(rule);
    return ruleExists;
}
std::mutex Mutex;
void Firewall::Add(Protocol protocol, int port)
{
    std::lock_guard<std::mutex> lock(Mutex);
    auto rule = std::make_pair(protocol, port);
    if (std::find(Rules.begin(), Rules.end(), rule) != Rules.end())
        return;
    if (Exists(protocol, port)) {
        Rules.push_back(rule);
        return;
    }
    std::wstring ruleName = GenerateWindowsFirewallName(protocol, port);
    CoInitialize(NULL);
    INetFwRule* pFwRule = NULL;
    CoCreateInstance(__uuidof(NetFwRule), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwRule), (void**)&pFwRule);
    pFwRule->put_Name(_bstr_t(ruleName.c_str()));
    pFwRule->put_Protocol((protocol == Protocol::TCP) ? NET_FW_IP_PROTOCOL_TCP : NET_FW_IP_PROTOCOL_UDP);
    pFwRule->put_LocalPorts(_bstr_t(std::to_wstring(port).c_str()));
    pFwRule->put_Action(NET_FW_ACTION_ALLOW);
    pFwRule->put_Enabled(VARIANT_TRUE);
    INetFwPolicy2* pNetFwPolicy2 = NULL;
    CoCreateInstance(__uuidof(NetFwPolicy2), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwPolicy2), (void**)&pNetFwPolicy2);
    INetFwRules* pFwRules = NULL;
    pNetFwPolicy2->get_Rules(&pFwRules);
    pFwRules->Add(pFwRule);
    pFwRule->put_EdgeTraversal(VARIANT_TRUE);
    pFwRule->Release();
    pFwRules->Release();
    pNetFwPolicy2->Release();
    CoUninitialize();
    Rules.push_back(rule);
}
void Firewall::Remove(Protocol protocol, int port)
{
    std::lock_guard<std::mutex> lock(Mutex);
    auto rule = std::make_pair(protocol, port);
    auto ruleIter = std::find(Rules.begin(), Rules.end(), rule);
    if (ruleIter == Rules.end())
        return;
    std::wstring ruleName = GenerateWindowsFirewallName(protocol, port);
    CoInitialize(NULL);
    INetFwPolicy2* pNetFwPolicy2 = NULL;
    CoCreateInstance(__uuidof(NetFwPolicy2), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwPolicy2), (void**)&pNetFwPolicy2);
    INetFwRules* pFwRules = NULL;
    pNetFwPolicy2->get_Rules(&pFwRules);
    pFwRules->Remove(_bstr_t(ruleName.c_str()));
    pFwRules->Release();
    pNetFwPolicy2->Release();
    CoUninitialize();
    Rules.erase(ruleIter);
}
