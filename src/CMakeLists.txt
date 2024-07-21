find_package(Clang REQUIRED)
set(LLVM_LINK_COMPONENTS support)

add_executable(serializer-generator
    Main.cpp
    ASTParser.cpp
    CodeGenerator.cpp
    SerializerTemplate.cpp
    Json.cpp
)
set_target_properties(serializer-generator PROPERTIES
    CXX_STANDARD 17
)
target_include_directories(serializer-generator
    PRIVATE
    /usr/include
    ${CLANG_INCLUDE_DIRS}
)
target_link_libraries(serializer-generator
    PRIVATE
    clang-cpp
    LLVM-17
)

# Generate serializer template before build
add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_SOURCE_DIR}/SerializerTemplate.cpp
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/generate-serializer-template.sh
            ${CMAKE_CURRENT_SOURCE_DIR}/Serializer.hpp.template
            ${CMAKE_CURRENT_SOURCE_DIR}/SerializerTemplate.cpp
    DEPENDS
        generate-serializer-template.sh
        Serializer.hpp.template
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_custom_target(gen_serializer_template_header
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/SerializerTemplate.cpp
)

add_dependencies(serializer-generator
    gen_serializer_template_header
)
