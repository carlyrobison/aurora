#include <vector>
#include <string>

#include <SD.h>
#include <Metro.h>

#include "ui.h"
#include "pattern.h"
#include "lights.h"


using std::vector;

#define SD_CS 17
#define MAX_COMMAND_ELEMENTS 5


bool card_inserted = false;

vector<Pattern> patterns;
unsigned int current_pattern_idx = 0;
long last_pattern_update = 0;

const long pattern_update_interval = 20; // ms

const long fps_update_millis = 1000;
long last_fps_update_time = 0;
long frames = 0;
bool print_fps = true;


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

int splitInPlace(char *str, char **p, int maxSize) {
    int n;
    Serial.println("splitting");

    *p++ = strtok(str, " ");
    Serial.println(*(p-1));
    for (n = 1; NULL != (*p++ = strtok(NULL, " ")); n++) {
        Serial.println(*(p - 1));
        if (maxSize == n)
            break;
    }

    return n;
}

string readDataFromSerial() {
    // Read the rest of the data
    string data;
    while (Serial.available()) {
        data += (char) Serial.read();
    }
    return data;
}


void setup() {
    Serial.begin(115200);
    Serial.println("Starting up");

    ui::init();
    lights::init();

    // Try to initialize the SD card.
    // If inserted, load the pattern list. If not, show the card removed page.
    Serial.print("Initializing SD card...");
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

void processCommand(char **commandElements, int elementCount) {
  char *instruction = commandElements[0];

  if (strcmp(instruction, "list") == 0) {
    Serial.println("Listing patterns:");
    for (unsigned int i = 0; i < patterns.size(); i++) {
      Serial.println(patterns[i].name.c_str());
    }
    return;
  } 
  
  if (strcmp(instruction, "select") == 0) {
    if (elementCount < 2) {
      Serial.println("Missing pattern name argument");
      return;
    }

    for (unsigned int i = 0; i < patterns.size(); i++) {
      if (strcmp(commandElements[1], patterns[i].name.c_str()) == 0) {
        ui::selected_pattern = i;
      }
      Serial.print("Didn't find pattern named ");
      Serial.println(commandElements[1]);
    }
    return;
  }

  if (strcmp(instruction, "fps") == 0) {
    if (elementCount < 2) {
      Serial.println("Missing pattern name argument");
    } else if (strcmp(commandElements[1], "stop") == 0) {
      print_fps = false;
    } else if (strcmp(commandElements[1], "start") == 0) {
      print_fps = true;
    } else {
      Serial.print("Didn't recognize argument to 'fps' command ");
      Serial.println(commandElements[1]);
    }
    return;
  }
  
  if (strcmp(instruction, "load") == 0) {
      lights::blank();
      patterns[current_pattern_idx].upload_code(readDataFromSerial().c_str());
      return;
  }
  
  if (strcmp(instruction, "save") == 0) {
      patterns[current_pattern_idx].save_code(readDataFromSerial().c_str());
      return;
  }
  
  Serial.println("Didn't recognize command");
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
            if (print_fps) {
              Serial.print("FPS: "); Serial.println(1000.0 * (double) frames / (double) (millis() - last_fps_update_time));
            }
            last_fps_update_time = millis();
            frames = 0;
        }

        frames++;
    }

    // Check to see if serial data is available. If so, write it to the SD card for the current pattern and reload the pattern.
    if (Serial.available()) {
        // Read first line to see what the command is.
        char command[33];
        strncpy(command, Serial.readStringUntil('\n').c_str(), 32);

        char *commandElements [MAX_COMMAND_ELEMENTS];
        int elementCount = splitInPlace(command, commandElements, MAX_COMMAND_ELEMENTS);

        Serial.print("Received command on serial: ");
        for (int i = 0; i < elementCount; i++) {
            Serial.print("`");
            Serial.print(commandElements[i]);
            Serial.print("` ");
        }
        Serial.println();
        
        processCommand(commandElements, elementCount);
    }

    delay(1);
}