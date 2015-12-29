.386
.model flat, c

EXTERN g_AA2Base:DWORD

EXTERN GetMessageHook:PROC
EXTERN PreGetMessageHook:PROC

EXTERNDEF getmessage_hook:PROC
EXTERNDEF pregetmessage_hook:PROC

.code

;AA2Edit.exe+425B - E8 10CC1800           - call AA2Edit.exe+190E70			void, no thiscall, nothing to care about
;AA2Edit.exe+4260 - 8B 3D 10444601        - mov edi,[AA2Edit.exe+2C4410]
;AA2Edit.exe+4266 - 8B 35 00444601        - mov esi,[AA2Edit.exe+2C4400]
;AA2Edit.exe+426C - 8D 64 24 00           - lea esp,[esp+00]
pregetmessage_hook:
	mov eax, [g_AA2Base]
	add eax, 190E70h
	call eax
	call PreGetMessageHook
	ret

;AA2Edit.exe+427F - 53                    - push ebx
;AA2Edit.exe+4280 - 53                    - push ebx
;AA2Edit.exe+4281 - 53                    - push ebx
;AA2Edit.exe+4282 - 8D 44 24 40           - lea eax,[esp+40]
;AA2Edit.exe+4286 - 50                    - push eax
;AA2Edit.exe+4287 - FF 15 0C444E01        - call dword ptr[AA2Edit.exe+2C440C] <-- GetMessageW
;AA2Edit.exe+428D - 85 C0                 - test eax,eax
getmessage_hook:
	push [esp+10h] ; copy parameters
	push [esp+10h]
	push [esp+10h]
	push [esp+10h]
	mov eax, [g_AA2Base]
	add eax, 2C440Ch
	call dword ptr [eax] ; call GetMessageW
	test eax,eax
	jz getmessage_hook_ret0
	push eax ; rescue return value
	push [esp+8] ; msg pointer (still from the original parameters)
	call GetMessageHook
	add esp, 4
	test eax,eax
	jz getmessage_hook_skipall
	pop eax ; return value
	ret 10h ; still have to get rid of those parameters
  getmessage_hook_skipall:
    add esp, 4 ; we still had eax saved
	mov eax, [g_AA2Base]
	add eax, 4270h ; calculate start of the message loop
	mov [esp], eax ; we are going to return there instead
	ret 10h ; parameters are still there tho
  getmessage_hook_ret0:
	xor eax,eax
	ret 10h

END