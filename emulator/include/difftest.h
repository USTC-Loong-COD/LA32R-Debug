#ifndef __DIFFTEST_H__
#define __DIFFTEST_H__
#include <stdint.h>
void init_difftest(char *ref_so_file, long img_size);
void difftest_sync();
void difftest_step(uint64_t n);

#endif