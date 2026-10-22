/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Jessica Clarke <jrtc27@jrtc27.com>
 */

#ifndef __SYS_TYPES_H__
#define __SYS_TYPES_H__

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_types.h>

ପ୍ରକାରବ୍ୟାଖ୍ୟା unsigned long u_long;

ପ୍ରକାରବ୍ୟାଖ୍ୟା int64_t quad_t;
ପ୍ରକାରବ୍ୟାଖ୍ୟା uint64_t u_quad_t;

#ଯଦି __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define _QUAD_LOWWORD 1
#define _QUAD_HIGHWORD 0
#ଅନ୍ୟଥା
#define _QUAD_LOWWORD 0
#define _QUAD_HIGHWORD 1
#endif

#endif
