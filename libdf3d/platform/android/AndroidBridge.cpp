#include "df3d_pch.h"

#include "../AppDelegate.h"
#include <base/SystemsMacro.h>
#include <platform/android/FileDataSourceAndroid.h>
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

JavaVM *g_JavaVm = nullptr;
df3d::platform::AppDelegate *g_appDelegate = nullptr;

extern void df3dInitialized();

namespace df3d { namespace platform {

void setupDelegate(df3d::platform::AppDelegate *appDelegate)
{
    g_appDelegate = appDelegate;

    df3d::base::glog << "App delegate was set up" << df3d::base::logmess;
}

void run()
{

}

} }

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    g_JavaVm = vm;

    JNIEnv *env;
    if (vm->GetEnv((void **)(&env), JNI_VERSION_1_6) != JNI_OK)
    {
        df3d::base::glog << "Failed to get environment" << df3d::base::logcritical;
        return -1;
    }

    df3d::base::glog << "JNI_OnLoad success" << df3d::base::logmess;

    df3dInitialized();

    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_setAssetManager(JNIEnv* env, jobject cls, jobject assetManager)
{
    df3d::platform::FileDataSourceAndroid::setAssetManager(AAssetManager_fromJava(env, assetManager));
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_init(JNIEnv* env, jclass cls, jint jscreenWidth, jint jscreenHeight)
{
    df3d::base::glog << "Doing native init" << df3d::base::logmess;

    // Must be initialized by client code!
    if (!g_engineController->initialized())
    {
        if (!g_appDelegate->onAppStarted(jscreenWidth, jscreenHeight))
            df3d::base::glog << "Failed to initialize game code." << df3d::base::logcritical;

        // TODO:
        // Shutdown on failure.
    }
    else
    {
        // TODO:
        // Recreate GL resources.
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onResume(JNIEnv* env, jclass cls)
{
    g_appDelegate->onAppResumed();
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onPause(JNIEnv* env, jclass cls)
{
    g_appDelegate->onAppPaused();
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onDestroy(JNIEnv* env, jclass cls)
{
    df3d::base::glog << "Activity was destroyed" << df3d::base::logmess;
    g_appDelegate->onAppEnded();
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_draw(JNIEnv* env, jclass cls, jdouble dt)
{
    g_appDelegate->onAppUpdate((float)dt);
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onTouchDown(JNIEnv* env, jclass cls, jint pointerId, jfloat x, jfloat y)
{
    df3d::base::MouseButtonEvent ev;
    ev.x = x;
    ev.y = y;
    ev.state = df3d::base::MouseButtonEvent::State::PRESSED;
    ev.button = df3d::base::MouseButtonEvent::Button::LEFT;
    g_appDelegate->onMouseButtonEvent(ev);
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onTouchUp(JNIEnv* env, jclass cls, jint pointerId, jfloat x, jfloat y)
{
    df3d::base::MouseButtonEvent ev;
    ev.x = x;
    ev.y = y;
    ev.state = df3d::base::MouseButtonEvent::State::RELEASED;
    ev.button = df3d::base::MouseButtonEvent::Button::LEFT;
    g_appDelegate->onMouseButtonEvent(ev);
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onTouchMove(JNIEnv* env, jclass cls, jint pointerId, jfloat x, jfloat y)
{
    df3d::base::MouseMotionEvent ev;
    ev.x = x;
    ev.y = y;
    ev.leftPressed = true;
    g_appDelegate->onMouseMotionEvent(ev);
}
