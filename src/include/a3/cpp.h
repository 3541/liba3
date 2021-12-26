#pragma once

#ifdef __cplusplus
#define A3_H_BEGIN   extern "C" {
#define A3_H_END     }
#define A3_CONSTEXPR constexpr
#define A3_EMPTY_INIT                                                                              \
    {}
#else
#define A3_H_BEGIN
#define A3_H_END
#define A3_CONSTEXPR
#define A3_EMPTY_INIT
#endif
