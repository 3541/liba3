/*
 * SOCKLEN SHIM — Cross-platform shim for socklen_t and in_port_t.
 *
 * Copyright (c) 2024, Alex O'Brien <3541@3541.website>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 */

#pragma once

#include <a3/cpp.h>

#ifndef _WIN32
#include <netinet/in.h>
#else
#include <winsock2.h>
#endif

A3_H_BEGIN

#ifdef _WIN32
typedef int            A3Socklen;
typedef unsigned short A3Port;
typedef SOCKET         A3Socket;
#else
typedef socklen_t A3Socklen;
typedef in_port_t A3Port;
typedef int       A3Socket;
#endif

A3_H_END
