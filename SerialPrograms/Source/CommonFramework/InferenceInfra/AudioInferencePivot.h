/*  Audio Inference Pivot
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_CommonFramework_AudioInferencePivot_H
#define PokemonAutomation_CommonFramework_AudioInferencePivot_H

#include "Common/Cpp/SpinLock.h"
#include "Common/Cpp/PeriodicScheduler.h"
#include "CommonFramework/Inference/StatAccumulator.h"
#include "AudioInferenceCallback.h"

namespace PokemonAutomation{

class AudioFeed;



class AudioInferencePivot final : public PeriodicRunner{
public:
    AudioInferencePivot(CancellableScope& scope, AudioFeed& feed, AsyncDispatcher& dispatcher);
    virtual ~AudioInferencePivot();

    //  If this callback returns true:
    //      1.  Cancel "scope".
    //      2.  Set "set_when_triggered" to the callback.
    //  If the callback throws an exception, "scope" will be canceled with that exception.
    void add_callback(
        Cancellable& scope,
        std::atomic<InferenceCallback*>* set_when_triggered,
        AudioInferenceCallback& callback,
        std::chrono::milliseconds period
    );

    //  Returns the latency stats for the callback. Units are microseconds.
    StatAccumulatorI32 remove_callback(AudioInferenceCallback& callback);

private:
    virtual void run(void* event, bool is_back_to_back) noexcept override;

private:
    struct PeriodicCallback;

    AudioFeed& m_feed;
    SpinLock m_lock;
    std::map<AudioInferenceCallback*, PeriodicCallback> m_map;

    uint64_t m_last_timestamp = ~(uint64_t)0;
};



}
#endif
