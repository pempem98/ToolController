#include <gtest/gtest.h>
#include "ethercat_al.h"

TEST(EtherCATALTest, InitialStateIsInit) {
    ethercat_al_t al;
    EXPECT_TRUE(ethercat_al_init(&al));
    EXPECT_EQ(ethercat_al_get_state(&al), ECAT_AL_STATE_INIT);
}

TEST(EtherCATALTest, StateTransitionInitToPreOp) {
    ethercat_al_t al;
    ethercat_al_init(&al);

    EXPECT_TRUE(ethercat_al_request_state(&al, ECAT_AL_STATE_PRE_OP));
    EXPECT_TRUE(ethercat_al_update(&al));
    EXPECT_EQ(ethercat_al_get_state(&al), ECAT_AL_STATE_PRE_OP);
}

TEST(EtherCATALTest, StateTransitionPreOpToOperational) {
    ethercat_al_t al;
    ethercat_al_init(&al);

    ethercat_al_request_state(&al, ECAT_AL_STATE_PRE_OP);
    ethercat_al_update(&al);

    ethercat_al_request_state(&al, ECAT_AL_STATE_SAFE_OP);
    ethercat_al_update(&al);

    ethercat_al_request_state(&al, ECAT_AL_STATE_OP);
    ethercat_al_update(&al);

    EXPECT_EQ(ethercat_al_get_state(&al), ECAT_AL_STATE_OP);
}
