#include "SoundPool.hpp"


SoundPool::SoundPool() : sounds_(MAX_SOUNDS) {
}

void SoundPool::setSound(size_t index, const std::string &filename) {
    std::shared_ptr<sf::SoundBuffer> buffer(new sf::SoundBuffer);
    buffer->loadFromFile(filename);

    buffers_[index] = std::make_pair(buffer, sounds_.end());
}

void SoundPool::unloadSound(size_t index) {
    (buffers_[index].second)->stop();
    buffers_.erase(index);
}

void SoundPool::play(size_t index, float volume) {
    // TODO: sounds_.end() could be invalidated?

    auto sound_it = buffers_[index].second;
    for (size_t i = 0; 
            sound_it == sounds_.end() && i < sounds_.size(); 
            ++i) {
        auto it = sounds_.begin();
        if (it->getStatus() != sf::Sound::Playing) {
            sound_it = it;
            buffers_[index].second = sound_it;
            sound_it->setBuffer(*buffers_[index].first);

            break;
        }
        move_to_end(it);
    }

    if (sound_it != sounds_.end()) {
        sound_it->setVolume(volume);
        sound_it->play();
        move_to_end(sound_it);
    }
}

void SoundPool::stop(size_t index) {
    if (buffers_[index].second != sounds_.end()) {
        (buffers_[index].second)->stop();
    } 
}

void SoundPool::move_to_end(list_iterator it) {
    sounds_.splice(sounds_.end(), sounds_, it);
}
