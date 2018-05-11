#include <string.h>
#include <stdlib.h>
#include "ping_cmd.h"
#include "ping_cli.h"


int ping_cli_handler(int len, char *param[])
{
    uint32_t count;
    uint32_t pktsz;
    uint32_t recv_timeout;
    uint32_t ping_period;

    if (len < 1) {
        return 1;
    } else {
        count = 3;
        pktsz = 64;
        recv_timeout = 1000; //ms
        ping_period = 1000;  //ms
    }

    if (len == 1 && (strcmp(param[0], "s") == NULL) ) {
        ping_stop();
        return 0;
    }

    if (len > 1) {
        count = strtol(param[1], NULL, 10);
    }

    if (len > 2) {
        pktsz = strtol(param[2], NULL, 10);
    }

    if (len > 3) {
        recv_timeout = strtol(param[3], NULL, 10);
    }

    if (len > 4) {
        ping_period = strtol(param[4], NULL, 10);
    }

    ping_init(count, param[0], strlen(param[0]), pktsz, recv_timeout, ping_period);

    return 0;
}

