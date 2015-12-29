.386
.model flat, c

EXTERN g_AA2Base:DWORD
EXTERN SystemDialogCharacterCreated:PROC
EXTERN SystemDialogCharacterLoaded:PROC

EXTERNDEF systemdialog_character_created:PROC
EXTERNDEF systemdialog_character_opened:PROC
.code

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


END