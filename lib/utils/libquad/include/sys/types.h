/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Jessica Clarke <jrtc27@jrtc27.com>
 */

#ifndef __SYS_TYPES_H__
#define __SYS_TYPES_H__

#ಅನ್ತರ್ಭಾವ <sbi/sbi_types.h>

ಪ್ರಕಾರನಿರ್ದೇಶ unsigned long u_long;

ಪ್ರಕಾರನಿರ್ದೇಶ int64_t quad_t;
ಪ್ರಕಾರನಿರ್ದೇಶ uint64_t u_quad_t;

#ಯದಿ __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define _QUAD_LOWWORD 1
#define _QUAD_HIGHWORD 0
#ಅನ್ಯಥಾ
#define _QUAD_LOWWORD 0
#define _QUAD_HIGHWORD 1
#endif

#endif
