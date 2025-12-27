/*
 * ACCEPT SHIM — Cross-platform shim for SOCK_CLOEXEC and SOCK_NONBLOCK.
 *
 * Copyright (c) 2024, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 */

#include "a3/shim/accept.h"

#ifdef A3_SHIM_ACCEPT_IOCTLSOCKET

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#include "a3/shim/socket_types.h"

A3Socket a3_shim_accept(A3Socket fd, struct sockaddr* addr, A3Socklen* len, int flags) {
    assert(!(flags & ~(A3_SOCK_CLOEXEC | A3_SOCK_NONBLOCK)));

    A3Socket res = accept(fd, addr, len);
    if (res == INVALID_SOCKET)
        return -WSAGetLastError();

    if (flags & A3_SOCK_NONBLOCK) {
        if (ioctlsocket(res, FIONBIO, &(u_long){1}) < 0) {
            perror("accept/ioctlsocket");
            abort();
        }
    }

    return res;
}

#endif
