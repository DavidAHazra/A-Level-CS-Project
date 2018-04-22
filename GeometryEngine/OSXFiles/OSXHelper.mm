#include "OSXHelper.hpp"
#import <Foundation/Foundation.h>

std::string OSXHelpers::get_resource_path(void){
    @autoreleasepool {
        std::string rpath;
        NSBundle* bundle = [NSBundle mainBundle];
        
        if (bundle == nil) {
            NSLog(@"Bundle is invalid (.mm returned bundle == nil)");

        } else {
            NSString* path = [bundle resourcePath];
            rpath = [path UTF8String] + std::string("/");
        }
        
        return rpath;
    }
}
