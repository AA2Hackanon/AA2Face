#include "Injections.h"
#include "Logger.h"

INITCOMMONCONTROLSEX g_commonControllsStruct;

void InitCCs(DWORD flags) {
	g_commonControllsStruct.dwSize = sizeof(g_commonControllsStruct);
	g_commonControllsStruct.dwICC = flags;
	InitCommonControlsEx(&g_commonControllsStruct);
}

void InjectionsInit() {
	//hotkeys (this should really be part of InjGeneral.cpp. whatever.)
	const std::vector<Config::Hotkey>& keys = g_config.GetHotkeys();
	if (keys.size() > 0) {
		auto logKey = [](const Config::Hotkey& key) {
			g_Logger << "Key " << key.key << ", context " << key.context << " as " << key.contextKind
				<< ", mods alt|ctrl|shift " << key.alt << "|" << key.ctrl << "|" << key.shift;
		};
		ACCEL* accels = new ACCEL[keys.size()];
		int size = 0;
		Config::Hotkey nokey;
		const Config::Hotkey* lastkey = &nokey; //initialise with invalid key
		for (unsigned int i = 0; i < keys.size(); i++) {
			if(keys[i].key == -1) {
				LOGPRIO(Logger::Priority::WARN) << "hotkey with undefined keyvalue value\n";
			}
			else if(!keys[i].SameKey(*lastkey)) { //only register first of a key/mod combo
				accels[size].key = (WORD)keys[i].key;
				if (keys[i].isVirt) accels[size].fVirt = FVIRTKEY;
				else				 accels[size].fVirt = 0;
				if (keys[i].alt) accels[size].fVirt |= FALT;
				if (keys[i].shift) accels[size].fVirt |= FSHIFT;
				if (keys[i].ctrl) accels[size].fVirt |= FCONTROL;
				accels[size].cmd = i;
				size++;
				g_Logger << Logger::Priority::INFO << "registered hotkey nr. " << size << "(" << i << "):";
				logKey(keys[i]);
				g_Logger << "\n";
			}
			else {
				g_Logger << Logger::Priority::INFO << "skipped hotkey " << i << ": ";
				logKey(keys[i]);
				g_Logger << " in accel generation\n";
			}
			lastkey = &keys[i];
		}
		g_acKeys = CreateAcceleratorTableW(accels,size);
		delete[] accels;
	}
	else {
		LOGPRIO(Logger::Priority::INFO) << "No hotkeys were loaded from config file\n";
	}
}