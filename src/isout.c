


#include <stdio.h>
#include <getopt.h>

#include "isout.h"
#include "iconfig.h"
#include "isshe_common.h"

void isout_usage_then_exit()
{
    printf("Usage: isout <-c filename> [-h]" ISSHE_LINEFEED
            "  -c filename   : set configuration file (default: None)" ISSHE_LINEFEED
            "  -l filename   : set log file (default: None)" ISSHE_LINEFEED
            "  -h            : this help" ISSHE_LINEFEED
    );
    exit(0);
}


void isout_optget(int argc, char *argv[], iconfig_t *config)
{
    int ch;

    // options descriptor
    static struct option longopts[] = {
        { "config",     required_argument,      NULL,           'c' },
        { "log",        required_argument,      NULL,           'l' },
        { "help",       no_argument,            NULL,           'h' },
        { NULL,         0,                      NULL,           0 }
    };

    while ((ch = getopt_long(argc, argv, "c:hl:", longopts, NULL)) != -1) {
        switch (ch) {
            case 'c':
                config->config_file = optarg;
                //printf("config file = %s\n", config->config_file);
                break;
            case 'l':
                config->log_file = optarg;
            case 'h':
            default:
                isout_usage_then_exit();
        }
    }
    //argc -= optind;
    //argv += optind;

    if (!config->config_file) {
        isout_usage_then_exit();
    }
}

int main(int argc, char *argv[])
{
    // 解析config
    iconfig_t *config;

    config = iconfig_new();
    if (!config) {
        printf("[error] failed to new config\n");
        exit(0);
    }

    isout_optget(argc, argv, config);

    iconfig_parse(config, config->config_file);

    iconfig_print(config);

    // 配置log
    //isout_log_init();

    // master接管进程
    //isout_start_master();

    iconfig_free(config);
}