//
//  NSObject_Extension.m
//  plugintest1
//
//  Created by Junichi Motohisa on 2016/01/20.
//  Copyright © 2016年 Hokkaido University. All rights reserved.
//


#import "NSObject_Extension.h"
#import "plugintest1.h"

@implementation NSObject (Xcode_Plugin_Template_Extension)

+ (void)pluginDidLoad:(NSBundle *)plugin
{
    static dispatch_once_t onceToken;
    NSString *currentApplicationName = [[NSBundle mainBundle] infoDictionary][@"CFBundleName"];
    if ([currentApplicationName isEqual:@"Xcode"]) {
        dispatch_once(&onceToken, ^{
            sharedPlugin = [[plugintest1 alloc] initWithBundle:plugin];
        });
    }
}
@end
