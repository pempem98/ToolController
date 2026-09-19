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
    connectivity/test_cli_protocol.c
    app/test_system_coordinator.c
    middleware/freertos/test_freertos_osal.c
    # Real drivers under test
    ${CMAKE_CURRENT_LIST_DIR}/../platform/stm32h7a3ziq/drivers/bldc_can_driver.c
    ${CMAKE_CURRENT_LIST_DIR}/../platform/stm32h7a3ziq/drivers/tmc2209_driver.c
    ${CMAKE_CURRENT_LIST_DIR}/../platform/stm32h7a3ziq/drivers/brake_driver.c
    ${CMAKE_CURRENT_LIST_DIR}/../platform/stm32h7a3ziq/drivers/operator_input_driver.c
    # System coordinator
    ${CMAKE_CURRENT_LIST_DIR}/../app/src/system_coordinator.c
)

add_executable(unit_tests ${TEST_SOURCES})

target_include_directories(unit_tests PRIVATE
    unity
    stubs
    ../interfaces
    ../platform/common/inc
    ../platform/stm32h7a3ziq/drivers
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

    if(GCOVR_BIN)
        add_custom_target(coverage_report
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage_report
            COMMAND ${GCOVR_BIN} --root ${CMAKE_SOURCE_DIR} --filter services --filter connectivity --filter middleware --filter app --filter platform/stm32h7a3ziq/drivers --exclude tests --exclude build --html --html-details -o ${CMAKE_BINARY_DIR}/coverage_report/index.html
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
