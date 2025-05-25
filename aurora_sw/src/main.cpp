#include <vector>
#include <string>

#include <SD.h>
#include <Metro.h>

#include "ui.h"
#include "pattern.h"
#include "lights.h"


using std::vector;

#define SD_CS 17


bool card_inserted = false;

vector<Pattern> patterns;
unsigned int current_pattern_idx = 0;
long last_pattern_update = 0;

const long pattern_update_interval = 20; // ms

const long fps_update_millis = 1000;
long last_fps_update_time = 0;
long frames = 0;


vector<Pattern> get_patterns_from_sd(File dir) {
    vector<Pattern> patterns;

    // Loop through all files.
    while (File entry = dir.openNextFile()) {
        if (!entry.isDirectory()) {
            string filename = entry.name();
            Serial.println(filename.c_str());

            // If it has a lua extension, add it to the list.
            if (filename.substr(filename.length() - 4) == ".lua") {
                string name = filename.substr(0, filename.length() - 4);
                patterns.push_back(Pattern(patterns.size(), name, filename));
            }
        }
        entry.close();
    }

    return patterns;
}


void load_pattern(unsigned int idx) {
    if (idx >= patterns.size()) return;

    Serial.print(F("Loading pattern "));
    Serial.println(patterns[idx].name.c_str());

    // Unload current pattern.
    patterns[current_pattern_idx].unload();

    lights::blank();

    // Load new pattern.
    patterns[idx].load();
    current_pattern_idx = idx;
}



void setup() {
    Serial.begin(115200);
    Serial.println("Starting up");

    ui::init();
    lights::init();

    // Try to initialize the SD card.
    // If inserted, load the pattern list. If not, show the card removed page.
    Serial.print(F("Initializing SD card..."));
    if (SD.begin(SD_CS)) {
        Serial.println(F("OK!"));
        card_inserted = true;
        patterns = get_patterns_from_sd(SD.open("/"));
        ui::populate_root_page(patterns);

        load_pattern(current_pattern_idx);
    } else {
        Serial.println(F("failed!"));
        ui::card_removed();
    }
}


void loop() {
    // Detect SD card insertion/removal.
    // If inserted, load the pattern list. If not, show the card removed page.
    if (SD.mediaPresent() && !card_inserted) {
        card_inserted = true;
        patterns = get_patterns_from_sd(SD.open("/"));
        ui::populate_root_page(patterns);

        load_pattern(current_pattern_idx);
    } else if (!SD.mediaPresent() && card_inserted) {
        card_inserted = false;
        ui::card_removed();

        patterns[current_pattern_idx].unload();
        current_pattern_idx = 0;
        ui::selected_pattern = 0;

        lights::blank();
    }

    ui::update();

    if (ui::selected_pattern != current_pattern_idx) {
        load_pattern(ui::selected_pattern);
    }

    // If pattern_update_interval has passed, update the current pattern.
    // Also update the FPS counter every fps_update_millis.
    if (millis() - last_pattern_update >= pattern_update_interval && current_pattern_idx < patterns.size()) {
        last_pattern_update = millis();

        // long start = millis();
        patterns[current_pattern_idx].update();
        lights::show();
        // Serial.print("Pattern update took: "); Serial.print(millis() - start); Serial.println("ms");

        if (millis() - last_fps_update_time >= fps_update_millis) {
            Serial.print("FPS: "); Serial.println(1000.0 * (double) frames / (double) (millis() - last_fps_update_time));
            last_fps_update_time = millis();
            frames = 0;
        }

        frames++;
    }

    // Check to see if serial data is available. If so, write it to the SD card for the current pattern and reload the pattern.
    if (Serial.available()) {
        // Read first line to see what the command is.
        string command = Serial.readStringUntil('\n').c_str();

        // Read the rest of the data
        string data;
        while (Serial.available()) {
            data += (char) Serial.read();
        }

        Serial.println("Received on serial: ");
        Serial.println(command.c_str());
        Serial.println(data.c_str());

        if (command == "LOAD" || command == "SAVE") {
            lights::blank();
            patterns[current_pattern_idx].upload_code(data.c_str());
        }

        if (command == "SAVE") {
            patterns[current_pattern_idx].save_code(data.c_str());
        }
    }

    delay(1);
}