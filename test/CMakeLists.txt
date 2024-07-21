#file(GLOB_RECURSE SERIALIZER_GENERATOR_TEST_SOURCES test/*.cpp )
set(SERIALIZER_GENERATOR_TEST_SOURCES
    test.cpp
    anotherfile.cpp
    main.cpp
)

add_executable(serializer-generator-test
    ${SERIALIZER_GENERATOR_TEST_SOURCES}
    Serializer.cpp
)
set_target_properties(serializer-generator-test PROPERTIES
    #EXCLUDE_FROM_ALL true
    EXPORT_COMPILE_COMMANDS ON
    CXX_STANDARD 23
)

add_serializer_generation(serializer-generator-test
    HEADER Serializer.hpp
    CODE   Serializer.cpp
    FILES  ${SERIALIZER_GENERATOR_TEST_SOURCES}
)

add_test(
    NAME serializer-generator-test
    COMMAND $<TARGET_FILE:serializer-generator-test>
)

