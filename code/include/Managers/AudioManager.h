

#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <vector>
#include <cstdint>
#include <PinConfig.h>

#define SAMPLE_RATE     8000
#define SAMPLE_BUFFER_SIZE 512
#define CLIP_SIZE 4

class AudioManager {
    public:
        static AudioManager& getInstance();
        void clearMic();
        void pollMic();
        void playSound(const std::vector<int32_t>& samples);

        std::vector<int32_t> getAudio();

    private:
        AudioManager();
        ~AudioManager();

        bool recording;

        std::vector<int32_t> audioSamples;
};

#endif