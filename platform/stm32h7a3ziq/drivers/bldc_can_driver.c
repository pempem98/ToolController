#include "bldc_can_driver.h"
#include <string.h>

/* Private BLDC Motor CAN Command & Register Definitions */
#define BLDC_CAN_BASE_TX_ID   0x140U
#define BLDC_CAN_BASE_RX_ID   0x240U

#define BLDC_CMD_ENABLE_RUN   0x88
#define BLDC_CMD_DISABLE_STOP 0x89
#define BLDC_CMD_EMG_STOP     0x80
#define BLDC_CMD_POS_CTRL     0xA4
#define BLDC_CMD_SPEED_CTRL   0xA2
#define BLDC_CMD_SET_ZERO     0x64
#define BLDC_CMD_READ_STATUS  0x9C

static void init_can_frame(can_frame_t *frame, uint8_t node_id) {
    memset(frame, 0, sizeof(can_frame_t));
    frame->id = BLDC_CAN_BASE_TX_ID + node_id;
    frame->len = 8;
    frame->is_extended = false;
    frame->is_fd = false;
}

static void pack_enable_frame(uint8_t node_id, bool enable, can_frame_t *frame) {
    init_can_frame(frame, node_id);
    frame->data[0] = enable ? BLDC_CMD_ENABLE_RUN : BLDC_CMD_DISABLE_STOP;
}

static void pack_move_frame(uint8_t node_id, int32_t target_pos, uint32_t speed, motor_direction_t dir, can_frame_t *frame) {
    init_can_frame(frame, node_id);
    frame->data[0] = BLDC_CMD_POS_CTRL;
    frame->data[1] = (uint8_t)dir;
    frame->data[2] = (uint8_t)(speed & 0xFF);
    frame->data[3] = (uint8_t)((speed >> 8) & 0xFF);
    frame->data[4] = (uint8_t)(target_pos & 0xFF);
    frame->data[5] = (uint8_t)((target_pos >> 8) & 0xFF);
    frame->data[6] = (uint8_t)((target_pos >> 16) & 0xFF);
    frame->data[7] = (uint8_t)((target_pos >> 24) & 0xFF);
}

static void pack_rotate_frame(uint8_t node_id, motor_direction_t dir, uint32_t speed, can_frame_t *frame) {
    init_can_frame(frame, node_id);
    frame->data[0] = BLDC_CMD_SPEED_CTRL;
    frame->data[1] = (uint8_t)dir;
    int32_t signed_speed = (dir == MOTOR_DIR_CW) ? (int32_t)speed : -(int32_t)speed;
    frame->data[4] = (uint8_t)(signed_speed & 0xFF);
    frame->data[5] = (uint8_t)((signed_speed >> 8) & 0xFF);
    frame->data[6] = (uint8_t)((signed_speed >> 16) & 0xFF);
    frame->data[7] = (uint8_t)((signed_speed >> 24) & 0xFF);
}

static void pack_home_frame(uint8_t node_id, can_frame_t *frame) {
    init_can_frame(frame, node_id);
    frame->data[0] = BLDC_CMD_SET_ZERO;
}

static void pack_stop_frame(uint8_t node_id, can_frame_t *frame) {
    init_can_frame(frame, node_id);
    frame->data[0] = BLDC_CMD_EMG_STOP;
}

static status_t bldc_can_init(motor_interface_t *self) {
    if (!self || !self->priv_data) return STATUS_INVALID_PARAM;
    bldc_can_motor_priv_t *priv = (bldc_can_motor_priv_t*)self->priv_data;

    priv->actual_position = 0;
    priv->target_position = 0;
    priv->encoder_ticks = 0;
    priv->actual_speed = 0;
    priv->current_dir = MOTOR_DIR_CW;
    priv->enabled = false;
    return STATUS_OK;
}

static status_t bldc_can_set_enabled(motor_interface_t *self, bool enabled) {
    if (!self || !self->priv_data) return STATUS_INVALID_PARAM;
    bldc_can_motor_priv_t *priv = (bldc_can_motor_priv_t*)self->priv_data;
    priv->enabled = enabled;

    if (!priv->can_bus || !priv->can_bus->send) return STATUS_ERROR;

    can_frame_t frame;
    pack_enable_frame(priv->node_id, enabled, &frame);
    return priv->can_bus->send(priv->can_bus, &frame) ? STATUS_OK : STATUS_ERROR;
}

static status_t bldc_can_set_direction(motor_interface_t *self, motor_direction_t dir) {
    if (!self || !self->priv_data) return STATUS_INVALID_PARAM;
    bldc_can_motor_priv_t *priv = (bldc_can_motor_priv_t*)self->priv_data;
    priv->current_dir = dir;
    return STATUS_OK;
}

static status_t bldc_can_move_to(motor_interface_t *self, int32_t target_pos, uint32_t speed, motor_direction_t dir) {
    if (!self || !self->priv_data) return STATUS_INVALID_PARAM;
    bldc_can_motor_priv_t *priv = (bldc_can_motor_priv_t*)self->priv_data;

    priv->target_position = target_pos;
    priv->current_dir = dir;

    if (!priv->can_bus || !priv->can_bus->send) return STATUS_ERROR;

    can_frame_t frame;
    pack_move_frame(priv->node_id, target_pos, speed, dir, &frame);
    return priv->can_bus->send(priv->can_bus, &frame) ? STATUS_OK : STATUS_ERROR;
}

static status_t bldc_can_rotate(motor_interface_t *self, motor_direction_t dir, uint32_t speed) {
    if (!self || !self->priv_data) return STATUS_INVALID_PARAM;
    bldc_can_motor_priv_t *priv = (bldc_can_motor_priv_t*)self->priv_data;
    priv->current_dir = dir;

    if (!priv->can_bus || !priv->can_bus->send) return STATUS_ERROR;

    can_frame_t frame;
    pack_rotate_frame(priv->node_id, dir, speed, &frame);
    return priv->can_bus->send(priv->can_bus, &frame) ? STATUS_OK : STATUS_ERROR;
}

static status_t bldc_can_get_actual_position(motor_interface_t *self, int32_t *pos) {
    if (!self || !self->priv_data || !pos) return STATUS_INVALID_PARAM;
    bldc_can_motor_priv_t *priv = (bldc_can_motor_priv_t*)self->priv_data;
    *pos = priv->actual_position;
    return STATUS_OK;
}

static status_t bldc_can_get_encoder_ticks(motor_interface_t *self, int32_t *ticks) {
    if (!self || !self->priv_data || !ticks) return STATUS_INVALID_PARAM;
    bldc_can_motor_priv_t *priv = (bldc_can_motor_priv_t*)self->priv_data;
    *ticks = priv->encoder_ticks;
    return STATUS_OK;
}

static status_t bldc_can_home(motor_interface_t *self) {
    if (!self || !self->priv_data) return STATUS_INVALID_PARAM;
    bldc_can_motor_priv_t *priv = (bldc_can_motor_priv_t*)self->priv_data;

    if (!priv->can_bus || !priv->can_bus->send) return STATUS_ERROR;

    can_frame_t frame;
    pack_home_frame(priv->node_id, &frame);

    priv->actual_position = 0;
    priv->encoder_ticks = 0;
    return priv->can_bus->send(priv->can_bus, &frame) ? STATUS_OK : STATUS_ERROR;
}

static status_t bldc_can_stop(motor_interface_t *self) {
    if (!self || !self->priv_data) return STATUS_INVALID_PARAM;
    bldc_can_motor_priv_t *priv = (bldc_can_motor_priv_t*)self->priv_data;

    if (!priv->can_bus || !priv->can_bus->send) return STATUS_ERROR;

    can_frame_t frame;
    pack_stop_frame(priv->node_id, &frame);
    return priv->can_bus->send(priv->can_bus, &frame) ? STATUS_OK : STATUS_ERROR;
}

void bldc_can_driver_create(motor_interface_t *adapter,
                            bldc_can_motor_priv_t *priv,
                            uint8_t node_id,
                            can_interface_t *can_bus) {
    if (!adapter || !priv) return;

    priv->node_id = node_id;
    priv->can_bus = can_bus;
    priv->actual_position = 0;
    priv->target_position = 0;
    priv->encoder_ticks = 0;
    priv->actual_speed = 0;
    priv->current_dir = MOTOR_DIR_CW;
    priv->enabled = false;

    adapter->init = bldc_can_init;
    adapter->set_enabled = bldc_can_set_enabled;
    adapter->set_direction = bldc_can_set_direction;
    adapter->move_to = bldc_can_move_to;
    adapter->rotate = bldc_can_rotate;
    adapter->get_actual_position = bldc_can_get_actual_position;
    adapter->get_encoder_ticks = bldc_can_get_encoder_ticks;
    adapter->home = bldc_can_home;
    adapter->stop = bldc_can_stop;
    adapter->priv_data = priv;
}

void bldc_can_driver_process_rx(motor_interface_t *adapter, const can_frame_t *frame) {
    if (!adapter || !adapter->priv_data || !frame) return;
    bldc_can_motor_priv_t *priv = (bldc_can_motor_priv_t*)adapter->priv_data;

    if (frame->id != ((uint32_t)BLDC_CAN_BASE_RX_ID + (uint32_t)priv->node_id)) return;

    uint8_t cmd = frame->data[0];
    if (cmd == BLDC_CMD_POS_CTRL || cmd == BLDC_CMD_READ_STATUS || cmd == BLDC_CMD_SPEED_CTRL) {
        priv->actual_speed = (int16_t)(frame->data[2] | (frame->data[3] << 8));
        priv->actual_position = (int32_t)(frame->data[4] | (frame->data[5] << 8) |
                                         (frame->data[6] << 16) | (frame->data[7] << 24));
        priv->encoder_ticks = priv->actual_position;
    }
}
