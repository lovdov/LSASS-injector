// dllmain.cpp: DLL 응용 프로그램의 진입점을 정의합니다.
//#define Universal

#include "LsassInjector.h"
#include "HandleFinder.h"
#include <string>

#define SYSTEMHANDLEINFORMATION 16

using namespace std;

DWORD FindPid(const char* strname) {
	//inlogger.LogString("Finding PID");
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32 procEntry;
	procEntry.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnapshot, &procEntry);

	do {
		if (!strcmp(procEntry.szExeFile, strname))
			return procEntry.th32ProcessID;
	} while (Process32Next(hSnapshot, &procEntry));
	return NULL;
}

void Initialize(LPVOID DLL_Param, LPVOID injection_flag) {
	DLL_PARAM* pdllParam = reinterpret_cast<DLL_PARAM*>(DLL_Param);
	BOOL* injectionflag = reinterpret_cast<BOOL*>(injection_flag);
	BOOL isNewHandle = false;
	HANDLE hProcess = NULL;

	hProcess = GetHandleIdTo(pdllParam->TargetProcessName);
	if (!hProcess){
		if(!(hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_DUP_HANDLE, 
			FALSE, FindPid(pdllParam->TargetProcessName)))) {
			*injectionflag = false;
			return;
		}
		isNewHandle = true;
	}
	if (ManualMap(hProcess, pdllParam)) {
		*injectionflag = true;
	}
	else {
		*injectionflag = false;
	}
	if (isNewHandle)
		CloseHandle(hProcess);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Initialize(lpReserved, hModule);
		break;
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

