/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#import <AVFoundation/AVFoundation.h>
#import <Foundation/Foundation.h>

#import <WebRTC/RTCVideoCapturer.h>
#import <WebRTC/RTCMacros.h>

NS_ASSUME_NONNULL_BEGIN

// shoplive: 캡처 출력 방향 고정 모드. 출력 connection 이 (재)생성될 때마다 다시 적용된다.
typedef NS_ENUM(NSInteger, RTCCameraVideoFixedOrientation) {
  // WebRTC 기본 — 버퍼는 센서 방향 그대로, 기기 방향은 프레임 rotation 메타데이터로 전달.
  RTCCameraVideoFixedOrientationNone = 0,
  // 출력 connection 을 세로로 고정. 버퍼가 이미 회전되어 있으므로 프레임 rotation 은 항상 0.
  RTCCameraVideoFixedOrientationPortrait,
  // 출력 connection 을 가로로 고정(후면 landscapeLeft / 전면 landscapeRight). 프레임 rotation 은 0.
  RTCCameraVideoFixedOrientationLandscape,
};

// Camera capture that implements RTCVideoCapturer. Delivers frames to a
// RTCVideoCapturerDelegate (usually RTCVideoSource).
NS_EXTENSION_UNAVAILABLE_IOS("Camera not available in app extensions.")
RTC_OBJC_EXPORT
@interface RTC_OBJC_TYPE (RTCCameraVideoCapturer) : RTC_OBJC_TYPE(RTCVideoCapturer)

// Capture session that is used for capturing. Valid from initialization to dealloc.
@property(readonly, nonatomic) AVCaptureSession *captureSession;

// Returns list of available capture devices that support video capture.
+ (NSArray<AVCaptureDevice *> *)captureDevices;
// Returns list of formats that are supported by this class for this device.
+ (NSArray<AVCaptureDeviceFormat *> *)supportedFormatsForDevice:
    (AVCaptureDevice *)device;

// Returns the most efficient supported output pixel format for this capturer.
- (FourCharCode)preferredOutputPixelFormat;

// Starts the capture session asynchronously and notifies callback on
// completion. The device will capture video in the format given in the `format`
// parameter. If the pixel format in `format` is supported by the WebRTC
// pipeline, the same pixel format will be used for the output. Otherwise, the
// format returned by `preferredOutputPixelFormat` will be used.
- (void)startCaptureWithDevice:(AVCaptureDevice *)device
                        format:(AVCaptureDeviceFormat *)format
                           fps:(NSInteger)fps
             completionHandler:
                 (nullable void (^)(NSError *_Nullable))completionHandler;

// Stops the capture session asynchronously and notifies callback on completion.
- (void)stopCaptureWithCompletionHandler:
    (nullable void (^)(void))completionHandler;

// Starts the capture session asynchronously.
- (void)startCaptureWithDevice:(AVCaptureDevice *)device
                        format:(AVCaptureDeviceFormat *)format
                           fps:(NSInteger)fps;
// Stops the capture session asynchronously.
- (void)stopCapture;

// shoplive: 캡처 출력 방향 고정(기본 Portrait). 프로세스 전역 값이라 모든 인스턴스가 공유한다.
// connection 재생성 시 함께 적용되며 고정 모드에서는 프레임 rotation 이 0.
@property(nonatomic) RTCCameraVideoFixedOrientation fixedOrientation;

// shoplive: 미러모드(송출 좌우 반전, 기본 YES). 전면 카메라에만 적용되고 후면은 항상 반전하지 않는다.
// 프로세스 전역 값이라 모든 인스턴스가 공유한다. 카메라 전환(connection 재생성) 시 그 카메라 기준으로 재적용.
// 프리뷰 레이어에는 영향 없음.
@property(nonatomic) BOOL captureVideoMirrored;

// 호환용 — fixedOrientation 의 얇은 래퍼 (added by hassan v1.0.4).
- (void)forcePortrait;
- (void)forceLandscape;

@end

NS_ASSUME_NONNULL_END
