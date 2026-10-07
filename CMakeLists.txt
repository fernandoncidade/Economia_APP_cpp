cmake_minimum_required(VERSION 3.20)
project(Economia_APP_cpp VERSION 2026.09.29 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    enable_language(RC)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(ECONOMIA_BUILD_HOST_APP "Construir a aplicacao principal Economia_APP" ON)
option(ECONOMIA_BUILD_MOCKS "Construir os utilitarios e mocks do Economia_APP" OFF)

set(APP_ASSETS_SUBDIR "economia")
set(APP_WINDOWS_ICON_NAME "economia.ico")
set(APP_EXECUTABLE_NAME "Economia_APP")
set(APP_EXPORT_DIR_BASENAME "Economia_APP_cpp")
set(APP_EXPORT_ROOT_DIR "D:/MISCELANEAS/VSCode/Economia_APP/Economia_APP_cpp")

if(WIN32)
    if(MSVC)
        set(ECONOMIA_DETECTED_TOOLCHAIN_SUFFIX "msvc")
    elseif(MINGW)
        set(ECONOMIA_DETECTED_TOOLCHAIN_SUFFIX "mingw")
    else()
        set(ECONOMIA_DETECTED_TOOLCHAIN_SUFFIX "windows")
    endif()

    if(NOT DEFINED ECONOMIA_BUILD_TOOLCHAIN_SUFFIX OR ECONOMIA_BUILD_TOOLCHAIN_SUFFIX STREQUAL "generic")
        set(ECONOMIA_BUILD_TOOLCHAIN_SUFFIX "${ECONOMIA_DETECTED_TOOLCHAIN_SUFFIX}" CACHE STRING "Suffix used for toolchain-specific build directories")
    endif()
endif()

set(ECONOMIA_DEFAULT_EXPORT_DIR "${APP_EXPORT_ROOT_DIR}/${APP_EXPORT_DIR_BASENAME}")
if(WIN32)
    if(ECONOMIA_BUILD_TOOLCHAIN_SUFFIX MATCHES "^msvc")
        set(ECONOMIA_DEFAULT_EXPORT_DIR "${APP_EXPORT_ROOT_DIR}/${APP_EXPORT_DIR_BASENAME}_msvc")
    elseif(ECONOMIA_BUILD_TOOLCHAIN_SUFFIX STREQUAL "mingw")
        set(ECONOMIA_DEFAULT_EXPORT_DIR "${APP_EXPORT_ROOT_DIR}/${APP_EXPORT_DIR_BASENAME}_mingw")
    endif()
endif()

set(
    ECONOMIA_EXPORT_DIR
    "${ECONOMIA_DEFAULT_EXPORT_DIR}"
    CACHE PATH
    "Additional directory that receives the deployed application runtime"
)

if(ECONOMIA_EXPORT_DIR)
    file(TO_CMAKE_PATH "${ECONOMIA_EXPORT_DIR}" ECONOMIA_EXPORT_DIR_NORMALIZED)
else()
    set(ECONOMIA_EXPORT_DIR_NORMALIZED "")
endif()

set(ECONOMIA_MSVC_REDIST_ROOT "")
set(ECONOMIA_MSVC_RUNTIME_ARCH "x64")
if(MSVC)
    if(DEFINED CMAKE_MSVC_REDIST_DIR AND EXISTS "${CMAKE_MSVC_REDIST_DIR}")
        set(ECONOMIA_MSVC_REDIST_ROOT "${CMAKE_MSVC_REDIST_DIR}")
    elseif(CMAKE_CXX_COMPILER MATCHES "/VC/Tools/MSVC/")
        get_filename_component(ECONOMIA_MSVC_COMPILER_ARCH_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
        get_filename_component(ECONOMIA_MSVC_COMPILER_HOST_DIR "${ECONOMIA_MSVC_COMPILER_ARCH_DIR}" DIRECTORY)
        get_filename_component(ECONOMIA_MSVC_COMPILER_BIN_DIR "${ECONOMIA_MSVC_COMPILER_HOST_DIR}" DIRECTORY)
        get_filename_component(ECONOMIA_MSVC_VERSION_DIR "${ECONOMIA_MSVC_COMPILER_BIN_DIR}" DIRECTORY)
        get_filename_component(ECONOMIA_MSVC_TOOLS_MSVC_DIR "${ECONOMIA_MSVC_VERSION_DIR}" DIRECTORY)
        get_filename_component(ECONOMIA_MSVC_TOOLS_DIR "${ECONOMIA_MSVC_TOOLS_MSVC_DIR}" DIRECTORY)
        get_filename_component(ECONOMIA_MSVC_VC_DIR "${ECONOMIA_MSVC_TOOLS_DIR}" DIRECTORY)
        if(EXISTS "${ECONOMIA_MSVC_VC_DIR}/Redist/MSVC")
            set(ECONOMIA_MSVC_REDIST_ROOT "${ECONOMIA_MSVC_VC_DIR}/Redist/MSVC")
        endif()
    endif()

    if(ECONOMIA_MSVC_REDIST_ROOT STREQUAL "")
        file(GLOB ECONOMIA_MSVC_REDIST_ROOT_CANDIDATES LIST_DIRECTORIES true
            "C:/Program Files/Microsoft Visual Studio/*/*/VC/Redist/MSVC"
            "C:/Program Files (x86)/Microsoft Visual Studio/*/*/VC/Redist/MSVC"
        )
        if(ECONOMIA_MSVC_REDIST_ROOT_CANDIDATES)
            list(SORT ECONOMIA_MSVC_REDIST_ROOT_CANDIDATES)
            list(REVERSE ECONOMIA_MSVC_REDIST_ROOT_CANDIDATES)
            list(GET ECONOMIA_MSVC_REDIST_ROOT_CANDIDATES 0 ECONOMIA_MSVC_REDIST_ROOT)
        endif()
    endif()
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets PrintSupport)

get_filename_component(QT_PREFIX_DIR "${Qt6_DIR}/../../.." ABSOLUTE)
set(QT_BIN_DIR "${QT_PREFIX_DIR}/bin")
set(WINDEPLOYQT_EXE "${QT_BIN_DIR}/windeployqt.exe")
set(QTPATHS_EXE "${QT_BIN_DIR}/qtpaths6.exe")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/source
    ${CMAKE_CURRENT_SOURCE_DIR}/source/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/source/services
    ${CMAKE_CURRENT_SOURCE_DIR}/source/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/source/language
)

if(ECONOMIA_BUILD_HOST_APP)
    # Coleta de fontes
    set(SOURCES_UTILS
        source/utils/LogManager.cpp
        source/utils/CaminhoPersistenteUtils.cpp
        source/utils/ApplicationPathUtils.cpp
        source/utils/IconUtils.cpp
        source/utils/FontManager.cpp
        source/utils/TextFormat.cpp
        source/utils/MathRenderer.cpp
        source/utils/TrialManager.cpp
    )

    set(SOURCES_LANGUAGE
        source/language/tr_01_gerenciadorTraducao.cpp
        source/language/tr_02_compileTranslations.cpp
    )

    set(SOURCES_UI
        source/ui/ui_01_create_layout.cpp
        source/ui/ui_02_get_float_from_line_edit.cpp
        source/ui/ui_03_create_interest_tab.cpp
        source/ui/ui_04_create_annuity_tab.cpp
        source/ui/ui_05_create_gradient_tab.cpp
        source/ui/ui_06_create_rates_tab.cpp
        source/ui/ui_07_create_amortization_tab.cpp
        source/ui/ui_08_create_investment_tab.cpp
        source/ui/ui_09_create_depreciation_tab.cpp
        source/ui/ui_10_create_effective_rate_tab.cpp
        source/ui/ui_11_create_minimum_return_tab.cpp
        source/ui/ui_12_create_fisher_tab.cpp
        source/ui/ui_13_create_vpl_tax_tab.cpp
        source/ui/ui_14_create_caue_tab.cpp
        source/ui/ui_15_generate_sac_table.cpp
        source/ui/ui_16_generate_sam_table.cpp
        source/ui/ui_17_generate_price_table.cpp
        source/ui/ui_18_generate_hamburgues_table.cpp
        source/ui/ui_19_generate_american_table.cpp
        source/ui/ui_20_generate_caue_input_table.cpp
        source/ui/ui_21_set_amort_table_row.cpp
        source/ui/ui_22_get_table_data.cpp
        source/ui/ui_23_history_container.cpp
        source/ui/ui_24_font_config_dialog.cpp
        source/ui/ui_25_export_pdf.cpp
        source/ui/ui_26_menu_bar.cpp
        source/ui/ui_27_SobreDialog.cpp
        source/ui/ui_28_exibir_sobre.cpp
        source/ui/ui_29_opcoes_sobre.cpp
        source/ui/ui_30_Manual.cpp
        source/ui/ui_31_manual_pt_BR.cpp
        source/ui/ui_32_manual_en_US.cpp
        source/ui/ui_33_exibir_manual.cpp
    )

    set(SOURCES_SERVICES
        source/services/sv_01_calculate_interest.cpp
        source/services/sv_02_calculate_annuity.cpp
        source/services/sv_03_calculate_gradient.cpp
        source/services/sv_04_calculate_real_rate_equivalence.cpp
        source/services/sv_05_calculate_amortization.cpp
        source/services/sv_06_calculate_investment.cpp
        source/services/sv_07_calculate_depreciation.cpp
        source/services/sv_08_calculate_effective_rate.cpp
        source/services/sv_09_calculate_minimum_return.cpp
        source/services/sv_10_calculate_fisher.cpp
        source/services/sv_11_calculate_value_at_k.cpp
        source/services/sv_12_calculate_vpl_with_taxes.cpp
        source/services/sv_13_calculate_caue.cpp
    )

    set(ECONOMIA_APP_RC)
    if(WIN32)
        set(ECONOMIA_APP_RC "${CMAKE_CURRENT_SOURCE_DIR}/source/assets/icones/Economia_APP.rc")
    endif()

    # Executável principal Economia_APP
    add_executable(Economia_APP
        WIN32
        main.cpp
        source/fca_01_FinancialCalculatorAPP.cpp
        ${SOURCES_UTILS}
        ${SOURCES_LANGUAGE}
        ${SOURCES_UI}
        ${SOURCES_SERVICES}
        ${ECONOMIA_APP_RC}
    )

    target_include_directories(Economia_APP PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/source/assets/icones"
    )

    target_link_libraries(Economia_APP PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::PrintSupport
    )

    # Gerar qt.conf para garantir isolamento e carregamento local de plugins
    file(GENERATE
        OUTPUT "$<TARGET_FILE_DIR:Economia_APP>/qt.conf"
        CONTENT "[Paths]\nPrefix = .\nPlugins = .\n"
    )

    # Teste 1 (Harness)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/teste.cpp")
        add_executable(teste
            WIN32
            teste.cpp
            ${SOURCES_UTILS}
            ${SOURCES_LANGUAGE}
            ${SOURCES_UI}
            ${SOURCES_SERVICES}
            source/fca_01_FinancialCalculatorAPP.cpp
        )
        target_link_libraries(teste PRIVATE
            Qt6::Core
            Qt6::Gui
            Qt6::Widgets
            Qt6::PrintSupport
        )
    endif()

    # Compilar traduções com lrelease automaticamente
    if(EXISTS "${QT_BIN_DIR}/lrelease.exe")
        add_custom_target(CompileTranslations
            COMMAND "${QT_BIN_DIR}/lrelease.exe"
                "${CMAKE_CURRENT_SOURCE_DIR}/source/language/translations/economia_pt_BR.ts"
                -qm "${CMAKE_CURRENT_SOURCE_DIR}/source/language/translations/economia_pt_BR.qm"
            COMMAND "${QT_BIN_DIR}/lrelease.exe"
                "${CMAKE_CURRENT_SOURCE_DIR}/source/language/translations/economia_en_US.ts"
                -qm "${CMAKE_CURRENT_SOURCE_DIR}/source/language/translations/economia_en_US.qm"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/translations"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/source/language/translations/economia_pt_BR.qm"
                "${CMAKE_CURRENT_BINARY_DIR}/translations/economia_pt_BR.qm"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/source/language/translations/economia_en_US.qm"
                "${CMAKE_CURRENT_BINARY_DIR}/translations/economia_en_US.qm"
            COMMENT "Compilando arquivos .ts para .qm com lrelease"
        )
        add_dependencies(Economia_APP CompileTranslations)
        if(TARGET teste)
            add_dependencies(teste CompileTranslations)
        endif()
    endif()

    # Comandos de deploy para build tree e distribuição externa
    set(ECONOMIA_BUILD_TREE_ASSET_COMMANDS
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:Economia_APP>/assets"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/source/assets"
            "$<TARGET_FILE_DIR:Economia_APP>/assets"
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:Economia_APP>/source/assets"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/source/assets"
            "$<TARGET_FILE_DIR:Economia_APP>/source/assets"
        COMMAND "${CMAKE_COMMAND}"
            "-DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/source/language/translations"
            "-DDEST_DIR=$<TARGET_FILE_DIR:Economia_APP>/translations"
            "-DCLEAN_DESTINATION=OFF"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/copy_qm_translations.cmake"
        COMMAND ${CMAKE_COMMAND} -E rm -f
            "$<TARGET_FILE_DIR:Economia_APP>/translations/economia_pt_BR.ts"
            "$<TARGET_FILE_DIR:Economia_APP>/translations/economia_en_US.ts"
    )

    set(ECONOMIA_EXPORT_ASSET_COMMANDS)
    if(ECONOMIA_EXPORT_DIR_NORMALIZED)
        set(ECONOMIA_EXPORT_ASSET_COMMANDS
            COMMAND ${CMAKE_COMMAND} -E make_directory "${ECONOMIA_EXPORT_DIR_NORMALIZED}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<TARGET_FILE:Economia_APP>"
                "${ECONOMIA_EXPORT_DIR_NORMALIZED}/"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<TARGET_FILE_DIR:Economia_APP>/qt.conf"
                "${ECONOMIA_EXPORT_DIR_NORMALIZED}/qt.conf"
            # 1. Passar source/assets para assets
            COMMAND ${CMAKE_COMMAND} -E make_directory "${ECONOMIA_EXPORT_DIR_NORMALIZED}/assets"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/source/assets"
                "${ECONOMIA_EXPORT_DIR_NORMALIZED}/assets"
            # 2. Excluir o diretório source (incluindo source/assets e source/language)
            COMMAND ${CMAKE_COMMAND} -E rm -rf "${ECONOMIA_EXPORT_DIR_NORMALIZED}/source"
            # 3. Distribuir somente catálogos compilados (.qm) em translations, sem arquivos .ts
            COMMAND "${CMAKE_COMMAND}"
                "-DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/source/language/translations"
                "-DDEST_DIR=${ECONOMIA_EXPORT_DIR_NORMALIZED}/translations"
                "-DCLEAN_DESTINATION=OFF"
                -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/copy_qm_translations.cmake"
            COMMAND ${CMAKE_COMMAND} -E rm -f
                "${ECONOMIA_EXPORT_DIR_NORMALIZED}/translations/economia_pt_BR.ts"
                "${ECONOMIA_EXPORT_DIR_NORMALIZED}/translations/economia_en_US.ts"
        )
    endif()

    set(ECONOMIA_WINDEPLOYQT_COMMANDS)
    if(WIN32 AND EXISTS "${WINDEPLOYQT_EXE}")
        set(ECONOMIA_WINDEPLOYQT_COMMANDS
            COMMAND "${WINDEPLOYQT_EXE}"
                --verbose 0
                --no-translations
                --compiler-runtime
                "$<TARGET_FILE:Economia_APP>"
        )
        if(ECONOMIA_EXPORT_DIR_NORMALIZED)
            list(APPEND ECONOMIA_WINDEPLOYQT_COMMANDS
                COMMAND "${WINDEPLOYQT_EXE}"
                    --verbose 0
                    --no-translations
                    --compiler-runtime
                    "${ECONOMIA_EXPORT_DIR_NORMALIZED}/$<TARGET_FILE_NAME:Economia_APP>"
            )
        endif()
    endif()

    set(ECONOMIA_REMOVE_INSTALLER_COMMANDS)
    list(APPEND ECONOMIA_REMOVE_INSTALLER_COMMANDS
        COMMAND "${CMAKE_COMMAND}"
            "-DDEPLOY_ROOT=$<TARGET_FILE_DIR:Economia_APP>"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/remove_compiler_runtime_installer.cmake"
    )
    if(ECONOMIA_EXPORT_DIR_NORMALIZED)
        list(APPEND ECONOMIA_REMOVE_INSTALLER_COMMANDS
            COMMAND "${CMAKE_COMMAND}"
                "-DDEPLOY_ROOT=${ECONOMIA_EXPORT_DIR_NORMALIZED}"
                -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/remove_compiler_runtime_installer.cmake"
        )
    endif()

    set(ECONOMIA_MSVC_RUNTIME_COMMANDS)
    if(MSVC)
        list(APPEND ECONOMIA_MSVC_RUNTIME_COMMANDS
            COMMAND "${CMAKE_COMMAND}"
                "-DDEPLOY_ROOT=$<TARGET_FILE_DIR:Economia_APP>"
                "-DMSVC_REDIST_ROOT=${ECONOMIA_MSVC_REDIST_ROOT}"
                "-DMSVC_RUNTIME_ARCH=${ECONOMIA_MSVC_RUNTIME_ARCH}"
                "-DCONFIG=$<CONFIG>"
                -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/copy_msvc_runtime.cmake"
        )
        if(ECONOMIA_EXPORT_DIR_NORMALIZED)
            list(APPEND ECONOMIA_MSVC_RUNTIME_COMMANDS
                COMMAND "${CMAKE_COMMAND}"
                    "-DDEPLOY_ROOT=${ECONOMIA_EXPORT_DIR_NORMALIZED}"
                    "-DMSVC_REDIST_ROOT=${ECONOMIA_MSVC_REDIST_ROOT}"
                    "-DMSVC_RUNTIME_ARCH=${ECONOMIA_MSVC_RUNTIME_ARCH}"
                    "-DCONFIG=$<CONFIG>"
                    -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/copy_msvc_runtime.cmake"
            )
        endif()
    endif()

    set(ECONOMIA_PRUNE_DLLS_COMMANDS)
    set(ECONOMIA_PRUNE_MSVC_ON_MINGW OFF)
    if(WIN32 AND NOT MSVC)
        set(ECONOMIA_PRUNE_MSVC_ON_MINGW ON)
    endif()
    list(APPEND ECONOMIA_PRUNE_DLLS_COMMANDS
        COMMAND "${CMAKE_COMMAND}"
            "-DDEPLOY_ROOT=$<TARGET_FILE_DIR:Economia_APP>"
            "-DPRUNE_MSVC_RUNTIME=${ECONOMIA_PRUNE_MSVC_ON_MINGW}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/prune_optional_root_runtime_dlls.cmake"
    )
    if(ECONOMIA_EXPORT_DIR_NORMALIZED)
        list(APPEND ECONOMIA_PRUNE_DLLS_COMMANDS
            COMMAND "${CMAKE_COMMAND}"
                "-DDEPLOY_ROOT=${ECONOMIA_EXPORT_DIR_NORMALIZED}"
                "-DPRUNE_MSVC_RUNTIME=${ECONOMIA_PRUNE_MSVC_ON_MINGW}"
                -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/prune_optional_root_runtime_dlls.cmake"
        )
    endif()

    set(ECONOMIA_MINGW_RUNTIME_COMMANDS)
    if(MINGW)
        get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
        set(GCC_RUNTIMES libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll)
        foreach(GCC_DLL IN LISTS GCC_RUNTIMES)
            if(EXISTS "${MINGW_BIN_DIR}/${GCC_DLL}")
                list(APPEND ECONOMIA_MINGW_RUNTIME_COMMANDS
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${MINGW_BIN_DIR}/${GCC_DLL}"
                        "$<TARGET_FILE_DIR:Economia_APP>/${GCC_DLL}"
                )
                if(ECONOMIA_EXPORT_DIR_NORMALIZED)
                    list(APPEND ECONOMIA_MINGW_RUNTIME_COMMANDS
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                            "${MINGW_BIN_DIR}/${GCC_DLL}"
                            "${ECONOMIA_EXPORT_DIR_NORMALIZED}/${GCC_DLL}"
                    )
                endif()
            elseif(EXISTS "${QT_BIN_DIR}/${GCC_DLL}")
                list(APPEND ECONOMIA_MINGW_RUNTIME_COMMANDS
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${QT_BIN_DIR}/${GCC_DLL}"
                        "$<TARGET_FILE_DIR:Economia_APP>/${GCC_DLL}"
                )
                if(ECONOMIA_EXPORT_DIR_NORMALIZED)
                    list(APPEND ECONOMIA_MINGW_RUNTIME_COMMANDS
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                            "${QT_BIN_DIR}/${GCC_DLL}"
                            "${ECONOMIA_EXPORT_DIR_NORMALIZED}/${GCC_DLL}"
                    )
                endif()
            endif()
        endforeach()
    endif()

    set(ECONOMIA_CRITICAL_DLL_COMMANDS)
    set(CRITICAL_QT_DLLS Qt6Core.dll Qt6Gui.dll Qt6Widgets.dll Qt6PrintSupport.dll)
    foreach(DLL_NAME IN LISTS CRITICAL_QT_DLLS)
        if(EXISTS "${QT_BIN_DIR}/${DLL_NAME}")
            list(APPEND ECONOMIA_CRITICAL_DLL_COMMANDS
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${QT_BIN_DIR}/${DLL_NAME}"
                    "$<TARGET_FILE_DIR:Economia_APP>/${DLL_NAME}"
            )
            if(ECONOMIA_EXPORT_DIR_NORMALIZED)
                list(APPEND ECONOMIA_CRITICAL_DLL_COMMANDS
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${QT_BIN_DIR}/${DLL_NAME}"
                        "${ECONOMIA_EXPORT_DIR_NORMALIZED}/${DLL_NAME}"
                )
            endif()
        endif()
    endforeach()

    if(EXISTS "${QT_PREFIX_DIR}/plugins/platforms/qwindows.dll")
        list(APPEND ECONOMIA_CRITICAL_DLL_COMMANDS
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:Economia_APP>/platforms"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${QT_PREFIX_DIR}/plugins/platforms/qwindows.dll"
                "$<TARGET_FILE_DIR:Economia_APP>/platforms/qwindows.dll"
        )
        if(ECONOMIA_EXPORT_DIR_NORMALIZED)
            list(APPEND ECONOMIA_CRITICAL_DLL_COMMANDS
                COMMAND ${CMAKE_COMMAND} -E make_directory "${ECONOMIA_EXPORT_DIR_NORMALIZED}/platforms"
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${QT_PREFIX_DIR}/plugins/platforms/qwindows.dll"
                    "${ECONOMIA_EXPORT_DIR_NORMALIZED}/platforms/qwindows.dll"
            )
        endif()
    endif()

    add_custom_target(
        Economia_APPDeploy ALL
        DEPENDS Economia_APP
        ${ECONOMIA_BUILD_TREE_ASSET_COMMANDS}
        ${ECONOMIA_EXPORT_ASSET_COMMANDS}
        ${ECONOMIA_WINDEPLOYQT_COMMANDS}
        ${ECONOMIA_CRITICAL_DLL_COMMANDS}
        ${ECONOMIA_MINGW_RUNTIME_COMMANDS}
        ${ECONOMIA_MSVC_RUNTIME_COMMANDS}
        ${ECONOMIA_REMOVE_INSTALLER_COMMANDS}
        ${ECONOMIA_PRUNE_DLLS_COMMANDS}
        COMMENT "Deploying Economia_APP runtime to build tree and export directory"
        VERBATIM
    )
endif()

if(ECONOMIA_BUILD_MOCKS)
    add_subdirectory(mocks)
endif()
