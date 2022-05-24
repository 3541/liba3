/*
 * ATOMIC SHIM -- Cross-platform shim for stdatomic.
 *
 * Copyright (c) 2022, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in the project root
 * for details.
 *
 * Windows does not provide stdatomic. For now, this shim provides only the primitives used by
 * A3Spmc.
 */

#include <a3/shim/atomic.h>
