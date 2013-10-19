#ifndef _SOUND_POOL_HPP_
#define _SOUND_POOL_HPP_

#include <list>
#include <string>
#include <map>
#include <utility>
#include <memory>

#include <SFML/Audio.hpp>

class SoundPool {
public:
    SoundPool();
    void setSound(size_t index, const std::string &filename); 
    void unloadSound(size_t index);
    void play(size_t index, float volume);
    void stop(size_t index);
private:
    static const int MAX_SOUNDS = 256;

    typedef std::list<sf::Sound>::iterator list_iterator;

    // sound index -> (buffer, pointer to sound)
    std::map<size_t, std::pair<std::shared_ptr<sf::SoundBuffer>, 
                               list_iterator>> buffers_;

    // LRU 
    std::list<sf::Sound> sounds_;

    void move_to_end(list_iterator it);
};

#endif
