/*
 * ACCEPT SHIM — Cross-platform shim for SOCK_CLOEXEC and SOCK_NONBLOCK.
 *
 * Copyright (c) 2024, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 */

#include "a3/shim/platform.h"

#ifdef A3_PLATFORM_OS_LINUX

#define _GNU_SOURCE

#include <sys/socket.h>

#include "a3/shim/accept.h"
#include "a3/shim/socket_types.h"

A3Socket a3_shim_accept(A3Socket fd, struct sockaddr* addr, A3Socklen* len, int flags) {
    return accept4(fd, addr, len, flags);
}

#endif
