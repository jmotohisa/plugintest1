//
//  plugintest1.h
//  plugintest1
//
//  Created by Junichi Motohisa on 2016/01/20.
//  Copyright © 2016年 Hokkaido University. All rights reserved.
//

#import <AppKit/AppKit.h>

@class plugintest1;

static plugintest1 *sharedPlugin;

@interface plugintest1 : NSObject

+ (instancetype)sharedPlugin;
- (id)initWithBundle:(NSBundle *)plugin;

@property (nonatomic, strong, readonly) NSBundle* bundle;
@end