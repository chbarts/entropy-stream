#include <netinet/in.h>
#include <sys/socket.h>
#include <event2/util.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/thread.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <float.h>
#include "entropy.h"

static void readcb(struct bufferevent *bev, void *ctx)
{
    struct evbuffer *input, *output;
    enctx *ectx = ctx;
    char res[LDBL_DIG + 10];
    unsigned char buf[BUFSIZ];
    size_t len;

    input = bufferevent_get_input(bev);
    output = bufferevent_get_output(bev);

    while (evbuffer_get_length(input) > 0) {
        len = evbuffer_remove(input, buf, sizeof(buf));
        en_add_buf(ectx, buf, len);
    }

    snprintf(res, sizeof(res) - 1, "%.*Lg\n", LDBL_DIG,
             en_entropy_ld(ectx));
    evbuffer_add(output, res, strlen(res));
}

static void errorcb(struct bufferevent *bev, short error, void *ctx)
{
    en_end((enctx *) ctx);
    bufferevent_free(bev);
}

static void acceptcb(evutil_socket_t listener, short event, void *arg)
{
    enctx *ectx;
    struct bufferevent *bev;
    struct event_base *base = arg;
    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);
    int fd;

    if ((fd = accept(listener, (struct sockaddr *) &ss, &slen)) == -1) {
        perror("accept()");
        return;
    }

    if (fd > FD_SETSIZE) {
        perror("fd > FD_SETSIZE");
        close(fd);
        return;
    }

    if ((ectx = en_start()) == NULL) {
        perror("en_start()");
        close(fd);
        return;
    }

    evutil_make_socket_nonblocking(fd);
    if ((bev =
         bufferevent_socket_new(base, fd,
                                BEV_OPT_CLOSE_ON_FREE)) == NULL) {
        perror("bufferevent_socket_new()");
        en_end(ectx);
        close(fd);
        return;
    }

    bufferevent_setcb(bev, readcb, (void *) ectx, errorcb, (void *) ectx);
    bufferevent_setwatermark(bev, EV_READ, 0, BUFSIZ);
    bufferevent_enable(bev, EV_READ | EV_WRITE);
}

int main(int argc, char *argv[])
{
    evutil_socket_t listener;
    struct sockaddr_in sin;
    struct event *listener_event;
    struct event_base *eventbase;

    setvbuf(stdout, NULL, _IONBF, 0);

    if ((eventbase = event_base_new()) == NULL) {
        perror("event_base_new()");
        exit(EXIT_FAILURE);
    }

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(40713);

    if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket()");
        event_base_free(eventbase);
        exit(EXIT_FAILURE);
    }

    evutil_make_socket_nonblocking(listener);

    if (bind(listener, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
        perror("bind()");
        event_base_free(eventbase);
        exit(EXIT_FAILURE);
    }

    if (listen(listener, 16) < 0) {
        perror("listen()");
        event_base_free(eventbase);
        exit(EXIT_FAILURE);
    }

    listener_event =
        event_new(eventbase, listener, EV_READ | EV_PERSIST, acceptcb,
                  eventbase);
    event_add(listener_event, NULL);

    while (event_base_dispatch(eventbase) == 1);

    event_base_free(eventbase);
    return 0;
}
