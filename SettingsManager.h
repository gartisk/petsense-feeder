#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <ArduinoJson.h>
#include "config.h"
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
        File file = FileManager::open(SETTINGS_FILE_PATH, "r");
        if (!file) {
            LOG_ERROR("SettingsManager", __FUNCTION__, "Failed to open settings file.");
            return false;
        }
        
        DeserializationError error = deserializeJson(cached_settings, file);
        file.close();
        
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
        File file = FileManager::open(SETTINGS_FILE_PATH, "w");
        if (!file) {
            LOG_ERROR("SettingsManager", __FUNCTION__, "Failed to open settings file for writing.");
            return false;
        }

        serializeJson(cached_settings, file);
        file.close();

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