
#ifndef _ISSHE_ISOUT_IEVENT_H_
#define _ISSHE_ISOUT_IEVENT_H_

#include "event2/event.h"
#include "event2/bufferevent.h"
#include "event2/buffer.h"
#include "event2/listener.h"

#include "isshe_common.h"

#define ievent_conn_listener_cb_t       evconnlistener_cb
#define ievent_socket_t                 evutil_socket_t
#define ievent_buffer_event_data_cb_t   bufferevent_data_cb
#define ievent_buffer_event_event_cb_t  bufferevent_event_cb

#define IEVENT_BEV_EVENT_READING        BEV_EVENT_READING
#define IEVENT_BEV_EVENT_WRITING        BEV_EVENT_WRITING
#define IEVENT_BEV_EVENT_EOF            BEV_EVENT_EOF
#define IEVENT_BEV_EVENT_ERROR          BEV_EVENT_ERROR
#define IEVENT_BEV_EVENT_TIMEOUT        BEV_EVENT_TIMEOUT
#define IEVENT_BEV_EVENT_CONNECTED      BEV_EVENT_CONNECTED

#define IEVENT_TIMEOUT                  EV_TIMEOUT
#define IEVENT_READ                     EV_READ
#define IEVENT_WRITE                    EV_WRITE
#define IEVENT_SIGNAL                   EV_SIGNAL

#define IEVENT_BUFFER_MAX_READ	        4096
#define IEVENT_BUFFER_MAX_READ_HALF     2048
#define IEVENT_OUTPUT_BUFFER_MAX        524288  // 512 * 1024

typedef struct event_base           ievent_base_t;
typedef struct evconnlistener       ievent_conn_listener_t;
typedef struct bufferevent          ievent_buffer_event_t;
typedef struct evbuffer             ievent_buffer_t;

typedef struct ievent_listener_s    ievent_listener_t;
typedef struct ievent_s             ievent_t;

struct ievent_listener_s
{
    ievent_conn_listener_t  *listener;
    ievent_listener_t       *next;
};

struct ievent_s
{
    ievent_base_t           *base;
    ievent_listener_t       *listeners;
    isshe_int_t             nlistener;
    isshe_mempool_t         *mempool;
    isshe_log_t             *log;
};


ievent_t *ievent_create(isshe_mempool_t *mempool, isshe_log_t *log);
isshe_void_t ievent_destroy(ievent_t *event);

ievent_listener_t *ievent_listener_create(ievent_t *event,
    ievent_conn_listener_cb_t cb, isshe_void_t *data,
    isshe_sockaddr_t *addr, isshe_socklen_t socklen);

isshe_void_t ievent_listener_destroy(
    ievent_t *event, ievent_listener_t *listener);

isshe_int_t ievent_dispatch(ievent_t *event);

isshe_int_t ievent_connection_close(isshe_fd_t fd);

ievent_buffer_event_t *ievent_buffer_event_socket_create(
    ievent_t *event, ievent_socket_t fd);

isshe_void_t ievent_buffer_event_socket_destroy(ievent_buffer_event_t * evb);

isshe_int_t ievent_buffer_event_socket_connect(ievent_buffer_event_t *evb,
    isshe_sockaddr_t *sockaddr, isshe_socklen_t socklen);

isshe_void_t ievent_buffer_event_setcb(ievent_buffer_event_t *bev,
    ievent_buffer_event_data_cb_t readcb,
    ievent_buffer_event_data_cb_t writecb,
    ievent_buffer_event_event_cb_t eventcb, isshe_void_t *cbarg);


isshe_int_t ievent_buffer_event_enable(
    ievent_buffer_event_t *bev, isshe_int_t event);

isshe_int_t ievent_buffer_event_disable(
    ievent_buffer_event_t *bev, isshe_int_t event);

ievent_buffer_t * ievent_buffer_event_get_input(
    ievent_buffer_event_t *bufev);

ievent_buffer_t * ievent_buffer_event_get_output(
    ievent_buffer_event_t *bev);

isshe_size_t ievent_buffer_get_length(const ievent_buffer_t *buffer);

isshe_size_t ievent_buffer_event_read(
    ievent_buffer_event_t *bev, isshe_void_t *data, isshe_size_t size);

isshe_int_t ievent_buffer_event_write(
    ievent_buffer_event_t *bev, const isshe_void_t *data, isshe_size_t size);

isshe_fd_t ievent_buffer_event_getfd(ievent_buffer_event_t *bev);

isshe_void_t ievent_buffer_event_free(ievent_buffer_event_t *bev);

isshe_int_t ievent_buffer_add_buffer(
    ievent_buffer_t *dst, ievent_buffer_t *src);

isshe_int_t ievent_buffer_copyout(
    ievent_buffer_t *buf,
    isshe_void_t *data_out,
    isshe_size_t data_len);

isshe_void_t ievent_buffer_event_water_mark_set(
    ievent_buffer_event_t *bev, isshe_int_t event,
    isshe_size_t lowmark, isshe_size_t highmark);

#endif