#pragma once
#include <Windows.h>
extern const char* g_strModuleName;
extern "C" DWORD g_AA2Base;
extern DWORD g_AA2RedrawFlagTable;
extern const HWND* g_AA2MainWndHandle;
extern const HWND* g_AA2DialogHandles;
extern const HWND* g_AA2UpperDialogHandle;
extern float* g_AA2CurrZoom;
extern float* g_AA2MaxTilt;
float* g_AA2GetTilt();
int g_AA2GetCurrentDialogTab();

class BaseDialogClass {
protected:
	void* GetChoiceDataBuffer();
	const void* GetChoiceDataBuffer() const;
	BYTE GetChoiceFlag(DWORD index) const;
	void SetChoiceFlag(DWORD index,BYTE slot);
	HWND GetButtonListWnd(int offset,int buttonNumber) const;
	int GetButtonListCount(int offset) const;
};

class FaceDialogClass : public BaseDialogClass {
public:
	//ebx+48
	HWND GetFaceSlotButton(BYTE slot) const;
	int GetButtonCount() const;
	void SetFaceSlot(BYTE slot);
	void SetChangeFlags();
};

/*
 * Emulates known behavior of the class used in the hair dialog init
 * functions etc. i should do one of those for face as well
 */
class HairDialogClass : public BaseDialogClass {
public:
	BYTE* HairOfTab(int tab);
	BYTE* FlipBoolOfTab(int tab);
	void SetHairChangeFlags(int tab);
	bool HairSlotExists(BYTE slot,int hairKind) const;
	bool HairSlotExists2(BYTE slot,int hairKind) const;
	const BYTE* GetHairSlotExistsField(int hairKind) const;
	HWND GetHairSlotButton(BYTE slot) const;
	HWND GetAdjustmentSliderWnd() const;
	HWND GetAdjustmentSliderEdit() const;
	HWND GetFlipButtonWnd() const;
	HWND GetHairSizeEditWnd() const;
	int GetButtonCount() const;
};
/*
 * the temp hair info struct as returned by the hair poll
 */
struct TempHairInfos {
	wchar_t targetPPMask[130];
	wchar_t targetXXMask[130];
	wchar_t targetUnknownMask[130];
	wchar_t targetTgaMask[130];
	wchar_t dunnoAlwaysEmpty[130];
	DWORD targetXAMaskStructPointer;
	DWORD hasFlip; //1 if has a flip, else 0
};
static_assert(sizeof(TempHairInfos) == 0x51C,"TempHairInfos struct should be 51C bytes long, as thats the constant they go through the array with");

class FacedetailsDialogClass : public BaseDialogClass {
public:
	BYTE GetEyelidSlot() const;
	BYTE GetUpperEyelidSlot() const;
	BYTE GetLowerEyelidSlot() const;
	BYTE GetGlassesSlot() const;
	void SetGlassesSlot(BYTE slot);
	BYTE GetLipColorSlot() const;
	void SetLipColorSlot(BYTE slot);
	BYTE GetLipOpacityValue() const;
	void SetLipOpacityValue(BYTE value);
	HWND GetGlassesButtonWnd(BYTE n) const;
	int GetGlassButtonCount() const;
	HWND GetLipColorButtonWnd(BYTE n) const;
	int GetLipColorButtonCount() const;
	BYTE GetLipOpacityGuiValue() const;
	void SetLipOpacityGuiValue(BYTE b);
	void SetChangeFlags();

	HWND GetLipOpacityEditWnd() const;
	HWND GetLipOpacityTrackWnd() const;
};

class BodycolorDialogClass : public BaseDialogClass {
public:
	HWND GetNipTypeButtonHwnd(BYTE n) const;
	int GetNipTypeButtonCount() const;
	HWND GetNipColorButtonHwnd(BYTE n) const;
	int GetNipColorButtonCount() const;
	HWND GetTanButtonHwnd(BYTE n) const;
	int GetTanButtonCount() const;
	HWND GetMosiacButtonHwnd(BYTE n) const;
	int GetMosaicButtonCount() const;
	HWND GetPubHairButtonHwnd(BYTE n) const;
	int GetPubHairButtonCount() const;
	HWND GetPubHairOpacityEditWnd() const;

	void SetChangeFlags();
	BYTE GetCurrentNipTypeSlot() const;
	void SetCurrentNipTypeSlot(BYTE slot);
	BYTE GetCurrentNipColorSlot() const;
	void SetCurrentNipColorSlot(BYTE slot);
	BYTE GetCurrentTanSlot() const;
	void SetCurrentTanSlot(BYTE slot);
	BYTE GetCurrentMosaicSlot() const;
	void SetCurrentMosaicSlot(BYTE slot);
	BYTE GetCurrentPubHairSlot() const;
	void SetCurrentPubHairSlot(BYTE slot);
	BYTE GetCurrentPubHairOpacityValue();
	void SetCurrentPubHairOpacityValue(BYTE value);
	BYTE GetCurrentPubHairOpacityGuiValue();
	void SetCurrentPubHairOpacityGuiValue(BYTE value);
};

float* GetMinZoom();
float* GetMaxZoom();

void ExternInit();

//functions we should be hooking:

//constructor of face dialog:
//AA2Edit.exe+22193 - 68 90AB5200           - push AA2Edit.exe+19AB90 //push dialog handler (not that its really used much)
//[...]
//AA2Edit.exe+2219F - FF 15 54436301        - call dword ptr [AA2Edit.exe+2C4354] (CreateDialogParamW)
//where ecx = param = hInst (pushed right before call)

//call that deterimnes which image has been clicked:
//AA2Edit.exe+222C7 - E8 D451FFFF           - call AA2Edit.exe+174A0
//return value is then put into variable that will be used to select the face
//AA2Edit.exe+222CC - 88 86 6A040000        - mov[esi+0000046A],al

//start of the dialog handler (general function, redirects to virtual call later i think)
//AA2Edit.exe+19AB90 - 53                    - push ebx
//AA2Edit.exe+19AB91 - 8B 5C 24 08           - mov ebx,[esp+08]
//AA2Edit.exe+19AB95 - 55                    - push ebp
//AA2Edit.exe+19AB96 - 56                    - push esi
//AA2Edit.exe+19AB97 - 57                    - push edi
//AA2Edit.exe+19AB98 - 68 A4004600           - push AA2Edit.exe+3100A4
//AA2Edit.exe+19AB9D - 53                    - push ebx
//AA2Edit.exe+19AB9E - FF 15 60434100        - call dword ptr[AA2Edit.exe+2C4360]
//note that HWND -> class is done by a call to GetPropW(HWND, L"__WND_BASE_CLASS__") (AA2Edit.exe+3100A4)

//starting of said virtual handler (the one for the face part)
//AA2Edit.exe+22410 - 8B 44 24 08           - mov eax,[esp+08]
//AA2Edit.exe+22414 - 3D 11010000           - cmp eax,00000111
//AA2Edit.exe+22419 - 77 53                 - ja AA2Edit.exe+2246E
//AA2Edit.exe+2241B - 74 2E                 - je AA2Edit.exe+2244B

//WM_INITDIALOG handler of face
//AA2Edit.exe+22429 - 51                    - push ecx
//AA2Edit.exe+2242A - E8 81FDFFFF           - call AA2Edit.exe+221B0

//in the system menu, this is a call to the WM_COMMAND handler (no thiscall!)
//void(?) handler(sysclass* c, HWND cwnd (for some reason), LPARAM lparam)
//controll identifier of the new character button seems to be 2731, while open file is 272F
//AA2Edit.exe+1BB19 - E8 22000000           - call AA2Edit.exe+1BB40

//part of a function which, at character load, limits the face to 0B if (BYTE)face > 0xB (in a really
//stupid inefficient way). we can use this function both to set our face in the selection field,
//and to let it actually load the other faces.
//this function should return 1 on success, and failure can only occurr with nullpointers, which cant be
//at this point anymore. we should probably just return 1.
//AA2Edit.exe+223D1 - 8A 82 6A040000        - mov al,[edx+0000046A]
//AA2Edit.exe+223D7 - 3C 0B                 - cmp al,0B
//AA2Edit.exe+223D9 - 77 0B                 - ja AA2Edit.exe+223E6
//AA2Edit.exe+223DB - 0FB6 C8               - movzx ecx,al

/////////////////////////////

//constructor of hair dialog:
//AA2Edit.exe+27EEC - FF 15 54435B00        - call dword ptr[AA2Edit.exe+2C4354]

//start of virtual handler of hair-dialog
//AA2Edit.exe+28650 - 8B 44 24 08           - mov eax,[esp+08]
//AA2Edit.exe+28654 - 56                    - push esi
//AA2Edit.exe+28655 - 57                    - push edi
//AA2Edit.exe+28656 - 8B F9                 - mov edi,ecx
//AA2Edit.exe+28658 - 3D 11010000           - cmp eax,00000111

//function that handles WM_INITDIALOG
//AA2Edit.exe+28674 - E8 97F8FFFF           - call AA2Edit.exe+27F10
//AA2Edit.exe+28679 - 5F                    - pop edi
//AA2Edit.exe+2867A - 33 C0                 - xor eax,eax
//AA2Edit.exe+2867C - 5E                    - pop esi
//AA2Edit.exe+2867D - C2 1000               - ret 0010


//WM_COMMAND part of the virtual handler of the hair-dialog. 
//AA2Edit.exe+286BC - 8B 44 24 14           - mov eax,[esp+14]
//AA2Edit.exe+286C0 - 50                    - push eax <-- lparam
//AA2Edit.exe+286C1 - 57                    - push edi <-- class ptr
//AA2Edit.exe+286C2 - E8 59000000           - call AA2Edit.exe+28720


//setting hair after button press or when tabs are switched. edx e [0:3] depending on tab
//AA2Edit.exe+280DA - E8 C1F3FEFF           - call AA2Edit.exe+174A0
//AA2Edit.exe+280DF - 0FB6 96 A0020000      - movzx edx,byte ptr[esi+000002A0]
//AA2Edit.exe+280E6 - 88 84 1A 9C060000     - mov[edx+ebx+0000069C],al

//AA2Edit.exe+1C1C7 - 8B BD C0000000        - mov edi,[ebp+000000C0] (open file)
//AA2Edit.exe+1C1CD - E8 8EC20000           - call AA2Edit.exe+28460
//,
//AA2Edit.exe+1C454 - 8B BD C0000000        - mov edi,[ebp+000000C0] (new character)
//AA2Edit.exe+1C45A - E8 01C00000           - call AA2Edit.exe+28460 (this=edi)
//this part is apparently limiting back hair on load (note how 69C became 69E because back hair has ebx=2).
//there are similar parts before and after this one for the other tabs
//note that edi is the this pointer, the same which is used in the virtual handler.
//if we change the border to 255 on creation, he creates one more icon only. that sucks.
//cause that would have been really nice.
//also, ebx is 0 in this context
//AA2Edit.exe+28460 - 8B 47 44              - mov eax,[edi+44]
//AA2Edit.exe+28463 - 8B 48 24              - mov ecx,[eax+24]
//AA2Edit.exe+28466 - 53                    - push ebx
//AA2Edit.exe+28467 - 33 DB                 - xor ebx,ebx
//AA2Edit.exe+28469 - 39 19                 - cmp[ecx],ebx
//AA2Edit.exe+2846B - 75 05                 - jne AA2Edit.exe+28472
//AA2Edit.exe+2846D - 8B 40 30              - mov eax,[eax+30]
//AA2Edit.exe+28470 - EB 03                 - jmp AA2Edit.exe+28475
//AA2Edit.exe+28472 - 8B 40 2C              - mov eax,[eax+2C]
//AA2Edit.exe+28475 - 3B C3                 - cmp eax,ebx
//AA2Edit.exe+28477 - 75 04                 - jne AA2Edit.exe+2847D
//AA2Edit.exe+28479 - 32 C0                 - xor al,al
//AA2Edit.exe+2847B - 5B                    - pop ebx
//AA2Edit.exe+2847C - C3                    - ret
//AA2Edit.exe+2847D - 8B 48 28              - mov ecx,[eax+28]
//AA2Edit.exe+28480 - 3B CB                 - cmp ecx,ebx
//AA2Edit.exe+28482 - 74 F5                 - je AA2Edit.exe+28479
//AA2Edit.exe+28484 - 0FB6 91 9C060000      - movzx edx,byte ptr[ecx+0000069C]
//[...]
//AA2Edit.exe+2853B - 0FB6 91 9E060000      - movzx edx,byte ptr[ecx+0000069E]		this loads the face index into edx
//AA2Edit.exe+28542 - 8B 77 6C              - mov esi,[edi+6C]
//AA2Edit.exe+28545 - 4E                    - dec esi								this seems to be the maximum allowed index (135)
//AA2Edit.exe+28546 - 3B F2                 - cmp esi,edx
//AA2Edit.exe+28548 - 7C 1D                 - jnge AA2Edit.exe+28567					take this jump if border < hair index
//AA2Edit.exe+2854A - 8B B7 A8050000        - mov esi,[edi+000005A8]
//AA2Edit.exe+28550 - 38 1C 32              - cmp[edx+esi],bl						no idea what this is. looks like a bit field. full of 1s.
//AA2Edit.exe+28553 - 75 22                 - jne AA2Edit.exe+28577
//AA2Edit.exe+28555 - 38 58 40              - cmp[eax+40],bl
//AA2Edit.exe+28558 - 0F95 C0               - setne al
//AA2Edit.exe+2855B - 8D 44 00 01           - lea eax,[eax+eax+01]
//AA2Edit.exe+2855F - 88 81 9E060000        - mov[ecx+0000069E],al
//AA2Edit.exe+28565 - EB 10                 - jmp AA2Edit.exe+28577
//AA2Edit.exe+28567 - 38 58 40              - cmp[eax+40],bl
//AA2Edit.exe+2856A - 0F95 C2               - setne dl
//AA2Edit.exe+2856D - 8D 54 12 01           - lea edx,[edx+edx+01]
//AA2Edit.exe+28571 - 88 91 9E060000        - mov[ecx+0000069E],dl
//AA2Edit.exe+28577 - 0FB6 81 9E060000      - movzx eax,byte ptr[ecx+0000069E]
//used borders:
//[edi+60] for ebx=0
//[edi+64] for ebx=1
//[edi+6C] for ebx=2
//[edi+68] for ebx=3 (now that makes sense, doesnt it...)
//changing them afterwards seems to cause crashes though (presumably cause he expects the buttons to be there?)
//how about we try to nop the nge jumps, maybe that works
//AA2Edit.exe+28492 - 7C 0B                 - jnge AA2Edit.exe+2849F
//AA2Edit.exe+284FD - 7C 0B                 - jnge AA2Edit.exe+2850A
//AA2Edit.exe+28548 - 7C 1D                 - jnge AA2Edit.exe+28567
//AA2Edit.exe+285CE - 7C 0B                 - jnge AA2Edit.exe+285DB


//edi is the HairDialog class
//ecx is the tab and given as parameter (0-3 again)
//AA2Edit.exe+29BAB - 8B 4C 24 14           - mov ecx,[esp+14]
//AA2Edit.exe+29BAF - C1 E1 04              - shl ecx,04
//AA2Edit.exe+29BB2 - 8B 84 39 08050000     - mov eax,[ecx+edi+00000508]
//AA2Edit.exe+29BB9 - 8D 1C 39              - lea ebx,[ecx+edi]
//sets hair buttons to disable/enable depending on wether this slot exists (ecx is bool) (on tab switch) (only custom buttons tho)
//AA2Edit.exe+29BF8 - 8B 93 88050000        - mov edx,[ebx+00000588]
//AA2Edit.exe+29BFE - 80 3C 16  00          - cmp byte ptr[esi+edx],00	bool field of loaded hair
//AA2Edit.exe+29C02 - 0F95 C0               - setne al
//AA2Edit.exe+29C05 - 0FB6 C8               - movzx ecx,al
//AA2Edit.exe+29C08 - 51                    - push ecx
//AA2Edit.exe+29C09 - 8B 4F 48              - mov ecx,[edi+48]
//AA2Edit.exe+29C0C - 8B C6                 - mov eax,esi
//AA2Edit.exe+29C0E - E8 CDD7FEFF           - call AA2Edit.exe+173E0
//note: [[ecx+14] + (hairslot << 4)] = HWND of button
//more generally, ecx+14 seems to be a pointer to an array, where ecx+18 is the pointer to the first invalid(?)
//element of said array (it is defintely used to calculate wether the given slot has a button or not)

//AA2Edit.exe+D9FEF - 3B F7                 - cmp esi,edi
//AA2Edit.exe+D9FF1 - 74 0E                 - je AA2Edit.exe+DA001 jump if hair does not exist, somewhere nowhere.
//sources:
//this makes edi. esp+40 is a parameter which appears to be tab+2 [2 for front, 3 for side, 4 for back, 5 for extension]
//AA2Edit.exe+D9F47 - 8B 44 24 40           - mov eax,[esp+40]
//AA2Edit.exe+D9F4B - 8D 50 02              - lea edx,[eax+02]
//AA2Edit.exe+D9F4E - C1 E0 04              - shl eax,04
//AA2Edit.exe+D9F51 - 8B 7C 08 24           - mov edi,[eax+ecx+24]
//now were missing ecx from that. ecx is this and comes from
//AA2Edit.exe+118F1F - 8B 47 0C              - mov eax,[edi+0C]			edi = HairDialogClass
//AA2Edit.exe+118F27 - 8B 88 9C0A0000        - mov ecx,[eax+00000A9C]
//this makes esi. the call seems to be a void call, not even a thiscall. lets try that.
//AA2Edit.exe+D9FBD - E8 2E0B0000           - call AA2Edit.exe+DAAF0
//AA2Edit.exe+D9FC2 - 8B 30                 - mov esi,[eax]


//AA2Edit.exe+28E76 - E8 45120000           - call AA2Edit.exe+2A0C0
//this function pushes another bool into the HairExists field. it does so [edi+60] times, which can be increased
//to add more fields as well as more buttons.

//this part limits, at construction, the amount of front-hairs.
//AA2Edit.exe+28D10 - 3D 87000000           - cmp eax,00000087
//AA2Edit.exe+28D15 - 77 0B                 - ja AA2Edit.exe+28D22
//AA2Edit.exe+28D17 - 3B C3                 - cmp eax,ebx				ebx=0
//AA2Edit.exe+28D19 - 76 0E                 - jna AA2Edit.exe+28D29
//AA2Edit.exe+28D1B - 3D 87000000           - cmp eax,00000087
//AA2Edit.exe+28D20 - 76 09                 - jna AA2Edit.exe+28D2B
//AA2Edit.exe+28D22 - B8 87000000           - mov eax,00000087
//AA2Edit.exe+28D27 - EB 02                 - jmp AA2Edit.exe+28D2B
//AA2Edit.exe+28D29 - 33 C0                 - xor eax,eax
//AA2Edit.exe+28D2B - 8D 8E C4040000        - lea ecx,[esi+000004C4]
//AA2Edit.exe+28D31 - 50                    - push eax
//AA2Edit.exe+28D32 - 89 46 60              - mov[esi+60],eax

//similar locations:
//AA2Edit.exe+28EEA - 3D 87000000           - cmp eax,00000087
//AA2Edit.exe+28EF5 - 3D 87000000           - cmp eax,00000087
//AA2Edit.exe+28EFC - B8 87000000           - mov eax,00000087

//AA2Edit.exe+291A0 - 3D 87000000           - cmp eax,00000087
//AA2Edit.exe+291AB - 3D 87000000           - cmp eax,00000087
//AA2Edit.exe+291B2 - B8 87000000           - mov eax,00000087

//AA2Edit.exe+294DB - 3D 87000000           - cmp eax,00000087
//AA2Edit.exe+294E6 - 3D 87000000           - cmp eax,00000087
//AA2Edit.exe+294ED - B8 87000000           - mov eax,00000087

//insert tab 1
//AA2Edit.exe+28E76 - E8 45120000           - call AA2Edit.exe+2A0C0
//getting tab 2 bool ptr
//AA2Edit.exe+290F4 - 8B 73 04              - mov esi,[ebx+04]
//insert tab 2?
//AA2Edit.exe+29121 - E8 1A100000           - call AA2Edit.exe+2A140
//getting tab 3 bool ptr
//AA2Edit.exe+29427 - 8B B7 A8050000        - mov esi,[edi+000005A8]
//inserting tab 3?
//AA2Edit.exe+2945D - E8 DE0C0000           - call AA2Edit.exe+2A140
//getting tab 4 bool ptr
//AA2Edit.exe+29761 - 8B B7 B8050000        - mov esi,[edi+000005B8]
//inserting tab 4?
//AA2Edit.exe+29797 - E8 A4090000           - call AA2Edit.exe+2A140



//if we change all these to FF, we would have all the slots covered in the bool array.
//the game would also create more buttons, but only show the annoying 135 one
//shit. this crashes if the last ones are changed.

//main message loop
/*AA2Edit.exe+4287 - FF 15 0C444E01        - call dword ptr[AA2Edit.exe+2C440C]
AA2Edit.exe+428D - 85 C0                 - test eax,eax
AA2Edit.exe+428F - 74 4D                 - je AA2Edit.exe+42DE
AA2Edit.exe+4291 - 8B 15 DCC65801        - mov edx,[AA2Edit.exe+36C6DC]
AA2Edit.exe+4297 - 8D 4C 24 34           - lea ecx,[esp+34]
AA2Edit.exe+429B - 51                    - push ecx
AA2Edit.exe+429C - 52                    - push edx
AA2Edit.exe+429D - E8 DE671900           - call AA2Edit.exe+19AA80
AA2Edit.exe+42A2 - 85 C0                 - test eax,eax
AA2Edit.exe+42A4 - 75 CA                 - jne AA2Edit.exe+4270
AA2Edit.exe+42A6 - 8D 44 24 34           - lea eax,[esp+34]
AA2Edit.exe+42AA - 50                    - push eax
AA2Edit.exe+42AB - FF 15 08444E01        - call dword ptr[AA2Edit.exe+2C4408]
AA2Edit.exe+42B1 - 8D 4C 24 34           - lea ecx,[esp+34]
AA2Edit.exe+42B5 - 51                    - push ecx
AA2Edit.exe+42B6 - FF 15 04444E01        - call dword ptr[AA2Edit.exe+2C4404]*/


//code directly before the main message loop, called after the
//initial window is answered
//AA2Edit.exe+425B - E8 10CC1800           - call AA2Edit.exe+190E70
//AA2Edit.exe+4260 - 8B 3D 10444601        - mov edi,[AA2Edit.exe+2C4410]
//AA2Edit.exe+4266 - 8B 35 00444601        - mov esi,[AA2Edit.exe+2C4400]
//AA2Edit.exe+426C - 8D 64 24 00           - lea esp,[esp+00]


//this jump seems to be taken whenever a hair is valid
//AA2Edit.exe+119CF2 - 74 0C                 - je AA2Edit.exe+119D00
//AA2Edit.exe+119CF4 - 8B 4C 24 1C           - mov ecx,[esp+1C]
//AA2Edit.exe+119CF8 - C6 84 0E 5C020000 00  - mov byte ptr[esi+ecx+0000025C],00
//AA2Edit.exe+119D00 - 33 D2                 - xor edx,edx

//mov esi, [edi+4C] edi is hair class
//mov ecx, [esi+24] ecx is hair size edit field

//---------FACE DETAILS--------

//part that create the dialog
//AA2Edit.exe+2630E - 68 90AB4801           - push AA2Edit.exe+19AB90
//AA2Edit.exe+26313 - 50                    - push eax
//AA2Edit.exe+26314 - 68 A1000000           - push 000000A1
//AA2Edit.exe+26319 - 51                    - push ecx
//AA2Edit.exe+2631A - FF 15 54435B01        - call dword ptr[AA2Edit.exe+2C4354]
//AA2Edit.exe+26320 - 8B 56 40              - mov edx,[esi+40]
//AA2Edit.exe+26323 - 89 42 28              - mov[edx+28],eax


//WRONGstart of virtual handler
//AA2Edit.exe+34160 - 8B 44 24 08           - mov eax,[esp+08]
//AA2Edit.exe+34164 - 56                    - push esi
//AA2Edit.exe+34165 - 8B F1                 - mov esi,ecx
//AA2Edit.exe+34167 - 3D 10010000           - cmp eax,00000110

//WRONGend of WM_INITDIALOG handler (void, but eax-thiscall/)
//AA2Edit.exe+34249 - E8 82FAFFFF           - call AA2Edit.exe+33CD0

//(fuck this one dialog that also grabs message, i got the wrong handler there)
//actual handler i think
//AA2Edit.exe+273D0 - 55                    - push ebp
//AA2Edit.exe+273D1 - 8B EC                 - mov ebp,esp
//AA2Edit.exe+273D3 - 83 E4 F8              - and esp,-08
//AA2Edit.exe+273D6 - 51                    - push ecx

//actual WM_INITDIALOG handler
//AA2Edit.exe+273F9 - 57                    - push edi
//AA2Edit.exe+273FA - E8 F1EFFFFF           - call AA2Edit.exe+263F0

//series of these calls are filling eyelid etc, 695 is glasses
//AA2Edit.exe+26D62 - E8 3907FFFF           - call AA2Edit.exe+174A0
//AA2Edit.exe+26D67 - 88 83 94060000        - mov[ebx+00000694],al //lower eyelid
//AA2Edit.exe+26D6D - 8B 7D 54              - mov edi,[ebp+54]
//AA2Edit.exe+26D70 - E8 2B07FFFF           - call AA2Edit.exe+174A0
//AA2Edit.exe+26D75 - 88 83 95060000        - mov[ebx+00000695],al //glasses
//AA2Edit.exe+26D7B - 8B 7D 58              - mov edi,[ebp+58]
//AA2Edit.exe+26D7E - E8 1D07FFFF           - call AA2Edit.exe+174A0
//AA2Edit.exe+26D83 - 88 83 96060000        - mov[ebx+00000696],al //lip color


//with ebx coming together as described here, and ebp = parameter = [ebp+B8] with ebp = class
//AA2Edit.exe+26D10 - 55                    - push ebp
//AA2Edit.exe+26D11 - 8B 6C 24 08           - mov ebp,[esp+08]
//AA2Edit.exe+26D15 - 8B 45 44              - mov eax,[ebp+44]
//AA2Edit.exe+26D18 - 8B 48 24              - mov ecx,[eax+24]
//AA2Edit.exe+26D1B - 83 39 00              - cmp dword ptr[ecx],00
//AA2Edit.exe+26D1E - 75 05                 - jne AA2Edit.exe+26D25
//AA2Edit.exe+26D20 - 8B 40 30              - mov eax,[eax+30]
//AA2Edit.exe+26D23 - EB 03                 - jmp AA2Edit.exe+26D28
//AA2Edit.exe+26D25 - 8B 40 2C              - mov eax,[eax+2C]
//AA2Edit.exe+26D28 - 85 C0                 - test eax,eax
//AA2Edit.exe+26D2A - 75 06                 - jne AA2Edit.exe+26D32
//AA2Edit.exe+26D2C - 32 C0                 - xor al,al
//AA2Edit.exe+26D2E - 5D                    - pop ebp
//AA2Edit.exe+26D2F - C2 0400               - ret 0004
//AA2Edit.exe+26D32 - 53                    - push ebx
//AA2Edit.exe+26D33 - 8B 58 28              - mov ebx,[eax+28]

//the glasses buttons function (call AA2Edit.exe+174A0):
//with edi = [ebp+54] from above,
//esi = [edi+14] is the first struct button, [edi+18] is the first unused button struct (as known from hair)
//esi is than += 0x10ed, while [esi] is the hwnd of the button. neato.

//call to this function initialises face details dialog, with ebp being the class ptr
//AA2Edit.exe+32708 - 55                    - push ebp
//AA2Edit.exe+32709 - E8 82020000           - call AA2Edit.exe+32990

//sets the current tab (0 = system etc). esi is class of the header dialog window
//AA2Edit.exe+341A6 - 88 46 74              - mov[esi+74],al

//start of virtual handler of body color tab
//AA2Edit.exe+21350 - 55                    - push ebp
//AA2Edit.exe+21351 - 8B EC                 - mov ebp,esp
//AA2Edit.exe+21353 - 83 E4 F8              - and esp,-08

//WM_INITDIALOG handler of body color tab
//AA2Edit.exe+21374 - 57                    - push edi
//AA2Edit.exe+21375 - E8 76E9FFFF           - call AA2Edit.exe+1FCF0

//sets the current tan when a new one selected
//AA2Edit.exe+20B51 - 8B 7B 54              - mov edi,[ebx+54]
//AA2Edit.exe+20B54 - E8 4769FFFF           - call AA2Edit.exe+174A0
//AA2Edit.exe+20B59 - 88 86 67040000        - mov[esi+00000467],al

//again, the esi for the tan byte (its always the same, is it)
/*AA2Edit.exe+20AE0 - 53                    - push ebx
AA2Edit.exe+20AE1 - 8B 5C 24 08           - mov ebx,[esp+08]
AA2Edit.exe+20AE5 - 8B 43 44              - mov eax,[ebx+44]
AA2Edit.exe+20AE8 - 8B 48 24              - mov ecx,[eax+24]
AA2Edit.exe+20AEB - 83 39 00              - cmp dword ptr [ecx],00
AA2Edit.exe+20AEE - 75 05                 - jne AA2Edit.exe+20AF5
AA2Edit.exe+20AF0 - 8B 40 30              - mov eax,[eax+30]
AA2Edit.exe+20AF3 - EB 03                 - jmp AA2Edit.exe+20AF8
AA2Edit.exe+20AF5 - 8B 40 2C              - mov eax,[eax+2C]
AA2Edit.exe+20AF8 - 85 C0                 - test eax,eax
AA2Edit.exe+20AFA - 75 06                 - jne AA2Edit.exe+20B02
AA2Edit.exe+20AFC - 32 C0                 - xor al,al
AA2Edit.exe+20AFE - 5B                    - pop ebx
AA2Edit.exe+20AFF - C2 0400               - ret 0004
AA2Edit.exe+20B02 - 56                    - push esi
AA2Edit.exe+20B03 - 8B 70 28              - mov esi,[eax+28]
AA2Edit.exe+20B06 - 85 F6                 - test esi,esi
AA2Edit.exe+20B08 - 75 07                 - jne AA2Edit.exe+20B11
AA2Edit.exe+20B0A - 5E                    - pop esi
AA2Edit.exe+20B0B - 32 C0                 - xor al,al
AA2Edit.exe+20B0D - 5B                    - pop ebx
AA2Edit.exe+20B0E - C2 0400               - ret 0004
AA2Edit.exe+20B11 - 57                    - push edi
AA2Edit.exe+20B12 - 8B 7B 48              - mov edi,[ebx+48]*/

//start of system dialog handler
//AA2Edit.exe+1BAD0 - 8B 44 24 08           - mov eax,[esp+08]
//AA2Edit.exe+1BAD4 - 3D 11010000           - cmp eax,00000111

//call of initdialog handler
//AA2Edit.exe+1BAE9 - 56                    - push esi
//AA2Edit.exe+1BAEA - 8B F1                 - mov esi,ecx
//AA2Edit.exe+1BAEC - E8 FFFCFFFF           - call AA2Edit.exe+1B7F0


//last call of (successful) open file dialog click:
//AA2Edit.exe+1C26C - E8 9F73FEFF           - call AA2Edit.exe+3610

//exit (last jump) of create character button
//AA2Edit.exe+1C4C1 - E9 97FAFFFF           - jmp AA2Edit.exe+1BF5D

//many sliders that go from 0 to 100 were not properly unlocked by the frontier launcher.
//the limiter parts look like this:
/*
AA2Edit.exe+20B34 - 83 F8 64              - cmp eax,64 { 100 }
AA2Edit.exe+20B37 - 7F 09                 - jg AA2Edit.exe+20B42
AA2Edit.exe+20B39 - 85 C0                 - test eax,eax
AA2Edit.exe+20B3B - 7E 0C                 - jle AA2Edit.exe+20B49
AA2Edit.exe+20B3D - 83 F8 64              - cmp eax,64 { 100 }
AA2Edit.exe+20B40 - 7E 09                 - jle AA2Edit.exe+20B4B
AA2Edit.exe+20B42 - B8 64000000           - mov eax,00000064 { 100 }
AA2Edit.exe+20B47 - EB 02                 - jmp AA2Edit.exe+20B4B
AA2Edit.exe+20B49 - 33 C0                 - xor eax,eax
AA2Edit.exe+20B4B - 88 86 66040000        - mov[esi+00000466],al
*/
//the easiest way to fix it is to make the jg to an unconditional jump to after xor eax,eax:
/*
AA2Edit.exe+20B34 - 83 F8 64              - cmp eax,64 { 100 }
AA2Edit.exe+20B37 - EB 12                 - jmp AA2Edit.exe+20B4B
AA2Edit.exe+20B39 - 85 C0                 - test eax,eax
AA2Edit.exe+20B3B - 7E 0C                 - jle AA2Edit.exe+20B49
AA2Edit.exe+20B3D - 83 F8 64              - cmp eax,64 { 100 }
AA2Edit.exe+20B40 - 7E 09                 - jle AA2Edit.exe+20B4B
AA2Edit.exe+20B42 - B8 64000000           - mov eax,00000064 { 100 }
AA2Edit.exe+20B47 - EB 02                 - jmp AA2Edit.exe+20B4B
AA2Edit.exe+20B49 - 33 C0                 - xor eax,eax
AA2Edit.exe+20B4B - 88 86 66040000        - mov [esi+00000466],al
*/
//so, since jmps are relative, its always 7F 09 -> EB 12
//locations to look at:
//nip:
//AA2Edit.exe+20B37 - 7F 09                 - jg AA2Edit.exe+20B42
//tan:
//AA2Edit.exe+20B68 - 7F 09                 - jg AA2Edit.exe+20B73
//pu hair:
//AA2Edit.exe+20BA8 - 7F 09                 - jg AA2Edit.exe+20BB3
//brow:
//AA2Edit.exe+25BAA - 7F 09                 - jg AA2Edit.exe+25BB5
//lips:
//AA2Edit.exe+26D92 - 7F 09                 - jg AA2Edit.exe+26D9D

//nip type and color poll (in this order)
/*
AA2Edit.exe+20B12 - 8B 7B 48              - mov edi,[ebx+48]
AA2Edit.exe+20B15 - E8 8669FFFF           - call AA2Edit.exe+174A0
AA2Edit.exe+20B1A - 88 86 64040000        - mov [esi+00000464],al
AA2Edit.exe+20B20 - 8B 7B 4C              - mov edi,[ebx+4C]
AA2Edit.exe+20B23 - E8 7869FFFF           - call AA2Edit.exe+174A0
AA2Edit.exe+20B28 - 88 86 65040000        - mov [esi+00000465],al
*/

//mosaic and pub hair poll (in this order). mosaic flag is 11, pub hair flag is B
/*
AA2Edit.exe+20B82 - 8B 7B 5C              - mov edi,[ebx+5C]
AA2Edit.exe+20B85 - E8 1669FFFF           - call AA2Edit.exe+174A0
AA2Edit.exe+20B8A - 88 86 69040000        - mov [esi+00000469],al
AA2Edit.exe+20B90 - 8B 7B 60              - mov edi,[ebx+60]
AA2Edit.exe+20B93 - E8 0869FFFF           - call AA2Edit.exe+174A0
AA2Edit.exe+20B98 - 88 86 5C040000        - mov [esi+0000045C],al
*/

//lip color
/*
AA2Edit.exe+26D7B - 8B 7D 58              - mov edi,[ebp+58]
AA2Edit.exe+26D7E - E8 1D07FFFF           - call AA2Edit.exe+174A0
AA2Edit.exe+26D83 - 88 83 96060000        - mov[ebx+00000696],al
*/

//this is a part of the INITDIALOG from the hair dialog
/*AA2Edit.exe+28CE0 - 53                    - push ebx <--note: ebx is 0 here
AA2Edit.exe+28CE1 - 8D 4C 24 48           - lea ecx,[esp+48]
AA2Edit.exe+28CE5 - 51                    - push ecx
AA2Edit.exe+28CE6 - 8D 56 50              - lea edx,[esi+50]
AA2Edit.exe+28CE9 - 8B CF                 - mov ecx,edi
AA2Edit.exe+28CEB - C6 44 24 78 02        - mov byte ptr[esp+78],02 { 2 }
AA2Edit.exe+28CF0 - E8 4B050F00           - call AA2Edit.exe+119240 { initialises the hair list }*/
//the function here returns a pointer of hair info into ecx. its an array of 255, one for each hair,
//with each entry having a size of 51C. It contains information such as in which pp to find it (if at all),
//what xx file is associated with it, and if it has a flip.
//im gonna paste the calls for the other tabs here:
/*AA2Edit.exe+28EBC - 6A 01                 - push 01 { 1 }
AA2Edit.exe+28EBE - 8D 4C 24 48           - lea ecx,[esp+48]
AA2Edit.exe+28EC2 - 51                    - push ecx
AA2Edit.exe+28EC3 - 8B 4C 24 2C           - mov ecx,[esp+2C]
AA2Edit.exe+28EC7 - 8D 56 54              - lea edx,[esi+54]
AA2Edit.exe+28ECA - E8 71030F00           - call AA2Edit.exe+119240*/
/*AA2Edit.exe+29176 - 6A 02                 - push 02 { 2 }
AA2Edit.exe+29178 - 8D 44 24 48           - lea eax,[esp+48]
AA2Edit.exe+2917C - 8D 56 5C              - lea edx,[esi+5C]			<-- this is curious, seems like its switched
AA2Edit.exe+2917F - 50                    - push eax
AA2Edit.exe+29180 - E8 BB000F00           - call AA2Edit.exe+119240*/
/*AA2Edit.exe+294B1 - 6A 03                 - push 03 { 3 }
AA2Edit.exe+294B3 - 8D 44 24 48           - lea eax,[esp+48]
AA2Edit.exe+294B7 - 8D 56 58              - lea edx,[esi+58]
AA2Edit.exe+294BA - 50                    - push eax
AA2Edit.exe+294BB - E8 80FD0E00           - call AA2Edit.exe+119240*/
//btw, the function has ret 8




//misc:
//AA2Edit.exe+196CA7 - D9 82 88010000        - fld dword ptr[edx+00000188]
//AA2Edit.exe+196CE0 - D9 82 8C010000        - fld dword ptr[edx+0000018C]
//load the min/max values for zooming, respectively for zoom limit
/*
push AA2Edit.exe+344F40
[esp+04] note the esp
[eax+E314] can be NULL if character is not loaded, make sure to check that
[edi+20]
[esi+04]
lots of carrying
[edx+188] / [edx+18C]
*/

//opens file for saving
/*AA2Edit.exe+1261C0 - 51                    - push ecx
AA2Edit.exe+1261C1 - 57                    - push edi
AA2Edit.exe+1261C2 - 6A 00                 - push 00 { 0 }
AA2Edit.exe+1261C4 - 68 80000000           - push 00000080 { 128 }
AA2Edit.exe+1261C9 - 6A 02                 - push 02 { 2 }
AA2Edit.exe+1261CB - 6A 00                 - push 00 { 0 }
AA2Edit.exe+1261CD - 6A 02                 - push 02 { 2 }
AA2Edit.exe+1261CF - 68 00000040           - push 40000000 { 2.00 }
AA2Edit.exe+1261D4 - 50                    - push eax
AA2Edit.exe+1261D5 - FF 15 B041C600        - call dword ptr [AA2Edit.exe+2C41B0] { ->kernel32.CreateFileW }
*/

//closes it again
/*AA2Edit.exe+126209 - 57                    - push edi
AA2Edit.exe+12620A - FF 15 8441C600        - call dword ptr [AA2Edit.exe+2C4184] { ->kernel32.CloseHandle }
*/

//function that reopens a character, ecx is cloth state (0 = nude, 1 = clothed, 2 = half off etc)
/*
AA2Edit.exe+332E9 - 51                    - push ecx
AA2Edit.exe+332EA - 8B CB                 - mov ecx,ebx
AA2Edit.exe+332EC - FF D2                 - call edx
*/

//al contains the outfit that is to be shown (0 = schoool, 1 = sport, 2 = bath, 3 = club)
/*AA2Edit.exe+10059F - 8A 43 44              - mov al,[ebx+44]
AA2Edit.exe+1005A2 - 8B 73 28              - mov esi,[ebx+28]
...
AA2Edit.exe+1005D3 - 8A 10                 - mov dl,[eax]
AA2Edit.exe+1005D5 - 89 44 24 38           - mov [esp+38],eax { writes pointer to struct that contains cloth slot to
wear (dword ptr [eax] = slot). note that chaning this slot will change clothes when saved. note that merely changing
dl will not suffice, [eax] needs to be changed temporarily
}
*/

//directly before the shirt and legs are opened. ebp+8 is cloth state from above
/*AA2Edit.exe+1006DA - 89 44 24 30           - mov[esp+30],eax
AA2Edit.exe+1006DE - 8B FF                 - mov edi,edi*/

//the test is the first instruction to look at the skirt
/*AA2Edit.exe+10190C - E8 5F9D0100           - call AA2Edit.exe+11B670			: 1 parameter, no thiscall
AA2Edit.exe+101911 - F6 45 08 01           - test byte ptr [ebp+08],01 { 1 }
*/

//the first instruction to stop looking at the skirt
/*AA2Edit.exe+101C29 - 8B 43 30              - mov eax,[ebx+30]
AA2Edit.exe+101C2C - 8B 40 28              - mov eax,[eax+28]*/

//this part is particularily interisting, [edi] is the cloth part to display.
//clothes with index >3 are handled specially, and the ones below _do_not_consider_body_thickness.
//if i make the jg to a jmp, it will consider them on other clothes as well. that would be better.
/*AA2Edit.exe+1006FB - 83 3F 03              - cmp dword ptr [edi],03 { 3 }
AA2Edit.exe+1006FE - 0F8F 87000000         - jg AA2Edit.exe+10078B { specialy handleded clothes?*/
//to
/*AA2Edit.exe+1006FE - E9 88000000           - jmp AA2Edit.exe+10078B
AA2Edit.exe+100703 - 90                    - nop */


//
//pose-cancelation: all of the comparisions have to be true, then the last call,
//taking al = animation as a parameter, will play that animation
/*AA2Edit.exe+33BEE - 80 7C 24 16 00        - cmp byte ptr [esp+16],00 { 0 }
AA2Edit.exe+33BF3 - 74 28                 - je AA2Edit.exe+33C1D
AA2Edit.exe+33BF5 - 32 D2                 - xor dl,dl
AA2Edit.exe+33BF7 - 8B CB                 - mov ecx,ebx
AA2Edit.exe+33BF9 - E8 22230000           - call AA2Edit.exe+35F20
AA2Edit.exe+33BFE - 84 C0                 - test al,al
AA2Edit.exe+33C00 - 75 1B                 - jne AA2Edit.exe+33C1D
AA2Edit.exe+33C02 - 38 54 24 1B           - cmp [esp+1B],dl
AA2Edit.exe+33C06 - 75 15                 - jne AA2Edit.exe+33C1D
AA2Edit.exe+33C08 - 8B FB                 - mov edi,ebx
AA2Edit.exe+33C0A - E8 A1230000           - call AA2Edit.exe+35FB0
*/

//inside the pose apply function, this jmp contains it to 0-3 poses.
//AA2Edit.exe+35FD0 - 83 F8 03              - cmp eax,03 { 3 }
//AA2Edit.exe+35FD3 - 0F87 D0000000         - ja AA2Edit.exe+360A9
//..
//AA2Edit.exe+3600B - B8 03000000           - mov eax,00000003 { 3 }
//AA2Edit.exe+36010 - 8B 8E 74C30100        - mov ecx,[esi+0001C374]
//the last jump of the switch ends at the eax,3 (for some retarded reason),
//i have to make it jump after that instead, like this
/*AA2Edit.exe+35FEA - 77 24                 - ja AA2Edit.exe+36010
AA2Edit.exe+35FEC - 90                    - nop
AA2Edit.exe+35FED - 90                    - nop
AA2Edit.exe+35FEE - 90                    - nop
AA2Edit.exe+35FEF - 90                    - nop*/

//activates eye tracking if bx == 1 && bx != esi+100E, deactivates if bx == 0 && ...
/*AA2Edit.exe+1ADFA2 - 66 39 9E 0E100000     - cmp[esi+0000100E],bx
AA2Edit.exe+1ADFA9 - 75 06                 - jne AA2Edit.exe+1ADFB1*/

//random hair call from all random button and single random button, respectively; esi thiscall
//AA2Edit.exe+1B932 - E8 F9C70000           - call AA2Edit.exe+28130
//AA2Edit.exe+34706 - E8 253AFFFF           - call AA2Edit.exe+28130


//random for body color, eax thisccall
//AA2Edit.exe+1B8F0 - E8 FB520000           - call AA2Edit.exe+20BF0
//AA2Edit.exe+34641 - E8 AAC5FEFF           - call AA2Edit.exe+20BF0

//and the face, edi thiscall
//AA2Edit.exe+1B8FB - E8 F0690000           - call AA2Edit.exe+222F0
//AA2Edit.exe+34663 - E8 88DCFEFF           - call AA2Edit.exe+222F0

//face details, eax thiscall
//AA2Edit.exe+1B927 - E8 C4B40000           - call AA2Edit.exe+26DF0
//AA2Edit.exe+346E6 - E8 0527FFFF           - call AA2Edit.exe+26DF0

