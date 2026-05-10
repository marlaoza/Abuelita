#include "Managers/AudioManager.h"
#include "PinConfig.h"
#include <driver/i2s.h>
#include <Arduino.h>


AudioManager& AudioManager::getInstance() {static AudioManager instance; return instance;}

AudioManager::AudioManager(){

    recording = false;

    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = 1024,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0};

    i2s_pin_config_t i2s_mic_pins = {
        .bck_io_num = I2S_BCLK,
        .ws_io_num = I2S_WS,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = MIC_SD};

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &i2s_mic_pins);

    i2s_config_t dac_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = 1024,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0};

    i2s_pin_config_t dac_pins = {
        .bck_io_num = I2S_BCLK,
        .ws_io_num = I2S_WS,
        .data_out_num = DAC_DIN,
        .data_in_num = I2S_PIN_NO_CHANGE};

    i2s_driver_install(I2S_NUM_1, &dac_config, 0, NULL);
    i2s_set_pin(I2S_NUM_1, &dac_pins);
}

void AudioManager::pollMic(){
    if(!recording){
        recording = true;
        audioSamples.clear();
        audioSamples.reserve(SAMPLE_RATE * CLIP_SIZE);
    }

    if (audioSamples.size() >= SAMPLE_RATE * CLIP_SIZE) return;

    int32_t buffer[SAMPLE_BUFFER_SIZE];
    size_t bytes_read = 0;
    i2s_read(I2S_NUM_0, buffer, sizeof(buffer), &bytes_read, portMAX_DELAY);

    int samples_read = bytes_read / sizeof(int32_t);
    audioSamples.insert(audioSamples.end(), buffer, buffer + samples_read);
   
}

std::vector<int32_t> AudioManager::getAudio(){
    std::vector<int32_t> result = std::move(audioSamples);
    clearMic();
    return result;
}

void AudioManager::clearMic(){
    audioSamples.clear();
    audioSamples.shrink_to_fit();
    recording = false;
}

void AudioManager::playSound(const std::vector<int32_t>& samples){
    size_t bytes_written = 0;
    size_t remaining = samples.size() * sizeof(int32_t);
    const uint8_t* ptr = (const uint8_t*)samples.data();

    size_t max = SAMPLE_BUFFER_SIZE * sizeof(int32_t);
    
    while (remaining > 0) {
        size_t chunk = (remaining > max ? max : remaining);
        i2s_write(I2S_NUM_1, ptr, chunk, &bytes_written, portMAX_DELAY);
        ptr += bytes_written;
        remaining -= bytes_written;
    }
}

AudioManager::~AudioManager(){}
