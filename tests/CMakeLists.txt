cmake_minimum_required(VERSION 3.20)
project(unit_tests C)

option(ENABLE_COVERAGE "Enable code coverage flags" OFF)

if(ENABLE_COVERAGE)
    message(STATUS "Building with code coverage enabled")
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

# Collect all test sources and stubs
set(TEST_SOURCES
    test_main.c
    unity/unity.c
    stubs/stm32_hal_stub.c
    drivers/test_bldc_can_driver.c
    drivers/test_tmc2209_driver.c
    drivers/test_brake_driver.c
    drivers/test_operator_input_driver.c
    services/motor_service/test_motor_service.c
    services/brake_service/test_brake_service.c
    services/operator_service/test_operator_service.c
    services/cli_service/test_cli_service.c
    connectivity/test_cli_protocol.c
    app/test_system_coordinator.c
    app/test_rtos_tasks.c
    middleware/freertos/test_freertos_osal.c
    # Real drivers under test
    ${CMAKE_CURRENT_LIST_DIR}/../platform/stm32h7a3zit6q/drivers/bldc_can_driver.c
    ${CMAKE_CURRENT_LIST_DIR}/../platform/stm32h7a3zit6q/drivers/tmc2209_driver.c
    ${CMAKE_CURRENT_LIST_DIR}/../platform/stm32h7a3zit6q/drivers/brake_driver.c
    ${CMAKE_CURRENT_LIST_DIR}/../platform/stm32h7a3zit6q/drivers/operator_input_driver.c
    # System coordinator & RTOS task orchestration logic (real, not stubbed)
    ${CMAKE_CURRENT_LIST_DIR}/../app/src/system_coordinator.c
    ${CMAKE_CURRENT_LIST_DIR}/../app/src/rtos_tasks.c
)

add_executable(unit_tests ${TEST_SOURCES})

target_include_directories(unit_tests PRIVATE
    unity
    stubs
    ../interfaces
    ../platform/common/inc
    ../platform/stm32h7a3zit6q/drivers
    ../app/inc
    ../middleware/freertos/inc
    ../services/motor_service/inc
    ../services/brake_service/inc
    ../services/operator_service/inc
    ../services/cli_service/inc
    ../connectivity/inc
)

target_link_libraries(unit_tests PRIVATE
    connectivity
    motor_service
    brake_service
    operator_service
    cli_service
    middleware_freertos
)

add_test(NAME unit_tests COMMAND unit_tests)

if(ENABLE_COVERAGE)
    find_program(GCOVR_BIN gcovr HINTS "$ENV{LOCALAPPDATA}/Programs/Python/Python313/Scripts")
    find_program(LCOV_BIN lcov)
    find_program(GENHTML_BIN genhtml)

    # Tìm đúng file gcov tương ứng với trình biên dịch C đang được CMake sử dụng
    get_filename_component(COMPILER_DIR "${CMAKE_C_COMPILER}" DIRECTORY)
    find_program(GCOV_BIN gcov HINTS "${COMPILER_DIR}")

    set(GCOVR_GCOV_ARG "")
    if(GCOV_BIN)
        set(GCOVR_GCOV_ARG "--gcov-executable" "${GCOV_BIN}")
    endif()

    if(GCOVR_BIN)
        # PYTHONUTF8=1 bat buoc tren Windows: gcovr (Python) mac dinh doc file nguon
        # bang codepage he thong (vd cp1252) thay vi UTF-8, lam hong cac comment
        # tieng Viet co dau (mojibake) trong bao cao HTML dau ra du file .c/.h goc
        # hoan toan dung UTF-8. Bien nay ep Python 3.7+ dung UTF-8 cho toan bo I/O
        # van ban bat ke codepage he thong dang active.
        add_custom_target(coverage_report
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage_report
            COMMAND ${CMAKE_COMMAND} -E env PYTHONUTF8=1 ${GCOVR_BIN} --root ${CMAKE_SOURCE_DIR} ${GCOVR_GCOV_ARG} --filter services --filter connectivity --filter middleware --filter app --filter platform/stm32h7a3zit6q/drivers --exclude tests --exclude build --html --html-details -o ${CMAKE_BINARY_DIR}/coverage_report/index.html
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Generating HTML code coverage report in build/coverage_report/index.html..."
        )
    elseif(LCOV_BIN AND GENHTML_BIN)
        add_custom_target(coverage_report
            COMMAND ${LCOV_BIN} --capture --directory . --output-file coverage.info --exclude '/usr/*' --exclude '*/_deps/*' --exclude '*/tests/*'
            COMMAND ${GENHTML_BIN} coverage.info --output-directory ${CMAKE_BINARY_DIR}/coverage_report
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating HTML code coverage report in build/coverage_report/index.html..."
        )
    endif()
endif()
