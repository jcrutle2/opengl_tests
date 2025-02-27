//
// Created by Caleb Rutledge on 2/9/24.
//

#include "UI.h"

bool my_tool_active = true;

namespace UI {
    std::unordered_map<std::string, bool> openWindows;
    bool openLoadModel = false;
    bool openLoadScene = false;
    bool openChangeSkybox = false;
    std::string filePath = fs::current_path();
    std::string modelPath = filePath + "/Assets/Models";
    std::string scenesPath = filePath + "/Assets/Scenes";
    std::string skyboxPath = filePath + "/Assets/Skyboxes";
    std::unordered_map<std::string, char[32]> charBuffers;
}