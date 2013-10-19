#ifndef _TECHNIKA_SONG_HPP_
#define _TECHNIKA_SONG_HPP_
#include <cstdio>
#include <string>
#include <vector>
#include <utility>

#include "SoundPool.hpp"

class TechnikaSong {
public:
    TechnikaSong(const std::string &filename);
    void play();

    enum CommandType {
        CommandTrackStart = 0,
        CommandSound,
        CommandUnknown,
        CommandBPM
    };

    struct __attribute__((packed)) Command {
        uint32_t position;
        uint8_t type;
        union {
            struct {
                uint32_t _num_commands; // num_commands * 16
                uint32_t num_commands;
            } start_params;

            struct {
                uint16_t index;
                uint8_t volume;
                uint8_t pan;
                uint8_t type;
                uint8_t _unknown;
                uint16_t _unused;
            } sound_params;

            struct {
                float bpm;
                uint32_t _unknown;
            } bpm_params;
        };
    };

    struct __attribute__((packed)) SoundEntry {
        uint16_t index;
        uint8_t _unused;
        char filename[0x40];
    };

    struct __attribute__((packed)) SongInfo {
        uint16_t num_sounds;
        uint16_t num_tracks;
        uint16_t positions_per_measure;
        float initial_bpm;
        uint8_t _unknown[20];
    };

    static_assert(sizeof(Command) == 0xd, "command struct size");
    static_assert(sizeof(SoundEntry) == 0x43, "sound_entry truct size");

private:
    static const int SEPARATE_DIST = 0x3d;
    void load_info(FILE *fp);
    void load_sounds(FILE *fp);
    void load_commands(FILE *fp);

    SongInfo info_;

    std::vector<std::pair<Command, int>> commands_;
    SoundPool soundpool_;
};

#endif
