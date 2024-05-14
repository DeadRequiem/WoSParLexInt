#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

std::string preprocess_script(const std::string &script) {
    std::istringstream stream(script);
    std::string line;
    std::ostringstream processed_script;

    while (std::getline(stream, line)) {
        // Skip lines starting with a semicolon
        if (line.empty() || line[0] == ';') {
            continue;
        }
        processed_script << line << '\n';
    }

    return processed_script.str();
}

int main() {
    std::string script_filename = "Quest.txt"; // Script file name
    std::ifstream script_file(script_filename);

    if (!script_file) {
        std::cerr << "Error: Unable to open script file: " << script_filename << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << script_file.rdbuf();
    std::string script = buffer.str();

    // Preprocess the script
    std::string processed_script = preprocess_script(script);

    try {
        Lexer lexer(processed_script);
        auto tokens = lexer.tokenize();

        Parser parser(tokens);
        auto scenes = parser.parse();

        Interpreter interpreter(scenes);

        // Run the initial scene
        interpreter.run_scene(1);
    } catch (const std::runtime_error &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
