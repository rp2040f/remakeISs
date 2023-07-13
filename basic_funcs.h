#ifndef basic_funcs_H
#define basic_funcs_H

#include <stdint.h>
#include <Arduino.h>

void core_debug(char *p_msg);
void core_print_uint32(uint32_t data);
bool write_to_other_core(uint32_t data);
bool read_from_other_core(uint32_t *p_data);

float uint32ToFloat(uint32_t value);
uint32_t floatToUint32(float value);

#endif