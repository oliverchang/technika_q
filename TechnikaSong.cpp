#include "TechnikaSong.hpp"

#include <SDL.h>

TechnikaSong::TechnikaSong(const std::string &filename) {
    FILE *fp = fopen(filename.c_str(), "r");

    load_info(fp);
    load_sounds(fp);
    load_commands(fp);

    fclose(fp);
}

void TechnikaSong::play() {
    float bpm = info_.initial_bpm;
    double multiplier = 1000.0 * (4.0 * 60.0 / bpm) / info_.positions_per_measure;

    uint32_t start = SDL_GetTicks();

    // The last position and elapsed time when the BPM was changed
    uint32_t last_position = 0;
    uint32_t last_elapsed = 0;

    for (auto &command : commands_) {
        int track = command.second;
        Command &cmd = command.first;

        uint32_t deadline = last_elapsed + (cmd.position - last_position) * multiplier;

        uint32_t elapsed;
        while (true) {
            elapsed = SDL_GetTicks() - start;
            if (elapsed >= deadline) {
                break;
            } else {
                // printf("sleeping for %u\n", deadline - elapsed);
                SDL_Delay(deadline - elapsed);
            }
        }

        switch (cmd.type) {
            case CommandSound:
                if (cmd.sound_params.index > 0) {
                    soundpool_.play(cmd.sound_params.index,
                                    static_cast<float>(cmd.sound_params.volume)/127.0f * 100.0f);
                }
                printf("track = %d time = %lf sound = %x type = %x unknown = %x unused? = %x\n",
                        track,
                        cmd.position * multiplier,
                        cmd.sound_params.index,
                        cmd.sound_params.type,
                        cmd.sound_params._unknown,
                        cmd.sound_params._unused);
                break;
            case CommandBPM:
                bpm = cmd.bpm_params.bpm;
                multiplier = 1000.0 * (4.0 * 60.0 / bpm) / info_.positions_per_measure;
                last_position = cmd.position;
                last_elapsed = elapsed;

                printf("changing bpm to %f\n", bpm);
                break;
            default:
                printf("unknown command %x\n", cmd.type);
                break;
        }

    }
}

void TechnikaSong::load_info(FILE *fp) {
    fseek(fp, 4, SEEK_SET);

    uint32_t info_offset;
    fread(&info_offset, sizeof(info_offset), 1, fp);

    fseek(fp, info_offset, SEEK_SET);
    fread(&info_, sizeof(info_), 1, fp);

    printf("num sounds = %d\n", info_.num_sounds);
    printf("num tracks = %d\n", info_.num_tracks);
    printf("positions per measure = %d\n", info_.positions_per_measure);
    printf("initial bpm = %f\n", info_.initial_bpm);
}

void TechnikaSong::load_sounds(FILE *fp) {
    fseek(fp, 8, SEEK_SET);
    for (int i = 0; i < info_.num_sounds; ++i) {
        SoundEntry entry;
        fread(&entry, sizeof(entry), 1, fp);
        soundpool_.setSound(entry.index, entry.filename);
        printf("loaded %d %s\n", entry.index, entry.filename);
    }
}

void TechnikaSong::load_commands(FILE *fp) {
    fseek(fp, 8 + info_.num_sounds * sizeof(SoundEntry) + SEPARATE_DIST, SEEK_SET);

    // Load all the commands
    for (int t = 0; t < info_.num_tracks; ++t) {
        Command command;
        fread(&command, sizeof(command), 1, fp);

        int num_commands = command.start_params.num_commands;

        for (int i = 0; i < num_commands; ++i) {
            fread(&command, sizeof(command), 1, fp);
            commands_.emplace_back(command, t);
        }

        fseek(fp, SEPARATE_DIST, SEEK_CUR);
    }

    struct {
        bool operator() (const std::pair<Command, int> &first, const std::pair<Command,int> &second) const {
            if (first.first.position == second.first.position) {
                // give priority to BPM changes
                return (first.first.type == CommandBPM && second.first.type != CommandBPM);
            } else {
                return (first.first.position < second.first.position);
            }
        }
    } command_cmp;

    // Sort by position/deadline
    sort(commands_.begin(), commands_.end(), command_cmp);
}
