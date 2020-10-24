.386
.model flat, c

EXTERN g_AA2Base:DWORD

EXTERN InitPersonalityTab:PROC
EXTERN PersonalityDialogNotification:PROC
EXTERN PersonalityAfterDialogInit:PROC
EXTERN GetPersonalitySelectorIndex:PROC
EXTERN RandomPersonalitySelect:PROC
EXTERN InitPersonalitySelector:PROC

EXTERNDEF personality_random_hook:PROC
EXTERNDEF personality_init_personality_inject:PROC
EXTERNDEF personality_refresh_personality_inject:PROC
EXTERNDEF personality_hooked_dialog_proc_afterinit:PROC
EXTERNDEF personality_hooked_dialog_proc:PROC
EXTERNDEF personality_constructor_inject:PROC

.data
	hInstTmp DWORD 0
	returnAddress DWORD 0
.code



;1: dialog proc hook, ecx thiscall
;AA2Edit.exe + 2DC40 - 8B 44 24 08 - mov eax, [esp + 08]
;AA2Edit.exe + 2DC44 - 83 E8 10 - sub eax, 10 { 16 }
personality_hooked_dialog_proc:
	; save ecx (since its this)
	push ecx
	; copy parameters [esp+C] [esp+10] [esp+14] [esp+18]
	push [esp+18h]
	push [esp+18h]
	push [esp+18h]
	push [esp+18h]
	push ecx
	call PersonalityDialogNotification
	add esp, 14h ; cdecl
	pop ecx ; restore this
	; remember that we had instructions to do
	mov eax, [esp + 0Ch] 
	sub eax, 10h
	ret

;2: after WM_INITDIALOG
;this is esi, no push for some reason?
;AA2Edit.exe+2DC66 - E8 35F6FFFF - call AA2Edit.exe + 2D2A0
;AA2Edit.exe+2DC6B - 33 C0 - xor eax, eax
personality_hooked_dialog_proc_afterinit:
	mov eax, [g_AA2Base]
	add eax, 2D2A0h
	call eax
	;since our outer function used ebp for stack reference and oddly anded esp, we can use ebp to access its parameters
	push [esp+0Ch] ; hwnd is at +8 at this point without our hooked call
	push esi
	call PersonalityAfterDialogInit
	add esp, 8
	ret 

;3: function that selects current personality
;AA2Edit.exe+2D450 - 8B 7B 4C              - mov edi,[ebx+4C]
;AA2Edit.exe+2D453 - E8 48A0FEFF           - call AA2Edit.exe+174A0
;AA2Edit.exe+2D458 - 8B 93 9C010000        - mov edx,[ebx+0000019C]
;AA2Edit.exe+2D45E - 8A 04 82              - mov al,[edx+eax*4]
;AA2Edit.exe+2D461 - 5F                    - pop edi
;AA2Edit.exe+2D462 - 88 86 45040000        - mov [esi+00000445],al
personality_refresh_personality_inject:
	mov eax, [g_AA2Base]
	add eax, 174A0h
	call eax
	push eax ; save return value for now
	push eax
	push ebx
	call GetPersonalitySelectorIndex
	add esp, 8
	cmp eax, -1
	jne personality_refresh_personality_inject_skipSelection ; if not -1, use this value
					  ; else, use original value from stack
	mov eax, [esp] 
  personality_refresh_personality_inject_skipSelection:
	add esp, 4 ; clean eax from stack
	ret

;init injects?
;AA2Edit.exe+1C475 - 8B B5 D8000000        - mov esi,[ebp+000000D8]
;AA2Edit.exe+1C47B - E8 00110100           - call AA2Edit.exe+2D580
;AA2Edit.exe+1C1E8 - 8B B5 D8000000        - mov esi,[ebp+000000D8]
personality_init_personality_inject:
	push 1 ; before = true
	push esi
	call InitPersonalityTab
	add esp, 8
	mov ecx, [g_AA2Base]
	add ecx, 2D580h
	call ecx
	push 0
	push esi
	call InitPersonalityTab
	add esp, 8
	ret


;random hooks?
;AA2Edit.exe+3475E - 8B BB D8000000        - mov edi,[ebx+000000D8]
;AA2Edit.exe+34764 - E8 078DFFFF           - call AA2Edit.exe+2D470
;AA2Edit.exe+1B94D - 8B BD D8000000        - mov edi,[ebp+000000D8]
;AA2Edit.exe+1B953 - E8 181B0100           - call AA2Edit.exe+2D470
personality_random_hook:
	mov edx, [g_AA2Base]
	add edx, 2D470h
	call edx

	push edi
	call RandomPersonalitySelect
	add esp, 4
	ret


;esi holds dialog class ptr
;AA2Edit.exe+2D275 - FF 15 54432801        - call dword ptr [AA2Edit.exe+2C4354] { ->USER32.CreateDialogParamW }
personality_constructor_inject:
	pop [returnAddress]
	mov dword ptr [hInstTmp], ecx
	mov eax, [g_AA2Base]
	add eax, 2C4354h
	mov eax, [eax]
	call eax ; original createDialogParam call
	push eax
	push [hInstTmp]
	push eax
	push esi
	call InitPersonalitySelector
	add esp, 0Ch
	pop eax
	push [returnAddress]
	ret

END