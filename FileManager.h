#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <LittleFS.h>
#include "Log.h"

class FileManager {
public:

    static void begin() {
        if (!LittleFS.begin()) {
            LOG_ERROR("FileManager", __FUNCTION__, "LittleFS mount failed!");
            return;
        }
        LOG_DEBUG("FileManager", __FUNCTION__, "LittleFS mounted successfully.");
    }
    
    static void end() {
        LittleFS.end();
        LOG_DEBUG("FileManager", __FUNCTION__, "LittleFS unmounted.");
    }

    static bool createFile(const char* path, const char* content) {
        File file = LittleFS.open(path, "w");
        if (!file) {
            return false;
        }
        
        file.print(content);
        file.close();
        return true;
    }

    static File open(const char* path, const char* mode) {
        File file = LittleFS.open(path, mode);
        if (!file) {
            Serial.printf("Failed to open file: %s\n", path);
        }
        return file;
    }

    static String readFile(const char* path) {
        File file = LittleFS.open(path, "r");
        if (!file) {
            return String();
        }
        String content = file.readString();
        file.close();
        return content;
    }

    static bool updateFile(const char* path, const char* newContent) {
        return createFile(path, newContent); // Overwrite the file
    }

    static bool deleteFile(const char* path) {
        return LittleFS.remove(path);
    }

    static bool fileExists(const char* path) {
        return LittleFS.exists(path);
    }   

    static String listFiles() {
        Dir dir = LittleFS.openDir("/");
        String fileList = "Files in LittleFS:\n";
        while (dir.next()) {
            fileList += "File: " + dir.fileName() + " Size: " + dir.fileSize() + "\n";
        }
        LOG_DEBUG("FileManager", __FUNCTION__, fileList.c_str());

        return fileList;
    }
    
};

#endif // FILEMANAGER_H