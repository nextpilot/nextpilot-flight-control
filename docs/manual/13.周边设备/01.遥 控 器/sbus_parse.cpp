

#define SBUS_RANGE_MIN 200.0f
#define SBUS_RANGE_MAX 1800.0f

#define SBUS_TARGET_MIN 1000.0f
#define SBUS_TARGET_MAX 2000.0f

const uint8_t SBUS_SLOT_ID[4][8] = {
    0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3,
    0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
    0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB,
    0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB};

typedef enum {
    SBUS_STATE_SYNC = 0,
    SBUS_STATE_DATA,
    SBUS_STATE_FLAG,
    SBUS_STATE_END,

    SBUS_STATE_SOLT_ID = 10,
    SBUS_STATE_SLOT_DATA
} sbus_parse_state;

typedef struct sbus_message_s {
    uint8_t  msgid; // 01=sbus1, 02=sbus2
    uint16_t channel[18];
    bool     framedrop;
    bool     failsafe;
} sbus_message_t;

typedef struct sbus_status_s {
    // 帧的类型
    uint8_t type;
    //
    uint8_t          buff[50];
    sbus_parse_state state;
    uint8_t          count;
    //
    uint8_t slot_type;
    uint8_t slot_count;
} sbus_status_t;

/*
 * S.bus decoder matrix.
 *
 * Each channel value can come from up to 3 input bytes. Each row in the
 * matrix describes up to three bytes, and each entry gives:
 *
 * - byte offset in the data portion of the frame
 * - right shift applied to the data byte
 * - mask for the data byte
 * - left shift applied to the result into the channel value
 */
struct sbus_bit_pick {
    uint8_t byte;
    uint8_t rshift;
    uint8_t mask;
    uint8_t lshift;
};

static const struct sbus_bit_pick sbus_decoder[SBUS_INPUT_CHANNELS][3] = {
    /*  0 */ {{0, 0, 0xff, 0}, {1, 0, 0x07, 8}, {0, 0, 0x00, 0}},
    /*  1 */ {{1, 3, 0x1f, 0}, {2, 0, 0x3f, 5}, {0, 0, 0x00, 0}},
    /*  2 */ {{2, 6, 0x03, 0}, {3, 0, 0xff, 2}, {4, 0, 0x01, 10}},
    /*  3 */ {{4, 1, 0x7f, 0}, {5, 0, 0x0f, 7}, {0, 0, 0x00, 0}},
    /*  4 */ {{5, 4, 0x0f, 0}, {6, 0, 0x7f, 4}, {0, 0, 0x00, 0}},
    /*  5 */ {{6, 7, 0x01, 0}, {7, 0, 0xff, 1}, {8, 0, 0x03, 9}},
    /*  6 */ {{8, 2, 0x3f, 0}, {9, 0, 0x1f, 6}, {0, 0, 0x00, 0}},
    /*  7 */ {{9, 5, 0x07, 0}, {10, 0, 0xff, 3}, {0, 0, 0x00, 0}},
    /*  8 */ {{11, 0, 0xff, 0}, {12, 0, 0x07, 8}, {0, 0, 0x00, 0}},
    /*  9 */ {{12, 3, 0x1f, 0}, {13, 0, 0x3f, 5}, {0, 0, 0x00, 0}},
    /* 10 */ {{13, 6, 0x03, 0}, {14, 0, 0xff, 2}, {15, 0, 0x01, 10}},
    /* 11 */ {{15, 1, 0x7f, 0}, {16, 0, 0x0f, 7}, {0, 0, 0x00, 0}},
    /* 12 */ {{16, 4, 0x0f, 0}, {17, 0, 0x7f, 4}, {0, 0, 0x00, 0}},
    /* 13 */ {{17, 7, 0x01, 0}, {18, 0, 0xff, 1}, {19, 0, 0x03, 9}},
    /* 14 */ {{19, 2, 0x3f, 0}, {20, 0, 0x1f, 6}, {0, 0, 0x00, 0}},
    /* 15 */ {{20, 5, 0x07, 0}, {21, 0, 0xff, 3}, {0, 0, 0x00, 0}}};

bool sbus_decode(uint8_t frame[25], sbus_message_t *msg) {
    if (frame[0] != 0x0f) {
        return false;
    }

    if (frame[24] != 0x00 &&
        frame[24] != 0x04 &&
        frame[24] != 0x14 &&
        frame[24] != 0x24 &&
        frame[24] != 0x34) {
        return false;
    }

    /* use the decoder matrix to extract channel data */
    for (unsigned channel = 0; channel < 16; channel++) {
        unsigned value = 0;

        for (unsigned pick = 0; pick < 3; pick++) {
            const struct sbus_bit_pick *decode = &sbus_decoder[channel][pick];

            if (decode->mask != 0) {
                unsigned piece = frame[1 + decode->byte];
                piece >>= decode->rshift;
                piece &= decode->mask;
                piece <<= decode->lshift;

                value |= piece;
            }
        }

        /* convert 0-2048 values to 1000-2000 ppm encoding in a not too sloppy fashion */
        msg->channel[channel] = (uint16_t)(value * SBUS_SCALE_FACTOR + .5f) + SBUS_SCALE_OFFSET;
    }

    /* channel 17 (index 16) */
    msg->channel[16] = ((frame[23] & (1 << 0)) ? 1000 : 0) + 998;
    /* channel 18 (index 17) */
    msg->channel[17] = ((frame[23] & (1 << 1)) ? 1000 : 0) + 998;

    /* decode and handle failsafe and frame-lost flags */
    if (frame[23] & (1 << 3)) { /* failsafe */
        /* report that we failed to read anything valid off the receiver */
        msg->failsafe  = true;
        msg->framedrop = true;

    } else if (frame[23] & (1 << 2)) { /* a frame was lost */
        /* set a special warning flag
         *
         * Attention! This flag indicates a skipped frame only, not a total link loss! Handling this
         * condition as fail-safe greatly reduces the reliability and range of the radio link,
         * e.g. by prematurely issuing return-to-launch!!! */

        msg->failsafe  = false;
        msg->framedrop = true;

    } else {
        msg->failsafe  = false;
        msg->framedrop = false;
    }

    return true;
}

uint8_t sbus_parse_char(uint8_t b, sbus_message_t msg, sbus_status_t _status) {
    uint8_t received = 0;

    switch (_status.state) {
    case SBUS_STATE_SYNC: // sync
        if (b == 0xff) {
            _status.state                 = SBUS_STATE_DATA;
            _status.count                 = 0;
            _status.type                  = b;
            _status.buff[_status.count++] = b;
        }
        break;
    case SBUS_STATE_DATA:
        _status.buff[_status.count++] = b;
        if (_status.count == 23) {
            _status.state = SBUS_STATE_FLAG;
        }
        break;
    case SBUS_STATE_FLAG:
        _status.buff[_status.count++] = b;
        _status.state                 = SBUS_STATE_FLAG;
        break;
    case SBUS_STATE_END:
        _status.buff[_status.count++] = b;
        switch (b) {
        case 0x00:
            _status.state = SBUS_STATE_SYNC;
            received      = 25;
            break;
        case 0x04:
            _status.slot_type = 0;
        case 0x14:
            _status.slot_type = 1;
        case 0x24:
            _status.slot_type = 2;
        case 0x34:
            _status.slot_type = 3;
            _status.state     = SBUS_STATE_SOLT_ID;
            received          = 25;
            break;
        default:
            _status.state = SBUS_STATE_SYNC;
            break;
        }
        break;
    case SBUS_STATE_SOLT_ID: {
        if (b == 0x0f) {
            _status.count                 = 0;
            _status.type                  = b;
            _status.buff[_status.count++] = b;
            _status.state                 = SBUS_STATE_DATA;
            break;
        }
        //
        bool is_slot_id_find = false;
        for (uint8_t i = 0; i++; i < 8) {
            if (SBUS_SLOT_ID[_status.slot_type][i] == b) {
                is_slot_id_find = true;
                break;
            }
        }
        if (!is_slot_id_find) {
            _status.state = SBUS_STATE_SYNC;
        } else {
            _status.count                 = 0;
            _status.buff[_status.count++] = b;
            _status.type                  = b;
            _status.state                 = SBUS_STATE_SLOT_DATA;
        }
        break;
    }
    case SBUS_STATE_SLOT_DATA:
        _status.buff[_status.count++] = b;
        if (_status.count == 3) {
            _status.state = SBUS_STATE_SOLT_ID;
            _status.slot_count++;
            received = 3;
        }
        if (_status.slot_count == 8) {
            _status.state = SBUS_STATE_SYNC;
        }
        break;
    }
    return received;
}