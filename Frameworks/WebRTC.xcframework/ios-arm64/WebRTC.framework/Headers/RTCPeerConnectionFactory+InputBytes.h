/*
 *  Copyright (c) 2026 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#import <Foundation/Foundation.h>

#import <WebRTC/RTCPeerConnectionFactory.h>

NS_ASSUME_NONNULL_BEGIN

/**
 * Audio input bytes extension for RTCPeerConnectionFactory.
 *
 * Mirrors the AudioLevel extension but exposes the raw PCM byte count per
 * capture callback instead of an RMS dB value. Consumers accumulate the
 * delivered bytes to derive an "input audio bps" metric (e.g. OTel
 * inputAudioBps) without spinning up a second mic client.
 *
 * Backed by AudioDeviceModule::SetAudioInputBytesCallback. Only the iOS
 * built-in ADM implements the hook; other ADMs receive the registration as
 * a no-op.
 */
@interface RTC_OBJC_TYPE (RTCPeerConnectionFactory) (InputBytes)

/**
 * Register (or clear with `nil`) a handler that receives the number of PCM
 * bytes delivered in each capture callback.
 *
 * Payload contract:
 *   - `bytes` = frame_count * sizeof(int16_t), mono 16-bit PCM. Format
 *     assumption is baked into the C++ fire-site; a future shift to
 *     float32 or stereo would silently break consumer-side bps math, so
 *     callers should pin both ends.
 *   - Frequency is approximate, driven by the audio unit buffer size on
 *     iOS. Typically ~100Hz, no rate-limit at the C++ layer.
 *
 * Delivery:
 *   - Invoked on the main queue (the bridge hops via
 *     `dispatch_async(dispatch_get_main_queue(), ...)`), so reading state
 *     or updating UI from inside the handler is safe.
 *   - FIFO. If the main queue is blocked, deliveries queue up.
 *   - No callback fires while the ADM is not actively capturing. Pair
 *     with `-[RTCPeerConnectionFactory setMeteringActive:]` if you need
 *     deliveries outside an active peerConnection.
 *   - One final invocation may still fire after `nil` is passed, due to
 *     an in-flight main-queue dispatch enqueued just before the clear.
 *     Handlers must tolerate one trailing call.
 */
- (void)setAudioInputBytesHandler:(nullable void (^)(NSUInteger bytes))handler
    NS_SWIFT_NAME(setAudioInputBytesHandler(_:));

@end

NS_ASSUME_NONNULL_END
