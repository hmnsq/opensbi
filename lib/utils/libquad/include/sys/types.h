/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Jessica Clarke <jrtc27@jrtc27.com>
 */

#ifndef __SYS_TYPES_H__
#define __SYS_TYPES_H__

#अन्तर्भाव <sbi/sbi_types.h>

प्रकारव्याख्या अचिन्हित दीर्घ u_long;

प्रकारव्याख्या int64_t quad_t;
प्रकारव्याख्या uint64_t u_quad_t;

#जर __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define _QUAD_LOWWORD 1
#define _QUAD_HIGHWORD 0
#अन्यथा
#define _QUAD_LOWWORD 0
#define _QUAD_HIGHWORD 1
#endif

#endif
