/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Jessica Clarke <jrtc27@jrtc27.com>
 */

#جيڪڏهن_وصف_نه_ٿيل __SYS_TYPES_H__
#وصف_ڪريو __SYS_TYPES_H__

#ضمن <sbi/sbi_types.h>

تعريفنوع بنانشان ڊگهو u_long;

تعريفنوع int64_t quad_t;
تعريفنوع uint64_t u_quad_t;

#إذا __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#وصف_ڪريو _QUAD_LOWWORD 1
#وصف_ڪريو _QUAD_HIGHWORD 0
#وإلا
#وصف_ڪريو _QUAD_LOWWORD 0
#وصف_ڪريو _QUAD_HIGHWORD 1
#جيڪڏهن_پورو

#جيڪڏهن_پورو
