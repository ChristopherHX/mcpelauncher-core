#include <mcpelauncher/minecraft_utils.h>
#include <mcpelauncher/patch_utils.h>
#include <mcpelauncher/hybris_utils.h>
#include <mcpelauncher/hook.h>
#include <mcpelauncher/path_helper.h>
#include <minecraft/imported/android_symbols.h>
#include <minecraft/imported/egl_symbols.h>
#include <minecraft/imported/libm_symbols.h>
#include <minecraft/imported/fmod_symbols.h>
#include <minecraft/imported/glesv2_symbols.h>
#include <minecraft/imported/libz_symbols.h>
#include <minecraft/symbols.h>
#include <minecraft/std/string.h>
#include <log.h>
#include <FileUtil.h>
#include <hybris/dlfcn.h>
#include <hybris/hook.h>
#include <stdexcept>
#include <thread>

extern "C" {
#include <hybris/jb/linker.h>
}

void MinecraftUtils::workaroundLocaleBug() {
    setenv("LC_ALL", "C", 1); // HACK: Force set locale to one recognized by MCPE so that the outdated C++ standard library MCPE uses doesn't fail to find one
}

void MinecraftUtils::setMallocZero() {
    hybris_hook("malloc", (void*) (void* (*)(size_t)) [](size_t n) {
        void* ret = malloc(n);
        memset(ret, 0, n);
        return ret;
    });
}

void* MinecraftUtils::loadLibM() {
#ifdef __APPLE__
    void* libmLib = HybrisUtils::loadLibraryOS("libm.dylib", libm_symbols);
#else
    void* libmLib = HybrisUtils::loadLibraryOS("libm.so.6", libm_symbols);
#endif
    if (libmLib == nullptr)
        throw std::runtime_error("Failed to load libm");
    return libmLib;
}

void* MinecraftUtils::loadFMod() {
#ifdef __APPLE__
    void* fmodLib = HybrisUtils::loadLibraryOS(PathHelper::findDataFile("libs/native/libfmod.dylib"), fmod_symbols);
#else
    void* fmodLib = HybrisUtils::loadLibraryOS(PathHelper::findDataFile("libs/native/libfmod.so.9.16"), fmod_symbols);
#endif
    if (fmodLib == nullptr)
        throw std::runtime_error("Failed to load fmod");
    return fmodLib;
}

void MinecraftUtils::stubFMod() {
    HybrisUtils::stubSymbols(fmod_symbols, (void*) (void* (*)()) []() {
        Log::warn("Launcher", "FMod stub called");
        return (void*) nullptr;
    });
}

void MinecraftUtils::setupHybris() {
#ifndef USE_BIONIC_LIBC
    loadLibM();
#endif
    hybris_hook("ANativeWindow_setBuffersGeometry", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called",
                "ANativeWindow_setBuffersGeometry");
    });
    hybris_hook("AAssetManager_open", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called", "AAssetManager_open");
    });
    hybris_hook("AAsset_getLength", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called", "AAsset_getLength");
    });
    hybris_hook("AAsset_getBuffer", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called", "AAsset_getBuffer");
    });
    hybris_hook("AAsset_close", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called", "AAsset_close");
    });
    hybris_hook("AAsset_read", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called", "AAsset_read");
    });
    hybris_hook("AAsset_seek64", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called", "AAsset_seek64");
    });
    hybris_hook("AAsset_getLength64", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called", "AAsset_getLength64");
    });
    hybris_hook("AAsset_getRemainingLength64", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called",
                "AAsset_getRemainingLength64");
    });
    struct Looper {
      int fd;
      int indent;
      void * data;
    };
    static Looper looper;
    hybris_hook("ALooper_pollAll", (void *)+[](  int timeoutMillis,
  int *outFd,
  int *outEvents,
  void **outData) {
      Log::warn("Launcher", "Android stub %s called", "ALooper_pollAll");
      // *outFd = 0;
      // *outEvents = 0;
      // *outData = 0;
      fd_set rfds;
      struct timeval tv;
      int retval;

      /* Watch stdin (fd 0) to see when it has input. */

      FD_ZERO(&rfds);
      FD_SET(looper.fd, &rfds);

      tv.tv_sec = 0;
      tv.tv_usec = 0;

      retval = select(looper.fd + 1, &rfds, NULL, NULL, &tv);
      /* Don't rely on the value of tv now! */

      if (retval == -1)
          perror("select()");
      else if (retval) {
          // printf("Data is available now.\n");
          *outData = looper.data;
          return looper.indent;
          /* FD_ISSET(0, &rfds) will be true. */
      }
      return -3;
    });
    hybris_hook("ANativeActivity_finish", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called", "ANativeActivity_finish");
    });
    hybris_hook("AInputQueue_getEvent", (void *)+[]() -> int32_t {
      Log::warn("Launcher", "Android stub %s called", "AInputQueue_getEvent");
      return -1;      
    });
    hybris_hook("AKeyEvent_getKeyCode", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called", "AKeyEvent_getKeyCode");
    });
    hybris_hook("AInputQueue_preDispatchEvent", (void *)+[]() ->int32_t {
      Log::warn("Launcher", "Android stub %s called",
                "AInputQueue_preDispatchEvent");
      return -1;
    });
    hybris_hook("AInputQueue_finishEvent", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called",
                "AInputQueue_finishEvent");
    });
    hybris_hook("AKeyEvent_getAction", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called", "AKeyEvent_getAction");
    });
    hybris_hook("AMotionEvent_getAxisValue", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called",
                "AMotionEvent_getAxisValue");
    });
    hybris_hook("AKeyEvent_getRepeatCount", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called",
                "AKeyEvent_getRepeatCount");
    });
    hybris_hook("AKeyEvent_getMetaState", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called", "AKeyEvent_getMetaState");
    });
    hybris_hook("AInputEvent_getDeviceId", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called",
                "AInputEvent_getDeviceId");
    });
    hybris_hook("AInputEvent_getType", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called", "AInputEvent_getType");
    });
    hybris_hook("AInputEvent_getSource", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called", "AInputEvent_getSource");
    });
    hybris_hook("AMotionEvent_getAction", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called", "AMotionEvent_getAction");
    });
    hybris_hook("AMotionEvent_getPointerId", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called",
                "AMotionEvent_getPointerId");
    });
    hybris_hook("AMotionEvent_getX", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called", "AMotionEvent_getX");
    });
    hybris_hook("AMotionEvent_getRawX", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called", "AMotionEvent_getRawX");
    });
    hybris_hook("AMotionEvent_getY", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called", "AMotionEvent_getY");
    });
    hybris_hook("AMotionEvent_getRawY", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called", "AMotionEvent_getRawY");
    });
    hybris_hook("AMotionEvent_getPointerCount", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called",
                "AMotionEvent_getPointerCount");
    });
    hybris_hook("AConfiguration_new", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called", "AConfiguration_new");
    });
    hybris_hook("AConfiguration_fromAssetManager", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called",
                "AConfiguration_fromAssetManager");
    });
    hybris_hook("AConfiguration_getLanguage", (void *)+[](class AConfiguration *config, char *outLanguage) {
      Log::warn("Launcher", "Android stub %s called",
                "AConfiguration_getLanguage");
                outLanguage[0] = 'd';
                outLanguage[1] = 'e';
    });
    hybris_hook("AConfiguration_getCountry", (void *)+[](class AConfiguration *config,
  char *outCountry) {
      Log::warn("Launcher", "Android stub %s called",
                "AConfiguration_getCountry");
                outCountry[0] = 'd';
                outCountry[1] = 'e';
    });
    hybris_hook("ALooper_prepare", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called", "ALooper_prepare");
    });
    hybris_hook("ALooper_addFd", (void *)+[](  void *loopere ,
      int fd,
      int ident,
      int events,
      int(* callback)(int fd, int events, void *data),
      void *data) {
      Log::warn("Launcher", "Android stub %s called", "ALooper_addFd");
      looper.fd = fd;
      looper.indent = ident;
      looper.data = data;
      // std::thread([=](){
      //   struct android_poll_source {
      //       // The identifier of this source.  May be LOOPER_ID_MAIN or
      //       // LOOPER_ID_INPUT.
      //       int32_t id;

      //       // The android_app this ident is associated with.
      //       struct android_app* app;

      //       // Function to call to perform the standard processing of data from
      //       // this source.
      //       void (*process)(struct android_app* app, struct android_poll_source* source);
      //   };
      //   while(true) {
      //     // need to sleep while no data => blocks main
      //     ((android_poll_source*)data)->process(((android_poll_source*)data)->app, (android_poll_source*)data);
      //   }
      // }).detach();
      return 1;
    });
    hybris_hook("AInputQueue_detachLooper", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called",
                "AInputQueue_detachLooper");
    });
    hybris_hook("AConfiguration_delete", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called", "AConfiguration_delete");
    });
    hybris_hook("AInputQueue_attachLooper", (void *)+[](  void *queue,
  void *looper,
  int ident,
  void* callback,
  void *data) {
      Log::warn("Launcher", "Android stub %s called",
                "AInputQueue_attachLooper");
      std::thread([=](){
        struct android_poll_source {
            // The identifier of this source.  May be LOOPER_ID_MAIN or
            // LOOPER_ID_INPUT.
            int32_t id;

            // The android_app this ident is associated with.
            struct android_app* app;

            // Function to call to perform the standard processing of data from
            // this source.
            void (*process)(struct android_app* app, struct android_poll_source* source);
        };
        // while(true) {
          ((android_poll_source*)data)->process(((android_poll_source*)data)->app, (android_poll_source*)data);
        //   std::this_thread::sleep_for(std::chrono::seconds(100));
        // }
      }).detach();
    });
    hybris_hook("AAssetManager_openDir", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called", "AAssetManager_openDir");
    });
    hybris_hook("AAssetDir_getNextFileName", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called",
                "AAssetDir_getNextFileName");
    });
    hybris_hook("AAssetDir_close", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called", "AAssetDir_close");
    });
    hybris_hook("AAssetManager_fromJava", (void *)(void (*)())[]() {
      Log::warn("Launcher", "Android stub %s called", "AAssetManager_fromJava");
    });
    // HybrisUtils::stubSymbols(egl_symbols, (void*) +[]() {
    //     Log::warn("Launcher", "EGL stub called");
    //     return 1;
    // });eglChooseConfig
    hybris_hook("eglChooseConfig", (void *)+[]() {
   Log::warn("Launcher", "EGL stub %s called", "eglChooseConfig");
   return 1;
});
        hybris_hook("eglGetError", (void *)(void (*)())[]() {
   Log::warn("Launcher", "EGL stub %s called", "eglGetError");
});
        hybris_hook("eglCreateWindowSurface", (void *)(void (*)())[]() {
   Log::warn("Launcher", "EGL stub %s called", "eglCreateWindowSurface");
});
        hybris_hook("eglGetConfigAttrib", (void *)(void (*)())[]() {
   Log::warn("Launcher", "EGL stub %s called", "eglGetConfigAttrib");
});
        hybris_hook("eglCreateContext", (void *)(void (*)())[]() {
   Log::warn("Launcher", "EGL stub %s called", "eglCreateContext");
});
        hybris_hook("eglDestroySurface", (void *)(void (*)())[]() {
   Log::warn("Launcher", "EGL stub %s called", "eglDestroySurface");
});
        hybris_hook("eglSwapBuffers", (void *)(void (*)())[]() {
   Log::warn("Launcher", "EGL stub %s called", "eglSwapBuffers");
});
        hybris_hook("eglMakeCurrent", (void *)(void (*)())[]() {
   Log::warn("Launcher", "EGL stub %s called", "eglMakeCurrent");
});
        hybris_hook("eglDestroyContext", (void *)(void (*)())[]() {
   Log::warn("Launcher", "EGL stub %s called", "eglDestroyContext");
});
        hybris_hook("eglTerminate", (void *)(void (*)())[]() {
   Log::warn("Launcher", "EGL stub %s called", "eglTerminate");
});
        hybris_hook("eglGetDisplay", (void *)+[]() {
   Log::warn("Launcher", "EGL stub %s called", "eglGetDisplay");
   return 1;
});
        hybris_hook("eglInitialize", (void *)+[](void* display,
 	uint32_t * major,
 	uint32_t * minor) {
   Log::warn("Launcher", "EGL stub %s called", "eglInitialize");
   if(major) {
     *major = 1;
   }
  if(minor) {
     *minor = 4;
   }
   return 1;
});
        hybris_hook("eglQuerySurface", (void *)(void (*)())[]() {
   Log::warn("Launcher", "EGL stub %s called", "eglQuerySurface");
});
        hybris_hook("eglSwapInterval", (void *)(void (*)())[]() {
   Log::warn("Launcher", "EGL stub %s called", "eglSwapInterval");
});
hybris_hook("eglQueryString", (void *)+[](void* display, int32_t name) {
   Log::warn("Launcher", "EGL stub %s called", "eglQueryString");
   switch (name) {
    case 12373:
      return "EGL_KHR_image EGL_KHR_image_base EGL_KHR_image_pixmap EGL_KHR_vg_parent_image EGL_KHR_gl_texture_2D_image EGL_KHR_gl_texture_cubemap_image EGL_KHR_lock_surface";
    case 12372:
      return "1.4";
    case 12371:
      return "Generic";
    default:
      return "Unknown";
   }
});
    HybrisUtils::loadLibraryOS("libz.so.1", libz_symbols);
    HybrisUtils::hookAndroidLog();
    setupHookApi();
    hybris_hook("mcpelauncher_log", (void*) Log::log);
    hybris_hook("mcpelauncher_vlog", (void*) Log::vlog);
    // load stub libraries
#ifdef USE_BIONIC_LIBC
    if (!load_empty_library("ld-android.so") ||
        !hybris_dlopen(PathHelper::findDataFile("libs/hybris/libc.so").c_str(), 0) ||
        !hybris_dlopen(PathHelper::findDataFile("libs/hybris/libm.so").c_str(), 0))
        throw std::runtime_error("Failed to load Android libc.so/libm.so libraries");
#else
    if (!load_empty_library("libc.so") || !load_empty_library("libm.so"))
        throw std::runtime_error("Failed to load stub libraries");
#endif
    if (!load_empty_library("libandroid.so") || !load_empty_library("liblog.so") || !load_empty_library("libEGL.so") || !load_empty_library("libGLESv2.so") || !load_empty_library("libOpenSLES.so") || !load_empty_library("libfmod.so") || !load_empty_library("libGLESv1_CM.so"))
        throw std::runtime_error("Failed to load stub libraries");
    if (!load_empty_library("libmcpelauncher_mod.so"))
        throw std::runtime_error("Failed to load stub libraries");
    load_empty_library("libstdc++.so");
    load_empty_library("libz.so"); // needed for <0.17
}

void MinecraftUtils::setupHookApi() {
    hybris_hook("mcpelauncher_hook", (void*) (void* (*)(void*, void*, void**)) [](void* sym, void* hook, void** orig) {
        Dl_info i;
        if (!hybris_dladdr(sym, &i)) {
            Log::error("Hook", "Failed to resolve hook for symbol %lx", (long unsigned) sym);
            return (void*) nullptr;
        }
        void* handle = hybris_dlopen(i.dli_fname, 0);
        std::string tName = HookManager::translateConstructorName(i.dli_sname);
        auto ret = HookManager::instance.createHook(handle, tName.empty() ? i.dli_sname : tName.c_str(), hook, orig);
        hybris_dlclose(handle);
        HookManager::instance.applyHooks();
        return (void*) ret;
    });

    hybris_hook("mcpelauncher_hook2", (void *) (void *(*)(void *, const char *, void *, void **))
            [](void *lib, const char *sym, void *hook, void **orig) {
                return (void *) HookManager::instance.createHook(lib, sym, hook, orig);
            });
    hybris_hook("mcpelauncher_hook2_add_library", (void *) (void (*)(void*)) [](void* lib) {
        HookManager::instance.addLibrary(lib);
    });
    hybris_hook("mcpelauncher_hook2_remove_library", (void *) (void (*)(void*)) [](void* lib) {
        HookManager::instance.removeLibrary(lib);
    });
    hybris_hook("mcpelauncher_hook2_delete", (void *) (void (*)(void*)) [](void* hook) {
        HookManager::instance.deleteHook((HookManager::HookInstance*) hook);
    });
    hybris_hook("mcpelauncher_hook2_apply", (void *) (void (*)()) []() {
        HookManager::instance.applyHooks();
    });
}

void* MinecraftUtils::loadMinecraftLib(std::string const& path) {
    // load gnustl_shared.so for <0.15.90.8
    std::string gnustlPath = FileUtil::getParent(path) + "/libgnustl_shared.so";
    if (FileUtil::exists(gnustlPath)) {
        hybris_dlopen(gnustlPath.c_str(), RTLD_LAZY);
    }

    void* handle = hybris_dlopen(path.c_str(), RTLD_LAZY);
    if (handle == nullptr)
        throw std::runtime_error(std::string("Failed to load Minecraft: ") + hybris_dlerror());
    HookManager::instance.addLibrary(handle);
    return handle;
}

void MinecraftUtils::setupForHeadless() {
    setupHybris();
    stubFMod();

    hybris_hook("eglGetProcAddress", (void*) (void (*)()) []() {
        Log::warn("Launcher", "EGL stub called");
    });
}

unsigned int MinecraftUtils::getLibraryBase(void *handle) {
    return ((soinfo*) handle)->base;
}

void MinecraftUtils::initSymbolBindings(void* handle) {
    mcpe::string::empty = (mcpe::string*) hybris_dlsym(handle, "_ZN4Util12EMPTY_STRINGE");
    minecraft_symbols_init(handle);
}

void MinecraftUtils::setupGLES2Symbols(void* (*resolver)(const char *)) {
    int i = 0;
    while (true) {
        const char* sym = glesv2_symbols[i];
        if (sym == nullptr)
            break;
        hybris_hook(sym, resolver(sym));
        i++;
    }
}

static void workerPoolDestroy(void* th) {
    Log::trace("Launcher", "WorkerPool-related class destroy %lu", (unsigned long) th);
}
void MinecraftUtils::workaroundShutdownCrash(void* handle) {
    // this is an ugly hack to workaround the close app crashes MCPE causes
    unsigned int patchOff = (unsigned int) hybris_dlsym(handle, "_ZN19AppPlatform_androidD2Ev");
    PatchUtils::patchCallInstruction((void*) patchOff, (void*) &workerPoolDestroy, true);
}
