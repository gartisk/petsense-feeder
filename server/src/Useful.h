#ifndef USEFUL_H
#define USEFUL_H

inline String convertUidToString(const byte* uid, byte size) {
    String sCardID = "";
    for (byte i = 0; i < size; i++) {
        if (i > 0) sCardID += ":";
        sCardID += uid[i] < 0x10 ? "0" : "";
        sCardID += String(uid[i], HEX);
    }
    sCardID.toUpperCase();

    return sCardID;
}

inline bool isDebounced(unsigned long lastPressTime, unsigned long debounceDelay, unsigned long time = millis() ) {
    return (time - lastPressTime) >= debounceDelay;
}

#endif // USEFUL_H