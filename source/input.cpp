#include "input.hpp"
#include <log.hpp>
#include <fstream>
#include <algorithm>
#include <cctype>

std::vector<std::string> Input::inputButtons;
std::map<std::string, std::string> Input::inputControls;
std::vector<std::string> Input::inputBuffer;
std::unordered_map<std::string, int> Input::keyHeldDuration;
std::unordered_set<Block *> Input::codePressedBlockOpcodes;
Input::Mouse Input::mousePointer;
Sprite *Input::draggingSprite = nullptr;

void Input::applyControls(std::string controlsFilePath) {
    Input::inputControls.clear();

    if (controlsFilePath != "" && Scratch::projectType == ProjectType::UNEMBEDDED) {
        // load controls from file
        std::ifstream file(controlsFilePath);
        if (file.is_open()) {
            Log::log("Loading controls from file: " + controlsFilePath);
            nlohmann::json controlsJson;
            file >> controlsJson;

            // Access the "controls" object specifically
            if (controlsJson.contains("controls")) {
                for (auto &[key, value] : controlsJson["controls"].items()) {
                    if (key.empty() || value.empty()) continue;
                    Input::inputControls[value.get<std::string>()] = key;
                    Log::log("Loaded control: " + key + " -> " + value.get<std::string>());
                }
                file.close();
                return;
            } else {
                Log::logWarning("settings file does not contain controls.");
                file.close();
            }
        } else {
            Log::logWarning("Failed to open controls file: " + controlsFilePath);
        }
    }

    // --- DOMYŚLNE STEROWANIE DLA 4 GRACZY ---

    // Domyślny pojedynczy kontroler / Gracz 1 (P1)
    Input::inputControls["dpadUp"] = "up arrow";
    Input::inputControls["dpadDown"] = "down arrow";
    Input::inputControls["dpadLeft"] = "left arrow";
    Input::inputControls["dpadRight"] = "right arrow";
    Input::inputControls["LeftStickUp"] = "up arrow";
    Input::inputControls["LeftStickDown"] = "down arrow";
    Input::inputControls["LeftStickLeft"] = "left arrow";
    Input::inputControls["LeftStickRight"] = "right arrow";
    Input::inputControls["A"] = "a";
    Input::inputControls["B"] = "b";
    Input::inputControls["X"] = "x";
    Input::inputControls["Y"] = "y";
    Input::inputControls["shoulderL"] = "l";
    Input::inputControls["shoulderR"] = "r";

    // P1 (Prefiks P1_)
    Input::inputControls["P1_dpadUp"] = "up arrow";
    Input::inputControls["P1_dpadDown"] = "down arrow";
    Input::inputControls["P1_dpadLeft"] = "left arrow";
    Input::inputControls["P1_dpadRight"] = "right arrow";
    Input::inputControls["P1_LeftStickUp"] = "up arrow";
    Input::inputControls["P1_LeftStickDown"] = "down arrow";
    Input::inputControls["P1_LeftStickLeft"] = "left arrow";
    Input::inputControls["P1_LeftStickRight"] = "right arrow";
    Input::inputControls["P1_A"] = "a";
    Input::inputControls["P1_B"] = "b";
    Input::inputControls["P1_X"] = "x";
    Input::inputControls["P1_Y"] = "y";

    // P2 (WASD + Spacja + Z)
    Input::inputControls["P2_dpadUp"] = "w";
    Input::inputControls["P2_dpadDown"] = "s";
    Input::inputControls["P2_dpadLeft"] = "a";
    Input::inputControls["P2_dpadRight"] = "d";
    Input::inputControls["P2_LeftStickUp"] = "w";
    Input::inputControls["P2_LeftStickDown"] = "s";
    Input::inputControls["P2_LeftStickLeft"] = "a";
    Input::inputControls["P2_LeftStickRight"] = "d";
    Input::inputControls["P2_A"] = "space";
    Input::inputControls["P2_B"] = "z";
    Input::inputControls["P2_X"] = "c";
    Input::inputControls["P2_Y"] = "v";

    // P3 (IJKL + U + O)
    Input::inputControls["P3_dpadUp"] = "i";
    Input::inputControls["P3_dpadDown"] = "k";
    Input::inputControls["P3_dpadLeft"] = "j";
    Input::inputControls["P3_dpadRight"] = "l";
    Input::inputControls["P3_LeftStickUp"] = "i";
    Input::inputControls["P3_LeftStickDown"] = "k";
    Input::inputControls["P3_LeftStickLeft"] = "j";
    Input::inputControls["P3_LeftStickRight"] = "l";
    Input::inputControls["P3_A"] = "u";
    Input::inputControls["P3_B"] = "o";

    // P4 (Numpad 8546 + 7 + 9)
    Input::inputControls["P4_dpadUp"] = "8";
    Input::inputControls["P4_dpadDown"] = "5";
    Input::inputControls["P4_dpadLeft"] = "4";
    Input::inputControls["P4_dpadRight"] = "6";
    Input::inputControls["P4_LeftStickUp"] = "8";
    Input::inputControls["P4_LeftStickDown"] = "5";
    Input::inputControls["P4_LeftStickLeft"] = "4";
    Input::inputControls["P4_LeftStickRight"] = "6";
    Input::inputControls["P4_A"] = "7";
    Input::inputControls["P4_B"] = "9";
}

void Input::buttonPress(std::string button) {
    if (Input::inputControls.find(button) != Input::inputControls.end()) {
        Input::inputButtons.push_back(Input::inputControls[button]);
    }
}

// Przeciążenie pozwalające na przekazywanie nazwy przycisku i numeru gracza (0, 1, 2, 3)
void Input::buttonPress(std::string button, int playerIndex) {
    std::string playerButton = "P" + std::to_string(playerIndex + 1) + "_" + button;
    if (Input::inputControls.find(playerButton) != Input::inputControls.end()) {
        Input::inputButtons.push_back(Input::inputControls[playerButton]);
    } else if (Input::inputControls.find(button) != Input::inputControls.end()) {
        Input::inputButtons.push_back(Input::inputControls[button]);
    }
}

std::string Input::convertToKey(const Value keyName, const bool uppercaseKeys) {
    if (keyName.isDouble()) {
        if (keyName.asDouble() >= 48 && keyName.asDouble() <= 90) {
            return std::string(1, std::tolower(static_cast<char>(static_cast<int>(keyName.asDouble()))));
        } else if (keyName.asDouble() == 32.0) {
            return "space";
        } else if (keyName.asDouble() == 37.0) {
            return "left arrow";
        } else if (keyName.asDouble() == 38.0) {
            return "up arrow";
        } else if (keyName.asDouble() == 39.0) {
            return "right arrow";
        } else if (keyName.asDouble() == 50.0) {
            return "down arrow";
        }
    }

    std::string key = keyName.asString();

    if (uppercaseKeys) {
        if (key == "SPACE") return "space";
        if (key == "LEFT") return "left arrow";
        if (key == "RIGHT") return "right arrow";
        if (key == "UP") return "up arrow";
        if (key == "DOWN") return "down arrow";
    }

    if (key == "space" || key == "left arrow" || key == "up arrow" || key == "right arrow" || key == "down arrow" || key == "enter" || key == "any") {
        return key;
    }

    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    if (key.length() > 0) {
        key = key.substr(0, 1);
    }

    return key;
}

bool Input::checkSequenceMatch(const std::vector<std::string> sequence) {
    if (inputBuffer.size() >= sequence.size()) {
        std::vector<std::string> slicedBuffer((Input::inputBuffer).end() - sequence.size(), Input::inputBuffer.end());
        for (unsigned int i = 0; i < sequence.size(); i++) {
            if (sequence[i] != slicedBuffer[i]) return false;
        }
        return true;
    }
    return false;
}