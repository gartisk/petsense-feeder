# 🐾 Petsense Feeder

## 🧰 Materials Required
- **ESP8266** ⚡  
    - [Arduino ESP8266 Package](http://arduino.esp8266.com/stable/package_esp8266com_index.json)
- **RFID RC522** 📡
- **Servo Motor** ⚙️

## 🔌 Plugins
- **MRFC522** by GitCommunity 🏷️
- **LittleFS** 🗂️  
    - [LittleFS Plugin](https://github.com/earlephilhower/arduino-littlefs-upload/)

## 🛠️ Installing LittleFS Plugin

To install the LittleFS plugin, use one of the following commands:

```sh
# Using wget 🐧
wget https://github.com/earlephilhower/arduino-littlefs-upload/releases/download/1.5.4/arduino-littlefs-upload-1.5.4.vsix

# Using curl 🌐
curl -LO https://github.com/earlephilhower/arduino-littlefs-upload/releases/download/1.5.4/arduino-littlefs-upload-1.5.4.vsix
```

Move the downloaded file to the Arduino plugins directory:

```sh
mv arduino-littlefs-upload-1.5.4.vsix ~/.arduino15/plugins/
```

## 🚀 MVP Features
- **WiFi Access Point** 📶
- **Web Server** 🌍  
    - Servo Configuration ⚙️  
    - RFID Registration 🏷️  
    - System Logs 📜
- **RFID Scanner** 🔍
- _(More pawsome features coming soon!)_ 🐕🐈

