/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Jessica Clarke <jrtc27@jrtc27.com>
 */

#ifndef __SYS_TYPES_H__
#define __SYS_TYPES_H__

#அந்தர்பாவ <sbi/sbi_types.h>

வகைவரை unsigned long u_long;

வகைவரை int64_t quad_t;
வகைவரை uint64_t u_quad_t;

#எனில் __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define _QUAD_LOWWORD 1
#define _QUAD_HIGHWORD 0
#இல்லையெனில்
#define _QUAD_LOWWORD 0
#define _QUAD_HIGHWORD 1
#endif

#endif
