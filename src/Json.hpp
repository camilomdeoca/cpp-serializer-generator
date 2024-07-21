#ifndef SERIALIZER_GENERATOR_JSON_HPP
#define SERIALIZER_GENERATOR_JSON_HPP

#include "ASTParser.hpp"

#include <filesystem>

namespace Json {

CXXParser::ExecutionData parseAll(std::vector<std::string> toLinkFiles);
void write(const CXXParser::ExecutionData &executionData, std::filesystem::path out);

} // namespace Json

#endif // SERIALIZER_GENERATOR_JSON_HPP

