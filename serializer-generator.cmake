function(add_serializer_generation TARGET)
    cmake_parse_arguments(
        ARG
        ""
        "COMPILATION_DB;HEADER;CODE"
        "FILES"
        ${ARGN}
    )

    #message("TARGET: ${TARGET}")
    #message("HEADER: ${ARG_HEADER}")
    #message("CODE: ${ARG_CODE}")
    #message("FILES: ${ARG_FILES}")

    if (NOT DEFINED ARG_COMPILATION_DB)
        set(ARG_COMPILATION_DB ${PROJECT_BINARY_DIR})
    endif()

    cmake_path(SET JSON_FILES_DIR ${CMAKE_CURRENT_BINARY_DIR})
    foreach(FILE ${ARG_FILES})
        cmake_path(SET JSON_FILE ${JSON_FILES_DIR}/${FILE}.json)
        set(JSON_FILES ${JSON_FILES} ${JSON_FILE})
        add_custom_command(
            OUTPUT ${JSON_FILE}
            COMMAND $<TARGET_FILE:serializer-generator>
                --compilation-db ${ARG_COMPILATION_DB}
                --no-link
                --out-json ${JSON_FILE}
                ${FILE}
            DEPENDS
                $<TARGET_FILE:serializer-generator>
            IMPLICIT_DEPENDS CXX ${FILE}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
    endforeach()

    #message("JSON_FILES: ${JSON_FILES}")

    cmake_path(SET HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_HEADER})
    cmake_path(SET CODE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_CODE})
    add_custom_command(
        OUTPUT
            ${HEADER_PATH}
            ${CODE_PATH}
        COMMAND $<TARGET_FILE:serializer-generator>
            --out-header ${ARG_HEADER}
            --out-code ${ARG_CODE}
            ${JSON_FILES}
        DEPENDS
            $<TARGET_FILE:serializer-generator>
            ${JSON_FILES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    add_custom_target(gen-serializer-for-${TARGET}
        DEPENDS
            ${HEADER_PATH}
            ${CODE_PATH}
    )
    add_dependencies(${TARGET}
        gen-serializer-for-${TARGET}
    )
endfunction()
