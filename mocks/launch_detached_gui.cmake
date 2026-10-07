if(NOT DEFINED LAUNCH_FILE OR NOT DEFINED LAUNCH_WORKING_DIR)
    message(FATAL_ERROR "LAUNCH_FILE and LAUNCH_WORKING_DIR must be provided")
endif()

string(REGEX REPLACE "^[\"']+|[\"']+$" "" LAUNCH_FILE "${LAUNCH_FILE}")
string(REGEX REPLACE "^[\"']+|[\"']+$" "" LAUNCH_WORKING_DIR "${LAUNCH_WORKING_DIR}")
if(DEFINED LAUNCH_ARGS)
    string(REGEX REPLACE "^[\"']+|[\"']+$" "" LAUNCH_ARGS "${LAUNCH_ARGS}")
endif()

if(NOT EXISTS "${LAUNCH_FILE}")
    message(FATAL_ERROR "Launcher target was not found: ${LAUNCH_FILE}")
endif()

file(SIZE "${LAUNCH_FILE}" LAUNCH_FILE_SIZE)
if(LAUNCH_FILE_SIZE EQUAL 0)
    message(FATAL_ERROR "Launcher target is empty (0 bytes): ${LAUNCH_FILE}. Rebuild the corresponding target before running MocksRun.")
endif()

file(TO_NATIVE_PATH "${LAUNCH_FILE}" LAUNCH_FILE_NATIVE)
file(TO_NATIVE_PATH "${LAUNCH_WORKING_DIR}" LAUNCH_WORKING_DIR_NATIVE)

if(DEFINED LAUNCH_ARGS AND NOT LAUNCH_ARGS STREQUAL "")
    set(LAUNCH_ARGS_EXPRESSION "-ArgumentList '${LAUNCH_ARGS}'")
else()
    set(LAUNCH_ARGS_EXPRESSION "")
endif()

execute_process(
    COMMAND powershell -NoProfile -ExecutionPolicy Bypass -Command
        "$ErrorActionPreference = 'Stop'; Start-Process -FilePath '${LAUNCH_FILE_NATIVE}' -WorkingDirectory '${LAUNCH_WORKING_DIR_NATIVE}' ${LAUNCH_ARGS_EXPRESSION}"
    WORKING_DIRECTORY "${LAUNCH_WORKING_DIR}"
    RESULT_VARIABLE LAUNCH_RESULT
)

if(NOT LAUNCH_RESULT EQUAL 0)
    message(FATAL_ERROR "Failed to launch ${LAUNCH_FILE} with exit code ${LAUNCH_RESULT}")
endif()