#include "ASTParser.hpp"
#include "SerializerTemplate.hpp"

#include <clang/Frontend/CompilerInstance.h>

#include <clang/Tooling/CompilationDatabase.h>
#include <memory>
#include <stdexcept>
#include <unordered_set>
#include <vector>

namespace CXXParser {

constexpr const char *AUTO_SERIALIZE_FLAG = "enable_auto_serialize";

bool MyASTConsumer::HandleTopLevelDecl(clang::DeclGroupRef DG)
{
    for (clang::DeclGroupRef::iterator it = DG.begin(), itEnd = DG.end();
         it != itEnd;
         it++)
    {
        if (clang::CXXRecordDecl *pRecord = llvm::dyn_cast<clang::CXXRecordDecl>(*it))
        {
            if (IsRecordFlaggedToSerialize(*pRecord))
                HandleSingleDecl(*pRecord);
        }

        if (clang::NamespaceDecl *childDeclContext = llvm::dyn_cast<clang::NamespaceDecl>(*it))
            HandleChildDeclarations(childDeclContext);
    }
    return true;
}

void MyASTConsumer::HandleChildDeclarations(clang::DeclContext *declContext)
{
    for (clang::Decl *child : declContext->decls())
    {
        if (clang::CXXRecordDecl *pRecord = llvm::dyn_cast<clang::CXXRecordDecl>(child))
            if (IsRecordFlaggedToSerialize(*pRecord))
                HandleSingleDecl(*pRecord);
        if (clang::NamespaceDecl *childDeclContext = llvm::dyn_cast<clang::NamespaceDecl>(child))
            HandleChildDeclarations(childDeclContext);
    }
}

bool MyASTConsumer::HandleDecl(clang::DeclGroupRef DG)
{
    for (clang::DeclGroupRef::iterator it = DG.begin(), itEnd = DG.end();
         it != itEnd;
         it++)
    {
        clang::CXXRecordDecl *pRecord = llvm::dyn_cast<clang::CXXRecordDecl>(*it);
        if (!pRecord)
            continue;
        if (IsRecordFlaggedToSerialize(*pRecord))
            HandleSingleDecl(*pRecord);
    }
    return true;
}

void MyASTConsumer::HandleSingleDecl(const clang::CXXRecordDecl &pRecord)
{
    clang::ASTContext &context = pRecord.getASTContext();
    clang::SourceLocation beginLoc = pRecord.getBeginLoc();
    clang::SourceManager &SM = context.getSourceManager();

    //if (!SM.isInMainFile(beginLoc))
    //    return;

    RecordDefinitionData record;
    record.name = pRecord.getNameAsString();
    if (m_alreadyParsedStructsNames.find(record.name) != m_alreadyParsedStructsNames.end())
        return;
    record.headerPath = SM.getFilename(beginLoc).str();
    if (pRecord.isClass()) record.type = RecordDefinitionData::Type::Class;
    else if (pRecord.isStruct()) record.type = RecordDefinitionData::Type::Struct;
    else throw std::runtime_error("Unions are not supported");

    for (const clang::CXXBaseSpecifier &baseClass : pRecord.bases())
    {
        record.bases.emplace_back(baseClass.getType().getAsString());
        const clang::CXXRecordDecl *baseClassDecl
            = baseClass.getType().getCanonicalType()->getAsCXXRecordDecl();
        if (!baseClassDecl)
            throw std::runtime_error("The base class isnt a record?!?");
        HandleSingleDecl(*baseClassDecl);
    }
    for (const clang::FieldDecl *field : pRecord.fields())
    {
        RecordDefinitionData::FieldData fieldData;
        fieldData.typeName = field->getType().getAsString();
        fieldData.name = field->getNameAsString();
        fieldData.length = 0;
        record.fields.emplace_back(fieldData);
    }

#if 0
    std::cout << record.headerPath << ":" << SM.getSpellingLineNumber(beginLoc) << std::endl;
    std::cout << "struct " << record.name << std::endl;
    bool firstIteration = true;
    for (const std::string &base : record.bases)
    {
        bool isLastIteration = base == record.bases.back();
        if (firstIteration)
        {
            firstIteration = false;

            if (isLastIteration)
                std::cout << "    : " << base << std::endl;
            else
                std::cout << "    : " << base << "," << std::endl;
        }
        else if (isLastIteration)
            std::cout << "      " << base << std::endl;
        else
            std::cout << "      " << base << "," << std::endl;
    }
    std::cout << "{" << std::endl;
    for (const RecordDefinitionData::FieldData &fieldData : record.fields) {
        std::cout << "    " << fieldData.typeName << " " << fieldData.name << ";" << std::endl;
    }
    std::cout << "};" << std::endl << std::endl;
#endif

    m_executionData.records.emplace_back(record);
    m_alreadyParsedStructsNames.emplace(record.name);
}

bool MyASTConsumer::IsRecordFlaggedToSerialize(const clang::CXXRecordDecl &pRecord)
{
    for (clang::Decl *D : pRecord.decls())
    {
        if (clang::TypeAliasDecl *typeAliasDecl = llvm::dyn_cast<clang::TypeAliasDecl>(D))
            if (typeAliasDecl->getNameAsString() == AUTO_SERIALIZE_FLAG)
                return true;
    }

    clang::ASTContext &context = pRecord.getASTContext();
    if (const clang::RawComment *comment = context.getRawCommentForDeclNoCache(&pRecord))
        if (std::string(comment->getBriefText(context)) == AUTO_SERIALIZE_FLAG)
            return true;

    return false;
}

std::unique_ptr<clang::ASTConsumer> MyAction::CreateASTConsumer(
    clang::CompilerInstance &CI, llvm::StringRef InFile)
{
    return std::make_unique<MyASTConsumer>(m_executionData, m_alreadyParsedStructsNames);
}

bool MyAction::BeginSourceFileAction(clang::CompilerInstance &CI)
{
    CI.getDiagnostics().setSuppressAllDiagnostics(true);
    CI.getPreprocessor().SetSuppressIncludeNotFoundError(true);
    return true;
}

std::unique_ptr<clang::FrontendAction> MyFactory::create()
{
    return std::make_unique<MyAction>(m_executionData, m_alreadyParsedStructsNames);
}

int parse(std::vector<std::string> files, std::string compilationDbPath, ExecutionData &executionData)
{
    std::string errorMessage;
    std::unique_ptr<clang::tooling::CompilationDatabase> pCompilationDb
        = clang::tooling::CompilationDatabase::loadFromDirectory(compilationDbPath, errorMessage);
    if (!pCompilationDb)
        return 1;

    clang::tooling::ClangTool Tool(*pCompilationDb, files);
    std::vector<std::string> clangArgs = {
        //"-fparse-all-comments",
        "-fsyntax-only",
        "-include", SERIALIZER_HEADER_TEMPLATE, // So if the serializer's files don't exist yet it doesn't fail
    };
    Tool.appendArgumentsAdjuster(clang::tooling::getInsertArgumentAdjuster(
              clangArgs, clang::tooling::ArgumentInsertPosition::BEGIN));

    MyFactory factory(executionData);

    return Tool.run(&factory);
}

} // namespace CXXParser

