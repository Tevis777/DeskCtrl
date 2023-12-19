#line 1 "C:\\Workspace\\DeskCtrl\\firmware\\DeskCtrl\\src\\Memory\\Memory.h"
#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdint.h>

class Memory
{
public:
  static constexpr uint32_t SIZE = 1024;

  static void Init(void);
  static bool Save(uint32_t addr, const void* data, uint32_t size);
  static bool Load(uint32_t addr, void* data, uint32_t size);

private:
};

#endif
