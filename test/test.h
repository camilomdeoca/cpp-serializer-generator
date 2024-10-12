#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "Serializer.hpp"

struct AnotherStructThatWillBeField {
    int i;
    float j;
};

struct Another {
    std::string name;
    std::vector<int> numbers;
    AnotherStructThatWillBeField structInside;
};

namespace what::whatagain {

    struct TestStruct : public Another {
        TestStruct(
                std::string name,
                std::vector<int> &&numbers,
                AnotherStructThatWillBeField structInside,
                float volume,
                uint32_t count)
            : Another(name, numbers, structInside), volume(volume), count(count)
        {}
    
        void testFunc() {}
    
        float volume;
        uint32_t count;
        AUTO_SERIALIZE
    };

}

bool vectorsAreSame(std::vector<int> vec1, std::vector<int> vec2);
