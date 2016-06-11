.386
.model flat, c

EXTERN g_AA2Base:DWORD

EXTERN BodycolorDialogNotification:PROC
EXTERN BodycolorAfterDialogInit:PROC
EXTERN GetTanSelectorIndex:PROC
EXTERN GetNipTypeSelectorIndex:PROC
EXTERN GetNipColorSelectorIndex:PROC
EXTERN GetMosaicSelectorIndex:PROC
EXTERN GetPubHairSelectorIndex:PROC
EXTERN InitBodycolorTab:PROC
EXTERN RandomBodyColorSelect:PROC

EXTERNDEF bodycolor_hooked_dialog_proc:PROC
EXTERNDEF bodycolor_hooked_dialog_proc_afterinit:PROC
EXTERNDEF bodycolor_refresh_tan_inject:PROC
EXTERNDEF bodycolor_refresh_nipcolor_inject:PROC
EXTERNDEF bodycolor_refresh_niptype_inject:PROC
EXTERNDEF bodycolor_init_hair_inject:PROC
EXTERNDEF bodycolor_refresh_pubhair_inject:PROC
EXTERNDEF bodycolor_refresh_mosaic_inject:PROC
EXTERNDEF bodycolor_random_hook:PROC

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
bodycolor_refresh_tan_inject:
	mov eax, [g_AA2Base]
	add eax, 174A0h
	call eax
	push eax ; save return value for now
	push eax
	push ebx ; ebx is the class
	call GetTanSelectorIndex
	add esp, 8
	cmp eax, -1
	jne bodycolor_refresh_tan_inject_skipSelection ; if not -1, use this value
					  ; else, use original value from stack
	mov eax, [esp] 
  bodycolor_refresh_tan_inject_skipSelection:
	add esp, 4 ; clean eax from stack
	ret

;AA2Edit.exe+20B12 - 8B 7B 48              - mov edi,[ebx+48]
;AA2Edit.exe+20B15 - E8 8669FFFF           - call AA2Edit.exe+174A0
;AA2Edit.exe+20B1A - 88 86 64040000        - mov [esi+00000464],al
bodycolor_refresh_niptype_inject:
	mov eax, [g_AA2Base]
	add eax, 174A0h
	call eax
	push eax ; save return value for now
	push eax
	push ebx ; ebx is the class
	call GetNipTypeSelectorIndex
	add esp, 8
	cmp eax, -1
	jne bodycolor_refresh_niptype_inject_skipSelection ; if not -1, use this value
					  ; else, use original value from stack
	mov eax, [esp] 
  bodycolor_refresh_niptype_inject_skipSelection:
	add esp, 4 ; clean eax from stack
	ret

;AA2Edit.exe+20B20 - 8B 7B 4C              - mov edi,[ebx+4C]
;AA2Edit.exe+20B23 - E8 7869FFFF           - call AA2Edit.exe+174A0
;AA2Edit.exe+20B28 - 88 86 65040000        - mov [esi+00000465],al
bodycolor_refresh_nipcolor_inject:
	mov eax, [g_AA2Base]
	add eax, 174A0h
	call eax
	push eax ; save return value for now
	push eax
	push ebx ; ebx is the class
	call GetNipColorSelectorIndex
	add esp, 8
	cmp eax, -1
	jne bodycolor_refresh_nipcolor_inject_skipSelection ; if not -1, use this value
					  ; else, use original value from stack
	mov eax, [esp] 
  bodycolor_refresh_nipcolor_inject_skipSelection:
	add esp, 4 ; clean eax from stack
	ret

;mosaic and pub hair poll (in this order). mosaic flag is 11, pub hair flag is B
;AA2Edit.exe+20B82 - 8B 7B 5C              - mov edi,[ebx+5C]
;AA2Edit.exe+20B85 - E8 1669FFFF           - call AA2Edit.exe+174A0
;AA2Edit.exe+20B8A - 88 86 69040000        - mov [esi+00000469],al
bodycolor_refresh_mosaic_inject:
	mov eax, [g_AA2Base]
	add eax, 174A0h
	call eax
	push eax ; save return value for now
	push eax
	push ebx ; ebx is the class
	call GetMosaicSelectorIndex
	add esp, 8
	cmp eax, -1
	jne bodycolor_refresh_mosaic_inject_skipSelection ; if not -1, use this value
					  ; else, use original value from stack
	mov eax, [esp] 
  bodycolor_refresh_mosaic_inject_skipSelection:
	add esp, 4 ; clean eax from stack
	ret

;AA2Edit.exe+20B90 - 8B 7B 60              - mov edi,[ebx+60]
;AA2Edit.exe+20B93 - E8 0869FFFF           - call AA2Edit.exe+174A0
;AA2Edit.exe+20B98 - 88 86 5C040000        - mov [esi+0000045C],al
bodycolor_refresh_pubhair_inject:
	mov eax, [g_AA2Base]
	add eax, 174A0h
	call eax
	push eax ; save return value for now
	push eax
	push ebx ; ebx is the class
	call GetPubHairSelectorIndex
	add esp, 8
	cmp eax, -1
	jne bodycolor_refresh_pubhair_inject_skipSelection ; if not -1, use this value
					  ; else, use original value from stack
	mov eax, [esp] 
  bodycolor_refresh_pubhair_inject_skipSelection:
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

;random for body color, eax thisccall
;AA2Edit.exe+1B8F0 - E8 FB520000           - call AA2Edit.exe+20BF0
bodycolor_random_hook:
	push eax ; push for later

	mov edx, [g_AA2Base]
	add edx, 20BF0h
	call edx

	call RandomBodyColorSelect
	add esp, 4
	
	ret
END
