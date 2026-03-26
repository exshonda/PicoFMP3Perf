#include <stdio.h>
#include "pico/stdlib.h"
#include "t_syslog.h"
#include "log_output.h"
#include "sil.h"

int main()
{
    stdio_init_all();

    Asm("cpsid f":::"memory");

    sta_ker();

    return 0;
}
