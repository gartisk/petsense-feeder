#include "SettingsManager.h"


StaticJsonDocument<SETTINGS_FILE_SIZE> SettingsManager::cached_settings;


// begin
void SettingsManager::begin() {
    FileManager::begin();
    LOG_DEBUG("SettingsManager", __FUNCTION__, "Settings manager started.");
    // SettingsManager::load();
}

// end
void SettingsManager::end() {
    FileManager::end();
    LOG_DEBUG("SettingsManager", __FUNCTION__, "Settings manager ended.");
}

// load
bool SettingsManager::load() {
    LOG_INFO(String(SETTINGS_FILE_PATH));
    
    File file = FileManager::open(SETTINGS_FILE_PATH, "r");
    if (!file) {
        LOG_ERROR("SettingsManager", __FUNCTION__, "Failed to open settings file.");
        return false;
    }
    
    // String fileContent;
    // while (file.available()) {
    //     fileContent += (char)file.read();
    // }
    // file.seek(0); // Reset file pointer for deserialization

    DeserializationError error = deserializeJson(SettingsManager::cached_settings, file);
    file.close();

    LOG_INFO("Settings file content: " + SettingsManager::cached_settings.as<String>());
    
    if (error) {
        LOG_ERROR("SettingsManager", __FUNCTION__, error.c_str());
        return false;
    }
    
    LOG_DEBUG("SettingsManager", __FUNCTION__, "Settings loaded successfully.");
    return true;
}

// get
bool SettingsManager::get(JsonObject& settings) {
    settings.set(SettingsManager::cached_settings.as<JsonObject>());
    return true;
}

// set
bool SettingsManager::set(const JsonObject& new_settings) {
    for (JsonPair kv : new_settings) {
        SettingsManager::cached_settings[kv.key()] = kv.value();
    }

    LOG_INFO("Settings updated successfully.");
    LOG_DEBUG("SettingsManager", __FUNCTION__, "Cached settings: " + String(SettingsManager::cached_settings.as<String>()));
    return true;
}


// save
bool SettingsManager::save() {
    // convert StaticJsonDocument to Char array
    char jsonString[SETTINGS_FILE_SIZE];
    serializeJson(SettingsManager::cached_settings, jsonString);

    LOG_INFO(String(jsonString));
    
    FileManager::saveFile(SETTINGS_FILE_PATH, jsonString);
    LOG_INFO("Settings saved successfully.");
    return true;
}

// reset
bool SettingsManager::reset() {
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