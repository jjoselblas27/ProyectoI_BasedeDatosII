#pragma once
#include <cstring>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

#pragma pack(push, 1)
struct AudioFeatures {
    char isrc[13]{'\0'};
    double acousticness;
    double danceability;
    int duration_ms;
    double energy;

    explicit AudioFeatures() = default;
    
    explicit AudioFeatures(const std::string& isrc, double acousticness, double danceability, int duration_ms, double energy) : 
        acousticness(acousticness), danceability(danceability), duration_ms(duration_ms), energy(energy) {
        std::strncpy(this->isrc, isrc.c_str(), sizeof(this->isrc));
    }

    friend std::ostream& operator<<(std::ostream& stream, const AudioFeatures& record) {
        stream.write(reinterpret_cast<const char*>(&record), sizeof(AudioFeatures));
        return stream;
    }

    friend std::istream& operator>>(std::istream& stream, AudioFeatures& record) {
        stream.read(reinterpret_cast<char*>(&record), sizeof(AudioFeatures));
        return stream;
    }

    std::string to_string() {
        std::stringstream stream;
        stream << "(" << isrc << ", " << acousticness << ", " << danceability << ", " << duration_ms << ", " << energy << ")";
        return stream.str();
    }
};
#pragma pack(pop)
