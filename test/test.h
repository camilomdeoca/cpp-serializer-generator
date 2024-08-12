#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "Serializer.hpp"

struct Another {
    std::string name;
    std::vector<int> numbers;
};

namespace what::whatagain {

    struct TestStruct : public Another {
        TestStruct(std::string name, std::vector<int> &&numbers, float volume, uint32_t count)
            : Another(name, numbers), volume(volume), count(count)
        {}
    
        void testFunc() {}
    
        float volume;
        uint32_t count;
        AUTO_SERIALIZE;
    };

}

bool vectorsAreSame(std::vector<int> vec1, std::vector<int> vec2);
