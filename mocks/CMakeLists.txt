cmake_minimum_required(VERSION 3.20)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(Economia_APP_Mocks LANGUAGES CXX)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)
    find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets PrintSupport)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

if(WIN32)
    enable_language(RC)
endif()

find_package(Qt6 QUIET COMPONENTS LinguistTools)

get_filename_component(MOCKS_QT_PREFIX_DIR "${Qt6_DIR}/../../.." ABSOLUTE)
set(MOCKS_QT_BIN_DIR "${MOCKS_QT_PREFIX_DIR}/bin")
set(MOCKS_WINDEPLOYQT_EXECUTABLE "${MOCKS_QT_BIN_DIR}/windeployqt.exe")
set(MOCKS_QTPATHS_EXECUTABLE "${MOCKS_QT_BIN_DIR}/qtpaths6.exe")

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

if(MOCKS_EXPORT_DIR)
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
        set_property(TARGET "${TARGET_NAME}" PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
        target_compile_options(
            "${TARGET_NAME}"
            PRIVATE
                $<$<CONFIG:Release>:/O2>
                $<$<CONFIG:Release>:/Oi>
                $<$<CONFIG:Release>:/Ot>
        )
    elseif(MINGW)
        target_compile_options(
            "${TARGET_NAME}"
            PRIVATE
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

# Gerar qt.conf para garantir isolamento e carregamento local de plugins nos mocks
file(GENERATE
    OUTPUT "${CMAKE_BINARY_DIR}/qt.conf"
    CONTENT "[Paths]\nPrefix = .\nPlugins = .\n"
)

# Target para lrelease
if(TARGET Qt6::lrelease)
    file(GLOB TS_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../source/language/translations/*.ts")
    set(QM_FILES)
    foreach(TS_FILE IN LISTS TS_FILES)
        get_filename_component(TS_NAME "${TS_FILE}" NAME_WE)
        set(QM_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../source/language/translations/${TS_NAME}.qm")
        add_custom_command(
            OUTPUT "${QM_FILE}"
            COMMAND Qt6::lrelease "${TS_FILE}" -qm "${QM_FILE}"
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

# 1. Copiar icones
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/icones")
    list(APPEND DEPLOY_COMMANDS
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_BINARY_DIR}/icones"
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/icones" "${CMAKE_BINARY_DIR}/icones"
    )
endif()

# 2. Executar windeployqt
if(WIN32 AND EXISTS "${MOCKS_WINDEPLOYQT_EXECUTABLE}")
    list(APPEND DEPLOY_COMMANDS
        COMMAND "${MOCKS_WINDEPLOYQT_EXECUTABLE}"
            --verbose 0
            --no-translations
            --compiler-runtime
            "${CMAKE_BINARY_DIR}/find_VersionEditor.exe"
    )
endif()

# 3. Copia explicita de seguranca de DLLs do Qt
set(CRITICAL_DLLS Qt6Core.dll Qt6Gui.dll Qt6Widgets.dll Qt6PrintSupport.dll)
foreach(DLL_NAME IN LISTS CRITICAL_DLLS)
    if(EXISTS "${MOCKS_QT_BIN_DIR}/${DLL_NAME}")
        list(APPEND DEPLOY_COMMANDS
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${MOCKS_QT_BIN_DIR}/${DLL_NAME}"
                "${CMAKE_BINARY_DIR}/${DLL_NAME}"
        )
    endif()
endforeach()

# 4. Garantir qwindows.dll
if(EXISTS "${MOCKS_QT_PREFIX_DIR}/plugins/platforms/qwindows.dll")
    list(APPEND DEPLOY_COMMANDS
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_BINARY_DIR}/platforms"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${MOCKS_QT_PREFIX_DIR}/plugins/platforms/qwindows.dll"
            "${CMAKE_BINARY_DIR}/platforms/qwindows.dll"
    )
endif()

# 5. Copiar runtimes do MinGW (libgcc_s_seh-1.dll, libstdc++-6.dll, libwinpthread-1.dll)
if(MINGW)
    get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    set(GCC_RUNTIMES libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll)
    foreach(GCC_DLL IN LISTS GCC_RUNTIMES)
        if(EXISTS "${MINGW_BIN_DIR}/${GCC_DLL}")
            list(APPEND DEPLOY_COMMANDS
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                    "${MINGW_BIN_DIR}/${GCC_DLL}"
                    "${CMAKE_BINARY_DIR}/${GCC_DLL}"
            )
        elseif(EXISTS "${MOCKS_QT_BIN_DIR}/${GCC_DLL}")
            list(APPEND DEPLOY_COMMANDS
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                    "${MOCKS_QT_BIN_DIR}/${GCC_DLL}"
                    "${CMAKE_BINARY_DIR}/${GCC_DLL}"
            )
        endif()
    endforeach()
endif()

# 6. Deploy para diretorio de distribuicao externa (MOCKS_EXPORT_DIR)
if(MOCKS_EXPORT_DIR_NORMALIZED)
    list(APPEND DEPLOY_COMMANDS
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${MOCKS_EXPORT_DIR_NORMALIZED}"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${MOCKS_EXPORT_DIR_NORMALIZED}/icones"
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/icones" "${MOCKS_EXPORT_DIR_NORMALIZED}/icones"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_BINARY_DIR}/find_VersionEditor.exe" "${MOCKS_EXPORT_DIR_NORMALIZED}/find_VersionEditor.exe"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_BINARY_DIR}/CompileTranslationsGUI.exe" "${MOCKS_EXPORT_DIR_NORMALIZED}/CompileTranslationsGUI.exe"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_BINARY_DIR}/CompileTranslationsTerminal.exe" "${MOCKS_EXPORT_DIR_NORMALIZED}/CompileTranslationsTerminal.exe"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_BINARY_DIR}/qt.conf" "${MOCKS_EXPORT_DIR_NORMALIZED}/qt.conf"
    )
    if(WIN32 AND EXISTS "${MOCKS_WINDEPLOYQT_EXECUTABLE}")
        list(APPEND DEPLOY_COMMANDS
            COMMAND "${MOCKS_WINDEPLOYQT_EXECUTABLE}"
                --verbose 0
                --no-translations
                --compiler-runtime
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
    if(EXISTS "${MOCKS_QT_PREFIX_DIR}/plugins/platforms/qwindows.dll")
        list(APPEND DEPLOY_COMMANDS
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${MOCKS_EXPORT_DIR_NORMALIZED}/platforms"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${MOCKS_QT_PREFIX_DIR}/plugins/platforms/qwindows.dll"
                "${MOCKS_EXPORT_DIR_NORMALIZED}/platforms/qwindows.dll"
        )
    endif()
    if(MINGW)
        get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
        set(GCC_RUNTIMES libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll)
        foreach(GCC_DLL IN LISTS GCC_RUNTIMES)
            if(EXISTS "${MINGW_BIN_DIR}/${GCC_DLL}")
                list(APPEND DEPLOY_COMMANDS
                    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                        "${MINGW_BIN_DIR}/${GCC_DLL}"
                        "${MOCKS_EXPORT_DIR_NORMALIZED}/${GCC_DLL}"
                )
            elseif(EXISTS "${MOCKS_QT_BIN_DIR}/${GCC_DLL}")
                list(APPEND DEPLOY_COMMANDS
                    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                        "${MOCKS_QT_BIN_DIR}/${GCC_DLL}"
                        "${MOCKS_EXPORT_DIR_NORMALIZED}/${GCC_DLL}"
                )
            endif()
        endforeach()
    endif()
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
            "-DLAUNCH_FILE=${CMAKE_BINARY_DIR}/find_VersionEditor.exe"
            "-DLAUNCH_WORKING_DIR=${CMAKE_BINARY_DIR}"
            -P "${MOCKS_DETACHED_GUI_LAUNCHER}"
        COMMAND "${CMAKE_COMMAND}"
            "-DLAUNCH_FILE=${CMAKE_BINARY_DIR}/CompileTranslationsGUI.exe"
            "-DLAUNCH_WORKING_DIR=${CMAKE_BINARY_DIR}"
            -P "${MOCKS_DETACHED_GUI_LAUNCHER}"
        COMMENT "Iniciando ferramentas mocks (find_VersionEditor e CompileTranslationsGUI)"
        VERBATIM
        USES_TERMINAL
    )

    add_custom_target(
        find_VersionEditorRun
        DEPENDS find_VersionEditorDeploy
        COMMAND "${CMAKE_COMMAND}"
            "-DLAUNCH_FILE=${CMAKE_BINARY_DIR}/find_VersionEditor.exe"
            "-DLAUNCH_WORKING_DIR=${CMAKE_BINARY_DIR}"
            -P "${MOCKS_DETACHED_GUI_LAUNCHER}"
        VERBATIM
        USES_TERMINAL
    )

    add_custom_target(
        CompileTranslationsGUIRunAll
        DEPENDS CompileTranslationsGUIDeploy
        COMMAND "${CMAKE_COMMAND}"
            "-DLAUNCH_FILE=${CMAKE_BINARY_DIR}/CompileTranslationsGUI.exe"
            "-DLAUNCH_WORKING_DIR=${CMAKE_BINARY_DIR}"
            "-DLAUNCH_ARGS=--all"
            -P "${MOCKS_DETACHED_GUI_LAUNCHER}"
        VERBATIM
        USES_TERMINAL
    )

    add_custom_target(
        CompileTranslationsGUIRunEconomia
        DEPENDS CompileTranslationsGUIDeploy
        COMMAND "${CMAKE_COMMAND}"
            "-DLAUNCH_FILE=${CMAKE_BINARY_DIR}/CompileTranslationsGUI.exe"
            "-DLAUNCH_WORKING_DIR=${CMAKE_BINARY_DIR}"
            "-DLAUNCH_ARGS=--module=economia"
            -P "${MOCKS_DETACHED_GUI_LAUNCHER}"
        VERBATIM
        USES_TERMINAL
    )

    add_custom_target(
        CompileTranslationsTerminalRunAll
        DEPENDS CompileTranslationsTerminalDeploy
        COMMAND "${CMAKE_BINARY_DIR}/CompileTranslationsTerminal.exe" --all
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        VERBATIM
        USES_TERMINAL
    )

    add_custom_target(
        CompileTranslationsTerminalRunEconomia
        DEPENDS CompileTranslationsTerminalDeploy
        COMMAND "${CMAKE_BINARY_DIR}/CompileTranslationsTerminal.exe" --module economia
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        VERBATIM
        USES_TERMINAL
    )
endif()
