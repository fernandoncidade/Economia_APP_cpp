if(NOT DEFINED DEPLOY_ROOT)
    message(FATAL_ERROR "DEPLOY_ROOT was not provided")
endif()

file(TO_CMAKE_PATH "${DEPLOY_ROOT}" DEPLOY_ROOT_NORMALIZED)
if(NOT EXISTS "${DEPLOY_ROOT_NORMALIZED}")
    return()
endif()

if(NOT DEFINED MSVC_RUNTIME_ARCH OR "${MSVC_RUNTIME_ARCH}" STREQUAL "")
    return()
endif()

set(MSVC_RUNTIME_ARCH_VALUE "${MSVC_RUNTIME_ARCH}")

if(NOT DEFINED MSVC_REDIST_ROOT OR "${MSVC_REDIST_ROOT}" STREQUAL "")
    message(WARNING "MSVC redist root was not provided. The deployed executable may require the Visual C++ Redistributable to be installed.")
    return()
endif()

file(TO_CMAKE_PATH "${MSVC_REDIST_ROOT}" MSVC_REDIST_ROOT_NORMALIZED)
if(NOT EXISTS "${MSVC_REDIST_ROOT_NORMALIZED}")
    message(WARNING "MSVC redist root was not found: ${MSVC_REDIST_ROOT_NORMALIZED}")
    return()
endif()

set(MSVC_RUNTIME_CONFIG_VALUE "Release")
if(DEFINED CONFIG AND NOT "${CONFIG}" STREQUAL "")
    set(MSVC_RUNTIME_CONFIG_VALUE "${CONFIG}")
endif()
string(TOUPPER "${MSVC_RUNTIME_CONFIG_VALUE}" MSVC_RUNTIME_CONFIG_UPPER)

file(GLOB MSVC_REDIST_VERSION_DIRS LIST_DIRECTORIES true "${MSVC_REDIST_ROOT_NORMALIZED}/*")
if(NOT MSVC_REDIST_VERSION_DIRS)
    message(WARNING "No MSVC redist versions were found under: ${MSVC_REDIST_ROOT_NORMALIZED}")
    return()
endif()

list(SORT MSVC_REDIST_VERSION_DIRS)
list(REVERSE MSVC_REDIST_VERSION_DIRS)

set(MSVC_RUNTIME_SOURCE_DIRS)
foreach(MSVC_REDIST_VERSION_DIR IN LISTS MSVC_REDIST_VERSION_DIRS)
    if(MSVC_RUNTIME_CONFIG_UPPER STREQUAL "DEBUG")
        file(GLOB MSVC_CRT_DIR_CANDIDATES LIST_DIRECTORIES true
            "${MSVC_REDIST_VERSION_DIR}/debug_nonredist/${MSVC_RUNTIME_ARCH_VALUE}/Microsoft.VC*.DebugCRT"
        )
        if(NOT MSVC_CRT_DIR_CANDIDATES)
            file(GLOB MSVC_CRT_DIR_CANDIDATES LIST_DIRECTORIES true
                "${MSVC_REDIST_VERSION_DIR}/onecore/debug_nonredist/${MSVC_RUNTIME_ARCH_VALUE}/Microsoft.VC*.DebugCRT"
            )
        endif()
        file(GLOB MSVC_OPENMP_DIR_CANDIDATES LIST_DIRECTORIES true
            "${MSVC_REDIST_VERSION_DIR}/debug_nonredist/${MSVC_RUNTIME_ARCH_VALUE}/Microsoft.VC*.DebugOpenMP"
        )
        if(NOT MSVC_OPENMP_DIR_CANDIDATES)
            file(GLOB MSVC_OPENMP_DIR_CANDIDATES LIST_DIRECTORIES true
                "${MSVC_REDIST_VERSION_DIR}/onecore/debug_nonredist/${MSVC_RUNTIME_ARCH_VALUE}/Microsoft.VC*.DebugOpenMP"
            )
        endif()
    else()
        file(GLOB MSVC_CRT_DIR_CANDIDATES LIST_DIRECTORIES true
            "${MSVC_REDIST_VERSION_DIR}/${MSVC_RUNTIME_ARCH_VALUE}/Microsoft.VC*.CRT"
        )
        if(NOT MSVC_CRT_DIR_CANDIDATES)
            file(GLOB MSVC_CRT_DIR_CANDIDATES LIST_DIRECTORIES true
                "${MSVC_REDIST_VERSION_DIR}/onecore/${MSVC_RUNTIME_ARCH_VALUE}/Microsoft.VC*.CRT"
            )
        endif()
        file(GLOB MSVC_OPENMP_DIR_CANDIDATES LIST_DIRECTORIES true
            "${MSVC_REDIST_VERSION_DIR}/${MSVC_RUNTIME_ARCH_VALUE}/Microsoft.VC*.OpenMP"
        )
        if(NOT MSVC_OPENMP_DIR_CANDIDATES)
            file(GLOB MSVC_OPENMP_DIR_CANDIDATES LIST_DIRECTORIES true
                "${MSVC_REDIST_VERSION_DIR}/onecore/${MSVC_RUNTIME_ARCH_VALUE}/Microsoft.VC*.OpenMP"
            )
        endif()
    endif()

    list(APPEND MSVC_RUNTIME_SOURCE_DIRS ${MSVC_CRT_DIR_CANDIDATES} ${MSVC_OPENMP_DIR_CANDIDATES})

    if(MSVC_RUNTIME_SOURCE_DIRS)
        break()
    endif()
endforeach()

if(NOT MSVC_RUNTIME_SOURCE_DIRS)
    message(WARNING "No MSVC runtime directory was found for arch ${MSVC_RUNTIME_ARCH_VALUE} under: ${MSVC_REDIST_ROOT_NORMALIZED}")
    return()
endif()

set(MSVC_RUNTIME_FILES)
foreach(MSVC_RUNTIME_SOURCE_DIR IN LISTS MSVC_RUNTIME_SOURCE_DIRS)
    if(NOT DEFINED MSVC_RUNTIME_FILE_NAMES OR NOT MSVC_RUNTIME_FILE_NAMES)
        if(MSVC_RUNTIME_CONFIG_UPPER STREQUAL "DEBUG")
            set(MSVC_RUNTIME_FILE_NAMES
                "msvcp140d.dll"
                "msvcp140_1d.dll"
                "msvcp140_2d.dll"
                "vcruntime140d.dll"
                "vcruntime140_1d.dll"
            )
        else()
            set(MSVC_RUNTIME_FILE_NAMES
                "msvcp140.dll"
                "msvcp140_1.dll"
                "msvcp140_2.dll"
                "vcruntime140.dll"
                "vcruntime140_1.dll"
            )
        endif()
    endif()

    foreach(MSVC_RUNTIME_FILE_NAME IN LISTS MSVC_RUNTIME_FILE_NAMES)
        if(EXISTS "${MSVC_RUNTIME_SOURCE_DIR}/${MSVC_RUNTIME_FILE_NAME}")
            list(APPEND MSVC_RUNTIME_FILES "${MSVC_RUNTIME_SOURCE_DIR}/${MSVC_RUNTIME_FILE_NAME}")
        endif()
    endforeach()
endforeach()

list(REMOVE_DUPLICATES MSVC_RUNTIME_FILES)
if(NOT MSVC_RUNTIME_FILES)
    message(WARNING "No MSVC runtime DLLs were found in: ${MSVC_RUNTIME_SOURCE_DIRS}")
    return()
endif()

file(COPY ${MSVC_RUNTIME_FILES} DESTINATION "${DEPLOY_ROOT_NORMALIZED}")
list(LENGTH MSVC_RUNTIME_FILES MSVC_RUNTIME_FILE_COUNT)
message(STATUS "Copied ${MSVC_RUNTIME_FILE_COUNT} MSVC runtime DLL(s) to: ${DEPLOY_ROOT_NORMALIZED}")
