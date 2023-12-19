#ifndef _STORAGE_H_
#define _STORAGE_H_

#include <stdint.h>

class Storage
{
public:
    void Init();
    void SavePosition(uint32_t position);
    void LoadPosition(uint32_t& position);


private:



};


#endif //_STORAGE_H_