/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Jessica Clarke <jrtc27@jrtc27.com>
 */

#अगर_अघोषित __SYS_TYPES_H__
#घोषणा __SYS_TYPES_H__

#समावेश <sbi/sbi_types.h>

प्रकार अचिन्हित दीर्घ u_long;

प्रकार int64_t quad_t;
प्रकार uint64_t u_quad_t;

#अगर __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#घोषणा _QUAD_LOWWORD 1
#घोषणा _QUAD_HIGHWORD 0
#अन्यथा
#घोषणा _QUAD_LOWWORD 0
#घोषणा _QUAD_HIGHWORD 1
#पूर्ण_अगर

#पूर्ण_अगर
