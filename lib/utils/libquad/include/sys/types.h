/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Jessica Clarke <jrtc27@jrtc27.com>
 */

#ifndef __SYS_TYPES_H__
#define __SYS_TYPES_H__

#അന്തര്ഭാവ <sbi/sbi_types.h>

തരനിർവചനം unsigned long u_long;

തരനിർവചനം int64_t quad_t;
തരനിർവചനം uint64_t u_quad_t;

#എങ്കിൽ __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define _QUAD_LOWWORD 1
#define _QUAD_HIGHWORD 0
#അല്ലെങ്കിൽ
#define _QUAD_LOWWORD 0
#define _QUAD_HIGHWORD 1
#endif

#endif
