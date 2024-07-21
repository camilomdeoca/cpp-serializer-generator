#include "ASTParser.hpp"
#include "CodeGenerator.hpp"
#include "Json.hpp"

#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

enum ExitCodes {
    EXIT_CODE_OK = 0,
    EXIT_CODE_NO_CPP_FILE = 1,
    EXIT_CODE_NO_JSON_FILES = 2,
    EXIT_CODE_BAD_PARAMETERS = 3,
};

struct ParsedArguments {
    std::string executableName;
    std::string serializerOutHeaderFilename = "serializer.h";
    std::string serializerOutCodeFilename = "serializer.cpp";
    std::string compilationDBFolderPath = ".";
    std::string jsonOutputPath;
    std::string cppFile;
    std::vector<std::string> jsonFiles;
    bool showHelp = false;
    bool generateJsonFile = false;
};

void printHelp(std::string_view programName)
{
    std::cout <<
    //  |1                                                                             |80
    "Description:\n"
    "  Generates a json file containing the data of the definitions of the classes or\n"
    "  structs with the AUTO_SERIALIZE macro. Also generates the serializer and\n"
    "  unserializer classes for serializing/unserializing the classes or structs with\n"
    "  the macro\n"
    "Json generation:\n"
    "Usage: " << programName << " [options] cpp_file\n"
    "  --out-json        The generated json filepath (`cpp_file.json` by default)\n"
    "  --compilation-db  The path of the folder including the compilation database\n"
    "                    (`.` by default)\n"
    "  -h, --help        Print the program options\n"
    "C++ files generation:\n"
    "Usage: " << programName << " [options] json_file...\n"
    "  --out-header      The generated header filepath (`serializer.h` by default)\n"
    "  --out-code        The generated code filepath (`serializer.cpp` by default)"
    << std::endl;
}

ParsedArguments getArgs(int argc, const char **argv)
{
    ParsedArguments result;
    std::vector<std::string> freeArgs;

    result.executableName = *argv;

    ++argv;
    while (*argv)
    {
        argc--;
        if (strcmp(*argv, "--out-header") == 0)
        {
            if (++argv)
            {
                argc--;
                result.generateJsonFile = false;
                result.serializerOutHeaderFilename = *argv;
            }
            else
            {
                std::cerr << "The " << *(argv - 1) << " parameter has no value" << std::endl;
                exit(EXIT_CODE_BAD_PARAMETERS);
            }
        }
        else if (strcmp(*argv, "--out-code") == 0)
        {
            if (++argv)
            {
                argc--;
                result.generateJsonFile = false;
                result.serializerOutCodeFilename = *argv;
            }
            else
            {
                std::cerr << "The " << *(argv - 1) << " parameter has no value" << std::endl;
                exit(EXIT_CODE_BAD_PARAMETERS);
            }
        }
        else if (strcmp(*argv, "--compilation-db") == 0)
        {
            if (++argv)
            {
                argc--;
                result.generateJsonFile = true;
                result.compilationDBFolderPath = *argv;
            }
            else
            {
                std::cerr << "The " << *(argv - 1) << " parameter has no value" << std::endl;
                exit(EXIT_CODE_BAD_PARAMETERS);
            }
        }
        else if (strcmp(*argv, "--out-json") == 0 || strcmp(*argv, "-o") == 0)
        {
            if (++argv)
            {
                argc--;
                result.generateJsonFile = true;
                result.jsonOutputPath = *argv;
            }
            else
            {
                std::cerr << "The " << *(argv - 1) << " parameter has no value" << std::endl;
                exit(EXIT_CODE_BAD_PARAMETERS);
            }
        }
        else if (strcmp(*argv, "--help") == 0 || strcmp(*argv, "-h") == 0)
        {
            result.showHelp = true;
        }
        else
        {
            freeArgs.emplace_back(*argv);
        }
        ++argv;
    }

    if (result.generateJsonFile)
    {
        if (freeArgs.empty())
        {
            std::cerr << "Error: No file provided." << std::endl;
            exit(EXIT_CODE_NO_CPP_FILE);
        }

        result.cppFile = std::move(freeArgs.back());

        if (result.jsonOutputPath.empty())
        {
            result.jsonOutputPath = result.cppFile + ".json";
        }
    }
    else
    {
        if (freeArgs.empty())
        {
            std::cerr << "Error: No json files provided." << std::endl;
            exit(EXIT_CODE_NO_JSON_FILES);
        }
        result.jsonFiles = std::move(freeArgs);
    }

    return result;
}

int main(int argc, const char *argv[])
{
    ParsedArguments args = getArgs(argc, argv);

    if (args.showHelp)
    {
        printHelp(argv[0]);
        return EXIT_CODE_OK;
    }

    int error = 0;
    if (args.generateJsonFile)
    {
        std::vector<std::string> files = { args.cppFile };
#if 0
        std::cout << "Files: " << std::endl;
        for (const std::string &file : files)
        {
            std::cout << "  " << file << std::endl;
        }
        std::cout << std::endl;
#endif

        CXXParser::ExecutionData executionData;
        error = CXXParser::parse(files, args.compilationDBFolderPath, executionData);
        Json::write(executionData, args.jsonOutputPath);
    }
    else
    {
        CodeGenerator::copy_files(args.serializerOutHeaderFilename, args.serializerOutCodeFilename);
#if 0
        std::cout << "Json files: " << std::endl;
        for (const std::string &file : args.jsonFiles)
        {
            std::cout << "  " << file << std::endl;
        }
        std::cout << std::endl;
#endif

        CXXParser::ExecutionData executionData = Json::parseAll(args.jsonFiles);
        CodeGenerator::generate(executionData, args.serializerOutHeaderFilename, args.serializerOutCodeFilename);
    }

    llvm::errs().flush();
    llvm::outs().flush();

    return error;
}
