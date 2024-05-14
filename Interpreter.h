#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "Parser.h"
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <stdexcept>

class Interpreter {
public:
    Interpreter(const std::vector<Scene> &scenes) {
        for (const auto &scene : scenes) {
            this->scenes[scene.scene_id] = scene;
        }
    }

    void run_scene(int scene_id) {
        while (true) {
            if (scenes.find(scene_id) == scenes.end()) {
                std::cout << "Scene " << scene_id << " not found." << std::endl;
                return;
            }
            if (!execute_commands(scenes[scene_id].commands, scene_id)) {
                break;
            }
        }
    }

private:
    bool execute_commands(const std::vector<Command> &commands, int &current_scene_id) {
        size_t command_pos = 0;
        while (command_pos < commands.size()) {
            const auto &command = commands[command_pos];
            if (command.type == "ACTOR") {
                execute_actor(command);
                command_pos++;
            } else if (command.type == "IF") {
                if (!execute_if(command, commands, current_scene_id, command_pos)) {
                    command_pos++;
                }
            } else if (command.type == "GOTO") {
                if (execute_goto(command, commands, current_scene_id, command_pos)) {
                    return true;  // Scene change detected, stop current scene execution
                } else {
                    command_pos++;
                }
            } else if (command.type == "GIVE") {
                execute_give(command);
                command_pos++;
            } else if (command.type == "TAKE") {
                execute_take(command);
                command_pos++;
            } else if (command.type == "DIALOGUE") {
                execute_dialogue(command);
                command_pos++;
            } else if (command.type == "LABEL") {
                command_pos++;
            } else {
                command_pos++;
            }
        }
        return false;
    }

    void execute_actor(const Command &command) {
        std::cout << "Actor " << command.args[0] << " (" << command.args[1] << ") at ("
                  << command.args[4] << ", " << command.args[5] << ")" << std::endl;
    }

    bool execute_if(const Command &command, const std::vector<Command> &commands, int &current_scene_id, size_t &command_pos) {
        std::string condition = command.args[0];
        if (tokens.find(condition) != tokens.end()) {
            execute_goto({"GOTO", {command.args[1]}}, commands, current_scene_id, command_pos);  // Pass current_scene_id
            return true;
        }
        return false;
    }

    bool execute_goto(const Command &command, const std::vector<Command> &commands, int &current_scene_id, size_t &command_pos) {
        std::string label_or_scene = command.args[0];
        if (label_or_scene.substr(0, 6) == "SCENE ") {
            int new_scene_id = std::stoi(label_or_scene.substr(6));
            std::cout << "Switching to Scene " << new_scene_id << std::endl;
            current_scene_id = new_scene_id;
            return true;
        } else {
            for (size_t i = 0; i < commands.size(); ++i) {
                if (commands[i].type == "LABEL" && commands[i].args[0] == label_or_scene) {
                    command_pos = i;
                    return true;
                }
            }
        }
        return false;
    }

    void execute_give(const Command &command) {
        tokens.insert(command.args[0]);
        std::cout << "Gave " << command.args[0] << std::endl;
    }

    void execute_take(const Command &command) {
        tokens.erase(command.args[0]);
        std::cout << "Took " << command.args[0] << std::endl;
    }

    void execute_dialogue(const Command &command) {
        std::cout << "Actor " << command.args[0] << ": " << command.args[1] << std::endl;
    }

    std::map<int, Scene> scenes;
    std::set<std::string> tokens;
};

#endif // INTERPRETER_H
