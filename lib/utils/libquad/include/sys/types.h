/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Jessica Clarke <jrtc27@jrtc27.com>
 */

#جے_بنا_تعریف __SYS_TYPES_H__
#تعریف_کرو __SYS_TYPES_H__

#ضمن <sbi/sbi_types.h>

تعريفنوع بنانشانی لما u_long;

تعريفنوع int64_t quad_t;
تعريفنوع uint64_t u_quad_t;

#إذا __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#تعریف_کرو _QUAD_LOWWORD 1
#تعریف_کرو _QUAD_HIGHWORD 0
#وإلا
#تعریف_کرو _QUAD_LOWWORD 0
#تعریف_کرو _QUAD_HIGHWORD 1
#جے_انت

#جے_انت
