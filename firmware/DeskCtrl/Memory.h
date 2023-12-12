#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdint.h>

class Memory
{
public:
  static constexpr uint32_t SIZE = 1024;

  enum class Map
  {
    Config = 0,
    Position = 128
  };

  void Init(void);
  bool Save(uint32_t addr, const uint8_t* data, uint32_t size);
  bool Load(uint32_t addr, uint8_t* data, uint32_t size);

private:
};

#endif
