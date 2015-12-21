.386
.model flat, stdcall

EXTERN g_AA2Base:DWORD
EXTERN InitSelector:PROC
EXTERN DialogNotification:PROC
EXTERNDEF facedialog_hooked_dialog_proc:PROC
EXTERNDEF facedialog_constructor_inject:PROC

.data
	hInstTmp DWORD 0
	returnAddress DWORD 0
.code

; we will be modifying this one. thats annoying.
;AA2Edit.exe+22410 - 8B 44 24 08           - mov eax,[esp+08]
;AA2Edit.exe+22414 - 3D 11010000           - cmp eax,00000111
;AA2Edit.exe+22419 - 77 53                 - ja AA2Edit.exe+2246E
;AA2Edit.exe+2241B - 74 2E                 - je AA2Edit.exe+2244B

facedialog_hooked_dialog_proc:
	; save ecx (since its this)
	push ecx
	; copy parameters [esp+C] [esp+10] [esp+14] [esp+18]
	push [esp+18h]
	push [esp+18h]
	push [esp+18h]
	push [esp+18h]
	push ecx
	call DialogNotification
	add esp, 14h ; cdecl
	pop ecx ; restore this
	;remember that we had instructions to do (i think ret doesnt alter flags)
	mov eax, [esp+0Ch] ; thats C now
	cmp eax, 00000111h
	ret

;constructor of face dialog:
;AA2Edit.exe+2219F - FF 15 54436301        - call dword ptr [AA2Edit.exe+2C4354] (CreateDialogParamW)
;where ecx = param = hInst (pushed right before call)
facedialog_constructor_inject:
	pop [returnAddress] ; since this is actually supposed to call something else, our return address
					  ; is kinda lying around after the parameters; and it shouldnt be there

	; ecx is supposed to be hInst at this point, so we might wanna save it for later
	mov dword ptr [hInstTmp], ecx
	; call original function
	mov eax, [g_AA2Base]
	add eax, 2C4354h
	mov eax, [eax]
	call eax
	push eax ; rescue the hwnd that is returned by the createDialog call
	push dword ptr [hInstTmp]
	push eax 
	call InitSelector
	add esp, 8 ; its cdecl,as stdcall has namemangling
	pop eax ; pop the saved hwnd

	push [returnAddress] ; dont forget this one
	ret
END
