#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdint.h>

class Memory
{
public:
  Memory(uint32_t size);

  void Init(void);
  bool Save(uint32_t addr, const uint8_t* data, uint32_t size);
  bool Load(uint32_t addr, uint8_t* data, uint32_t size);

private:
  uint32_t m_size;


};

#endif
