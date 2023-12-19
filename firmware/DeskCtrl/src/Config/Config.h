#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <vector>
#include <stdint.h>

class Config
{
public:
  static constexpr uint32_t MAX_PRESETS = 4;

  void SavePreset(uint32_t preset);
  void DeletePreset(uint32_t preset);

  std::vector<uint32_t> GetPresets(void);


private:


};

#endif //_CONFIG_H_