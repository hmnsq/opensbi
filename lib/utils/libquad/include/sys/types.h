/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Jessica Clarke <jrtc27@jrtc27.com>
 */

#ifndef __SYS_TYPES_H__
#define __SYS_TYPES_H__

#અન્તર્ભાવ <sbi/sbi_types.h>

પ્રકારવ્યાખ્યા unsigned long u_long;

પ્રકારવ્યાખ્યા int64_t quad_t;
પ્રકારવ્યાખ્યા uint64_t u_quad_t;

#જો __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define _QUAD_LOWWORD 1
#define _QUAD_HIGHWORD 0
#નહીંતર
#define _QUAD_LOWWORD 0
#define _QUAD_HIGHWORD 1
#endif

#endif
