#ifndef COMMANDLINE_202208250905
#define COMMANDLINE_202208250905

enum AppMode {
    VisualProminence
};

struct VisualProminenceConfig {
    int windowIndex;
};

struct Configuration {
    AppMode mode;
    union {
        VisualProminenceConfig visualProminenceConfig;
    };
};

Configuration parseCommandLine();

#endif // #ifndef COMMANDLINE_202208250905
