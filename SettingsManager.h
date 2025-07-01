#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <ArduinoJson.h>
#include "FileManager.h"
#include "config.h"

class SettingsManager {
public:
    static StaticJsonDocument<SETTINGS_FILE_SIZE> cached_settings;


    static bool restore_default() {
        File file = FileManager::open(SETTINGS_DEFAULT_FILE, "r");
        if (!file) return false;

        StaticJsonDocument<SETTINGS_FILE_SIZE> doc;
        DeserializationError error = deserializeJson(doc, file);
        file.close();
        if (error) return false;

        JsonObject settings = doc.as<JsonObject>();

        return create(settings);
    }

    static bool create(const JsonObject& settings) {
        File file = FileManager::open(SETTINGS_FILE, "w");
        if (!file) return false;
        serializeJson(settings, file);
        file.close();

        return true;
    }
    
    static bool get(JsonObject& settings) {
        if ( cached_settings.isNull() ) {
            return refresh(settings);
        }

        settings.set(cached_settings.as<JsonObject>());
        return true;
    }

    static bool refresh(JsonObject& settings) {
        File file = FileManager::open(SETTINGS_FILE, "r");
        if (!file) return false;
        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, file);
        file.close();
        if (error) return false;
        settings.set(doc.as<JsonObject>());
        return true;
    }

    static bool update(const JsonObject& newSettings) {
        cached_settings.set(newSettings);
        return true;
    }

    static bool save(const JsonObject& newSettings) {
        return create(newSettings);
    }

};

#endif // SETTINGSMANAGER_H