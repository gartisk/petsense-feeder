#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <ArduinoJson.h>
#include "config.h"
#include "Log.h"
#include "FileManager.h"

class SettingsManager {
public:
    static StaticJsonDocument<SETTINGS_FILE_SIZE> cached_settings;
    
    // begin
    static void begin() {
        FileManager::begin();
        static SettingsManager instance;
        instance.load();
    }

    // end
    static void end() {
        FileManager::end();
        LOG_DEBUG("SettingsManager", __FUNCTION__, "Settings manager ended.");
    }

    // load
    static bool load() {
        LOG_INFO(String(SETTINGS_FILE_PATH));
        
        File file = FileManager::open(SETTINGS_FILE_PATH, "r");
        if (!file) {
            LOG_ERROR("SettingsManager", __FUNCTION__, "Failed to open settings file.");
            return false;
        }
        

        String fileContent;
        while (file.available()) {
            fileContent += (char)file.read();
        }
        file.seek(0); // Reset file pointer for deserialization

        DeserializationError error = deserializeJson(cached_settings, file);
        file.close();

        LOG_INFO("Settings file content: " + fileContent);
        
        
        if (error) {
            LOG_ERROR("SettingsManager", __FUNCTION__, error.c_str());
            return false;
        }
        
        LOG_DEBUG("SettingsManager", __FUNCTION__, "Settings loaded successfully.");
        return true;
    }

    // get
    static bool get(JsonObject& settings) {
        settings.set(cached_settings.as<JsonObject>());
        return true;
    }

    // set
    static bool set(const JsonObject& new_settings) {
        for (JsonPair kv : new_settings) {
            cached_settings[kv.key()] = kv.value();
        }
        return true;
    }


    // save
    static bool save() {
        // convert StaticJsonDocument to Char array
        char jsonString[SETTINGS_FILE_SIZE];
        serializeJson(cached_settings, jsonString);

        LOG_INFO(String(jsonString));
        
        FileManager::saveFile(SETTINGS_FILE_PATH, jsonString);
        LOG_INFO("Settings saved successfully.");
        return true;
    }

    // reset
    static bool reset() {
        File file = FileManager::open(SETTINGS_FILE_DEFAULT_PATH, "r");
        if (!file) return false;

        StaticJsonDocument<SETTINGS_FILE_SIZE> doc;
        DeserializationError error = deserializeJson(doc, file);
        file.close();
        if (error) return false;

        JsonObject settings = doc.as<JsonObject>();
        set(settings);
        save();
        return true;
    }
};

#endif // SETTINGSMANAGER_H