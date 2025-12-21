/*
 * ACCEPT SHIM — Cross-platform shim for SOCK_CLOEXEC and SOCK_NONBLOCK.
 *
 * Copyright (c) 2024, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 */

#pragma once

#ifdef _WIN32
#include <windows.h>
#include <ws2def.h>
#else
#include <sys/socket.h>
#endif

#include <a3/shim/socket_types.h>

#include <a3/cpp.h>

#ifdef SOCK_NONBLOCK
#define A3_SOCK_NONBLOCK SOCK_NONBLOCK
#else
#define A3_SOCK_NONBLOCK 0x4000
#endif

#ifdef SOCK_CLOEXEC
#define A3_SOCK_CLOEXEC SOCK_CLOEXEC
#else
#define A3_SOCK_CLOEXEC 0x8000
#endif

A3_H_BEGIN

A3Socket a3_shim_accept(A3Socket, struct sockaddr*, A3Socklen*, int flags);

A3_H_END
