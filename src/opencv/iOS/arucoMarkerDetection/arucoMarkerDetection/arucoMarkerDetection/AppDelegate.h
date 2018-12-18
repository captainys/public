//
//  AppDelegate.h
//  arucoMarkerDetection
//
//  Created by Soji Yamakawa on 2017/03/23.
//  Copyright © 2017年 ysflight.com. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <CoreData/CoreData.h>

@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

@property (readonly, strong) NSPersistentContainer *persistentContainer;

- (void)saveContext;


@end

