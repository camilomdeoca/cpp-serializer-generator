#include "anotherfile.hpp"

void StructInOtherFile::add(const std::string &val)
{
    values.emplace_back(val);
}

bool vectorsAreSame(std::vector<std::string> vec1, std::vector<std::string> vec2)
{
    if (vec1.size() != vec2.size())
        return false;
    for (size_t i = 0; i < vec1.size(); i++)
        if (vec1[i] != vec2[i])
            return false;
    return true;
}
