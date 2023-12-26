
#include <stdio.h>
#include "arm_data_processing.h"
#include "arm_core.h"
#include "registers.h"
#include "memory.h"

void test_add(arm_core p)
{
  arm_data_processing_immediate_msr(p, 0);
}

int main()
{
  arm_core p = arm_create(registers_create(), memory_create(2048));
  test_add(p);

  return 0;
}