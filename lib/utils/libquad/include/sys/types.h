/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Jessica Clarke <jrtc27@jrtc27.com>
 */

#ifndef __SYS_TYPES_H__
#define __SYS_TYPES_H__

#شامل <sbi/sbi_types.h>

تعریفنوع unsigned long u_long;

تعریفنوع int64_t quad_t;
تعریفنوع uint64_t u_quad_t;

#اگر __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define _QUAD_LOWWORD 1
#define _QUAD_HIGHWORD 0
#وگرنه
#define _QUAD_LOWWORD 0
#define _QUAD_HIGHWORD 1
#endif

#endif
