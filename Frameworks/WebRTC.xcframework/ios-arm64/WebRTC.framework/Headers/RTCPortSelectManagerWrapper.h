//
//  PortSelectManagerWrapper.h
//  all
//
//  Created by SLSLave on 5/9/25.
//


#import <Foundation/Foundation.h>
#import <WebRTC/RTCMacros.h>

NS_ASSUME_NONNULL_BEGIN

RTC_OBJC_EXPORT
@interface RTCPortSelectManagerWrapper : NSObject

- (void)hasWifi:(BOOL)hasWifi;

- (void)setForceWifiOnly:(BOOL)value;

@end

NS_ASSUME_NONNULL_END
