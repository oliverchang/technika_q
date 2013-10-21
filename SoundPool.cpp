#include <cstdio>
#include <cstdint>
#include <sndfile.h>

#include <vector>

#include "SoundPool.hpp"

bool SoundPool::openal_initialised = false;
ALCdevice *SoundPool::device = nullptr;
ALCcontext *SoundPool::context = nullptr;

SoundPool::SoundPool() : sounds_(MAX_SOUNDS) {
    if (!openal_initialised) {
        init_openal();
    }
}

void SoundPool::setSound(size_t index, const std::string &filename) {
    // generate buffer
    ALuint buffer;
    alGenBuffers(1, &buffer);

    // decode 
    SF_INFO info;
    SNDFILE *file = sf_open(filename.c_str(), SFM_READ, &info);
    if (file == nullptr) {
        printf("error = %s\n", sf_strerror(nullptr));
    }

    std::vector<short> decoded;

    short decode_buf[2048];
    size_t samples_read;
    while ((samples_read = sf_read_short(file, decode_buf, 2048))) {
        decoded.insert(decoded.end(), decode_buf, decode_buf + samples_read);
    }

    // attach to buffer
    alBufferData(
            buffer, 
            info.channels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16,
            &decoded[0],
            sizeof(short) * decoded.size(),
            info.samplerate);

    buffers_[index] = std::make_pair(buffer, sounds_.end());

    sf_close(file);
}

void SoundPool::unloadSound(size_t index) {
    alSourceStop(*buffers_[index].second);
    alDeleteBuffers(1, &buffers_[index].first);
    buffers_.erase(index);
}

void SoundPool::play(size_t index, float volume) {
    // TODO: sounds_.end() could be invalidated?

    auto sound_it = buffers_[index].second;
    for (size_t i = 0; 
            sound_it == sounds_.end() && i < sounds_.size(); 
            ++i) {
        auto it = sounds_.begin();
        ALenum state;
        alGetSourcei(*it, AL_SOURCE_STATE, &state);

        if (state != AL_PLAYING) {
            sound_it = it;
            buffers_[index].second = sound_it;
            alSourcei(*sound_it, AL_BUFFER, buffers_[index].first);

            break;
        }
        move_to_end(it);
    }

    if (sound_it != sounds_.end()) {
        alSourcef(*sound_it, AL_GAIN, volume/100.0f);
        alSourcePlay(*sound_it);
        move_to_end(sound_it);
    }
}

void SoundPool::stop(size_t index) {
    if (buffers_[index].second != sounds_.end()) {
        alSourceStop(*(buffers_[index].second));
    } 
}

void SoundPool::move_to_end(list_iterator it) {
    sounds_.splice(sounds_.end(), sounds_, it);
}

void SoundPool::init_openal() {
    printf("initialising OpenAL\n");

    device = alcOpenDevice(NULL);
    context = alcCreateContext(device, NULL);
    alcMakeContextCurrent(context);

    ALfloat orientation[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };

    alListener3f(AL_POSITION, 0, 0, 1.0f);
    alListener3f(AL_VELOCITY, 0, 0, 0);
    alListenerfv(AL_ORIENTATION, orientation);

    for (auto &source : sounds_) {
        alGenSources(1, &source);
        alSourcef(source, AL_GAIN, 1.0f);
        alSourcef(source, AL_PITCH, 1.0f);
        alSource3f(source, AL_POSITION, 0, 0, 0);
        alSource3f(source, AL_VELOCITY, 0, 0, 0);
        alSourcei(source, AL_LOOPING, AL_FALSE);
    }

    openal_initialised = true;
}
