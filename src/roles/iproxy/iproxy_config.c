

#include "iproxy.h"

isshe_int_t
iproxy_config_parse(iproxy_config_t *config,
    isshe_json_t *json, isshe_log_t *log)
{
    isshe_json_t *iproxy_json;
    isshe_json_t *tmp;

    if (!json) {
        return ISSHE_ERROR;
    }

    iproxy_json = isshe_json_object_get(json, IPROXY_CONFIG_KEY);
    if (!iproxy_json) {
        isshe_log_alert(log, "iproxy: cannot found iproxy config");
        return ISSHE_ERROR;
    }

    // 解析日志级别、日志文件路径
    config->log_file = iconfig_log_parse(iproxy_json, &config->log_level);

    // 解析出口/入口的地址、端口、协议
    tmp = isshe_json_object_get(iproxy_json, "in");
    config->inarray = iconfig_connection_parse(config->mempool, tmp, &config->nin);
    if (!config->inarray) {
        isshe_log_alert(log, "iproxy: invalid inbound config");
        return ISSHE_ERROR;
    }

    tmp = isshe_json_object_get(iproxy_json, "connpool");
    config->connpool_size = IPROXY_CONNPOOL_DEFAULT_SIZE;
    if (isshe_json_is_number(tmp)) {
        config->connpool_size = (isshe_size_t)tmp->vnumber;
    }

    return ISSHE_OK;
}

void
iproxy_config_print(iproxy_config_t *config, isshe_log_t *log)
{
    isshe_int_t i;

    if (!config || !log) {
        return ;
    }

    isshe_log_info(log,
        "==================== iproxy config ==================");
    isshe_log_info(log,
        "- log level:file       : %s:%s",
        isshe_log_level_to_string(config->log_level), config->log_file);

    for (i = 0; i < config->nin; i++) {
        isshe_log_info(log,
            "- in addr              : %s:%d:%s",
            config->inarray[i].addr->addr,
            config->inarray[i].addr->port,
            config->inarray[i].protocol_text);
    }

    isshe_log_info(log,
        "- connection pool size : %d", config->connpool_size);
    isshe_log_info(log,
        "=====================================================");
}
