#pragma once
#include "Parser.h"

class SlotFile : Parser {
public:	
	enum SlotTypes {
		HAIR_FRONT,HAIR_SIDE,HAIR_BACK,HAIR_EXTENSION,
		HAIR_FRONT_FLIPS,HAIR_SIDE_FLIPS,HAIR_BACK_FLIPS,HAIR_EXTENSION_FLIPS,
		FACE,
		NIP_TYPE,NIP_COLOR,TAN,MOSAIC,PUB_SHAPE,
		GLASSES,LIP_COLOR
	};
private:
	static const char* slotCategories[16];

	bool m_slotExists[16][256];
	int m_validEntrys[16];
public:
	SlotFile();
	SlotFile(const char* filename);
	
	inline int ValidSlotCount(SlotTypes tab)  { return m_validEntrys[tab]; }
	inline bool SlotExists(SlotTypes tab,BYTE slot) { return m_slotExists[tab][slot]; }
private:

};

extern SlotFile g_slotFile;