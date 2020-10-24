#include "Config.h"
#include "Logger.h"
#include <Windows.h>

Config g_config;

const Config::Keywords Config::Commands[] = {
	{ "KEY", &Config::InterpretKey },
	{ "ZOOM", &Config::InterpretZoom },
	{ "DISABLE", &Config::InterpretDisable },
	{ "VERBOSITY", &Config::InterpretLogger }

};

Config::Config() : zoomMin(1.5f),zoomMax(0.01f),
				disableFlags(0),
				logPrio(Logger::Priority::WARN)
{
}

//KEY ALT CTRL SHIFT 1
Config::Config(const char* file) : Config()
{
	HANDLE f = CreateFile(file,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	if (f == NULL || f == INVALID_HANDLE_VALUE) {
		LOGPRIO(Logger::Priority::WARN) << "Could not open config file\r\n";
		return; //failed to open
	}
	DWORD fhi; //this really shouldnt be needed, but GetFilieSize crashes if its not given (it shouldnt, but it does)
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
	do {
		it = nextLine;
		GetLine(it,&nextLine);
		//get first token
		char* token;

		token = GetToken(it,&it);
		if (token != NULL) {
			//possible keywords:
			bool found = false;
			for (int i = 0; i < sizeof(Commands) / sizeof(Commands[0]); i++) {
				if (strcmp(token,Commands[i].key) == 0) {
					(this->*(Commands[i].handler))(it);
					found = true;
					break;
				}
			}
			if(!found) {
				LOGPRIO(Logger::Priority::WARN) << "Unkown Config Command \"" << token << "\" found in config file\n";
			}
		}

	} while (nextLine != NULL);
	delete[] buffer;
}


Config::~Config()
{
}

bool Config::InterpretKey(char * str)
{
	char* nextToken = str;
	char* token = NULL;
	Hotkey key;
	//i really should have made this a data structure instead. fuck.
	while (nextToken != NULL && (token = GetToken(nextToken,&nextToken))) {
		if (StartsWith(token,"MOD:")) {
			token += 4;
			if (strcmp(token,"ALT") == 0) {
				key.alt = true;
			}
			else if (strcmp(token,"CTRL") == 0) {
				key.ctrl = true;
			}
			else if (strcmp(token,"SHIFT") == 0) {
				key.shift = true;
			}
			else {
				LOGPRIO(Logger::Priority::WARN) << "Unknown Modification \"" << token << "\" in config file\n";
			}
		}
		else if (StartsWith(token,"CONTEXT:")) {
			token += 8;
			if(StartsWith(token, "FOCUS:")) {
				token += 6;
				key.contextKind = Hotkey::FOCUS;
			}
			else if(StartsWith(token, "ACTIVE:")) {
				token += 7;
				key.contextKind = Hotkey::ACTIVE;
			}
			else {
				key.contextKind = Hotkey::FOCUS;
				//gonna allow it, assuming they know what they do
				//g_Logger << Logger::Priority::WARN << "Unrecognized context kind in " << token << "\n";
			}
			if (StartsWith(token, "DIALOG_")) {
				token += 7;
				if(strcmp(token, "SYSTEM") == 0) {
					key.context = Hotkey::DIALOG_SYSTEM;
				}
				else if(strcmp(token,"FIGURE") == 0) {
					key.context = Hotkey::DIALOG_FIGURE;
				}
				else if (strcmp(token,"CHEST") == 0) {
					key.context = Hotkey::DIALOG_CHEST;
				}
				else if (strcmp(token,"BODY_COLOR") == 0) {
					key.context = Hotkey::DIALOG_BODY_COLOR;
				}
				else if (strcmp(token,"FACE") == 0) {
					key.context = Hotkey::DIALOG_FACE;
				}
				else if (strcmp(token,"EYES") == 0) {
					key.context = Hotkey::DIALOG_EYES;
				}
				else if (strcmp(token,"EYE_COLOR") == 0) {
					key.context = Hotkey::DIALOG_EYE_COLOR;
				}
				else if(strcmp(token, "EYEBROWS") == 0) {
					key.context = Hotkey::DIALOG_EYEBROWS;
				}
				else if (strcmp(token,"FACE_DETAILS") == 0) {
					key.context = Hotkey::DIALOG_FACE_DETAILS;
				}
				else if (strcmp(token,"HAIR") == 0) {
					key.context = Hotkey::DIALOG_HAIR;
				}
				else if (strcmp(token,"HAIR_COLOR") == 0) {
					key.context = Hotkey::DIALOG_HAIR_COLOR;
				}
				else if (strcmp(token,"CHARACTER") == 0) {
					key.context = Hotkey::DIALOG_CHARACTER;
				}
				else if (strcmp(token,"PERSONALITY") == 0) {
					key.context = Hotkey::DIALOG_PERSONALITY;
				}
				else if (strcmp(token,"TRAITS") == 0) {
					key.context = Hotkey::DIALOG_TRAITS;
				}
				else if (strcmp(token,"ALL") == 0) {
					key.context = Hotkey::DIALOG_ALL;
				}
				else {
					LOGPRIO(Logger::Priority::WARN) << "Unknown Dialog \"" << token << "\" in config file\n";
				}
			}
			else if (strcmp(token,"PREVIEW_WINDOW") == 0) {
				key.context = Hotkey::PREVIEW_WINDOW;
			}
			else if (strcmp(token,"ALL") == 0) {
				key.context = Hotkey::ALL;
			}
			else {
				//unrecognized context
				LOGPRIO(Logger::Priority::WARN) << "Unknown Context \"" << token << "\" in config file\n";
			}
		}
		else if (StartsWith(token,"KEYVAL:")) {
			token += 7;
			if(StartsWith(token, "VIRT:")) {
				token += 5;
				key.isVirt = true;
			}
			else if(StartsWith(token, "ASCII:")) {
				token += 6;
				key.isVirt = false;
			}
			else {
				key.isVirt = true;
			}
			key.key = strtol(token,NULL,16);
		}
		else if (StartsWith(token,"FUNCTION:")) {
			token += 9;
			if (strcmp(token,"HAIR_FLIP") == 0) {
				key.func = Hotkey::HAIR_FLIP;
			}
			else if (strcmp(token,"HAIR_ADD") == 0) {
				key.func = Hotkey::HAIR_ADD;
			}
			else if (strcmp(token,"HAIR_SET") == 0) {
				key.func = Hotkey::HAIR_SET;
			}
			else if (strcmp(token,"HAIR_SIZEADD") == 0) {
				key.func = Hotkey::HAIR_SIZEADD;
			}
			else if (strcmp(token,"HAIR_SIZESET") == 0) {
				key.func = Hotkey::HAIR_SIZESET;
			}
			else if (strcmp(token,"FACEDETAILS_ADDGLASSES") == 0) {
				key.func = Hotkey::FACEDETAILS_ADDGLASSES;
			}
			else if (strcmp(token,"FACEDETAILS_SETGLASSES") == 0) {
				key.func = Hotkey::FACEDETAILS_SETGLASSES;
			}
			else if (strcmp(token,"BODYCOLOR_ADDTAN") == 0) {
				key.func = Hotkey::BODYCOLOR_ADDTAN;
			}
			else if (strcmp(token,"BODYCOLOR_SETTANS") == 0) {
				key.func = Hotkey::BODYCOLOR_SETTAN;
			}
			else if(strcmp(token, "ZOOM_ADD") == 0) {
				key.func = Hotkey::ZOOM_ADD;
			}
			else if (strcmp(token,"ZOOM_SET") == 0) {
				key.func = Hotkey::ZOOM_SET;
			}
			else if (strcmp(token,"TILT_ADD") == 0) {
				key.func = Hotkey::TILT_ADD;
			}
			else if (strcmp(token,"TILT_SET") == 0) {
				key.func = Hotkey::TILT_SET;
			}
			else if (strcmp(token,"POSE_ADD") == 0) {
				key.func = Hotkey::POSE_ADD;
			}
			else if (strcmp(token,"POSE_APPLY") == 0) {
				key.func = Hotkey::POSE_APPLY;
			}
			else if(strcmp(token, "NOP") == 0) {
				key.func = Hotkey::NOP;
			}
			else {
				LOGPRIO(Logger::Priority::WARN) << "Unknown Function \"" << token << "\" in config file\n";
			}
		}
		else if(StartsWith(token, "PARAM:")) {
			token += 6;
			if(StartsWith(token, "INT:")) {
				token += 4;
				key.iparam = atoi(token);
			}
			else if(StartsWith(token, "FLOAT:")) {
				token += 6;
				key.fparam = (float)atof(token);
			}
			else {
				LOGPRIO(Logger::Priority::WARN) << "Unknown Param \"" << token << "\" in config file\n";
			}
		}
		else {
			//unrecognized parameter type
			LOGPRIO(Logger::Priority::WARN) << "Unknown Parameter Type \"" << token << "\" in config file\n";
		}
	}
	if (key.key == -1) {
		//no virtual key id given
		LOGPRIO(Logger::Priority::WARN) << "Key Definition without KEYVAL found\n";
		return false;
	}
	else if(key.key <= 0 || key.key > 255) {
		LOGPRIO(Logger::Priority::WARN) << "key with invalid KEYVAL " << key.key << " found.\n";
		return false;
	}
	hotkeys.push_back(key);
	return true;
}

bool Config::InterpretZoom(char* str)
{
	char* number;
	char* minmax;
	minmax = GetToken(str,&number);
	if (minmax == NULL) return false;
	number = GetToken(number,NULL);
	if (number == NULL) return false;
	float f = strtof(number,NULL);
	if (f == 0) return false;
	if (strcmp(minmax,"MAX") == 0) {
		zoomMax = f;
	}
	else if (strcmp(minmax,"MIN") == 0) {
		zoomMin = f;
	}
	else {
		return false;
	}
	return true;
}

bool Config::InterpretDisable(char * str)
{
	char* token = GetToken(str,NULL);
	if(token != NULL) {
		if(strcmp(token, "GENERAL") == 0) {
			disableFlags |= DISABLE_GENERAL;
		}
		else if(strcmp(token, "FACE") == 0) {
			disableFlags |= DISABLE_FACE;
		}
		else if (strcmp(token,"HAIR") == 0) {
			disableFlags |= DISABLE_HAIR;
		}
		else if (strcmp(token,"HAIR_SKIPINVALID") == 0) {
			disableFlags |= DISABLE_HAIR_SKIPINVALID;
		}
		else if (strcmp(token,"FACEDETAILS") == 0) {
			disableFlags |= DISABLE_FACEDETAILS;
		}
		else if (strcmp(token, "BODY_COLOR") == 0) {
			disableFlags |= DISABLE_BODY_COLOR;
		}
		else if(strcmp(token, "LIMITS") == 0) {
			disableFlags |= DISABLE_LIMITS;
		}
		else if (strcmp(token,"PRE_BACKUP") == 0) {
			disableFlags |= DISABLE_PRE_BACKUP;
		}
		else if (strcmp(token,"POST_BACKUP") == 0) {
			disableFlags |= DISABLE_POST_BACKUP;
		}
		else if (strcmp(token,"BACKUP_UNIQUE") == 0) {
			disableFlags |= DISABLE_BACKUP_UNIQUE;
		}
		else if(strcmp(token, "RANDOM_SLOT_MOD") == 0) {
			disableFlags |= DISABLE_RANDOM_SLOT_MOD;
		}
		else if (strcmp(token, "PERSONALITY") == 0) {
			disableFlags |= DISABLE_PERSONALITY;
		}
		else if (strcmp(token, "GENDERLOCKED_PERSONALITY") == 0) {
			disableFlags |= DISABLE_GENDERLOCKED_PERSONALITY;
		}
		else {
			LOGPRIO(Logger::Priority::WARN) << "Unknown Hook " << token << " disabled in config\n";
		}
	}
	return false;
}

bool Config::InterpretLogger(char* str) {
	char* token = GetToken(str,NULL);
	if(token != NULL) {
		if(strcmp(token, "SPAM") == 0) {
			logPrio = Logger::Priority::SPAM;
			g_Logger.SetPriority(logPrio);
		}
		else if(strcmp(token, "INFO") == 0) {
			logPrio = Logger::Priority::INFO;
			g_Logger.SetPriority(logPrio);
		}
		else if (strcmp(token,"WARNING") == 0) {
			logPrio = Logger::Priority::WARN;
			g_Logger.SetPriority(logPrio);
		}
		else if (strcmp(token,"ERROR") == 0) {
			logPrio = Logger::Priority::ERR;
			g_Logger.SetPriority(logPrio);
		}
		else if (strcmp(token,"CRITICAL_ERROR") == 0) {
			logPrio = Logger::Priority::CRIT_ERR;
			g_Logger.SetPriority(logPrio);
		}
		else {
			LOGPRIO(Logger::Priority::WARN) << "Invalid logger verbosity " << token << " in config file\n";
			return false;
		}
	}
	else {
		return false;
	}
	return true;
}

const std::vector<Config::Hotkey>& Config::GetHotkeys() const
{
	return hotkeys;
}

float Config::GetZoomMin() const
{
	return zoomMin;
}
float Config::GetZoomMax() const
{
	return zoomMax;
}

bool Config::IsDisabled(Disable check) const {
	return disableFlags & check;
}