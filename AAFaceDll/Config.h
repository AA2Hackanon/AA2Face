#pragma once
#include <vector>
#include "Logger.h"
#include "Parser.h"

class Config : Parser
{
public:
	struct Hotkey {
		enum Function {
			NONE,
			HAIR_FLIP,HAIR_ADD,HAIR_SET,
			HAIR_SIZEADD, HAIR_SIZESET,
			FACEDETAILS_ADDGLASSES, FACEDETAILS_SETGLASSES,
			BODYCOLOR_ADDTAN,BODYCOLOR_SETTAN,
			ZOOM_ADD, ZOOM_SET,
			TILT_ADD, TILT_SET,
			POSE_ADD, POSE_APPLY,
			
			NOP
		};
		enum Context {
			//ordered by number so you can plug the enum into the g_AA2Dialogs array
			DIALOG_SYSTEM = 0, DIALOG_FIGURE, DIALOG_CHEST, DIALOG_BODY_COLOR, 
			DIALOG_FACE, DIALOG_EYES, DIALOG_EYE_COLOR, DIALOG_EYEBROWS, DIALOG_FACE_DETAILS, 
			DIALOG_HAIR, DIALOG_HAIR_COLOR, DIALOG_CHARACTER, DIALOG_PERSONALITY, DIALOG_TRAITS,
			DIALOG_ALL,
			PREVIEW_WINDOW,
			ALL
		};
		enum ContextKind {
			FOCUS, //window must be in focus
			ACTIVE //window must be active
		};
		bool ctrl,alt,shift;
		Context context;
		ContextKind contextKind;
		long key;
		bool isVirt;
		Function func;
		int iparam;
		float fparam;
		Hotkey() : ctrl(false),alt(false),shift(false),
			context(ALL),contextKind(FOCUS),func(NONE),key(-1),isVirt(false),iparam(0), fparam(0) {}
		bool SameKey(const Hotkey& rhs) const {
			return isVirt == rhs.isVirt && key == rhs.key && ctrl == rhs.ctrl && alt == rhs.alt && shift == rhs.shift;
		}
	};
	enum Disable {
		DISABLE_GENERAL = 1,DISABLE_FACE = 2,DISABLE_HAIR = 4,DISABLE_HAIR_SKIPINVALID = 8,
		DISABLE_FACEDETAILS = 0x10,DISABLE_BODY_COLOR = 0x20,DISABLE_LIMITS = 0x40,
		DISABLE_PRE_BACKUP = 0x80, DISABLE_POST_BACKUP = 0x100, DISABLE_BACKUP_UNIQUE = 0x200,
		DISABLE_RANDOM_SLOT_MOD = 0x400
	};
private:
	struct Keywords {
		const char* key;
		bool(Config::*handler)(char* line);
	};
public:
	Config();
	Config(const char* file);
	~Config();

	const std::vector<Hotkey>& GetHotkeys() const;
	float GetZoomMin() const;
	float GetZoomMax() const;
	bool IsDisabled(Disable check) const;
private:
	
	bool InterpretKey(char* str);
	bool InterpretZoom(char* str);
	bool InterpretDisable(char* str);
	bool InterpretLogger(char* str);

private:
	std::vector<Hotkey> hotkeys;
	float zoomMin,zoomMax;
	int disableFlags;
	Logger::Priority logPrio;
	static const Keywords Commands[];

	
};

extern Config g_config;