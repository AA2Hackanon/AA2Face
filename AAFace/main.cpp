#include <Windows.h>
#include <Psapi.h>
#include "Error.h"

#pragma comment( lib, "psapi.lib" )

#pragma pack(1)
struct LoadfuncParam {
	HMODULE(*LoadLibraryPtr)(LPCSTR);
	void(*GetLastErrorPtr)();
	TCHAR dllname[];
};

extern "C" DWORD __cdecl threadinjectproc(_In_ LPVOID lpParameter); //its actually stdcall, but linker n stuff.
DWORD WriteAsmCodeToBuffer(char* code) {
	const BYTE int3 = 0xCC;
	const BYTE int3end[8] = {int3, int3, int3, int3, int3, int3, int3, int3};
	DWORD offset = (DWORD)threadinjectproc;
	DWORD it = offset;
	while(memcmp((void*)it, int3end, 8) != 0) {
		it++;
	}
	DWORD size = it - offset;
	if(size > 0) {
		memcpy(code,(void*)offset,size);
	}
	return size;
}

int SetDebugPrevilege(HANDLE token,BOOL state) {
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if (!LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&luid)) {
		Error::SetLastError(GetLastError(),"LookupPrivilegeValue");
		return -1;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (state == TRUE) //true == enable
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	// Enable the privilege or disable all privileges.
	BOOL adjSuccess = AdjustTokenPrivileges(token,FALSE,&tp,sizeof(TOKEN_PRIVILEGES),
		(PTOKEN_PRIVILEGES)NULL,(PDWORD)NULL);
	DWORD lastError = GetLastError();
	if (lastError == ERROR_NOT_ALL_ASSIGNED) {
		return 0;
	}
	else if (!adjSuccess) {
		Error::SetLastError(lastError,"ImpersonateSelf");
		return -1;
	}

	return 1;
}

BOOL GetDebugAdjustToken(HANDLE* token) {
	if (!OpenThreadToken(GetCurrentThread(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,FALSE,token)) {
		if (!ImpersonateSelf(SecurityImpersonation)) {
			Error::SetLastError(GetLastError(),"ImpersonateSelf");
			return FALSE;
		}

		if (!OpenThreadToken(GetCurrentThread(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,FALSE,token)) {
			Error::SetLastError(GetLastError(),"OpenThreadToken");
			return FALSE;
		}
	}
	return TRUE;
}

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
	)
{
	//get debug rights
	HANDLE token;
	int privSuc = GetDebugAdjustToken(&token);
	if(privSuc == FALSE) {
		Error::PrintLastError("Failed to get Debug-Rights:");
		return 0;
	}
	privSuc &= SetDebugPrevilege(token,TRUE);
	if(privSuc == -1) {
		Error::PrintLastError("Failed to get Debug-Rights:");
		return 0;
	}
	else if(privSuc == 0) {
		/*MessageBox(NULL,"Could not get all rights. Injection might fail\r\n"
			"(just dont be surprised if it fails.)","Warning",MB_ICONWARNING);*/
	}

	if(lpCmdLine[0] != '\0') {
		//we have a command line
		int length = strlen(lpCmdLine);
		if(length < 7 || strncmp(lpCmdLine, "start", 5) != 0) {
			MessageBox(NULL,"Command line parameter unknown. Valid parameter: start <path>","Error",MB_ICONERROR);
			return 0;
		}
		char* path = lpCmdLine + 6;
		if(GetFileAttributes(path) == INVALID_FILE_ATTRIBUTES && GetLastError()==ERROR_FILE_NOT_FOUND) {
			char errorMsg[512] = {"File does not exist: "};
			strcat_s(errorMsg,512,path);
			MessageBox(NULL,errorMsg,"Error",MB_ICONERROR);
			return 0;
		}
		char* file;
		char fullPath[MAX_PATH];
		GetFullPathName(path,MAX_PATH,fullPath,&file);
		if (file > fullPath) *(file-1) = '\0';
		STARTUPINFO sInfo = {0};
		sInfo.cb = sizeof(sInfo);
		PROCESS_INFORMATION pInfo = {0};
		BOOL result = CreateProcess(path,file,NULL,NULL,FALSE,NORMAL_PRIORITY_CLASS,NULL,fullPath,&sInfo,&pInfo);
		if(result == FALSE) {
			Error::SetLastError(GetLastError(),"CreateProcess");
			Error::PrintLastError("Failed to Create Process:");
		}
		CloseHandle(pInfo.hProcess);
		CloseHandle(pInfo.hThread);
		Sleep(500); //half a second should be enough
	}

	const char name[] = "AA2Edit.exe";
	const char dllName[] = "AAFace\\AAFaceDll.dll";
	DWORD* procBuffer = new DWORD[2048];
	DWORD nProcs;
	DWORD procId = (DWORD)-1;

	//find process
	while(true) {
		EnumProcesses(procBuffer,2048*sizeof(DWORD),&nProcs);
		char buffer[MAX_PATH];
		for (int i = 0; i < nProcs/sizeof(DWORD); i++) {
			HANDLE proc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
				FALSE,procBuffer[i]);
			if (proc != NULL) {
				
				if(GetModuleFileNameEx(proc,0,buffer,MAX_PATH)) {
					char* buffit = buffer;
					while (*buffit) buffit++;
					while (buffit != buffer && *buffit != '\\') buffit--;
					buffit++;
					if (strcmp(buffit,name) == 0) {
						procId = procBuffer[i];
						CloseHandle(proc);
						break;
					}
				} else {
					int i = GetLastError();
					i++;
				}
			
			}
			CloseHandle(proc);
		}
		if (procId == (DWORD)-1) {
			int res = MessageBox(NULL,"Could not find instance of AA2Edit.exe. Retry?","Info",MB_YESNO);
			if(res == IDNO) {
				return 0;
			}
		} else {
			break;
		}
	}

	delete[] procBuffer;

	HANDLE hProc = OpenProcess(PROCESS_CREATE_THREAD | 
				PROCESS_QUERY_INFORMATION|
				PROCESS_VM_READ|PROCESS_VM_WRITE|
				PROCESS_VM_OPERATION,
				FALSE,procId);
	if(hProc == NULL) {
		Error::SetLastError(GetLastError(),"OpenProcess");
		Error::PrintLastError("Failed to Open AA2Edit:");
		return 0;
	}
	//dont need debug prevs anymore
	SetDebugPrevilege(token,FALSE);

	//prepare inject call
	HMODULE(*loadLibraryPtr)(LPCSTR) = (HMODULE(*)(LPCSTR))GetProcAddress(GetModuleHandle("kernel32.dll"),"LoadLibraryA");
	if(loadLibraryPtr == NULL) {
		Error::SetLastError(GetLastError(),"GetProcAddress");
		Error::PrintLastError("Could not get LoadLibraryA Adress:",
			"Make sure youre not in hell because this should never happen.");
		return 0;
	}
	void(*getLastErrorPtr)() = (void(*)())GetProcAddress(GetModuleHandle("kernel32.dll"),"GetLastError");
	if (getLastErrorPtr == NULL) {
		Error::SetLastError(GetLastError(),"GetProcAddress");
		Error::PrintLastError("Could not get GetLastError Adress:",
			"Make sure youre not in hell because this should never happen.");
		return 0;
	}
	//prepare buffer
	char* parambuffer = new char[2048];
	DWORD size = WriteAsmCodeToBuffer(parambuffer); //first, injection function
	LoadfuncParam* paramstruct = (LoadfuncParam*)(parambuffer + size);
	paramstruct->LoadLibraryPtr = loadLibraryPtr; //then the loadlibrary ptr 
	paramstruct->GetLastErrorPtr = getLastErrorPtr; //then the getlasterror ptr
	strcpy_s(paramstruct->dllname,2048-size-8,dllName); //then the name of the dll to open


	void* strMem = (void*)VirtualAllocEx(hProc,NULL,2048,MEM_RESERVE | MEM_COMMIT,PAGE_EXECUTE_READWRITE);
	if(strMem == NULL) {
		Error::SetLastError(GetLastError(),"VirtualAllocEx");
		Error::PrintLastError("Failed to inject dll:");
		return 0;
	}
	
	BOOL suc = WriteProcessMemory(hProc,strMem,parambuffer,2048,NULL);
	if(suc == FALSE) {
		Error::SetLastError(GetLastError(),"WriteProcessMemory");
		Error::PrintLastError("Failed to inject dll : ");
		return 0;
	}
	
	HANDLE thread = CreateRemoteThread(hProc,NULL,NULL,(LPTHREAD_START_ROUTINE)strMem,(LPVOID)((DWORD)strMem + size),0,0);
	if(thread == NULL) {
		Error::SetLastError(GetLastError(),"OpenProcess");
		Error::PrintLastError("Could not create remote thread:");
		return 0;
	}
	WaitForSingleObject(thread,10000);
	
	DWORD exitcode = 0;
	suc = GetExitCodeThread(thread,&exitcode);
	if(suc == FALSE) {
		Error::SetLastError(GetLastError(),"GetExitCodeThread");
		Error::PrintLastError("GetExitCodeThread failed");
	}
	else if(exitcode == 0) {
		//loadlibrary failed, read error from memory
		DWORD read;
		ReadProcessMemory(hProc,strMem,parambuffer,2048,&read);
		if(read < 4) {
			Error::SetLastError(GetLastError(),"ReadProcessMemory");
			Error::PrintLastError("Failed to inject Dll, but could not retrieve error message:", "sorry :(");
		}
		else {
			int errorcode = (int)(paramstruct->LoadLibraryPtr);
			Error::SetLastError(errorcode,"LoadLibrary");
			Error::PrintLastError("Failed to inject Dll: Could not load library in target process:");
		}
	}
	VirtualFreeEx(hProc,strMem,0,MEM_FREE);
	delete[] parambuffer;
	CloseHandle(hProc);
	CloseHandle(thread);
	return 0;
}