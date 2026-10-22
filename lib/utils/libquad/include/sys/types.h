/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Jessica Clarke <jrtc27@jrtc27.com>
 */

#ifndef __SYS_TYPES_H__
#define __SYS_TYPES_H__

#अन्तर्भाव <sbi/sbi_types.h>

प्रकारनिर्देश अचिन्हित दीर्घ u_long;

प्रकारनिर्देश int64_t quad_t;
प्रकारनिर्देश uint64_t u_quad_t;

#यदि __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define _QUAD_LOWWORD 1
#define _QUAD_HIGHWORD 0
#अन्यथा
#define _QUAD_LOWWORD 0
#define _QUAD_HIGHWORD 1
#endif

#endif
