#include <Windows.h>
#include <Psapi.h>

int SetDebugPrevilege(HANDLE token,BOOL state) {
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if (!LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&luid)) {
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
		return -1;
	}

	return 1;
}

BOOL GetDebugAdjustToken(HANDLE* token) {
	if (!OpenThreadToken(GetCurrentThread(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,FALSE,token)) {
		if (!ImpersonateSelf(SecurityImpersonation)) {
			return FALSE;
		}

		if (!OpenThreadToken(GetCurrentThread(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,FALSE,token)) {
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
	HANDLE token;
	int privSuc = GetDebugAdjustToken(&token);
	if(privSuc == FALSE) {
		MessageBox(NULL,"Could not get rights","Error",MB_ICONERROR);
		return 0;
	}
	privSuc &= SetDebugPrevilege(token,TRUE);
	if(privSuc == -1) {
		MessageBox(NULL,"Could not get rights","Error",MB_ICONERROR);
		return 0;
	}
	else if(privSuc == 0) {
		MessageBox(NULL,"Could not get all rights. Injection might fail\r\n"
			"(just dont be surprised if it fails.)","Warning",MB_ICONWARNING);
	}

	const char name[] = "AA2Edit.exe";
	const char dllName[] = "AAFaceDll.dll";
	DWORD* procBuffer = new DWORD[2048];
	DWORD nProcs;
	DWORD procId = (DWORD)-1;

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
		MessageBox(NULL,"Could not open AA2Edit process","Error",MB_ICONERROR);
		return 0;
	}
	//dont need debug prevs anymore
	SetDebugPrevilege(token,FALSE);

	void* loadLibraryPtr = (void*)GetProcAddress(GetModuleHandle("kernel32.dll"),"LoadLibraryA");
	if(loadLibraryPtr == NULL) {
		MessageBox(NULL,"Well, that one sucks.","Error",MB_ICONERROR);
		return 0;
	}
	void* strMem = (void*)VirtualAllocEx(hProc,NULL,sizeof(dllName),MEM_RESERVE | MEM_COMMIT,PAGE_READWRITE);
	BOOL suc = WriteProcessMemory(hProc,strMem,dllName,sizeof(dllName),NULL);
	if(suc == FALSE) {
		MessageBox(NULL,"Could not write into process","Error",MB_ICONERROR);
		return 0;
	}
	HANDLE thread = CreateRemoteThread(hProc,NULL,NULL,(LPTHREAD_START_ROUTINE)loadLibraryPtr,strMem,0,0);
	if(thread == NULL) {
		MessageBox(NULL,"Could not create remote thread","Error",MB_ICONERROR);
		return 0;
	}
	WaitForSingleObject(thread,10000);
	VirtualFreeEx(hProc,strMem,0,MEM_FREE);
	DWORD exitcode = 0;
	suc = GetExitCodeThread(thread,&exitcode);
	if(suc == FALSE) {
		MessageBox(NULL,"GetExitCodeThread failed","Error",MB_ICONERROR);
	}
	else if(exitcode == 0) {
		MessageBox(NULL,"LoadLibrary returned 0!","Error",MB_ICONERROR);
	}
	CloseHandle(hProc);
	CloseHandle(thread);
	return 0;
}