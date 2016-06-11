#include <Windows.h>
#include "Logger.h"
#include "SlotFile.h"

SlotFile g_slotFile;

const char* SlotFile::slotCategories[16] = {
		"[FrontHair]", "[SideHair]", "[BackHair]", "[HairExtension]", 
		"[FrontHairFlip]", "[SideHairFlip]", "[BackHairFlip]", "[HairExtensionFlip]",
		"[Face]", 
		"[NipType]", "[NipColor]", "[Tan]", "[Mosaic]", "[PubShape]",
		"[Glasses]", "[LipColor]"
};

SlotFile::SlotFile() : m_slotExists { 0 },m_validEntrys { 0 } {

}

SlotFile::SlotFile(const char* filename) : SlotFile() {
	HANDLE f = CreateFile(filename,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	if (f == NULL || f == INVALID_HANDLE_VALUE) {
		LOGPRIO(Logger::Priority::WARN) << "Could not open slot file\r\n";
		return; //failed to open
	}
	DWORD fhi;
	DWORD flo = GetFileSize(f,&fhi);
	char* buffer = new char[flo+1];
	buffer[flo] = '\0';
	DWORD bufferLength = 0;
	ReadFile(f,buffer,flo,&bufferLength,NULL);
	CloseHandle(f);
	if (bufferLength == 0) return; //nothing read for some reason
	buffer[bufferLength] = '\0';

	//read buffer, line for line
	char* it = NULL,*nextLine = buffer;
	int currSlotType = -1;
	do {
		it = nextLine;
		GetLine(it,&nextLine);
		char* token;
		while(it != NULL && (token = GetToken(it, &it))) {
			if(token[0] == '[') {
				//new slot type
				for (int i = 0; i < sizeof(slotCategories)/sizeof(slotCategories[0]); i++) {
					if(strcmp(token, slotCategories[i]) == 0) {
						currSlotType = i;
						break;
					}
					if(i == sizeof(slotCategories)/sizeof(slotCategories[0]) - 1) {
						LOGPRIO(Logger::Priority::WARN) << "unrecognized slot type " << token << "\r\n";
					}
				}
			}
			else {
				//number or number range
				if(currSlotType == -1) {
					LOGPRIO(Logger::Priority::WARN) << "slot number without defined slot type: " << token << "\r\n";
					continue;
				}
				char* end = token;
				int num = strtol(token,&end,10);
				if(num < 0 || num > 255) {
					LOGPRIO(Logger::Priority::WARN) << "slot " << token << " out of range; slot number must be 8 bit unsigned integer\r\n";
					continue;
				}
				if(end[0] == '\0') {
					//was standalone number
					m_slotExists[currSlotType][num] = true;
					m_validEntrys[currSlotType]++;
				}
				else if(end[0] == '-') {
					//number range
					end++;
					char* end2 = end;
					int num2 = strtol(end,&end2,10);
					if (num2 < 0 || num2 > 255) {
						LOGPRIO(Logger::Priority::WARN) << "slot " << token << " out of range; slot number must be 8 bit unsigned integer\r\n";
						continue;
					}
					if(num > num2) {
						LOGPRIO(Logger::Priority::WARN) << "number range " << token << "does not contain any integers\r\n";
						continue;
					}
					for (num; num <= num2; num++) {
						m_slotExists[currSlotType][num] = true;
						m_validEntrys[currSlotType]++;
					}
				}
			}
		}
	} while (nextLine != NULL);
	
	delete[] buffer;
}