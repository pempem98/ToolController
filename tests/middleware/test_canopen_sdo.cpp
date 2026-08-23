#include <gtest/gtest.h>
#include "canopen_sdo.h"
#include "can_mock.h"

TEST(CANopenSDOTest, InitAndNullGuards) {
    can_interface_t mock;
    can_mock_create(&mock);

    canopen_sdo_t sdo;
    EXPECT_FALSE(canopen_sdo_init(NULL, 1, &mock, false));
    EXPECT_TRUE(canopen_sdo_init(&sdo, 1, &mock, false));

    EXPECT_FALSE(canopen_sdo_write_expedited(NULL, 0x607A, 0, &sdo, 4));
    EXPECT_FALSE(canopen_sdo_read_expedited(NULL, 0x6064, 0));
}

TEST(CANopenSDOTest, WriteExpedited4BytesAndFD) {
    can_interface_t mock;
    can_mock_create(&mock);

    canopen_sdo_t sdo;
    EXPECT_TRUE(canopen_sdo_init(&sdo, 1, &mock, false));

    uint32_t target_pos = 100000;
    EXPECT_TRUE(canopen_sdo_write_expedited(&sdo, 0x607A, 0, &target_pos, 4));

    // Write > 4 bytes (CAN FD payload)
    uint8_t large_payload[16] = {0};
    EXPECT_TRUE(canopen_sdo_write_expedited(&sdo, 0x2000, 1, large_payload, 16));
}

TEST(CANopenSDOTest, ReadExpeditedRequest) {
    can_interface_t mock;
    can_mock_create(&mock);

    canopen_sdo_t sdo;
    canopen_sdo_init(&sdo, 1, &mock, false);

    EXPECT_TRUE(canopen_sdo_read_expedited(&sdo, 0x6064, 0));
}

TEST(CANopenSDOTest, ParseResponseSuccessAndAbort) {
    can_frame_t frame;
    frame.id = CANOPEN_COB_TSDO_BASE + 1;
    frame.len = 8;
    frame.data[0] = (SDO_CCS_UPLOAD_INITIATE << 5) | (0 << 2) | 0x03; // e=1, s=1, n=0 (4 bytes)
    frame.data[1] = 0x64; // Index 0x6064
    frame.data[2] = 0x60;
    frame.data[3] = 0x00; // Subindex 0
    int32_t act_pos = 54321;
    memcpy(&frame.data[4], &act_pos, 4);

    uint16_t idx = 0;
    uint8_t subidx = 0, len = 0;
    int32_t val = 0;

    EXPECT_TRUE(canopen_sdo_parse_response(&frame, &idx, &subidx, &val, &len, NULL));
    EXPECT_EQ(idx, 0x6064);
    EXPECT_EQ(subidx, 0);
    EXPECT_EQ(len, 4);
    EXPECT_EQ(val, 54321);

    // Abort transfer response
    can_frame_t abort_frame;
    abort_frame.id = CANOPEN_COB_TSDO_BASE + 1;
    abort_frame.len = 8;
    abort_frame.data[0] = (SDO_CCS_ABORT_TRANSFER << 5);
    abort_frame.data[1] = 0x64;
    abort_frame.data[2] = 0x60;
    abort_frame.data[3] = 0x00;
    uint32_t abort_code = 0x06020000;
    memcpy(&abort_frame.data[4], &abort_code, 4);

    uint32_t rec_abort = 0;
    EXPECT_FALSE(canopen_sdo_parse_response(&abort_frame, &idx, &subidx, NULL, NULL, &rec_abort));
    EXPECT_EQ(rec_abort, 0x06020000);

    // CAN FD Upload Response (len > 8)
    can_frame_t fd_upload;
    fd_upload.id = CANOPEN_COB_TSDO_BASE + 1;
    fd_upload.len = 20;
    fd_upload.data[0] = (SDO_CCS_UPLOAD_INITIATE << 5); // e=0, s=0
    fd_upload.data[1] = 0x00;
    fd_upload.data[2] = 0x20;
    fd_upload.data[3] = 0x01;
    EXPECT_TRUE(canopen_sdo_parse_response(&fd_upload, &idx, &subidx, NULL, &len, NULL));
    EXPECT_EQ(len, 16);

    // Download initiate response
    can_frame_t dl_init;
    dl_init.id = CANOPEN_COB_TSDO_BASE + 1;
    dl_init.len = 8;
    dl_init.data[0] = (SDO_CCS_DOWNLOAD_INITIATE << 5);
    dl_init.data[1] = 0x7A;
    dl_init.data[2] = 0x60;
    dl_init.data[3] = 0x00;
    EXPECT_TRUE(canopen_sdo_parse_response(&dl_init, &idx, &subidx, NULL, &len, NULL));
    EXPECT_EQ(len, 0);

    // Invalid command (scs = 0x07)
    can_frame_t invalid_scs;
    invalid_scs.id = CANOPEN_COB_TSDO_BASE + 1;
    invalid_scs.len = 8;
    invalid_scs.data[0] = (0x07 << 5);
    EXPECT_FALSE(canopen_sdo_parse_response(&invalid_scs, &idx, &subidx, NULL, &len, NULL));

    // Null/short frame guards
    EXPECT_FALSE(canopen_sdo_parse_response(NULL, NULL, NULL, NULL, NULL, NULL));
    invalid_scs.len = 2;
    EXPECT_FALSE(canopen_sdo_parse_response(&invalid_scs, NULL, NULL, NULL, NULL, NULL));
}
