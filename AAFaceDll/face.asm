.386
.model flat, c

EXTERN g_AA2Base:DWORD

EXTERN GetFaceSelectorIndex:PROC
EXTERN LoadFace:PROC
EXTERN InitFaceSelector:PROC
EXTERN FaceDialogNotification:PROC
EXTERN FaceDialogAfterInit:PROC

EXTERNDEF facedialog_refresh_face_inject:PROC
EXTERNDEF facedialog_load_face_inject:PROC
EXTERNDEF facedialog_hooked_dialog_proc:PROC
EXTERNDEF facedialog_constructor_inject:PROC
EXTERNDEF facedialog_hooked_dialog_proc_afterinit:PROC

.data
	hInstTmp DWORD 0
	returnAddress DWORD 0
.code


;mov al,[edx+0000046A] would load the face.
;AA2Edit.exe+223D1 - 8A 82 6A040000        - mov al,[edx+0000046A]
;AA2Edit.exe+223D7 - 3C 0B                 - cmp al,0B
;AA2Edit.exe+223D9 - 77 0B                 - ja AA2Edit.exe+223E6
;AA2Edit.exe+223DB - 0FB6 C8               - movzx ecx,al
facedialog_load_face_inject:
	push edx ; save this one for safety
	mov al, [edx+0000046Ah]
	movzx eax, al
	push eax
	call LoadFace
	add esp, 4
	pop edx

	mov al, [edx+0000046Ah]
	ret

;call that deterimnes which image has been clicked:
;AA2Edit.exe+222C7 - E8 D451FFFF           - call AA2Edit.exe+174A0
;return value is then put into variable that will be used to select the face
;AA2Edit.exe+222CC - 88 86 6A040000        - mov[esi+0000046A],al
facedialog_refresh_face_inject:
	pop [returnAddress] ; since this is actually supposed to call something else, our return address
					  ; is kinda lying around after the parameters; and it shouldnt be there

	mov eax, [g_AA2Base]
	add eax, 174A0h
	call eax
	push eax ; save return value for now
	call GetFaceSelectorIndex
	cmp eax, -1
	jne skipSelection ; if not -1, use this value
					  ; else, use original value from stack
	mov eax, [esp] 
  skipSelection:
	add esp, 4 ; clean eax from stack

	push [returnAddress] ; dont forget this one
	ret

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
	call FaceDialogNotification
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
	call InitFaceSelector
	add esp, 8 ; its cdecl,as stdcall has namemangling
	pop eax ; pop the saved hwnd

	push [returnAddress] ; dont forget this one
	ret


;AA2Edit.exe+22429 - 51                    - push ecx
;AA2Edit.exe+2242A - E8 81FDFFFF           - call AA2Edit.exe+221B0
facedialog_hooked_dialog_proc_afterinit:
	mov eax, [esp+0Ch]
	push eax ; hwnd parameter for later
	push ecx ; save this
	push ecx ; parameter
	mov eax, [g_AA2Base]
	add eax, 221B0h
	call eax
	; eax = hwnd still on stack
	;ecx = this still on stack
	call FaceDialogAfterInit
	add esp, 8

	ret 4
END