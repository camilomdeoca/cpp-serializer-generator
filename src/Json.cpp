#include "Json.hpp"

#include "ASTParser.hpp"

#include <filesystem>
#include <iterator>
#include <llvm/ADT/StringRef.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/JSON.h>
#include <fstream>
#include <llvm/Support/raw_ostream.h>
#include <optional>

namespace Json {

std::string readFile(const std::string &path)
{
    std::ifstream ifs(path);
    return std::string(
        (std::istreambuf_iterator<char>(ifs)),
        (std::istreambuf_iterator<char>())
    );
}

std::string recordTypeToString(CXXParser::RecordDefinitionData::Type type)
{
    switch (type) {
    case CXXParser::RecordDefinitionData::Type::Struct:
        return "struct";
    case CXXParser::RecordDefinitionData::Type::Class:
        return "class";
    default:
        exit(5);
    }
}

CXXParser::RecordDefinitionData::Type parseRecordType(std::string type)
{
    if (type == "struct")
        return CXXParser::RecordDefinitionData::Type::Struct;
    if (type == "class")
        return CXXParser::RecordDefinitionData::Type::Class;
    exit(6);
}

void write(const CXXParser::ExecutionData &executionData, std::filesystem::path out)
{
    llvm::json::Array recordsArray;
    for (const CXXParser::RecordDefinitionData &data : executionData.records)
    {
        llvm::json::Array fields;
        for (const CXXParser::RecordDefinitionData::FieldData &field : data.fields)
        {
            fields.push_back(llvm::json::Object {
                {"name", field.name},
                {"type", field.typeName},
                {"length", field.length}
            });
        }

        llvm::json::Array bases;
        for (const std::string &baseClass : data.bases)
        {
            bases.emplace_back(llvm::json::Value(baseClass));
        }

        llvm::json::Object recordJsonObject{
            {"name", data.name},
            {"namespace", data.namespaceName},
            {"type", recordTypeToString(data.type)},
            {"header_path", data.headerPath},
            {"fields", std::move(fields)},
            {"bases", std::move(bases)}
        };

        recordsArray.push_back(std::move(recordJsonObject));
    }

    std::ofstream ofs(out);
    std::string serializedJson;
    llvm::raw_string_ostream serializedJsonStream(serializedJson);
    serializedJsonStream << llvm::formatv("{0:2}", llvm::json::Value(std::move(recordsArray)));
    serializedJsonStream.flush();
    ofs << serializedJson;
}

// TODO: Warn on continues while parsing something invalid
CXXParser::ExecutionData parseAll(const std::vector<std::string> toLinkFiles)
{
    CXXParser::ExecutionData executionData;
    for (const std::string &file : toLinkFiles)
    {
        llvm::Expected<llvm::json::Value> parsedFile = llvm::json::parse(readFile(file));
        if (!parsedFile)
            continue;
        const llvm::json::Array *array = parsedFile->getAsArray();
        if (!array)
            continue;

        for (const llvm::json::Value &elementOfArray : *array)
        {
            // The record definition data
            const llvm::json::Object *object = elementOfArray.getAsObject();
            if (!object)
                continue;

            const std::optional<llvm::StringRef> name = object->getString("name");
            const std::optional<llvm::StringRef> namespaceName = object->getString("namespace");
            const std::optional<llvm::StringRef> type = object->getString("type");
            const std::optional<llvm::StringRef> headerPath = object->getString("header_path");
            const llvm::json::Array *fields = object->getArray("fields");
            const llvm::json::Array *bases = object->getArray("bases");
            if (!name || !namespaceName || !type || !fields || !bases)
                continue;

            bool alreadyInExecutionData = false;
            for (const CXXParser::RecordDefinitionData &record : executionData.records)
            {
                if (record.name == name->str())
                {
                    alreadyInExecutionData = true;
                    break;
                }
            }
            if (alreadyInExecutionData) continue;

            CXXParser::RecordDefinitionData &record = executionData.records.emplace_back();
            record.name = name.value();
            record.namespaceName = namespaceName.value();
            record.type = parseRecordType(type.value().str());
            record.headerPath = headerPath.value();
            for (const llvm::json::Value &fieldValue : *fields)
            {
                const llvm::json::Object *field = fieldValue.getAsObject();
                if (!field)
                    continue;
                const std::optional<llvm::StringRef> fieldName = field->getString("name");
                const std::optional<llvm::StringRef> fieldType = field->getString("type");
                const std::optional<int64_t> fieldLength = field->getInteger("length");
                if (!fieldName || !fieldType || !fieldLength)
                    continue;
                CXXParser::RecordDefinitionData::FieldData fieldData;
                fieldData.name = fieldName.value();
                fieldData.typeName = fieldType.value();
                fieldData.length = fieldLength.value();
                record.fields.push_back(fieldData);
            }

            for (const llvm::json::Value &baseValue : *bases)
            {
                const std::optional<llvm::StringRef> baseName = baseValue.getAsString();
                if (!baseName)
                    continue;
                record.bases.push_back(baseName.value().str());
            }
        }
    }
    return executionData;
}

} // namespace Json
