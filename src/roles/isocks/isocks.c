
#include "isocks.h"

void isocks_start(void *ctx)
{
    iconfig_t *config = (iconfig_t *)ctx;

    // TODO 设置进程标题

    // TODO...
    ilog_debug(config->log, "---in isocks_start: pid = %d", getpid());
    
    // never return!
    exit(0);
}