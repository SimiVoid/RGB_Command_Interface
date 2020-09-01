#include <iostream>
#include <unordered_map>
#include <vector>
#include <any>
#include <string>

#include "ReturnedCodes.h"

typedef std::vector<std::any> CmdParams;

void help();

[[nodiscard]]
std::unordered_map<std::basic_string<char>, CmdParams> parseCmd(int argc, char** argv);

int main(int argc, char** argv) {
    if(argc < 2)  {
        help();
        exit(ReturnedCodes::NoCommand);
    }

    auto cmd = parseCmd(argc, argv);

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

    for(auto i = 1; i < argc; ++i) {
        if (argv[i] == "-C" || argv[i] == "--color") {
            if (argc < i + 3) exit(ReturnedCodes::InvalidCommand);
            commands["-C"] = CmdParams{
                    std::stoi(argv[i + 1]),
                    std::stoi(argv[i + 2]),
                    std::stoi(argv[i + 3]),
            };
        } else if (argv[i] == "-A" || argv[i] == "--alpha") {
            if (argc < i + 1) exit(ReturnedCodes::InvalidCommand);
            commands["-A"] = CmdParams{std::stoi(argv[i + 1])};
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