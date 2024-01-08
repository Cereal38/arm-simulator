
#include <stdio.h>
#include <assert.h>
#include "arm_core.h"
#include "registers.h"
#include "memory.h"
#include "arm_constants.h"
#include "util.h"

int main()
{
  arm_core p = arm_create(registers_create(), memory_create(2048));

  memory_destroy(p->mem);
  registers_destroy(p->reg);
  arm_destroy(p);

  return 0;
}