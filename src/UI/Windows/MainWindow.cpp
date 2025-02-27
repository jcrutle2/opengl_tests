//
// Created by Caleb Rutledge on 2/18/24.
//

#include "../UI.h"

void UI::mainWindow(Scene &s, Camera &c, const std::string &frameRate) {

    if (ImGui::CollapsingHeader("Info")) {
        ImGui::Text("%s", ("Frame Rate: " + frameRate).c_str());
        ImGui::InputFloat3("Camera", &c.cameraPos.x);
    }

    if (ImGui::CollapsingHeader("Boxes")) {
        int boxCount = 0;
        ImGui::BeginChild("Boxes", ImVec2(175, 100), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeY);
        for (auto const &b: s.boxes) {
            if (ImGui::Selectable(b.getName().c_str())) {
                openWindows[getKey("Box", b.getName())] = true;
            }
            boxCount++;
        }
        ImGui::EndChild();

        ImGui::SameLine();
        ImGui::BeginChild("Boxes Options", ImVec2(125, 100),  ImGuiChildFlags_ResizeY);
        if (ImGui::Button("Add Box")) {
            std::string name = "Box" + std::to_string(s.boxes.size() + 1);

            for (int i = 0; i < s.boxes.size(); i++) {
                if (s.boxes[i].getName() == name) {
                    name += "*";
                    i = 0;
                }
            }

            s.boxes.emplace_back(name, glm::vec3(0.0f, 0.0f, 0.0f));
        }
        ImGui::EndChild();
    }

    if (ImGui::CollapsingHeader("Entities")) {
        int modelCount = 0;
        ImGui::BeginChild("Entities", ImVec2(175, 100), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeY);
            for (auto const &m : s.entities) {
                if (ImGui::Selectable(m.getName().c_str())) {
                    openWindows[getKey("Entity", m.getName())] = true;
                }
                modelCount++;
            }
        ImGui::EndChild();

        ImGui::SameLine();
        ImGui::BeginChild("Entity Options", ImVec2(125, 100), ImGuiChildFlags_ResizeY);
            if (ImGui::Button("Add Entity"))
                ImGui::OpenPopup("add_entity_popup");

        if (ImGui::BeginPopup("add_entity_popup"))
        {
            ImGui::Text("%s", (modelPath.substr(filePath.size(), modelPath.size() - filePath.size())).c_str());

            ImGui::BeginChild("File Paths", ImVec2(175, 100), ImGuiChildFlags_ResizeY |ImGuiChildFlags_Border);
            for (const auto& entry : fs::directory_iterator(modelPath)) {
                std::string entryPath = entry.path();
                std::string shortPath = UI::shorten(entryPath);

                if (shortPath[1] != '.') {
                    std::string f = fileExtension(shortPath);
                    if (f != ".jpg" && f != ".png") {
                        if (ImGui::Selectable(shortPath.c_str())) {
                            if (fs::is_directory(entry))
                                modelPath = entry.path();
                            else {
                                std::string newModelName = shortPath.substr(1, shortPath.length() - 1);
                                for (int i = 0; i < s.entities.size(); i++) {
                                    if (s.entities[i].getName() == newModelName) {
                                        newModelName += "*";
                                        i = 0;
                                    }
                                }

                                s.entities.emplace_back(Entity(newModelName, static_cast<std::string>(entry.path())));
                                modelPath = filePath + "/Assets/Models";
                                ImGui::CloseCurrentPopup();
                            }
                        }
                    }
                }
            }
            ImGui::EndChild();

            if(ImGui::Button("Back")) {
                UI::back(modelPath);
            }

            if(ImGui::MenuItem("Close")) {
                modelPath = filePath + "/Assets/Models";
            }

            ImGui::EndPopup();
        }
        ImGui::EndChild();
    }


    if (ImGui::CollapsingHeader("Environment")) {
        ImGui::BeginChild("Lights", ImVec2(175, 100), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeY);
            if (ImGui::Selectable("Direction Light")) {
                openWindows[getKey("Light", "Direction Light")] = true;
            }
            for (int i = 0; i < s.pointLights.size(); i++) {
                std::string n = s.pointLights[i].getName();
                if (ImGui::Selectable(n.c_str())) {
                    openWindows[getKey("Light", n)] = true;
                }
            }
        ImGui::EndChild();

        ImGui::SameLine();
        ImGui::BeginChild("Lights Options", ImVec2(125, 100), ImGuiChildFlags_ResizeY);
            if (ImGui::Button("Add Light")) {
                std::string name = "Light" + std::to_string(s.pointLights.size() + 1);
                s.pointLights.emplace_back(name);
            }
            if (ImGui::Button("Change Skybox")) {
                openChangeSkybox = true;
            }
        ImGui::EndChild();
    }

    if (ImGui::CollapsingHeader("Scene")) {
        std::string bufferKey = "Scene_Buf";
        ImGui::InputText("Name", charBuffers[bufferKey], 64);
        if (ImGui::Button("Save Scene")) {
            std::string savePath = "Assets/Scenes/";
            savePath += (strcmp(charBuffers[bufferKey], "")) ? charBuffers[bufferKey] : "scene";
            savePath += ".bin";
            SceneLoader::storeScene(s, savePath);
        }
        ImGui::SameLine();
        if (ImGui::Button("Load Scene")) {
            ImGui::OpenPopup("add_scene_popup");
        }

        if (ImGui::BeginPopup("add_scene_popup")) {
            ImGui::Text("Load Scene: ");

            ImGui::BeginChild("Load Scene", ImVec2(175, 100), ImGuiChildFlags_ResizeY | ImGuiChildFlags_Border);
            for (const auto &entry: fs::directory_iterator(scenesPath)) {
                std::string entryPath = entry.path();
                std::string shortPath = UI::shorten(entryPath);

                if (shortPath[1] != '.') {
                    std::string f = fileExtension(shortPath);
                    if (f != ".jpg" && f != ".png") {
                        if (ImGui::Selectable(shortPath.c_str())) {
                            std::string getScenePath = "Assets/Scenes";
                            getScenePath += shortPath;
                            s = SceneLoader::getSceneFromDisk(getScenePath);
                            ImGui::CloseCurrentPopup();
                        }
                    }
                }
            }
            ImGui::EndChild();

            ImGui::EndPopup();
        }
    }
}