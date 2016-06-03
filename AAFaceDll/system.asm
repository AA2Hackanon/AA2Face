.386
.model flat, c

EXTERN g_AA2Base:DWORD
EXTERN SystemDialogCharacterCreated:PROC
EXTERN SystemDialogCharacterLoaded:PROC
EXTERN BeforeCharacterOverwrite:PROC
EXTERN AfterCharacterOverwrite:PROC
EXTERN SystemDialogAfterDialogInit:PROC
EXTERN SystemDialogNotification:PROC
EXTERN TopBarDialogNotification:PROC

EXTERN GetClothingState:PROC
EXTERN GetOutfitToDisplay:PROC
EXTERN GetOverwriteOutfitToDisplay:PROC
EXTERN GetOutfitShirtState:PROC
EXTERN GetOutfitSkirtState:PROC
EXTERN AfterOutfitCallback:PROC

EXTERN GetPlayPosePosition:PROC

EXTERNDEF systemdialog_hooked_dialog_proc_afterinit:PROC
EXTERNDEF systemdialog_hooked_dialog_proc:PROC
EXTERNDEF systemdialog_character_created:PROC
EXTERNDEF systemdialog_character_opened:PROC
EXTERNDEF systemdialog_before_character_saved:PROC
EXTERNDEF systemdialog_after_character_saved:PROC

EXTERNDEF systemdialog_bar_hooked_dialog_proc:PROC
EXTERNDEF systemdialog_load_character_3dobject:PROC
EXTERNDEF systemdialog_load_change_outfit:PROC
EXTERNDEF systemdialog_load_overwrite_outfit:PROC
EXTERNDEF systemdialog_load_shirt_state:PROC
EXTERNDEF systemdialog_load_skirt_state:PROC
EXTERNDEF systemdialog_load_restore_skirt_state:PROC

EXTERNDEF systemdialog_pose_cancel_hook:PROC

.data

outfit_override_value	db 0
outfit_override_pointer	dd 0
outfit_clothstate_value dd 0

.code

;init dialog handler function (we want our callback after it), its an esi thiscall, no parameters
;AA2Edit.exe+1BAE9 - 56                    - push esi
;AA2Edit.exe+1BAEA - 8B F1                 - mov esi,ecx
;AA2Edit.exe+1BAEC - E8 FFFCFFFF           - call AA2Edit.exe+1B7F0
;AA2Edit.exe+1BAF1 - 5E                    - pop esi
systemdialog_hooked_dialog_proc_afterinit:
	push ecx				;this-pointer, save it for later
	mov eax, [g_AA2Base]
	add eax, 1B7F0h
	call eax

	pop ecx					; get this pointer back
	mov eax, [esp+0Ch]		; first parameter = HWND
	push eax
	push ecx
	call SystemDialogAfterDialogInit
	add esp, 8

	ret

;AA2Edit.exe+1BAD0 - 8B 44 24 08           - mov eax,[esp+08]
;AA2Edit.exe+1BAD4 - 3D 11010000           - cmp eax,00000111
systemdialog_hooked_dialog_proc:
	; save ecx (since its this)
	push ecx
	; copy parameters [esp+C] [esp+10] [esp+14] [esp+18]
	push [esp+18h]
	push [esp+18h]
	push [esp+18h]
	push [esp+18h]
	push ecx
	call SystemDialogNotification
	add esp, 14h ; cdecl
	pop ecx ; restore this
	;remember that we had instructions to do
	mov eax, [esp+0Ch]
	cmp eax, 111h
	ret

;AA2Edit.exe+332E9 - 51                    - push ecx
;AA2Edit.exe+332EA - 8B CB                 - mov ecx,ebx
;AA2Edit.exe+332EC - FF D2                 - call edx
systemdialog_load_character_3dobject:
	;replicate parameters (3 are allready pushed)
	push [esp+0Ch]
	push [esp+0Ch]
	push [esp+0Ch]
	push ecx ; original last parameter

	push eax
	push edx ; save general purpose registers

	mov eax, [esp+8]
	mov [outfit_clothstate_value], eax ; save for later restore after skirt
	push ecx
	call GetClothingState
	add esp, 4
	mov [esp+8], eax ; put return value in here
	

	pop edx
	pop eax			;restore general purpose registers

	mov ecx, ebx
	call edx

	;restore override outfit
	push eax
	push edx
	mov eax, [outfit_override_pointer]
	mov dl, byte ptr [outfit_override_value]
	mov byte ptr [eax], dl
	pop edx
	pop eax

	push eax ; in case this is a return value
	call AfterOutfitCallback
	pop eax

	ret 0Ch ;cause 3 parameters allready pushed

;our job: call GetOutfitToDisplay(int) to determine which of the 3 outfits to display; put value in al
;AA2Edit.exe+10059F - 8A 43 44              - mov al,[ebx+44]
;AA2Edit.exe+1005A2 - 8B 73 28              - mov esi,[ebx+28]
systemdialog_load_change_outfit:
	mov al, [ebx+44h]
	push eax
	push ecx
	push edx

	movzx eax, al
	push eax
	call GetOutfitToDisplay
	add esp, 4
	mov [ebx+44h], al

	pop edx
	pop ecx
	mov byte ptr [esp], al ;so we keep our new low byte
	pop eax

	mov esi, [ebx+28h]
	ret

;call GetOverwriteOutfitToDisplay(int), move return value into [eax]
;AA2Edit.exe+1005D3 - 8A 10                 - mov dl,[eax]
;AA2Edit.exe+1005D5 - 89 44 24 38           - mov [esp+38],eax
systemdialog_load_overwrite_outfit:
	mov dl, [eax]
	
	mov byte ptr [outfit_override_value], dl	; save to restore later
	mov [outfit_override_pointer], eax

	push edx
	push ecx
	push eax
	
	movzx eax, dl
	push eax
	call GetOverwriteOutfitToDisplay
	add esp, 4
	mov dl, al

	pop eax
	mov byte ptr [eax], dl
	
	pop ecx
	pop edx
	
	mov [esp+3Ch], eax
	ret

;GetOutfitShirtState(int), put value into ebp+8
;AA2Edit.exe+1006DA - 89 44 24 30           - mov[esp+30],eax
;AA2Edit.exe+1006DE - 8B FF                 - mov edi,edi
systemdialog_load_shirt_state:
	mov [esp+34h], eax
	pushad
	mov eax, [ebp+8]
	push eax
	call GetOutfitShirtState
	add esp, 4
	mov [ebp+8], eax
	popad

	ret

;GetOutfitSkirtState(int), put value into ebp+8
;AA2Edit.exe+10190C - E8 5F9D0100           - call AA2Edit.exe+11B670			: 1 parameter, no thiscall
systemdialog_load_skirt_state:
	push [esp+4]
	mov eax, [g_AA2Base]
	add eax, 11B670h
	call eax			; replicate call
	pushad
	mov eax, [ebp+8]
	push eax
	call GetOutfitSkirtState
	add esp, 4
	mov [ebp+8], eax
	popad 

	ret 4				; remember to remove our parameter

;call GetOutfitRestoreAfterSkirt(); put return value in ebp+8
;AA2Edit.exe+101C29 - 8B 43 30              - mov eax,[ebx+30]
;AA2Edit.exe+101C2C - 8B 40 28              - mov eax,[eax+28]
systemdialog_load_restore_skirt_state:
	pushad
	mov eax, [outfit_clothstate_value]
	mov [ebp+8], eax
	popad

	mov eax, [ebx+30h]
	mov eax, [eax+28h]
	ret

;we replacec first line only. in special cases, we want to jump to mov edi, ebx directly, or skip it entirely to cmp byte ptr [espetc]
;AA2Edit.exe+33BEE - 80 7C 24 16 00        - cmp byte ptr [esp+16],00 { 0 }
;AA2Edit.exe+33BF3 - 74 28                 - je AA2Edit.exe+33C1D
;...
;AA2Edit.exe+33C08 - 8B FB                 - mov edi,ebx
;AA2Edit.exe+33C0A - E8 A1230000           - call AA2Edit.exe+35FB0
;...
;AA2Edit.exe+33C1D - 80 7C 24 18 00        - cmp byte ptr [esp+18],00 { 0 }
systemdialog_pose_cancel_hook:
	;note that we do not have to save eax here (not edx either, actually)
	push ecx
	push edx
	call GetPlayPosePosition
	pop edx
	pop ecx
	cmp eax, -1
	je systemdialog_pose_cancel_normal
	cmp eax, -2
	je systemdialog_pose_cancel_skip
	; eax is our new slot then, jump to place
	mov ecx, [g_AA2Base]
	add ecx, 33C08h
	mov [esp], ecx ; we call, so jump means return there
	ret
  systemdialog_pose_cancel_skip:
	mov eax, [g_AA2Base]
	add eax, 33C1Dh
	mov [esp], eax
	ret
  systemdialog_pose_cancel_normal:
	cmp byte ptr [esp+1Ah], 00 
	ret

;AA2Edit.exe+34160 - 8B 44 24 08           - mov eax,[esp+08]
;AA2Edit.exe+34164 - 56                    - push esi <--we overwrote till here
;AA2Edit.exe+34165 - 8B F1                 - mov esi,ecx
;AA2Edit.exe+34167 - 3D 10010000           - cmp eax,00000110
systemdialog_bar_hooked_dialog_proc:
	; save ecx (since its this)
	push ecx
	; copy parameters [esp+C] [esp+10] [esp+14] [esp+18]
	push [esp+18h]
	push [esp+18h]
	push [esp+18h]
	push [esp+18h]
	push ecx
	call TopBarDialogNotification
	add esp, 14h ; cdecl
	pop ecx ; restore this
	
	mov eax, [esp+0Ch] ;this is C now
	;gotta push esi UNDER our stack somehow
	sub esp, 4
	mov edx, [esp+4]
	mov [esp], edx ;copy return valie
	mov [esp+4], esi ; push esi below it

	ret

;exit (last jump) of create character button
;AA2Edit.exe+1C4C1 - E9 97FAFFFF           - jmp AA2Edit.exe+1BF5D
systemdialog_character_created:
	push eax
	push ecx
	push edx ; save the registers that might be altered by this call
	call SystemDialogCharacterCreated
	mov eax, [g_AA2Base] ; replace the return address with
	add eax, 1BF5Dh		; the target of the jump
	mov [esp+0Ch], eax
	pop edx
	pop ecx
	pop eax

	ret

;last call of (successful) open file dialog click:
;AA2Edit.exe+1C26A - 8B CE                 - mov ecx,esi
;AA2Edit.exe+1C26C - E8 9F73FEFF           - call AA2Edit.exe+3610 0-param thiscall
systemdialog_character_opened:
	mov eax, [g_AA2Base]
	add eax, 3610h
	call eax
	pushad ;dayum im lazy
	call SystemDialogCharacterLoaded
	popad
	ret

;AA2Edit.exe+1261D5 - FF 15 B041C600        - call dword ptr [AA2Edit.exe+2C41B0] { ->kernel32.CreateFileW }
systemdialog_before_character_saved:
	;we are a createFileW call here
	;so, first parameter [esp+4] is our path
	push [esp+4]
	call BeforeCharacterOverwrite
	add esp, 4
	mov eax, [g_AA2Base]
	add eax, 2C41B0h
	jmp dword ptr [eax] ;redirect to the call, we're done here anyway

;AA2Edit.exe+12620A - FF 15 8441C600        - call dword ptr [AA2Edit.exe+2C4184] { ->kernel32.CloseHandle }
systemdialog_after_character_saved:
	;replicate call
	push [esp+4]
	mov eax, [g_AA2Base]
	add eax, 2C4184h
	call dword ptr [eax]

	call AfterCharacterOverwrite
	
	ret 4

END