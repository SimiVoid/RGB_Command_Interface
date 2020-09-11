#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <any>
#include <string>

#ifdef __unix__

constexpr bool isWindows = false;

#elif defined(_WIN32) || defined(WIN32)
#include <Windows.h>

constexpr bool isWindows = true;

#endif

#include "ReturnedCodes.h"

typedef std::vector<std::any> CmdParams;

void help();
void send(const std::unordered_map<std::basic_string<char>, CmdParams>& cmd);

[[nodiscard]]
std::unordered_map<std::basic_string<char>, CmdParams> parseCmd(int argc, char** argv);

int main(int argc, char** argv) {
    if(argc < 2)  {
        help();
        exit(ReturnedCodes::NoCommand);
    }

    auto cmd = parseCmd(argc, argv);

    send(cmd);

    return 0;
}

void help() {
    std::cout << "RGB Command Interface\n\n"
                 "Using with arduino and program form github:\n"
                 "\n" //TODO: Link
                 "Commands:\n"
                 "-C, --color       Three parameters -> red green blue\n"
                 "-A, --alpha       One parameter -> alpha\n"
                 "-B, --blink       Blink delay in miliseconds\n"
                 "-D, --device      Device port name (without it auto devices search)\n"
                 "-S, --spectrum    RGB Spectrum\n\n"
                 "--on              Turn on leds\n"
                 "--off             Turn off leds\n";
}

std::unordered_map<std::basic_string<char>, CmdParams> parseCmd(int argc, char** argv) {
    std::unordered_map<std::basic_string<char>, CmdParams> commands;

    auto checkCorrectColorValue = [](int colorValue) {
        if(colorValue > 255)
            return 255;
        else if(colorValue < 0)
            return 0;

        return colorValue;
    };

    for(auto i = 1; i < argc; ++i) {
        if (argv[i] == "-C" || argv[i] == "--color") {
            if (argc < i + 3) exit(ReturnedCodes::InvalidCommand);
            try {
                commands["-C"] = CmdParams{
                    checkCorrectColorValue(std::stoi(argv[i + 1])),
                    checkCorrectColorValue(std::stoi(argv[i + 2])),
                    checkCorrectColorValue(std::stoi(argv[i + 3]))
                };
            } catch (const std::exception& exception) {
                std::cerr << exception.what() << "\n";
                exit(ReturnedCodes::InvalidCommand);
            }

        } else if (argv[i] == "-A" || argv[i] == "--alpha") {
            if (argc < i + 1) exit(ReturnedCodes::InvalidCommand);
            try {
                commands["-A"] = CmdParams{checkCorrectColorValue(std::stoi(argv[i + 1]))};
            } catch(const std::exception& exception) {
                std::cerr << exception.what() << "\n";
                exit(ReturnedCodes::InvalidCommand);
            }
        } else if ((argv[i] == "-S" || argv[i] == "--spectrum") && commands.find("-B") == commands.end()) {
            commands["-S"] = CmdParams{};
        } else if ((argv[i] == "-B" || argv[i] == "--blink") && commands.find("-S") == commands.end()) {
            if (argc < i + 1) exit(ReturnedCodes::InvalidCommand);
            commands["-B"] = CmdParams{std::stoi(argv[i+1])};
        } else if (argv[i] == "-D" || argv[i] == "--device") {
            if (argc < i + 1) exit(ReturnedCodes::InvalidCommand);
            commands["-D"] = CmdParams{argv[i+1]};
        } else if (argv[i] == "--on") {
            commands["-M"] = CmdParams{true};
        } else if (argv[i] == "--off") {
            commands["-M"] = CmdParams{false};
        }
    }

    if(commands.find("-M") != commands.end() && !std::any_cast<bool>(commands["-M"][0])) {
        commands.clear();
        commands["-M"] = CmdParams{false};
    } else {
        commands["-M"] = CmdParams{true};
    }

    return commands;
}

void send(std::unordered_map<std::basic_string<char>, CmdParams>& cmd) {
    if(isWindows) {

    }
    else {
        std::ofstream file("/dev/ttyACM5");

        if(file.good() && file.is_open()) {
            if(cmd["-M"] == CmdParams{false})
                file << "--off\n";
            else {
                file << "--on\n";
                for(auto& param : cmd) {
                    if(param.first == "-M") continue;

                    file << param.first;
                    for(auto value : param.second)
                        file << " " << std::any_cast<int>(value);

                    file << "\n";
                }
            }
        }

        file.close();
    }
}
