#include "utils.h"

uint16_t rng16(uint16_t seed = 0)
{
  // Call four times with non-zero args to seed.
  // Period is greater than one quintillion.
  static uint32_t k = 1148543851;
  static uint32_t i = 1234567891;
  if (seed)
  {
    i = (i << 16) + ((k << 16) >> 16);
    k = (k << 16) + seed;
    return (0);
  }

  k = 30903 * (k & 65535) + (k >> 16);
  i = 31083 * (i & 65535) + (i >> 16);
  return (k + i);
}

uint16_t generate_id16()
{

  randomSeed(analogRead(0));
  for (uint8_t i = 0; i < 4; i++)
  {
    rng16(rand());
  }

  return rng16();
}

uint32_t crc32(void *buff, size_t length)
{

  const uint32_t crc_table[16] = {
      0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
      0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
      0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
      0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c};

  uint32_t crc = ~0L;

  for (size_t index = 0; index < length; ++index)
  {
    crc = crc_table[(crc ^ (*((uint8_t *)buff) + index)) & 0x0f] ^ (crc >> 4);
    crc = crc_table[(crc ^ (*((uint8_t *)buff) + index) >> 4) & 0x0f] ^ (crc >> 4);
    crc = ~crc;
  }

  return crc;
}

uint16_t readVCC()
{
  uint32_t result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2);            // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA, ADSC))
    ;
  result = ADCL;
  result |= ADCH << 8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return result;
}

void reboot()
{
  asm volatile("jmp 0");
}

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char *sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif // __arm__

int freeMemory()
{
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char *>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif // __arm__
}