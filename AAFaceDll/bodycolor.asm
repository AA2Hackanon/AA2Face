.386
.model flat, c

EXTERN g_AA2Base:DWORD

EXTERN BodycolorDialogNotification:PROC
EXTERN BodycolorAfterDialogInit:PROC
EXTERN GetTanSelectorIndex:PROC
EXTERN InitBodycolorTab:PROC

EXTERNDEF bodycolor_hooked_dialog_proc:PROC
EXTERNDEF bodycolor_hooked_dialog_proc_afterinit:PROC
EXTERNDEF bodycolor_refresh_glasses_inject:PROC
EXTERNDEF bodycolor_init_hair_inject:PROC

.code

;1: Dialog Proc
;AA2Edit.exe+21350 - 55                    - push ebp
;AA2Edit.exe+21351 - 8B EC                 - mov ebp,esp
;AA2Edit.exe+21353 - 83 E4 F8              - and esp,-08
bodycolor_hooked_dialog_proc:
	; save ecx (since its this)
	push ecx
	; copy parameters [esp+C] [esp+10] [esp+14] [esp+18]
	push [esp+18h]
	push [esp+18h]
	push [esp+18h]
	push [esp+18h]
	push ecx
	call BodycolorDialogNotification
	add esp, 14h ; cdecl
	pop ecx ; restore this
	;remember that we had instructions to do
	;ok, these are gonna be really fucking annoying. lets... lets use eax, we have it
	pop eax ; return point, our stack is now flat
	push ebp
	mov ebp, esp
	and esp, -08 ;wew, stack alignment
	push eax ; return point
	ret

;2: after WM_INITDIALOG
;AA2Edit.exe+21374 - 57                    - push edi
;AA2Edit.exe+21375 - E8 76E9FFFF           - call AA2Edit.exe+1FCF0
bodycolor_hooked_dialog_proc_afterinit:
	push edi
	mov eax, [g_AA2Base]
	add eax, 1FCF0h
	call eax
	;since our outer function used ebp for stack reference and oddly anded esp, we have can use ebp to access its parameters
	push [ebp+8] ;ebp+8 is the first parameter, so the hwnd
	push edi
	call BodycolorAfterDialogInit
	add esp, 8
	ret 4

;3: function that selects the current tan
;AA2Edit.exe+20B51 - 8B 7B 54              - mov edi,[ebx+54]
;AA2Edit.exe+20B54 - E8 4769FFFF           - call AA2Edit.exe+174A0
;AA2Edit.exe+20B59 - 88 86 67040000        - mov[esi+00000467],al
bodycolor_refresh_glasses_inject:
	mov eax, [g_AA2Base]
	add eax, 174A0h
	call eax
	push eax ; save return value for now
	push eax
	push ebx ; ebx is the class
	call GetTanSelectorIndex
	add esp, 8
	cmp eax, -1
	jne bodycolor_refresh_glasses_inject_skipSelection ; if not -1, use this value
					  ; else, use original value from stack
	mov eax, [esp] 
  bodycolor_refresh_glasses_inject_skipSelection:
	add esp, 4 ; clean eax from stack
	ret


;AA2Edit.exe+1C185 - 8B 85 90000000        - mov eax,[ebp+00000090]
;AA2Edit.exe+1C18B - E8 304E0000           - call AA2Edit.exe+20FC0
;AA2Edit.exe+1C412 - 8B 85 90000000        - mov eax,[ebp+00000090]
;AA2Edit.exe+1C418 - E8 A34B0000           - call AA2Edit.exe+20FC0
bodycolor_init_hair_inject:
	push eax ; cause its this
	push 1 ; before = true
	push eax
	call InitBodycolorTab
	add esp, 8
	mov eax, [esp] ; thiscall, so restore it
	mov ecx, [g_AA2Base]
	add ecx, 20FC0h
	call ecx
	pop eax
	push 0
	push eax
	call InitBodycolorTab
	add esp, 8
	ret
END
