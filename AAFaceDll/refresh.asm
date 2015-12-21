.386
.model flat, c

EXTERN g_AA2Base:DWORD
EXTERN GetSelectorIndex:PROC
EXTERN LoadFace:PROC
EXTERNDEF facedialog_refresh_face_inject:PROC
EXTERNDEF facedialog_load_face_inject:PROC

.data
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
	call GetSelectorIndex
	cmp eax, -1
	jne skipSelection ; if not -1, use this value
					  ; else, use original value from stack
	mov eax, [esp] 
  skipSelection:
	add esp, 4 ; clean eax from stack

	push [returnAddress] ; dont forget this one
	ret
END
