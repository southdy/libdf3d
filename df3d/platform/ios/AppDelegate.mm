//
//  AppDelegate.mm
//  df3d
//

#import "AppDelegate.h"
#import "GameViewController.h"
#import <df3d/df3d.h>

namespace df3d {

extern void EngineShutdown();

void Application::setTitle(const std::string &title)
{

}

void Application::quit()
{
    DF3D_ASSERT(false);
}

void Application::showCursor(bool)
{

}

}

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    df3d::MemoryManager::init();

    UIScreen* mainScreen = [UIScreen mainScreen];
    CGRect mainScreenSize = mainScreen.bounds;

    self.window = [[UIWindow alloc] initWithFrame:mainScreenSize];
    self.window.backgroundColor = [UIColor blackColor];
    self.window.rootViewController = [[GameViewController new] autorelease];
    [self.window makeKeyAndVisible];

    return YES;
}

- (void)dealloc
{
    [self.window release];
    [super dealloc];
}

- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
    df3d::AppDelegate::getInstance()->onAppWillResignActive();
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
    df3d::AppDelegate::getInstance()->onAppDidEnterBackground();
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
    df3d::AppDelegate::getInstance()->onAppWillEnterForeground();
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
    // FIXME: this is client code!
    application.applicationIconBadgeNumber = 0;
    df3d::AppDelegate::getInstance()->onAppDidBecomeActive();
}

- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
    df3d::AppDelegate::getInstance()->onAppEnded();

    df3d::EngineShutdown();

    df3d::MemoryManager::shutdown();
}

@end
