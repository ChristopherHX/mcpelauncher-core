#include <mcpelauncher/app_platform.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <sys/types.h>
#include <sys/statvfs.h>
#include <hybris/dlfcn.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <minecraft/Crypto.h>
#include <minecraft/legacy/UUID.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mcpelauncher/minecraft_version.h>
#include <random>
#ifdef HAS_LIBPNG
#include <png.h>
#endif

const char* LauncherAppPlatform::TAG = "AppPlatform";

bool enablePocketGuis = false;

void LauncherAppPlatform::initVtable(void* lib) {
    void** vt = &((void**) hybris_dlsym(lib, "_ZTV21AppPlatform_android23"))[2];
    void** vta = &((void**) hybris_dlsym(lib, "_ZTV19AppPlatform_android"))[2];
    auto myVtableSize = PatchUtils::getVtableSize(vta);
    Log::trace("AppPlatform", "Vtable size = %u", myVtableSize);

    PatchUtils::VtableReplaceHelper vtr (lib, vt, vta);
    vtr.replace("_ZN19AppPlatform_android25getGraphicsTearingSupportEv", &LauncherAppPlatform::getGraphicsTearingSupport);
    vtr.replace("_ZNK11AppPlatform14useCenteredGUIEv", &LauncherAppPlatform::useCenteredGUI);
    vtr.replace("_ZN19AppPlatform_android16getApplicationIdEv", &LauncherAppPlatform::getApplicationId);
    vtr.replace("_ZNK19AppPlatform_android16getApplicationIdEv", &LauncherAppPlatform::getApplicationId);
    vtr.replace("_ZN19AppPlatform_android11getDeviceIdEv", &LauncherAppPlatform::getDeviceId);
    vtr.replace("_ZN19AppPlatform_android18isFirstSnoopLaunchEv", &LauncherAppPlatform::isFirstSnoopLaunch);
    vtr.replace("_ZN19AppPlatform_android29hasHardwareInformationChangedEv", &LauncherAppPlatform::hasHardwareInformationChanged);
    vtr.replace("_ZN19AppPlatform_android8isTabletEv", &LauncherAppPlatform::isTablet);
    vtr.replace("_ZN11AppPlatform17setFullscreenModeE14FullscreenMode", &LauncherAppPlatform::setFullscreenMode);
    vtr.replace("_ZNK19AppPlatform_android10getEditionEv", &LauncherAppPlatform::getEdition);
    vtr.replace("_ZNK19AppPlatform_android16getBuildPlatformEv", &LauncherAppPlatform::getBuildPlatform);
    vtr.replace("_ZNK19AppPlatform_android17getPlatformStringEv", &LauncherAppPlatform::getPlatformString);
    vtr.replace("_ZNK19AppPlatform_android20getSubPlatformStringEv", &LauncherAppPlatform::getSubPlatformString);
    vtr.replace("_ZNK19AppPlatform_android25getPlatformUIScalingRulesEv", &LauncherAppPlatform::getPlatformUIScalingRules);
    vtr.replace("_ZN19AppPlatform_android14createDeviceIDEv", &LauncherAppPlatform::createDeviceID_old);
    vtr.replace("_ZN11AppPlatform16allowSplitScreenEv", &LauncherAppPlatform::allowSplitScreen);
    vtr.replace("_ZN19AppPlatform_android21calculateHardwareTierEv", &LauncherAppPlatform::calculateHardwareTier);
    vtr.replace("_ZNK11AppPlatform17supportsScriptingEv", &LauncherAppPlatform::supportsScripting);
    vtr.replace("_ZNK19AppPlatform_android17supportsScriptingEv", &LauncherAppPlatform::supportsScripting);
    vtr.replace("_ZNK11AppPlatform16supports3DExportEv", &LauncherAppPlatform::supports3DExport);
    vtr.replace("_ZNK19AppPlatform_android21getPlatformTTSEnabledEv", &LauncherAppPlatform::getPlatformTTSEnabled);
    vtr.replace("_ZN19AppPlatform_android10createUUIDEv", &LauncherAppPlatform::createUUID);
    vtr.replace("_ZNK11AppPlatform10getEditionEv", &LauncherAppPlatform::getEdition);
    vtr.replace("_ZNK19AppPlatform_android16getApplicationIdEv", &LauncherAppPlatform::getApplicationId);
    vtr.replace("_ZNK19AppPlatform_android13getScreenTypeEv", &LauncherAppPlatform::getScreenType);
}

void LauncherAppPlatform::calculateHardwareTier() {
    hardwareTier = 3;
}

mcpe::string LauncherAppPlatform::createUUID() {
    if (!MinecraftVersion::isAtLeast(0, 14, 99)) {
        static std::independent_bits_engine<std::random_device, CHAR_BIT, unsigned char> engine;
        unsigned char rawBytes[16];
        std::generate(rawBytes, rawBytes + 16, std::ref(engine));
        rawBytes[6] = (rawBytes[6] & (unsigned char) 0x0F) | (unsigned char) 0x40;
        rawBytes[8] = (rawBytes[6] & (unsigned char) 0x3F) | (unsigned char) 0x80;
        mcpe::string ret;
        ret.resize(36);
        snprintf((char*) ret.c_str(), 36, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                rawBytes[0], rawBytes[1], rawBytes[2], rawBytes[3],
                rawBytes[4], rawBytes[5], rawBytes[6], rawBytes[7], rawBytes[8], rawBytes[9],
                rawBytes[10], rawBytes[11], rawBytes[12], rawBytes[13], rawBytes[14], rawBytes[15]);
        return ret;
    }
    auto uuid = Crypto::Random::generateUUID();
    if (!MinecraftVersion::isAtLeast(0, 16))
        return ((Legacy::Pre_1_0_4::mce::UUID*) &uuid)->toString();
    return uuid.asString();
}