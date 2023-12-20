#ifndef _FLASH_H_
#define _FLASH_H_

#include <stdint.h>

class Flash
{
public:
  static constexpr uint32_t SIZE = 1024;

  void Init(void);
  bool Save(uint32_t addr, const void* data, uint32_t size);
  bool Load(uint32_t addr, void* data, uint32_t size);
private:
};

#endif //_FLASH_H_