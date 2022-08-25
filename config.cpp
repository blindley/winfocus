
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>
#undef min
#undef max

#include "config.h"
#include <exception>
#include <string>
#include <vector>
#include <stdio.h>

static void usage() {
    char const* usageString =
R"DELIM(
usage: winfocus <mode> [<args>]
       winfocus --help <mode>

Modes:
    vp          Visual Prominence Mode
)DELIM";

    MessageBoxA(NULL, usageString, "winfocus", MB_OK);
}

static void helpModeVp() {
    char const* usageString =
R"DELIM(
usage: winfocus vp <windowIndex>

    Enumerate (large) visible windows from left to right. Set foreground
    window to window at position <windowIndex>, zero based.
)DELIM";

    MessageBoxA(NULL, usageString, "winfocus", MB_OK);
}

static void helpMode(std::wstring mode) {
    if (mode == L"vp") {
        helpModeVp();
    }
}

enum ParseStep {
    ParseStart,
    ParseDone,
    _ParseStep
};

static bool startsWith(std::wstring const& str, std::wstring const& prefix) {
    if (prefix.size() > str.size())
        return false;
    for (size_t i = 0; i < prefix.size(); i++) {
        if (str[i] != prefix[i])
            return false;
    }
    return true;
}

static VisualProminenceConfig parseVpCommandLine(size_t* pArgIndex, std::vector<std::wstring> const& args) {
    if (*pArgIndex >= args.size()) {
        helpModeVp();
        std::exit(1);
    }

    int windowIndex = -1;
    try {
        windowIndex = std::stoi(args[(*pArgIndex)++]);
    } catch (std::exception const& _) {
        helpModeVp();
        std::exit(1);
    }

    VisualProminenceConfig cfg = {};
    cfg.windowIndex = windowIndex;

    return cfg;
}

Configuration parseCommandLine() {
    Configuration result;

    LPWSTR commandLine = GetCommandLineW();
    int argc = -1;
    LPWSTR* argv = CommandLineToArgvW(commandLine, &argc);
    std::vector<std::wstring> args(argv, argv + argc);

    if (argc < 2) {
        usage();
        std::exit(1);
    }

    ParseStep parseStep = ParseStart;

    size_t argIndex = 1;
    while (argIndex < args.size()) {
        switch (parseStep) {
            case ParseStart: {
                if (args[argIndex] == L"--help") {
                    if (argIndex + 1 == args.size()) {
                        usage();
                        std::exit(0);
                    } else {
                        helpMode(args[argIndex + 1]);
                        std::exit(0);
                    }
                } else if (startsWith(args[argIndex], L"-")) {
                    usage();
                    std::exit(1);
                } else {
                    if (args[argIndex] == L"vp") {
                        result.mode = VisualProminence;
                        argIndex++;
                        result.visualProminenceConfig = parseVpCommandLine(&argIndex, args);
                        parseStep = ParseDone;
                    } else {
                        usage();
                        std::exit(1);
                    }
                }
            } break;

            case ParseDone: {
                usage();
                std::exit(1);
            } break;
        }
    }

    return result;
}
