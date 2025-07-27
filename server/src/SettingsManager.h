#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <ArduinoJson.h>
#include "config.h"
#include "Log.h"
#include "FileManager.h"

class SettingsManager {
public:
    static StaticJsonDocument<SETTINGS_FILE_SIZE> cached_settings;
    
    static void begin();
    static void end();

    static bool load();
    static bool get(JsonObject& settings);
    static String getProperty(String key);
    static bool set(const JsonObject& new_settings);

    static bool save();
    static bool reset();
};

#endif // SETTINGSMANAGER_H