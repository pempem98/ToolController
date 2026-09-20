#include "board_interface.h"
#include "main.h"
#include "usart.h"
#include "tmc2209_driver.h"
#include "bldc_can_driver.h"
#include "brake_driver.h"
#include "operator_input_driver.h"
#include "can_mcal.h"
#include "uart_mcal.h"

// Hardware domain instances
static brake_interface_t          s_brake;
static stm32_brake_priv_t         s_brake_priv;
static operator_input_interface_t s_operator_input;
static console_interface_t        s_console;
static motor_interface_t          s_motors[BOARD_MAX_MOTORS];

// Internal peripheral instances for motor drivers & console (encapsulated within Platform)
static can_interface_t            s_motor_can_bus;
static uart_interface_t           s_console_uart;
static uart_interface_t           s_tmc_uart;
static stm32_uart_priv_t         s_console_uart_priv;
static stm32_uart_priv_t         s_tmc_uart_priv;

// Driver private memory
static tmc2209_motor_priv_t       s_tmc_priv[2];
static bldc_can_motor_priv_t      s_bldc_priv[2];

static board_hardware_t           s_board_hw;
static bool                       s_board_initialized = false;

static status_t console_write(console_interface_t *self, const uint8_t *data, size_t len) {
    (void)self;
    return (s_console_uart.send && s_console_uart.send(&s_console_uart, data, len) == STATUS_OK) ? STATUS_OK : STATUS_ERROR;
}

static status_t console_read(console_interface_t *self, uint8_t *buffer, size_t max_len, size_t *received_len) {
    (void)self;
    return (s_console_uart.receive && s_console_uart.receive(&s_console_uart, buffer, max_len, received_len) == STATUS_OK) ? STATUS_OK : STATUS_ERROR;
}

status_t board_init(void) {
    if (s_board_initialized) return STATUS_OK;

    // 1. Phanh an toàn vật lý (EN_Pin / PD4)
    stm32_brake_driver_create(&s_brake, &s_brake_priv, EN_GPIO_Port, EN_Pin);

    // 2. Bảng điều khiển người vận hành (4 chiết áp ADC1 DMA + nút bấm PB0)
    operator_input_driver_create(&s_operator_input);

    // 3. Cổng UART Console (USART3 - PD8/PD9) bọc thành console_interface_t
    uart_mcal_create(&s_console_uart, &s_console_uart_priv, &huart3);
    s_console_uart.init(&s_console_uart, 115200);
    s_console.write = console_write;
    s_console.read = console_read;
    s_console.priv_data = &s_console_uart;

    // 4. Kênh truyền MCAL phục vụ Motor Drivers:
    can_mcal_create(&s_motor_can_bus);
    uart_mcal_create(&s_tmc_uart, &s_tmc_uart_priv, &huart1);
    s_tmc_uart.init(&s_tmc_uart, 115200);

    // 5. Cấu hình 4 động cơ:
    // Prototype 1 (Trục 0 & 1): TMC2209 Stepper Drivers (Step/Dir + UART)
    tmc2209_driver_create(&s_motors[0], &s_tmc_priv[0], 0,
                          STEP_1_GPIO_Port, STEP_1_Pin,
                          DIR_1_GPIO_Port, DIR_1_Pin,
                          EN_GPIO_Port, EN_Pin,
                          &s_tmc_uart, false);

    tmc2209_driver_create(&s_motors[1], &s_tmc_priv[1], 1,
                          STEP_2_GPIO_Port, STEP_2_Pin,
                          DIR_2_GPIO_Port, DIR_2_Pin,
                          EN_GPIO_Port, EN_Pin,
                          &s_tmc_uart, false);

    // Prototype 2 (Trục 2 & 3): BLDC All-In-One CAN 2.0B Motors (node_id 3 & 4)
    bldc_can_driver_create(&s_motors[2], &s_bldc_priv[0], 3, &s_motor_can_bus);
    bldc_can_driver_create(&s_motors[3], &s_bldc_priv[1], 4, &s_motor_can_bus);

    // 6. Gán các domain interface vào struct board_hardware_t
    s_board_hw.brake = &s_brake;
    s_board_hw.operator_input = &s_operator_input;
    s_board_hw.console = &s_console;
    s_board_hw.motor_count = BOARD_MAX_MOTORS;
    for (uint8_t i = 0; i < BOARD_MAX_MOTORS; i++) {
        s_board_hw.motors[i] = &s_motors[i];
    }

    s_board_initialized = true;
    return STATUS_OK;
}

const board_hardware_t* board_get_hardware(void) {
    if (!s_board_initialized) {
        board_init();
    }
    return &s_board_hw;
}
