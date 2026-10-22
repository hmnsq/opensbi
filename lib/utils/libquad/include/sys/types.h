/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Jessica Clarke <jrtc27@jrtc27.com>
 */

#यदि_अपरिभाषितम् __SYS_TYPES_H__
#परिभाषय __SYS_TYPES_H__

#अन्तर्भाव <sbi/sbi_types.h>

प्रकारनिर्देश अचिह्नित दीर्घ u_long;

प्रकारनिर्देश int64_t quad_t;
प्रकारनिर्देश uint64_t u_quad_t;

#यदि __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#परिभाषय _QUAD_LOWWORD 1
#परिभाषय _QUAD_HIGHWORD 0
#अन्यथा
#परिभाषय _QUAD_LOWWORD 0
#परिभाषय _QUAD_HIGHWORD 1
#यदि_अन्तः

#यदि_अन्तः
