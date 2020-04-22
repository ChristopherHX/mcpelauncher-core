#pragma once

#include <mcpelauncher/path_helper.h>

class MinecraftUtils {

private:
    static void setupApi();

public:
    static void workaroundLocaleBug();

    static std::unordered_map<std::string, void*> getLibCSymbols();
    static void* loadLibM();

    static void setupHybris();

    static void* loadMinecraftLib(std::string const& path = PathHelper::findGameFile(std::string("lib/") + getLibraryAbi() + "/libminecraftpe.so"));

    static void* loadFMod();
    static void stubFMod();

    static const char *getLibraryAbi();

    static size_t getLibraryBase(void* handle);

    static void setupGLES2Symbols(void* (*resolver)(const char*));

};
