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

#import <WebRTC/RTCMacros.h>

NS_ASSUME_NONNULL_BEGIN

/**
 * Turns audio time match on or off.
 *
 * When capture stops while sending (audio session interruption, audio unit
 * restart), the audio RTP timestamp skips over the gap. The next RTCP sender
 * report then stays monotonic, and the receiver keeps audio and video in sync
 * after the gap.
 *
 * Process-wide. Off by default. The value applies to the next audio frame, so
 * it can change at any time.
 */
RTC_EXTERN void RTCSetAudioTimeMatchEnabled(BOOL enabled);

/** Returns the value set by RTCSetAudioTimeMatchEnabled. */
RTC_EXTERN BOOL RTCIsAudioTimeMatchEnabled(void);

NS_ASSUME_NONNULL_END
