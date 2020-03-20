#pragma once

#include <string>
#include <functional>
#include <unistd.h>
#include <sys/param.h>
#include <vector>
#include <mutex>
#include <memory>
#include <log.h>
#include "patch_utils.h"
#include <minecraft/gl.h>
#include <minecraft/AppPlatform.h>
#include <minecraft/Core.h>
#include <minecraft/std/function.h>
#include <minecraft/std/shared_ptr.h>
#include <minecraft/legacy/FilePickerSettings.h>
#include <minecraft/legacy/ImageData.h>
#include <minecraft/legacy/Core.h>

class ImagePickingCallback;
class FilePickerSettings;
class GameWindow;

extern bool enablePocketGuis;

class LauncherAppPlatform : public AppPlatform {

private:
    static const char* TAG;

public:
    static void initVtable(void* lib);

    LauncherAppPlatform();

    bool getGraphicsTearingSupport() {
        return false;
    }

    int getScreenType() {
        if (enablePocketGuis)
            return 1;
        return 0; // Win 10 Ed. GUIs
    }
    bool useCenteredGUI() {
        return (enablePocketGuis ? false : true);
    }
    mcpe::string getApplicationId() {
        Log::trace(TAG, "getApplicationId: com.mojang.minecraftpe");
        return "com.mojang.minecraftpe";
    }
    mcpe::string getDeviceId() {
        Log::trace(TAG, "getDeviceId: linux");
        return "linux";
    }
    bool isFirstSnoopLaunch() {
        Log::trace(TAG, "isFirstSnoopLaunch: true");
        return true;
    }
    bool hasHardwareInformationChanged() {
        Log::trace(TAG, "hasHardwareInformationChanged: false");
        return false;
    }
    bool isTablet() {
        Log::trace(TAG, "isTablet: true");
        return true;
    }
    void setFullscreenMode(int mode) {}
    mcpe::string getEdition() {
        if (enablePocketGuis)
            return "pocket";
        return "win10";
    }
    int getBuildPlatform() const {
        return 1;
    }
    mcpe::string getPlatformString() const {
        return "Linux";
    }
    mcpe::string getSubPlatformString() const {
        return "Linux";
    }
    int getPlatformUIScalingRules() {
        return enablePocketGuis ? 2 : 0;
    }

    mcpe::string createDeviceID_old() {
        return "linux";
    }

    mcpe::string createUUID();

    bool allowSplitScreen() {
        return true;
    }

    void calculateHardwareTier();

    bool supportsScripting() {
        return true;
    }

    bool supports3DExport() {
        return true;
    }

    bool getPlatformTTSEnabled() {
        return false;
    }
};
