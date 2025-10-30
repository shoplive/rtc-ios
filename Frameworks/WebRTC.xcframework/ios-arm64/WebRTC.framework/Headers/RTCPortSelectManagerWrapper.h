//
//  PortSelectManagerWrapper.h
//  all
//
//  Created by SLSLave on 5/9/25.
//


#import <Foundation/Foundation.h>
#import <WebRTC/RTCMacros.h>

NS_ASSUME_NONNULL_BEGIN

// enum 정의를 인터페이스 선언 전에 추가
typedef NS_ENUM(NSInteger, RTCPortSelectType) {
    RTCPortSelectTypeWifi = 0,
    RTCPortSelectTypeCellular = 1,
    RTCPortSelectTypeBoth = 2
};

RTC_OBJC_EXPORT
@interface RTCPortSelectManagerWrapper : NSObject

- (void)hasWifi:(BOOL)hasWifi;

- (void)setForceWifiOnly:(BOOL)value;

- (void)setPortType:(RTCPortSelectType)type;

@end

NS_ASSUME_NONNULL_END
