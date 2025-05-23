/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#import <Foundation/Foundation.h>

#import <WebRTC/RTCVideoCodecInfo.h>
#import <WebRTC/RTCVideoEncoder.h>
#import <WebRTC/RTCMacros.h>


RTC_OBJC_EXPORT
@interface RTC_OBJC_TYPE (RTCVideoEncoderH264) : NSObject <RTC_OBJC_TYPE(RTCVideoEncoder)>


- (instancetype)initWithCodecInfo:(RTC_OBJC_TYPE(RTCVideoCodecInfo) *)codecInfo;
//MARK: - added by hassan v1.0.3
+ (void)setKeyFrameInterval : (int) keyFrameInterval;

//MARK: - added by hassan v1.0.17
+ (void)setKeyFrameIntervalForVTCompressSession: (int) keyFrameInterval;
+ (void)setKeyFrameIntervalDurationForVTCompressSession : (int) keyFrameIntervalDuration;

@end
