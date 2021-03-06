#ifndef _ISOUT_IPROXY_SESSION_H_
#define _ISOUT_IPROXY_SESSION_H_

#include "iproxy.h"

#define IPROXY_SESSION_FREE_IN          0x01        // 按位
#define IPROXY_SESSION_FREE_OUT         0x02

typedef struct iproxy_session_s iproxy_session_t;

struct iproxy_session_s
{
    isshe_connection_t          *inconn;
    isshe_connection_t          *outconn;
    ievent_buffer_event_t       *inbev;
    ievent_buffer_event_t       *outbev;
    iproxy_config_t             *config;
    isout_protocol_header_t     *inhdr;
    isout_protocol_options_t    *inopts;
    isshe_mempool_t             *mempool;
    isshe_uint64_t              inbytes;
    isshe_uint64_t              outbytes;
};

void iproxy_session_free(iproxy_session_t *session, isshe_int_t flag);

#endif