#include "basic_funcs.h"

void core_print_uint32(uint32_t data)
{
  Serial.printf("core%d - %d \n",get_core_num(),data);
}


bool write_to_other_core(uint32_t data)
{
  if (rp2040.fifo.push_nb(data)==false)
  {
    core_debug((char*)"failed to push\n");
    return false;
  }
  return true;
}

bool read_from_other_core(uint32_t *p_data)
{
  if (rp2040.fifo.available())
  {
    if (rp2040.fifo.pop_nb(p_data)==false)
    {
      core_debug((char*)"failed to pop\n");
      return false;
    }
    core_print_uint32(*p_data);
    return true;
  }
  return false;
}

void core_debug(char *p_msg)
{
  Serial.printf("core%d - %s",get_core_num(),p_msg);
}

uint32_t floatToUint32(float value) {
  // Create a union of float and uint32_t types
  union {
    float floatValue;
    uint32_t uintValue;
  } converter;

  // Assign the float value to the union
  converter.floatValue = value;

  // Return the uint32_t value from the union
  return converter.uintValue;
}

float uint32ToFloat(uint32_t value) {
  // Create a union of float and uint32_t types
  union {
    float floatValue;
    uint32_t uintValue;
  } converter;

  // Assign the uint32_t value to the union
  converter.uintValue = value;

  // Return the float value from the union
  return converter.floatValue;
}




