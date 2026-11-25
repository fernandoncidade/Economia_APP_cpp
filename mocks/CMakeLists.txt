cmake_minimum_required(VERSION 3.28)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(Economia_APP_Mocks LANGUAGES CXX)
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)
    find_package(Qt6 6.12 REQUIRED COMPONENTS Core Gui Widgets PrintSupport)
endif()

if(WIN32)
    enable_language(RC)
endif()

function(mocks_get_qt_target_location TARGET_NAME OUT_VAR)
    set(${OUT_VAR} "" PARENT_SCOPE)
    if(NOT TARGET "${TARGET_NAME}")
        return()
    endif()
    foreach(PROP_NAME IN ITEMS
        IMPORTED_LOCATION
        IMPORTED_LOCATION_RELEASE
        IMPORTED_LOCATION_RELWITHDEBINFO
        IMPORTED_LOCATION_MINSIZEREL
        IMPORTED_LOCATION_DEBUG
    )
        get_target_property(PROP_VALUE "${TARGET_NAME}" "${PROP_NAME}")
        if(PROP_VALUE AND NOT PROP_VALUE STREQUAL "PROP_VALUE-NOTFOUND" AND EXISTS "${PROP_VALUE}")
            set(${OUT_VAR} "${PROP_VALUE}" PARENT_SCOPE)
            return()
        endif()
    endforeach()
endfunction()

find_package(Qt6 6.12 QUIET COMPONENTS LinguistTools)

mocks_get_qt_target_location(Qt6::qmake MOCKS_QT_QMAKE_EXECUTABLE)
mocks_get_qt_target_location(Qt6::Core MOCKS_QT_CORE_LIBRARY)
if(MOCKS_QT_QMAKE_EXECUTABLE)
    get_filename_component(MOCKS_QT_BIN_DIR "${MOCKS_QT_QMAKE_EXECUTABLE}" DIRECTORY)
    get_filename_component(MOCKS_QT_PREFIX_DIR "${MOCKS_QT_BIN_DIR}" DIRECTORY)
elseif(MOCKS_QT_CORE_LIBRARY)
    get_filename_component(MOCKS_QT_BIN_DIR "${MOCKS_QT_CORE_LIBRARY}" DIRECTORY)
    get_filename_component(MOCKS_QT_PREFIX_DIR "${MOCKS_QT_BIN_DIR}" DIRECTORY)
else()
    get_filename_component(MOCKS_QT_PREFIX_DIR "${Qt6_DIR}/../../.." ABSOLUTE)
    set(MOCKS_QT_BIN_DIR "${MOCKS_QT_PREFIX_DIR}/bin")
endif()

set(MOCKS_WINDEPLOYQT_EXECUTABLE "${MOCKS_QT_BIN_DIR}/windeployqt.exe")
set(MOCKS_QTPATHS_EXECUTABLE "${MOCKS_QT_BIN_DIR}/qtpaths6.exe")
if(NOT EXISTS "${MOCKS_QTPATHS_EXECUTABLE}" AND EXISTS "${MOCKS_QT_BIN_DIR}/qtpaths.exe")
    set(MOCKS_QTPATHS_EXECUTABLE "${MOCKS_QT_BIN_DIR}/qtpaths.exe")
endif()
mocks_get_qt_target_location(Qt6::lrelease MOCKS_LRELEASE_EXECUTABLE)
if(NOT MOCKS_LRELEASE_EXECUTABLE AND EXISTS "${MOCKS_QT_BIN_DIR}/lrelease.exe")
    set(MOCKS_LRELEASE_EXECUTABLE "${MOCKS_QT_BIN_DIR}/lrelease.exe")
endif()

set(MOCKS_MSVC_REDIST_ROOT "")
set(MOCKS_MSVC_TOOLSET_VERSION "")
set(MOCKS_MSVC_RUNTIME_ARCH "x64")
if(MSVC)
    if(DEFINED ECONOMIA_MSVC_REDIST_ROOT AND NOT "${ECONOMIA_MSVC_REDIST_ROOT}" STREQUAL "")
        set(MOCKS_MSVC_REDIST_ROOT "${ECONOMIA_MSVC_REDIST_ROOT}")
    endif()
    if(DEFINED ECONOMIA_MSVC_TOOLSET_VERSION AND NOT "${ECONOMIA_MSVC_TOOLSET_VERSION}" STREQUAL "")
        set(MOCKS_MSVC_TOOLSET_VERSION "${ECONOMIA_MSVC_TOOLSET_VERSION}")
    endif()
    if(MOCKS_MSVC_TOOLSET_VERSION STREQUAL "" AND CMAKE_CXX_COMPILER MATCHES "/VC/Tools/MSVC/")
        get_filename_component(MOCKS_MSVC_COMPILER_ARCH_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
        get_filename_component(MOCKS_MSVC_COMPILER_HOST_DIR "${MOCKS_MSVC_COMPILER_ARCH_DIR}" DIRECTORY)
        get_filename_component(MOCKS_MSVC_COMPILER_BIN_DIR "${MOCKS_MSVC_COMPILER_HOST_DIR}" DIRECTORY)
        get_filename_component(MOCKS_MSVC_VERSION_DIR "${MOCKS_MSVC_COMPILER_BIN_DIR}" DIRECTORY)
        get_filename_component(MOCKS_MSVC_TOOLSET_VERSION "${MOCKS_MSVC_VERSION_DIR}" NAME)
        if(MOCKS_MSVC_REDIST_ROOT STREQUAL "")
            get_filename_component(MOCKS_MSVC_TOOLS_MSVC_DIR "${MOCKS_MSVC_VERSION_DIR}" DIRECTORY)
            get_filename_component(MOCKS_MSVC_TOOLS_DIR "${MOCKS_MSVC_TOOLS_MSVC_DIR}" DIRECTORY)
            get_filename_component(MOCKS_MSVC_VC_DIR "${MOCKS_MSVC_TOOLS_DIR}" DIRECTORY)
            if(EXISTS "${MOCKS_MSVC_VC_DIR}/Redist/MSVC")
                set(MOCKS_MSVC_REDIST_ROOT "${MOCKS_MSVC_VC_DIR}/Redist/MSVC")
            endif()
        endif()
    endif()
    if(MOCKS_MSVC_REDIST_ROOT STREQUAL "" AND DEFINED CMAKE_MSVC_REDIST_DIR AND EXISTS "${CMAKE_MSVC_REDIST_DIR}")
        set(MOCKS_MSVC_REDIST_ROOT "${CMAKE_MSVC_REDIST_DIR}")
    endif()
    if(MOCKS_MSVC_REDIST_ROOT STREQUAL "")
        file(GLOB MOCKS_MSVC_REDIST_ROOT_CANDIDATES LIST_DIRECTORIES true
            "C:/Program Files (x86)/Microsoft Visual Studio/*/*/VC/Redist/MSVC"
            "C:/Program Files/Microsoft Visual Studio/*/*/VC/Redist/MSVC"
        )
        if(MOCKS_MSVC_REDIST_ROOT_CANDIDATES)
            list(SORT MOCKS_MSVC_REDIST_ROOT_CANDIDATES COMPARE NATURAL ORDER DESCENDING)
            list(GET MOCKS_MSVC_REDIST_ROOT_CANDIDATES 0 MOCKS_MSVC_REDIST_ROOT)
        endif()
    endif()
endif()

if(WIN32)
    if(MSVC)
        set(MOCKS_DETECTED_TOOLCHAIN_SUFFIX "msvc")
    elseif(MINGW)
        set(MOCKS_DETECTED_TOOLCHAIN_SUFFIX "mingw")
    else()
        set(MOCKS_DETECTED_TOOLCHAIN_SUFFIX "windows")
    endif()

    set(MOCKS_DEFAULT_EXPORT_DIR "D:/MISCELANEAS/VSCode/Economia_APP/Mocks/mocks_${MOCKS_DETECTED_TOOLCHAIN_SUFFIX}")
else()
    set(MOCKS_DEFAULT_EXPORT_DIR "")
endif()

set(
    MOCKS_EXPORT_DIR
    "${MOCKS_DEFAULT_EXPORT_DIR}"
    CACHE PATH
    "Additional directory that receives the deployed mocks runtime"
)

if(DEFINED ECONOMIA_BUILD_SUITE AND ECONOMIA_BUILD_SUITE)
    set(MOCKS_EXPORT_DIR "" CACHE PATH "Distribuicao externa desabilitada para a suite de testes" FORCE)
    set(MOCKS_EXPORT_DIR_NORMALIZED "")
elseif(MOCKS_EXPORT_DIR)
    file(TO_CMAKE_PATH "${MOCKS_EXPORT_DIR}" MOCKS_EXPORT_DIR_NORMALIZED)
else()
    set(MOCKS_EXPORT_DIR_NORMALIZED "")
endif()

set(MOCKS_WINDOWS_ICON_NAME "economia.ico")
set(MOCKS_WINDOWS_ICON "${CMAKE_CURRENT_SOURCE_DIR}/icones/${MOCKS_WINDOWS_ICON_NAME}")
if(NOT EXISTS "${MOCKS_WINDOWS_ICON}")
    set(MOCKS_WINDOWS_ICON "${CMAKE_CURRENT_SOURCE_DIR}/../source/assets/icones/${MOCKS_WINDOWS_ICON_NAME}")
endif()

set(MOCKS_RC_FILE)
if(WIN32)
    set(MOCKS_RC_FILE "${CMAKE_CURRENT_SOURCE_DIR}/icones/mocks.rc")
endif()

function(mocks_apply_common_options TARGET_NAME)
    set_target_properties(
        "${TARGET_NAME}"
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
            RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}"
            RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}"
            RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}"
            RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug"
    )

    target_include_directories(
        "${TARGET_NAME}"
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}"
            "${CMAKE_CURRENT_SOURCE_DIR}/icones"
            "${CMAKE_CURRENT_SOURCE_DIR}/.."
            "${CMAKE_CURRENT_SOURCE_DIR}/../source"
    )

    target_compile_definitions(
        "${TARGET_NAME}"
        PRIVATE
            "MOCKS_WINDOWS_ICON_NAME=\"${MOCKS_WINDOWS_ICON_NAME}\""
            $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
            $<$<CONFIG:Release>:QT_NO_INFO_OUTPUT>
    )

    if(MSVC)
        set_property(TARGET "${TARGET_NAME}" PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
        target_compile_options(
            "${TARGET_NAME}"
            PRIVATE
                /utf-8
                /EHsc
                /GS
                /permissive-
                $<$<CONFIG:Release>:/O2 /Oi /Ot>
        )
        target_link_options(
            "${TARGET_NAME}"
            PRIVATE
                /DYNAMICBASE
                /NXCOMPAT
                /HIGHENTROPYVA
        )
    elseif(MINGW)
        target_compile_options(
            "${TARGET_NAME}"
            PRIVATE
                -finput-charset=UTF-8
                -fexec-charset=UTF-8
                $<$<CONFIG:Release>:-O2>
        )
    endif()
endfunction()

# Executáveis
add_executable(
    CompileTranslationsGUI
    WIN32
    CompileTranslationsGUI.cpp
    CompileTranslationsGUI.hpp
    ${MOCKS_RC_FILE}
)
target_link_libraries(CompileTranslationsGUI PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
mocks_apply_common_options(CompileTranslationsGUI)

add_executable(
    CompileTranslationsTerminal
    CompileTranslationsTerminal.cpp
    CompileTranslationsTerminal.hpp
    ${MOCKS_RC_FILE}
)
target_link_libraries(CompileTranslationsTerminal PRIVATE Qt6::Core)
if(WIN32)
    target_link_libraries(CompileTranslationsTerminal PRIVATE User32)
endif()
mocks_apply_common_options(CompileTranslationsTerminal)

add_executable(
    find_VersionEditor
    WIN32
    find_VersionEditor.cpp
    find_VersionEditor.hpp
    ${MOCKS_RC_FILE}
)
target_link_libraries(find_VersionEditor PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
mocks_apply_common_options(find_VersionEditor)

# Em modo suite, o deploy unificado fica sob responsabilidade de EconomiaSuiteDeploy (em tests/CMakeLists.txt)
if(NOT (DEFINED ECONOMIA_BUILD_SUITE AND ECONOMIA_BUILD_SUITE))
    # Gerar qt.conf para garantir isolamento e carregamento local de plugins nos mocks
    file(GENERATE
        OUTPUT "$<TARGET_FILE_DIR:find_VersionEditor>/qt.conf"
        CONTENT "[Paths]\nPrefix = .\nPlugins = .\n"
    )

    # Target para lrelease
    if(MOCKS_LRELEASE_EXECUTABLE AND EXISTS "${MOCKS_LRELEASE_EXECUTABLE}")
        file(GLOB TS_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../source/language/translations/*.ts")
        set(QM_FILES)
        foreach(TS_FILE IN LISTS TS_FILES)
            get_filename_component(TS_NAME "${TS_FILE}" NAME_WE)
            set(QM_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../source/language/translations/${TS_NAME}.qm")
            add_custom_command(
                OUTPUT "${QM_FILE}"
                COMMAND "${MOCKS_LRELEASE_EXECUTABLE}" "${TS_FILE}" -qm "${QM_FILE}"
                DEPENDS "${TS_FILE}"
                COMMENT "Compilando ${TS_FILE} para ${QM_FILE}"
                VERBATIM
            )
            list(APPEND QM_FILES "${QM_FILE}")
        endforeach()
        if(QM_FILES)
            add_custom_target(CompileTranslationsQm ALL DEPENDS ${QM_FILES})
        endif()
    endif()

    # Comandos de deploy sequenciais no target MocksDeploy
    set(DEPLOY_COMMANDS)

    # 1. Copiar somente o icone necessario em runtime
    if(EXISTS "${MOCKS_WINDOWS_ICON}")
        list(APPEND DEPLOY_COMMANDS
            COMMAND "${CMAKE_COMMAND}" -E rm -rf "$<TARGET_FILE_DIR:find_VersionEditor>/icones"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:find_VersionEditor>/icones"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${MOCKS_WINDOWS_ICON}"
                "$<TARGET_FILE_DIR:find_VersionEditor>/icones/${MOCKS_WINDOWS_ICON_NAME}"
        )
    endif()

    # 2. Executar windeployqt com conjunto minimo de modulos e plugins
    set(MOCKS_WINDEPLOYQT_QTPATHS_ARGS)
    if(EXISTS "${MOCKS_QTPATHS_EXECUTABLE}")
        set(MOCKS_WINDEPLOYQT_QTPATHS_ARGS --qtpaths "${MOCKS_QTPATHS_EXECUTABLE}")
    endif()
    set(MOCKS_WINDEPLOYQT_FLAGS
        --verbose 0
        --no-translations
        --no-compiler-runtime
        --no-system-d3d-compiler
        --no-system-dxc-compiler
        --no-opengl-sw
        --no-svg
        --no-pdf
        --no-network
        --skip-plugin-types generic,iconengines,networkinformation,tls
        --exclude-plugins qgif,qicns,qjpeg,qpdf,qsvg,qtga,qtiff,qwbmp,qwebp
    )
    if(WIN32 AND EXISTS "${MOCKS_WINDEPLOYQT_EXECUTABLE}")
        list(APPEND DEPLOY_COMMANDS
            COMMAND "${MOCKS_WINDEPLOYQT_EXECUTABLE}"
                ${MOCKS_WINDEPLOYQT_FLAGS}
                $<$<CONFIG:Debug>:--debug>$<$<NOT:$<CONFIG:Debug>>:--release>
                ${MOCKS_WINDEPLOYQT_QTPATHS_ARGS}
                "$<TARGET_FILE:find_VersionEditor>"
        )
    endif()

    # 3. Copia explicita de seguranca de DLLs do Qt (apenas modulos efetivamente ligados aos mocks)
    set(CRITICAL_DLLS Qt6Core.dll Qt6Gui.dll Qt6Widgets.dll)
    foreach(DLL_NAME IN LISTS CRITICAL_DLLS)
        if(EXISTS "${MOCKS_QT_BIN_DIR}/${DLL_NAME}")
            list(APPEND DEPLOY_COMMANDS
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                    "${MOCKS_QT_BIN_DIR}/${DLL_NAME}"
                    "$<TARGET_FILE_DIR:find_VersionEditor>/${DLL_NAME}"
            )
        endif()
    endforeach()

    # 4. Garantir plugins essenciais (qwindows, qmodernwindowsstyle, qico)
    foreach(PLUGIN_REL_PATH IN ITEMS
        "platforms/qwindows.dll"
        "styles/qmodernwindowsstyle.dll"
        "imageformats/qico.dll"
    )
        if(EXISTS "${MOCKS_QT_PREFIX_DIR}/plugins/${PLUGIN_REL_PATH}")
            get_filename_component(PLUGIN_SUBDIR "${PLUGIN_REL_PATH}" DIRECTORY)
            list(APPEND DEPLOY_COMMANDS
                COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:find_VersionEditor>/${PLUGIN_SUBDIR}"
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                    "${MOCKS_QT_PREFIX_DIR}/plugins/${PLUGIN_REL_PATH}"
                    "$<TARGET_FILE_DIR:find_VersionEditor>/${PLUGIN_REL_PATH}"
            )
        endif()
    endforeach()

    # 5. Copiar runtimes do compilador (MinGW ou MSVC) e isolar toolchains
    if(MINGW)
        get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
        list(APPEND DEPLOY_COMMANDS
            COMMAND "${CMAKE_COMMAND}"
                "-DDEPLOY_ROOT=$<TARGET_FILE_DIR:find_VersionEditor>"
                "-DMINGW_BIN_DIR=${MINGW_BIN_DIR}"
                "-DQT_BIN_DIR=${MOCKS_QT_BIN_DIR}"
                -P "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/copy_mingw_runtime.cmake"
        )
    elseif(MSVC)
        list(APPEND DEPLOY_COMMANDS
            COMMAND "${CMAKE_COMMAND}"
                "-DDEPLOY_ROOT=$<TARGET_FILE_DIR:find_VersionEditor>"
                "-DMSVC_REDIST_ROOT=${MOCKS_MSVC_REDIST_ROOT}"
                "-DMSVC_TOOLSET_VERSION=${MOCKS_MSVC_TOOLSET_VERSION}"
                "-DMSVC_RUNTIME_ARCH=${MOCKS_MSVC_RUNTIME_ARCH}"
                "-DCONFIG=$<CONFIG>"
                -P "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/copy_msvc_runtime.cmake"
        )
    endif()

    set(MOCKS_PRUNE_MSVC_ON_MINGW OFF)
    set(MOCKS_PRUNE_MINGW_ON_MSVC OFF)
    if(WIN32 AND NOT MSVC)
        set(MOCKS_PRUNE_MSVC_ON_MINGW ON)
    elseif(MSVC)
        set(MOCKS_PRUNE_MINGW_ON_MSVC ON)
    endif()

    list(APPEND DEPLOY_COMMANDS
        COMMAND "${CMAKE_COMMAND}"
            "-DDEPLOY_ROOT=$<TARGET_FILE_DIR:find_VersionEditor>"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/remove_compiler_runtime_installer.cmake"
        COMMAND "${CMAKE_COMMAND}"
            "-DDEPLOY_ROOT=$<TARGET_FILE_DIR:find_VersionEditor>"
            "-DPRUNE_MSVC_RUNTIME=${MOCKS_PRUNE_MSVC_ON_MINGW}"
            "-DPRUNE_MINGW_RUNTIME=${MOCKS_PRUNE_MINGW_ON_MSVC}"
            "-DPRUNE_PRINTSUPPORT=ON"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/prune_optional_root_runtime_dlls.cmake"
    )

    # 6. Deploy para diretorio de distribuicao externa (MOCKS_EXPORT_DIR)
    if(MOCKS_EXPORT_DIR_NORMALIZED)
        list(APPEND DEPLOY_COMMANDS
            COMMAND "${CMAKE_COMMAND}" -E rm -rf "${MOCKS_EXPORT_DIR_NORMALIZED}"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${MOCKS_EXPORT_DIR_NORMALIZED}"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:find_VersionEditor>" "${MOCKS_EXPORT_DIR_NORMALIZED}/find_VersionEditor.exe"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:CompileTranslationsGUI>" "${MOCKS_EXPORT_DIR_NORMALIZED}/CompileTranslationsGUI.exe"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:CompileTranslationsTerminal>" "${MOCKS_EXPORT_DIR_NORMALIZED}/CompileTranslationsTerminal.exe"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE_DIR:find_VersionEditor>/qt.conf" "${MOCKS_EXPORT_DIR_NORMALIZED}/qt.conf"
        )
        if(EXISTS "${MOCKS_WINDOWS_ICON}")
            list(APPEND DEPLOY_COMMANDS
                COMMAND "${CMAKE_COMMAND}" -E make_directory "${MOCKS_EXPORT_DIR_NORMALIZED}/icones"
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                    "${MOCKS_WINDOWS_ICON}"
                    "${MOCKS_EXPORT_DIR_NORMALIZED}/icones/${MOCKS_WINDOWS_ICON_NAME}"
            )
        endif()
        if(WIN32 AND EXISTS "${MOCKS_WINDEPLOYQT_EXECUTABLE}")
            list(APPEND DEPLOY_COMMANDS
                COMMAND "${MOCKS_WINDEPLOYQT_EXECUTABLE}"
                    ${MOCKS_WINDEPLOYQT_FLAGS}
                    $<$<CONFIG:Debug>:--debug>$<$<NOT:$<CONFIG:Debug>>:--release>
                    ${MOCKS_WINDEPLOYQT_QTPATHS_ARGS}
                    "${MOCKS_EXPORT_DIR_NORMALIZED}/find_VersionEditor.exe"
            )
        endif()
        foreach(DLL_NAME IN LISTS CRITICAL_DLLS)
            if(EXISTS "${MOCKS_QT_BIN_DIR}/${DLL_NAME}")
                list(APPEND DEPLOY_COMMANDS
                    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                        "${MOCKS_QT_BIN_DIR}/${DLL_NAME}"
                        "${MOCKS_EXPORT_DIR_NORMALIZED}/${DLL_NAME}"
                )
            endif()
        endforeach()
        foreach(PLUGIN_REL_PATH IN ITEMS
            "platforms/qwindows.dll"
            "styles/qmodernwindowsstyle.dll"
            "imageformats/qico.dll"
        )
            if(EXISTS "${MOCKS_QT_PREFIX_DIR}/plugins/${PLUGIN_REL_PATH}")
                get_filename_component(PLUGIN_SUBDIR "${PLUGIN_REL_PATH}" DIRECTORY)
                list(APPEND DEPLOY_COMMANDS
                    COMMAND "${CMAKE_COMMAND}" -E make_directory "${MOCKS_EXPORT_DIR_NORMALIZED}/${PLUGIN_SUBDIR}"
                    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                        "${MOCKS_QT_PREFIX_DIR}/plugins/${PLUGIN_REL_PATH}"
                        "${MOCKS_EXPORT_DIR_NORMALIZED}/${PLUGIN_REL_PATH}"
                )
            endif()
        endforeach()
        if(MINGW)
            get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
            list(APPEND DEPLOY_COMMANDS
                COMMAND "${CMAKE_COMMAND}"
                    "-DDEPLOY_ROOT=${MOCKS_EXPORT_DIR_NORMALIZED}"
                    "-DMINGW_BIN_DIR=${MINGW_BIN_DIR}"
                    "-DQT_BIN_DIR=${MOCKS_QT_BIN_DIR}"
                    -P "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/copy_mingw_runtime.cmake"
            )
        elseif(MSVC)
            list(APPEND DEPLOY_COMMANDS
                COMMAND "${CMAKE_COMMAND}"
                    "-DDEPLOY_ROOT=${MOCKS_EXPORT_DIR_NORMALIZED}"
                    "-DMSVC_REDIST_ROOT=${MOCKS_MSVC_REDIST_ROOT}"
                    "-DMSVC_TOOLSET_VERSION=${MOCKS_MSVC_TOOLSET_VERSION}"
                    "-DMSVC_RUNTIME_ARCH=${MOCKS_MSVC_RUNTIME_ARCH}"
                    "-DCONFIG=$<CONFIG>"
                    -P "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/copy_msvc_runtime.cmake"
            )
        endif()
        list(APPEND DEPLOY_COMMANDS
            COMMAND "${CMAKE_COMMAND}"
                "-DDEPLOY_ROOT=${MOCKS_EXPORT_DIR_NORMALIZED}"
                -P "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/remove_compiler_runtime_installer.cmake"
            COMMAND "${CMAKE_COMMAND}"
                "-DDEPLOY_ROOT=${MOCKS_EXPORT_DIR_NORMALIZED}"
                "-DPRUNE_MSVC_RUNTIME=${MOCKS_PRUNE_MSVC_ON_MINGW}"
                "-DPRUNE_MINGW_RUNTIME=${MOCKS_PRUNE_MINGW_ON_MSVC}"
                "-DPRUNE_PRINTSUPPORT=ON"
                -P "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/prune_optional_root_runtime_dlls.cmake"
        )
    endif()

    add_custom_target(
        MocksDeploy ALL
        DEPENDS
            CompileTranslationsGUI
            CompileTranslationsTerminal
            find_VersionEditor
        ${DEPLOY_COMMANDS}
        COMMENT "Implantando dependencias e runtimes para os executaveis de mocks"
        VERBATIM
    )

    add_custom_target(find_VersionEditorDeploy ALL DEPENDS MocksDeploy)
    add_custom_target(CompileTranslationsGUIDeploy ALL DEPENDS MocksDeploy)
    add_custom_target(CompileTranslationsTerminalDeploy ALL DEPENDS MocksDeploy)

    # Executores
    if(WIN32)
        set(MOCKS_DETACHED_GUI_LAUNCHER "${CMAKE_CURRENT_SOURCE_DIR}/launch_detached_gui.cmake")

        add_custom_target(
            MocksRun
            DEPENDS MocksDeploy
            COMMAND "${CMAKE_COMMAND}"
                "-DLAUNCH_FILE=$<TARGET_FILE:find_VersionEditor>"
                "-DLAUNCH_WORKING_DIR=$<TARGET_FILE_DIR:find_VersionEditor>"
                -P "${MOCKS_DETACHED_GUI_LAUNCHER}"
            COMMAND "${CMAKE_COMMAND}"
                "-DLAUNCH_FILE=$<TARGET_FILE:CompileTranslationsGUI>"
                "-DLAUNCH_WORKING_DIR=$<TARGET_FILE_DIR:CompileTranslationsGUI>"
                -P "${MOCKS_DETACHED_GUI_LAUNCHER}"
            COMMENT "Iniciando ferramentas mocks (find_VersionEditor e CompileTranslationsGUI)"
            VERBATIM
            USES_TERMINAL
        )

        add_custom_target(
            find_VersionEditorRun
            DEPENDS find_VersionEditorDeploy
            COMMAND "${CMAKE_COMMAND}"
                "-DLAUNCH_FILE=$<TARGET_FILE:find_VersionEditor>"
                "-DLAUNCH_WORKING_DIR=$<TARGET_FILE_DIR:find_VersionEditor>"
                -P "${MOCKS_DETACHED_GUI_LAUNCHER}"
            VERBATIM
            USES_TERMINAL
        )

        add_custom_target(
            CompileTranslationsGUIRunAll
            DEPENDS CompileTranslationsGUIDeploy
            COMMAND "${CMAKE_COMMAND}"
                "-DLAUNCH_FILE=$<TARGET_FILE:CompileTranslationsGUI>"
                "-DLAUNCH_WORKING_DIR=$<TARGET_FILE_DIR:CompileTranslationsGUI>"
                "-DLAUNCH_ARGS=--all"
                -P "${MOCKS_DETACHED_GUI_LAUNCHER}"
            VERBATIM
            USES_TERMINAL
        )

        add_custom_target(
            CompileTranslationsGUIRunEconomia
            DEPENDS CompileTranslationsGUIDeploy
            COMMAND "${CMAKE_COMMAND}"
                "-DLAUNCH_FILE=$<TARGET_FILE:CompileTranslationsGUI>"
                "-DLAUNCH_WORKING_DIR=$<TARGET_FILE_DIR:CompileTranslationsGUI>"
                "-DLAUNCH_ARGS=--module=economia"
                -P "${MOCKS_DETACHED_GUI_LAUNCHER}"
            VERBATIM
            USES_TERMINAL
        )

        add_custom_target(
            CompileTranslationsTerminalRunAll
            DEPENDS CompileTranslationsTerminalDeploy
            COMMAND "$<TARGET_FILE:CompileTranslationsTerminal>" --all
            WORKING_DIRECTORY "$<TARGET_FILE_DIR:CompileTranslationsTerminal>"
            VERBATIM
            USES_TERMINAL
        )

        add_custom_target(
            CompileTranslationsTerminalRunEconomia
            DEPENDS CompileTranslationsTerminalDeploy
            COMMAND "$<TARGET_FILE:CompileTranslationsTerminal>" --module economia
            WORKING_DIRECTORY "$<TARGET_FILE_DIR:CompileTranslationsTerminal>"
            VERBATIM
            USES_TERMINAL
        )
    endif()
endif()
