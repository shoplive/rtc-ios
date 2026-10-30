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
 * In-app audio mixing extension for RTCPeerConnectionFactory.
 *
 * Lets the host app mix its own PCM (e.g. media playback, sound effects) into
 * the captured microphone uplink without spinning up a custom ADM. The pushed
 * audio is resampled / downmixed to the capture format and added, after
 * WebRTC's audio processing, only to the send stream selected with
 * -setInAppMixTargetSsrc:, so that remote peer hears "mic + in-app" mixed
 * together.
 *
 * Backed by AudioDeviceModule::SetInAppMixingEnabled /
 * AudioDeviceModule::PushInAppMixerData. Only the iOS built-in ADM implements
 * the hooks; other ADMs receive the calls as a no-op.
 *
 * This category also exposes the downlink counterpart: a tap on the decoded
 * remote PCM WebRTC renders to the speaker (see
 * -setPlayoutAudioPCMHandler:), so the host app can pull the received audio
 * and combine it with its own in-app audio.
 */
@interface RTC_OBJC_TYPE (RTCPeerConnectionFactory) (InAppAudio)

/**
 * Enable or disable mixing host-app PCM into the uplink.
 *
 * When disabled (the default) the uplink is mic-only and any buffered in-app
 * PCM is dropped. Safe to call from any thread.
 */
- (void)setInAppAudioMixingEnabled:(BOOL)enabled
    NS_SWIFT_NAME(setInAppAudioMixingEnabled(_:));

/**
 * Push a block of interleaved int16 PCM to be mixed into the uplink.
 *
 * @param data        Interleaved signed 16-bit PCM samples.
 * @param frameCount  Number of frames (samples per channel) in `data`.
 * @param sampleRate  Sample rate of `data` in Hz. May differ from the capture
 *                    rate; the ADM resamples internally. Must be >= 8000 and a
 *                    multiple of 100 (e.g. 44100, 48000); others are dropped.
 * @param channels    Channel count of `data`. >1 is downmixed to mono.
 *
 * Threading: must be called from a single producer thread (the host app's
 * audio / media thread). The resample + downmix happens synchronously on the
 * calling thread; the result is queued lock-free for the realtime audio io
 * thread. Do NOT call concurrently from multiple threads.
 *
 * Backpressure: if the internal queue is full (consumer not draining), the
 * excess tail is dropped. If it underruns, the mic uplink simply continues
 * without the in-app contribution for those samples (never a gap).
 *
 * No-op until mixing is enabled via -setInAppAudioMixingEnabled: and the
 * capture format has been negotiated.
 */
- (void)pushInAppAudioPCM:(const int16_t *)data
               frameCount:(NSUInteger)frameCount
               sampleRate:(NSInteger)sampleRate
                 channels:(NSUInteger)channels
    NS_SWIFT_NAME(pushInAppAudioPCM(_:frameCount:sampleRate:channels:));

/**
 * Select which send stream the queued in-app PCM is mixed into, by its local
 * RTP SSRC.
 *
 * When this factory drives several send streams off one shared ADM (e.g. a
 * composite "viewer" stream and a mic-only "camera" stream), the pushed in-app
 * audio is added ONLY to the stream whose SSRC equals `ssrc`; the others stay
 * mic-only. Pass 0 (the default) to disable mixing entirely.
 *
 * The SSRC is the local sending SSRC of the audio RtpSender (e.g. from the
 * sender's parameters / SDP). Safe to call from any thread; takes effect on the
 * next captured frame.
 */
- (void)setInAppMixTargetSsrc:(uint32_t)ssrc
    NS_SWIFT_NAME(setInAppMixTargetSsrc(_:));

/**
 * Register (or clear with `nil`) a handler that receives the decoded remote
 * (downlink) PCM that WebRTC is about to render to the speaker — i.e. the
 * audio coming from the other side of the call ("시청쪽" / received audio).
 *
 * This is the read-side counterpart to -pushInAppAudioPCM:…, letting the host
 * app grab the received audio and mix it with its own in-app audio.
 *
 * Block parameters:
 *   @param pcm         Interleaved signed 16-bit PCM (mono — `channels` is 1).
 *   @param frameCount  Number of frames (samples per channel) in `pcm`.
 *   @param sampleRate  Sample rate of `pcm` in Hz (the device playout rate).
 *   @param channels    Channel count (currently always 1).
 *
 * Threading / realtime contract (IMPORTANT — differs from
 * -setAudioLevelHandler:): the handler is invoked SYNCHRONOUSLY on WebRTC's
 * realtime audio io thread, NOT the main queue. It MUST be realtime-safe — no
 * blocking, no allocation, no logging, no lock acquisition, no UI work. The
 * `pcm` pointer is only valid for the duration of the call: copy the samples
 * into your own buffer if you need to retain them. Dispatch any heavy /
 * UI work to another queue using a copy. The handler runs under the lock this
 * setter takes, so calling the setter from inside the handler deadlocks. In
 * return, once the setter returns, the previous handler is not running and is
 * never called again.
 *
 * No-op until playout is active. Pass `nil` to remove a previously set
 * handler.
 */
- (void)setPlayoutAudioPCMHandler:
    (nullable void (^)(const int16_t *pcm,
                       NSUInteger frameCount,
                       NSInteger sampleRate,
                       NSUInteger channels))handler
    NS_SWIFT_NAME(setPlayoutAudioPCMHandler(_:));

/**
 * Register (or clear with `nil`) a handler that receives the CAPTURED mic
 * (uplink) PCM right before it enters the send pipeline — the uplink
 * counterpart to -setPlayoutAudioPCMHandler:.
 *
 * The buffer is mic-only BY DESIGN: audio pushed via -pushInAppAudioPCM:… is
 * mixed per-sender after the transport fan-out and never appears here, so the
 * handler's output can be forwarded to a second RTC stack (e.g. LiveKit)
 * without looping the in-app mix back.
 *
 * Block parameters and the threading / realtime contract are identical to
 * -setPlayoutAudioPCMHandler: (synchronous on the realtime audio io thread;
 * copy the samples; no blocking/allocation/logging/locking in the block; never
 * call the setter from inside the block).
 *
 * No-op until capture is active. Pass `nil` to remove a previously set
 * handler.
 */
- (void)setCaptureAudioPCMHandler:
    (nullable void (^)(const int16_t *pcm,
                       NSUInteger frameCount,
                       NSInteger sampleRate,
                       NSUInteger channels))handler
    NS_SWIFT_NAME(setCaptureAudioPCMHandler(_:));

/**
 * Push interleaved int16 PCM to be PLAYED on the device through this
 * factory's ADM audio unit (speaker-side counterpart to -pushInAppAudioPCM:…).
 *
 * Because the audio exits through the ADM's own Voice-Processing unit, the
 * hardware AEC treats it as far-end reference and cancels its speaker→mic
 * pickup — playing the same audio via a separate output stack leaves that
 * pickup uncancelled and it re-enters the uplink as echo/reverb.
 *
 * Threading and sampleRate constraints are identical to -pushInAppAudioPCM:…
 * (single producer thread; internal resample/downmix). Standing latency is
 * capped at ~200ms (oldest dropped). No-op until the audio unit is initialized.
 */
- (void)pushInAppPlayoutPCM:(const int16_t *)data
                 frameCount:(NSUInteger)frameCount
                 sampleRate:(NSInteger)sampleRate
                   channels:(NSUInteger)channels
    NS_SWIFT_NAME(pushInAppPlayoutPCM(_:frameCount:sampleRate:channels:));

@end

NS_ASSUME_NONNULL_END
