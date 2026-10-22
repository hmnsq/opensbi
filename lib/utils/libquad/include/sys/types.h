/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Jessica Clarke <jrtc27@jrtc27.com>
 */

#اگر_غیر_تعریف_شدہ __SYS_TYPES_H__
#تعریف_کرو __SYS_TYPES_H__

#شامل <sbi/sbi_types.h>

قسمتعریف غیرعلامتی طویل u_long;

قسمتعریف int64_t quad_t;
قسمتعریف uint64_t u_quad_t;

#اگر __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#تعریف_کرو _QUAD_LOWWORD 1
#تعریف_کرو _QUAD_HIGHWORD 0
#ورنہ
#تعریف_کرو _QUAD_LOWWORD 0
#تعریف_کرو _QUAD_HIGHWORD 1
#اگر_ختم

#اگر_ختم
