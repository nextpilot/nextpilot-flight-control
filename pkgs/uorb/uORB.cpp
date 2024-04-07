
// /*****************************************************************
//  *     _   __             __   ____   _  __        __
//  *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
//  *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
//  *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
//  * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
//  *
//  * Copyright All Reserved © 2015-2024 NextPilot Development Team
//  ******************************************************************/

#include "uORBDeviceNode.hpp"
#include "uORBPublication.h"
#include "uORBSubscription.h"

#include <rtdbg.h>
#include <stddef.h>
#include <uORB.h>
#include <hrtimer.h>
#include <matrix/Quaternion.hpp>
#include <mathlib/mathlib.h>

const char *orb_get_c_type(unsigned char short_type) {
    // this matches with the uorb o_fields generator
    switch (short_type) {
    case 0x82: return "int8_t";

    case 0x83: return "int16_t";

    case 0x84: return "int32_t";

    case 0x85: return "int64_t";

    case 0x86: return "uint8_t";

    case 0x87: return "uint16_t";

    case 0x88: return "uint32_t";

    case 0x89: return "uint64_t";

    case 0x8a: return "float";

    case 0x8b: return "double";

    case 0x8c: return "bool";

    case 0x8d: return "char";
    }

    return nullptr;
}

void orb_print_message_internal(const orb_metadata *meta, const void *data, bool print_topic_name) {
    //     if (print_topic_name) {
    //         LOG_RAW(" %s\n", meta->o_name);
    //     }

    //     const hrt_abstime now             = hrt_absolute_time();
    //     hrt_abstime       topic_timestamp = 0;

    //     const uint8_t *data_ptr    = (const uint8_t *)data;
    //     int            data_offset = 0;

    //     for (int format_idx = 0; meta->o_fields[format_idx] != 0;) {
    //         const char *end_field = strchr(meta->o_fields + format_idx, ';');

    //         if (!end_field) {
    //             LOG_E("Format error in %s", meta->o_fields);
    //             return;
    //         }

    //         const char *c_type        = orb_get_c_type(meta->o_fields[format_idx]);
    //         const int   end_field_idx = end_field - meta->o_fields;

    //         int array_idx      = -1;
    //         int field_name_idx = -1;

    //         for (int field_idx = format_idx; field_idx != end_field_idx; ++field_idx) {
    //             if (meta->o_fields[field_idx] == '[') {
    //                 array_idx = field_idx + 1;

    //             } else if (meta->o_fields[field_idx] == ' ') {
    //                 field_name_idx = field_idx + 1;
    //                 break;
    //             }
    //         }

    //         int array_size = 1;

    //         if (array_idx >= 0) {
    //             array_size = strtol(meta->o_fields + array_idx, nullptr, 10);
    //         }

    //         char   field_name[80];
    //         size_t field_name_len = end_field_idx - field_name_idx;

    //         if (field_name_len >= sizeof(field_name)) {
    //             LOG_E("field name too long %s (max: %u)", meta->o_fields, (unsigned)sizeof(field_name));
    //             return;
    //         }

    //         memcpy(field_name, meta->o_fields + field_name_idx, field_name_len);
    //         field_name[field_name_len] = '\0';

    //         if (c_type) { // built-in type
    //             bool dont_print = false;

    //             // handle special cases
    //             if (strncmp(field_name, "_padding", 8) == 0) {
    //                 dont_print = true;

    //             } else if (strcmp(c_type, "char") == 0 && array_size > 1) { // string
    //                 LOG_RAW("    %s: \"%.*s\"\n", field_name, array_size, (char *)(data_ptr + data_offset));
    //                 dont_print = true;
    //             }

    //             if (!dont_print) {
    //                 LOG_RAW("    %s: ", field_name);
    //             }

    //             if (!dont_print && array_size > 1) {
    //                 LOG_RAW("[");
    //             }

    //             const int previous_data_offset = data_offset;

    // #pragma GCC diagnostic push
    // #pragma GCC diagnostic ignored "-Wcast-align" // the caller ensures data is aligned

    //             for (int i = 0; i < array_size; ++i) {
    //                 if (strcmp(c_type, "int8_t") == 0) {
    //                     if (!dont_print) { LOG_RAW("%" PRIi8, *(int8_t *)(data_ptr + data_offset)); }

    //                     data_offset += sizeof(int8_t);

    //                 } else if (strcmp(c_type, "int16_t") == 0) {
    //                     if (!dont_print) { LOG_RAW("%" PRIi16, *(int16_t *)(data_ptr + data_offset)); }

    //                     data_offset += sizeof(int16_t);

    //                 } else if (strcmp(c_type, "int32_t") == 0) {
    //                     if (!dont_print) { LOG_RAW("%" PRIi32, *(int32_t *)(data_ptr + data_offset)); }

    //                     data_offset += sizeof(int32_t);

    //                 } else if (strcmp(c_type, "int64_t") == 0) {
    //                     if (!dont_print) { LOG_RAW("%" PRIi64, *(int64_t *)(data_ptr + data_offset)); }

    //                     data_offset += sizeof(int64_t);

    //                 } else if (strcmp(c_type, "uint8_t") == 0) {
    //                     if (!dont_print) { LOG_RAW("%" PRIu8, *(uint8_t *)(data_ptr + data_offset)); }

    //                     data_offset += sizeof(uint8_t);

    //                 } else if (strcmp(c_type, "uint16_t") == 0) {
    //                     if (!dont_print) { LOG_RAW("%" PRIu16, *(uint16_t *)(data_ptr + data_offset)); }

    //                     data_offset += sizeof(uint16_t);

    //                 } else if (strcmp(c_type, "uint32_t") == 0) {
    //                     if (!dont_print) { LOG_RAW("%" PRIu32, *(uint32_t *)(data_ptr + data_offset)); }

    //                     data_offset += sizeof(uint32_t);

    //                 } else if (strcmp(c_type, "uint64_t") == 0) {
    //                     if (!dont_print) { LOG_RAW("%" PRIu64, *(uint64_t *)(data_ptr + data_offset)); }

    //                     data_offset += sizeof(uint64_t);

    //                 } else if (strcmp(c_type, "float") == 0) {
    //                     if (!dont_print) { LOG_RAW("%.5f", (double)*(float *)(data_ptr + data_offset)); }

    //                     data_offset += sizeof(float);

    //                 } else if (strcmp(c_type, "double") == 0) {
    //                     if (!dont_print) { LOG_RAW("%.6f", *(double *)(data_ptr + data_offset)); }

    //                     data_offset += sizeof(double);

    //                 } else if (strcmp(c_type, "bool") == 0) {
    //                     if (!dont_print) { LOG_RAW("%s", *(bool *)(data_ptr + data_offset) ? "True" : "False"); }

    //                     data_offset += sizeof(bool);

    //                 } else if (strcmp(c_type, "char") == 0) {
    //                     if (!dont_print) { LOG_RAW("%i", (int)*(char *)(data_ptr + data_offset)); }

    //                     data_offset += sizeof(char);

    //                 } else {
    //                     LOG_E("unknown type: %s", c_type);
    //                     return;
    //                 }

    //                 if (!dont_print && i < array_size - 1) {
    //                     LOG_RAW(", ");
    //                 }
    //             }

    //             if (!dont_print && array_size > 1) {
    //                 LOG_RAW("]");
    //             }

    //             // handle special cases
    //             if (array_size == 1) {
    //                 if (strcmp(c_type, "uint64_t") == 0 && strcmp(field_name, "timestamp") == 0) {
    //                     topic_timestamp = *(uint64_t *)(data_ptr + previous_data_offset);

    //                     if (topic_timestamp != 0) {
    //                         LOG_RAW(" (%.6f seconds ago)", (double)((now - topic_timestamp) / 1e6f));
    //                     }

    //                 } else if (strcmp(c_type, "uint64_t") == 0 && strcmp(field_name, "timestamp_sample") == 0) {
    //                     hrt_abstime timestamp = *(uint64_t *)(data_ptr + previous_data_offset);

    //                     if (topic_timestamp != 0 && timestamp != 0) {
    //                         LOG_RAW(" (%i us before timestamp)", (int)(topic_timestamp - timestamp));
    //                     }

    //                 } else if (strstr(field_name, "flags") != nullptr) {
    //                     // bitfield
    //                     unsigned field_size = 0;
    //                     uint64_t value      = 0;

    //                     if (strcmp(c_type, "uint8_t") == 0) {
    //                         field_size = sizeof(uint8_t);
    //                         value      = *(uint8_t *)(data_ptr + previous_data_offset);

    //                     } else if (strcmp(c_type, "uint16_t") == 0) {
    //                         field_size = sizeof(uint16_t);
    //                         value      = *(uint16_t *)(data_ptr + previous_data_offset);

    //                     } else if (strcmp(c_type, "uint32_t") == 0) {
    //                         field_size = sizeof(uint32_t);
    //                         value      = *(uint32_t *)(data_ptr + previous_data_offset);

    //                     } else if (strcmp(c_type, "uint64_t") == 0) {
    //                         field_size = sizeof(uint64_t);
    //                         value      = *(uint64_t *)(data_ptr + previous_data_offset);
    //                     }

    //                     if (field_size > 0 && value != 0) {
    //                         LOG_RAW(" (0b");

    //                         bool got_set_bit = false;

    //                         for (int i = (field_size * 8) - 1; i >= 0; i--) {
    //                             unsigned current_bit = (value >> i) & 1;
    //                             got_set_bit |= current_bit;

    //                             if (got_set_bit) {
    //                                 LOG_RAW("%u%s", current_bit, ((unsigned)i < (field_size * 8) - 1 && i % 4 == 0 && i > 0) ? "'" : "");
    //                             }
    //                         }

    //                         LOG_RAW(")");
    //                     }

    //                 } else if (strcmp(c_type, "uint32_t") == 0 && strstr(field_name, "device_id") != nullptr) {
    //                     // Device ID
    //                     uint32_t device_id = *(uint32_t *)(data_ptr + previous_data_offset);
    //                     char     device_id_buffer[80];
    //                     device::Device::device_id_print_buffer(device_id_buffer, sizeof(device_id_buffer), device_id);
    //                     LOG_RAW(" (%s)", device_id_buffer);
    //                 }

    //             } else if (array_size == 4 && strcmp(c_type, "float") == 0 && (strcmp(field_name, "q") == 0 || strncmp(field_name, "q_", 2) == 0)) {
    //                 // attitude
    //                 float         *attitude = (float *)(data_ptr + previous_data_offset);
    //                 matrix::Eulerf euler{matrix::Quatf{attitude}};
    //                 LOG_RAW(" (Roll: %.1f deg, Pitch: %.1f deg, Yaw: %.1f deg)",
    //                         (double)math::degrees(euler(0)), (double)math::degrees(euler(1)), (double)math::degrees(euler(2)));
    //             }

    // #pragma GCC diagnostic pop

    //             LOG_RAW("\n");

    //         } else {
    //             // extract the topic name
    //             char         topic_name[80];
    //             const size_t topic_name_len = array_size > 1 ? array_idx - format_idx - 1 : field_name_idx - format_idx - 1;

    //             if (topic_name_len >= sizeof(topic_name)) {
    //                 LOG_E("topic name too long in %s (max: %u)", meta->o_name, (unsigned)sizeof(topic_name));
    //                 return;
    //             }

    //             memcpy(topic_name, meta->o_fields + format_idx, topic_name_len);
    //             topic_name[topic_name_len] = '\0';

    //             // find the metadata
    //             const orb_metadata *const *topics      = orb_get_topics();
    //             const orb_metadata        *found_topic = nullptr;

    //             for (size_t i = 0; i < orb_topics_count(); i++) {
    //                 if (rt_strcmp(topics[i]->o_name, topic_name) == 0) {
    //                     found_topic = topics[i];
    //                     break;
    //                 }
    //             }

    //             if (!found_topic) {
    //                 LOG_E("Topic %s did not match any known topics", topic_name);
    //                 return;
    //             }

    //             // print recursively
    //             for (int i = 0; i < array_size; ++i) {
    //                 LOG_RAW("  %s", field_name);

    //                 if (array_size > 1) {
    //                     LOG_RAW("[%i]", i);
    //                 }

    //                 LOG_RAW(" (%s):\n", topic_name);
    //                 orb_print_message_internal(found_topic, data_ptr + data_offset, false);
    //                 data_offset += found_topic->o_size;
    //             }
    //         }

    //         format_idx = end_field_idx + 1;
    //     }
}
