.386
.model flat, c

EXTERN g_AA2Base:DWORD

EXTERN FacedetailsDialogNotification:PROC
EXTERN FacedetailsAfterDialogInit:PROC
EXTERN FacedetailsAfterInit:PROC
EXTERN GetGlassesSelectorIndex:PROC
EXTERN GetLipColorSelectorIndex:PROC
EXTERN InitGlassesSelector:PROC
EXTERN InitFacedetailsTab:PROC
EXTERN RandomFaceDetailsSelect:PROC

EXTERNDEF facedetails_init_hair_inject:PROC
EXTERNDEF facedetails_hooked_dialog_proc:PROC
EXTERNDEF facedetails_hooked_dialog_proc_afterinit:PROC
EXTERNDEF facedetails_afterinit:PROC
EXTERNDEF facedetails_refresh_glasses_inject:PROC
EXTERNDEF facedetails_refresh_lipcolor_inject:PROC
EXTERNDEF facedetails_constructor_inject:PROC
EXTERNDEF facedetails_random_hook:PROC

.data
	hInstTmp DWORD 0
	returnAddress DWORD 0
.code

;AA2Edit.exe+2631A - FF 15 54435B01        - call dword ptr[AA2Edit.exe+2C4354]
facedetails_constructor_inject:
	pop [returnAddress]
	mov dword ptr [hInstTmp], ecx
	mov eax, [g_AA2Base]
	add eax, 2C4354h
	mov eax, [eax]
	call eax ; original createDialogParam call
	push eax
	push [hInstTmp]
	push eax
	call InitGlassesSelector
	add esp, 8
	pop eax
	push [returnAddress]
	ret

;start of virtual handler
;cool, classic stack frame. dont see that too often anymore
;AA2Edit.exe+273D0 - 55                    - push ebp
;AA2Edit.exe+273D1 - 8B EC                 - mov ebp,esp
;AA2Edit.exe+273D3 - 83 E4 F8              - and esp,-08
facedetails_hooked_dialog_proc:
	; save ecx (since its this)
	push ecx
	; copy parameters [esp+C] [esp+10] [esp+14] [esp+18]
	push [esp+18h]
	push [esp+18h]
	push [esp+18h]
	push [esp+18h]
	push ecx
	call FacedetailsDialogNotification
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

;end of WM_INITDIALOG handler
;AA2Edit.exe+273F9 - 57                    - push edi
;AA2Edit.exe+273FA - E8 F1EFFFFF           - call AA2Edit.exe+263F0
facedetails_hooked_dialog_proc_afterinit:
	push edi
	mov eax, [g_AA2Base]
	add eax, 263F0h
	call eax
	;since our outer function used ebp for stack reference and oddly anded esp, we can use ebp to access its parameters
	push [ebp+8] ;ebp+8 is the first parameter, so the hwnd
	push edi
	call FacedetailsAfterDialogInit
	add esp, 8
	ret 4

;some function that initialises facedetails handler, so after this one, every button exists. void, 1 param only, no thiscall
;AA2Edit.exe+32708 - 55                    - push ebp
;AA2Edit.exe+32709 - E8 82020000           - call AA2Edit.exe+32990
facedetails_afterinit:
	push [esp+4] ; copy parameter
	mov eax, [g_AA2Base]
	add eax, 32990h
	call eax
	push [esp+4] ; copy parameter again
	call FacedetailsAfterInit
	add esp, 4
	ret 4


;this call polls the selected glasses and writes it into al
;AA2Edit.exe+26D6D - 8B 7D 54              - mov edi,[ebp+54]
;AA2Edit.exe+26D70 - E8 2B07FFFF           - call AA2Edit.exe+174A0
;AA2Edit.exe+26D75 - 88 83 95060000        - mov[ebx+00000695],al
facedetails_refresh_glasses_inject:
	mov eax, [g_AA2Base]
	add eax, 174A0h
	call eax ; i like how this is always the same function for every single dialog
	push eax ; save return value for now
	push eax
	push ebp
	call GetGlassesSelectorIndex
	add esp, 8
	cmp eax, -1
	jne facedetails_refresh_glasses_inject_skipSelection ; if not -1, use this value
					  ; else, use original value from stack
	mov eax, [esp] 
  facedetails_refresh_glasses_inject_skipSelection:
	add esp, 4 ; clean eax from stack
	ret


;AA2Edit.exe+26D7B - 8B 7D 58              - mov edi,[ebp+58]
;AA2Edit.exe+26D7E - E8 1D07FFFF           - call AA2Edit.exe+174A0
;AA2Edit.exe+26D83 - 88 83 96060000        - mov[ebx+00000696],al
facedetails_refresh_lipcolor_inject:
	mov eax, [g_AA2Base]
	add eax, 174A0h
	call eax ; i like how this is always the same function for every single dialog
	push eax ; save return value for now
	push eax
	push ebp
	call GetLipColorSelectorIndex
	add esp, 8
	cmp eax, -1
	jne facedetails_refresh_lipcolor_inject_skipSelection ; if not -1, use this value
					  ; else, use original value from stack
	mov eax, [esp] 
  facedetails_refresh_lipcolor_inject_skipSelection:
	add esp, 4 ; clean eax from stack
	ret


;AA2Edit.exe+1C449 - 8B 85 B8000000        - mov eax,[ebp+000000B8]
;AA2Edit.exe+1C44F - E8 9CAC0000           - call AA2Edit.exe+270F0
;AA2Edit.exe+1C1BC - 8B 85 B8000000        - mov eax,[ebp+000000B8]
;AA2Edit.exe+1C1C2 - E8 29AF0000           - call AA2Edit.exe+270F0
facedetails_init_hair_inject:
	push eax ; cause its this
	push 1 ; before = true
	push eax
	call InitFacedetailsTab
	add esp, 8
	mov eax, [esp] ; thiscall, so restore it
	mov ecx, [g_AA2Base]
	add ecx, 270F0h
	call ecx
	pop eax
	push 0
	push eax
	call InitFacedetailsTab
	add esp, 8
	ret

;face details, eax thiscall
;AA2Edit.exe+1B927 - E8 C4B40000           - call AA2Edit.exe+26DF0
facedetails_random_hook:
	push eax ; for later

	mov edx, [g_AA2Base]
	add edx, 26DF0h
	call edx

	call RandomFaceDetailsSelect
	add esp, 4
	ret
END