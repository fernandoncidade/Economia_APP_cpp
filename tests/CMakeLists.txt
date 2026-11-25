# Suite completa de testes automatizados (Qt6Test + CTest) para Economia_APP
# Sem distribuicao externa: todos os binarios e dependencias de runtime permanecem
# exclusivamente dentro do diretorio de build da suite (build_suite_msvc / build_suite_mingw).

set(SUITE_TEST_TARGETS
    test_01_core_interest_annuity_gradient
    test_02_rates_fisher_minreturn_effrate
    test_03_amortization_depreciation
    test_04_investment_vpl_taxes_caue
    test_05_textformat_mathrenderer_typography
    test_06_history_session_pdf_and_persistence
    test_07_ui_dialogs_about_manual_and_i18n
    test_08_qt_plugins_modules_and_cpp23
    test_09_toolchain_isolation_and_binaries
    test_10_tools_audit_and_docs_integrity
)

if(MSVC)
    set(SUITE_TOOLCHAIN_NAME "MSVC")
else()
    set(SUITE_TOOLCHAIN_NAME "MinGW")
endif()

foreach(TARGET_NAME IN LISTS SUITE_TEST_TARGETS)
    set(SOURCE_FILE "${TARGET_NAME}.cpp")

    qt6_add_executable(${TARGET_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE}")
    economia_apply_common_options(${TARGET_NAME})
    target_link_libraries(${TARGET_NAME} PRIVATE
        Economia_APP_core
        Qt6::Test
    )
    target_compile_definitions(${TARGET_NAME} PRIVATE
        ECONOMIA_PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
        ECONOMIA_BUILD_DIR="${CMAKE_BINARY_DIR}"
        ECONOMIA_TOOLCHAIN="${SUITE_TOOLCHAIN_NAME}"
    )

    if(MSVC)
        set_target_properties(${TARGET_NAME} PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:CONSOLE"
        )
    endif()

    add_test(
        NAME ${TARGET_NAME}
        COMMAND $<TARGET_FILE:${TARGET_NAME}>
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    )
    set_tests_properties(${TARGET_NAME} PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
        TIMEOUT 120
    )
endforeach()

set(SUITE_DEPLOY_COMMANDS)

list(APPEND SUITE_DEPLOY_COMMANDS
    COMMAND "${CMAKE_COMMAND}"
        "-DSOURCE_ASSETS_DIR=${CMAKE_SOURCE_DIR}/source/assets"
        "-DDEST_ASSETS_DIR=${CMAKE_BINARY_DIR}/source/assets"
        "-DCLEAN_DESTINATION=ON"
        -P "${CMAKE_SOURCE_DIR}/cmake/copy_runtime_assets.cmake"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_BINARY_DIR}/source/language/translations"
)
foreach(QM_NAME IN ITEMS economia_pt_BR.qm economia_en_US.qm qtbase_pt_BR.qm)
    if(EXISTS "${CMAKE_SOURCE_DIR}/source/language/translations/${QM_NAME}")
        list(APPEND SUITE_DEPLOY_COMMANDS
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${CMAKE_SOURCE_DIR}/source/language/translations/${QM_NAME}"
                "${CMAKE_BINARY_DIR}/source/language/translations/${QM_NAME}"
        )
    endif()
endforeach()

if(EXISTS "${WINDEPLOYQT_EXE}")
    list(APPEND SUITE_DEPLOY_COMMANDS
        COMMAND "${WINDEPLOYQT_EXE}"
            --no-compiler-runtime
            --no-system-d3d-compiler
            --no-opengl-sw
            --no-svg
            --no-translations
            --dir "${CMAKE_BINARY_DIR}"
            --plugindir "${CMAKE_BINARY_DIR}"
            "$<TARGET_FILE:test_01_core_interest_annuity_gradient>"
    )
endif()

foreach(DLL_NAME IN ITEMS Qt6Core.dll Qt6Gui.dll Qt6Widgets.dll Qt6PrintSupport.dll Qt6Test.dll)
    if(EXISTS "${QT_BIN_DIR}/${DLL_NAME}")
        list(APPEND SUITE_DEPLOY_COMMANDS
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${QT_BIN_DIR}/${DLL_NAME}"
                "${CMAKE_BINARY_DIR}/${DLL_NAME}"
        )
    endif()
endforeach()

foreach(PLUGIN_REL_PATH IN ITEMS
    "platforms/qwindows.dll"
    "platforms/qoffscreen.dll"
    "styles/qmodernwindowsstyle.dll"
    "imageformats/qico.dll"
)
    if(EXISTS "${QT_PREFIX_DIR}/plugins/${PLUGIN_REL_PATH}")
        get_filename_component(PLUGIN_SUBDIR "${PLUGIN_REL_PATH}" DIRECTORY)
        list(APPEND SUITE_DEPLOY_COMMANDS
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_BINARY_DIR}/${PLUGIN_SUBDIR}"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${QT_PREFIX_DIR}/plugins/${PLUGIN_REL_PATH}"
                "${CMAKE_BINARY_DIR}/${PLUGIN_REL_PATH}"
        )
    endif()
endforeach()

if(MSVC AND ECONOMIA_MSVC_REDIST_ROOT)
    list(APPEND SUITE_DEPLOY_COMMANDS
        COMMAND "${CMAKE_COMMAND}"
            "-DDEPLOY_ROOT=${CMAKE_BINARY_DIR}"
            "-DMSVC_REDIST_ROOT=${ECONOMIA_MSVC_REDIST_ROOT}"
            "-DMSVC_TOOLSET_VERSION=${ECONOMIA_MSVC_TOOLSET_VERSION}"
            "-DMSVC_RUNTIME_ARCH=${ECONOMIA_MSVC_RUNTIME_ARCH}"
            "-DCONFIG=$<CONFIG>"
            -P "${CMAKE_SOURCE_DIR}/cmake/copy_msvc_runtime.cmake"
    )
elseif(MINGW)
    get_filename_component(SUITE_MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    list(APPEND SUITE_DEPLOY_COMMANDS
        COMMAND "${CMAKE_COMMAND}"
            "-DDEPLOY_ROOT=${CMAKE_BINARY_DIR}"
            "-DMINGW_BIN_DIR=${SUITE_MINGW_BIN_DIR}"
            "-DQT_BIN_DIR=${QT_BIN_DIR}"
            -P "${CMAKE_SOURCE_DIR}/cmake/copy_mingw_runtime.cmake"
    )
endif()

set(SUITE_PRUNE_MSVC OFF)
set(SUITE_PRUNE_MINGW OFF)
if(MINGW)
    set(SUITE_PRUNE_MSVC ON)
elseif(MSVC)
    set(SUITE_PRUNE_MINGW ON)
endif()
list(APPEND SUITE_DEPLOY_COMMANDS
    COMMAND "${CMAKE_COMMAND}"
        "-DDEPLOY_ROOT=${CMAKE_BINARY_DIR}"
        "-DPRUNE_MSVC_RUNTIME=${SUITE_PRUNE_MSVC}"
        "-DPRUNE_MINGW_RUNTIME=${SUITE_PRUNE_MINGW}"
        -P "${CMAKE_SOURCE_DIR}/cmake/prune_optional_root_runtime_dlls.cmake"
)

add_custom_target(
    EconomiaSuiteDeploy ALL
    DEPENDS ${SUITE_TEST_TARGETS} find_VersionEditor CompileTranslationsTerminal
    ${SUITE_DEPLOY_COMMANDS}
    COMMENT "Deploying Economia_APP test suite locally into build_suite tree (zero external deploy)"
    VERBATIM
)
