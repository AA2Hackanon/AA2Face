#pragma once
#include <vector>
#include "Logger.h"

class Config
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
	bool GetDisabledGeneral() const;
	bool GetDisabledGlasses() const;
	bool GetDisabledHair() const;
	bool GetDisabledFace() const;
	bool GetDisabledHairSkipInvalid() const;
	bool GetDisabledBodycolor() const;
private:
	void GetLine(char* str,char** nextLine);
	char* GetToken(char* str, char** nextLine);
	bool InterpretKey(char* str);
	bool InterpretZoom(char* str);
	bool InterpretDisable(char* str);
	bool InterpretLogger(char* str);

	bool StartsWith(const char* str,const char* word);

private:
	std::vector<Hotkey> hotkeys;
	float zoomMin,zoomMax;
	bool hookGeneral,hookGlasses,hookHair,hookFace,hookBodycolor;
	bool hookInvalidHair;
	Logger::Priority logPrio;
	static const Keywords Commands[];


};

extern Config g_config;