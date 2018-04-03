#include <mcpelauncher/app_platform.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <sys/types.h>
#ifndef __APPLE__
#include <sys/sysinfo.h>
#endif
#include <sys/statvfs.h>
#include <hybris/dlfcn.h>

const char* LauncherAppPlatform::TAG = "AppPlatform";

void** LauncherAppPlatform::myVtable = nullptr;
bool enablePocketGuis = false;

LauncherAppPlatform::LauncherAppPlatform() : AppPlatform() {
    this->vtable = myVtable;
    dataDir = PathHelper::getPrimaryDataDirectory();
    assetsDir = PathHelper::findDataFile("assets/");
    tmpPath = PathHelper::getCacheDirectory();
    internalStorage = dataDir;
    externalStorage = dataDir;
    currentStorage = dataDir;
    userdata = dataDir;
    userdataPathForLevels = dataDir;
    region = "0xdeadbeef";
}

void LauncherAppPlatform::replaceVtableEntry(void* lib, void** vtable, const char* sym, void* nw) {
    void* sm = hybris_dlsym(lib, sym);
    for (int i = 0; ; i++) {
        if (vtable[i] == nullptr)
            break;
        if (vtable[i] == sm) {
            myVtable[i] = nw;
            return;
        }
    }
}

void LauncherAppPlatform::initVtable(void* lib) {
    void** vt = AppPlatform::myVtable;
    void** vta = &((void**) hybris_dlsym(lib, "_ZTV19AppPlatform_android"))[2];
    // get vtable size
    int size;
    for (size = 2; ; size++) {
        if (vt[size] == nullptr)
            break;
    }
    Log::trace("AppPlatform", "Vtable size = %i", size);

    myVtable = (void**) ::operator new(size * sizeof(void*));
    memcpy(&myVtable[0], &vt[2], (size - 2) * sizeof(void*));

    replaceVtableEntry(lib, vta, "_ZNK19AppPlatform_android10getDataUrlEv", &LauncherAppPlatform::getDataUrl);
    replaceVtableEntry(lib, vta, "_ZNK19AppPlatform_android14getUserDataUrlEv", &LauncherAppPlatform::getUserDataUrl);
    replaceVtableEntry(lib, vta, "_ZN19AppPlatform_android14getPackagePathEv", &LauncherAppPlatform::getPackagePath);
    replaceVtableEntry(lib, vta, "_ZN11AppPlatform16hideMousePointerEv", &LauncherAppPlatform::hideMousePointer);
    replaceVtableEntry(lib, vta, "_ZN11AppPlatform16showMousePointerEv", &LauncherAppPlatform::showMousePointer);
    replaceVtableEntry(lib, vta, "_ZN19AppPlatform_android11swapBuffersEv", &LauncherAppPlatform::swapBuffers);
    replaceVtableEntry(lib, vta, "_ZNK19AppPlatform_android15getSystemRegionEv", &LauncherAppPlatform::getSystemRegion);
    replaceVtableEntry(lib, vta, "_ZN19AppPlatform_android25getGraphicsTearingSupportEv", &LauncherAppPlatform::getGraphicsTearingSupport);
    replaceVtableEntry(lib, vta, "_ZN19AppPlatform_android9pickImageER20ImagePickingCallback", &LauncherAppPlatform::pickImage);
    replaceVtableEntry(lib, vta, "_ZN11AppPlatform8pickFileER18FilePickerSettings", &LauncherAppPlatform::pickFile);
    replaceVtableEntry(lib, vta, "_ZNK11AppPlatform19supportsFilePickingEv", &LauncherAppPlatform::supportsFilePicking);
    replaceVtableEntry(lib, vta, "_ZN19AppPlatform_android22getExternalStoragePathEv", &LauncherAppPlatform::getExternalStoragePath);
    replaceVtableEntry(lib, vta, "_ZN19AppPlatform_android22getInternalStoragePathEv", &LauncherAppPlatform::getInternalStoragePath);
    replaceVtableEntry(lib, vta, "_ZN19AppPlatform_android21getCurrentStoragePathEv", &LauncherAppPlatform::getCurrentStoragePath);
    replaceVtableEntry(lib, vta, "_ZN19AppPlatform_android15getUserdataPathEv", &LauncherAppPlatform::getUserdataPath);
    replaceVtableEntry(lib, vta, "_ZN19AppPlatform_android24getUserdataPathForLevelsEv", &LauncherAppPlatform::getUserdataPathForLevels);
    replaceVtableEntry(lib, vta, "_ZN11AppPlatform20getAssetFileFullPathERKSs", &LauncherAppPlatform::getAssetFileFullPath);
    replaceVtableEntry(lib, vta, "_ZNK11AppPlatform14useCenteredGUIEv", &LauncherAppPlatform::useCenteredGUI);
    replaceVtableEntry(lib, vta, "_ZN19AppPlatform_android16getApplicationIdEv", &LauncherAppPlatform::getApplicationId);
    replaceVtableEntry(lib, vta, "_ZN19AppPlatform_android25_updateUsedMemorySnapshotEv", &LauncherAppPlatform::_updateUsedMemorySnapshot);
    replaceVtableEntry(lib, vta, "_ZN19AppPlatform_android30_updateAvailableMemorySnapshotEv", &LauncherAppPlatform::_updateAvailableMemorySnapshot);
    replaceVtableEntry(lib, vta, "_ZN19AppPlatform_android26_updateTotalMemorySnapshotEv", &LauncherAppPlatform::_updateTotalMemorySnapshot);
    replaceVtableEntry(lib, vta, "_ZN19AppPlatform_android11getDeviceIdEv", &LauncherAppPlatform::getDeviceId);
    replaceVtableEntry(lib, vta, "_ZN19AppPlatform_android18isFirstSnoopLaunchEv", &LauncherAppPlatform::isFirstSnoopLaunch);
    replaceVtableEntry(lib, vta, "_ZN19AppPlatform_android29hasHardwareInformationChangedEv", &LauncherAppPlatform::hasHardwareInformationChanged);
    replaceVtableEntry(lib, vta, "_ZN19AppPlatform_android8isTabletEv", &LauncherAppPlatform::isTablet);
    replaceVtableEntry(lib, vta, "_ZN11AppPlatform17setFullscreenModeE14FullscreenMode", &LauncherAppPlatform::setFullscreenMode);
    replaceVtableEntry(lib, vta, "_ZNK19AppPlatform_android10getEditionEv", &LauncherAppPlatform::getEdition);
    replaceVtableEntry(lib, vta, "_ZNK19AppPlatform_android16getBuildPlatformEv", &LauncherAppPlatform::getBuildPlatform);
    replaceVtableEntry(lib, vta, "_ZN19AppPlatform_android31calculateAvailableDiskFreeSpaceERKSs", &LauncherAppPlatform::calculateAvailableDiskFreeSpace);
    replaceVtableEntry(lib, vta, "_ZNK19AppPlatform_android25getPlatformUIScalingRulesEv", &LauncherAppPlatform::getPlatformUIScalingRules);
    replaceVtableEntry(lib, vta, "_ZN19AppPlatform_android19getPlatformTempPathEv", &LauncherAppPlatform::getPlatformTempPath);
    replaceVtableEntry(lib, vta, "_ZN19AppPlatform_android14createDeviceIDEv", &LauncherAppPlatform::createDeviceID_old);
    replaceVtableEntry(lib, vta, "_ZN19AppPlatform_android14createDeviceIDERSs", &LauncherAppPlatform::createDeviceID);
    replaceVtableEntry(lib, vta, "_ZN19AppPlatform_android18queueForMainThreadESt8functionIFvvEE", &LauncherAppPlatform::queueForMainThread);
    replaceVtableEntry(lib, vta, "_ZN19AppPlatform_android35getMultiplayerServiceListToRegisterEv", &LauncherAppPlatform::getMultiplayerServiceListToRegister);
    replaceVtableEntry(lib, vta, "_ZN11AppPlatform16allowSplitScreenEv", &LauncherAppPlatform::allowSplitScreen);
}

long long LauncherAppPlatform::calculateAvailableDiskFreeSpace() {
    struct statvfs buf;
    statvfs(dataDir.c_str(), &buf);
    return (long long int) buf.f_bsize * buf.f_bfree;
}

void LauncherAppPlatform::_updateUsedMemorySnapshot() {
    FILE* file = fopen("/proc/self/statm", "r");
    if (file == nullptr)
        return;
    int pageSize = getpagesize();
    long long pageCount = 0L;
    fscanf(file, "%lld", &pageCount);
    fclose(file);
    usedMemory = pageCount * pageSize;
}

void LauncherAppPlatform::_updateAvailableMemorySnapshot() {
#ifndef __APPLE__
    struct sysinfo memInfo;
    sysinfo (&memInfo);
    long long total = memInfo.freeram;
    total += memInfo.freeswap;
    total *= memInfo.mem_unit;
    availableMemory = total;
#endif
}

void LauncherAppPlatform::_updateTotalMemorySnapshot() {
#ifndef __APPLE__
    struct sysinfo memInfo;
    sysinfo (&memInfo);
    long long total = memInfo.totalram;
    total += memInfo.totalswap;
    total *= memInfo.mem_unit;
    totalMemory = total;
#endif
}
