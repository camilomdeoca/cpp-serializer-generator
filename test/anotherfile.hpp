#pragma once

#include <string>
#include <vector>

#include "Serializer.hpp"

struct StructInOtherFile {
    std::string key;
    std::vector<std::string> values;

    void add(const std::string &val);
    AUTO_SERIALIZE
};

bool vectorsAreSame(std::vector<std::string> vec1, std::vector<std::string> vec2);
