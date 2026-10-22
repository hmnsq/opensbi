/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Jessica Clarke <jrtc27@jrtc27.com>
 */

#اگر_تعریف_نشده __SYS_TYPES_H__
#تعریف_کن __SYS_TYPES_H__

#ضمن <sbi/sbi_types.h>

تعريفنوع بیعلامت دراز u_long;

تعريفنوع int64_t quad_t;
تعريفنوع uint64_t u_quad_t;

#إذا __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#تعریف_کن _QUAD_LOWWORD 1
#تعریف_کن _QUAD_HIGHWORD 0
#وإلا
#تعریف_کن _QUAD_LOWWORD 0
#تعریف_کن _QUAD_HIGHWORD 1
#پایان_اگر

#پایان_اگر
