#ifndef LEDBLINKER_H
#define LEDBLINKER_H

#include <Arduino.h>

class LedBlinker {
public:
    LedBlinker(int pin) : pin(pin) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
        reset();
    }

    void start(int times, int delayMs) {
        if (active) return; // Prevent overlapping blinks
        blinkTimes = times;
        blinkDelay = delayMs;
        blinkCount = 0;
        active = true;
        lastToggle = millis();
        state = false;
        digitalWrite(pin, LOW);
    }

    void update() {
        if (!active) return;
        if (millis() - lastToggle >= (unsigned long)blinkDelay) {
            lastToggle = millis();
            state = !state;
            digitalWrite(pin, state ? HIGH : LOW);
            if (!state) { // Only count completed on-off cycles
                blinkCount++;
                if (blinkCount >= blinkTimes) {
                    active = false;
                    digitalWrite(pin, LOW);
                }
            }
        }
    }

    bool isActive() const { return active; }

    void reset() {
        active = false;
        state = false;
        blinkCount = 0;
        blinkTimes = 0;
        blinkDelay = 0;
        lastToggle = 0;
        digitalWrite(pin, LOW);
    }

private:
    int pin;
    int blinkTimes = 0;
    int blinkCount = 0;
    int blinkDelay = 0;
    bool active = false;
    bool state = false;
    unsigned long lastToggle = 0;
};

// --- Static GreenBlinker and RedBlinker utility classes ---

class GreenBlinker {
public:
    static void begin(int pin) {
        staticInstance = LedBlinker(pin);
    }
    static void start(int times, int delayMs) {
        staticInstance.start(times, delayMs);
    }
    static void update() {
        staticInstance.update();
    }
    static bool isActive() {
        return staticInstance.isActive();
    }
    static void reset() {
        staticInstance.reset();
    }
private:
    static LedBlinker staticInstance;
};

class RedBlinker {
public:
    static void begin(int pin) {
        staticInstance = LedBlinker(pin);
    }
    static void start(int times, int delayMs) {
        staticInstance.start(times, delayMs);
    }
    static void update() {
        staticInstance.update();
    }
    static bool isActive() {
        return staticInstance.isActive();
    }
    static void reset() {
        staticInstance.reset();
    }
private:
    static LedBlinker staticInstance;
};

#endif // LEDBLINKER_H