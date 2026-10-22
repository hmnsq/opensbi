/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Jessica Clarke <jrtc27@jrtc27.com>
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __SYS_TYPES_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __SYS_TYPES_H__

#ಅನ್ತರ್ಭಾವ <sbi/sbi_types.h>

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ u_long;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ int64_t quad_t;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ uint64_t u_quad_t;

#ಆದರೆ __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#ವ್ಯಾಖ್ಯಾನಿಸು _QUAD_LOWWORD 1
#ವ್ಯಾಖ್ಯಾನಿಸು _QUAD_HIGHWORD 0
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು _QUAD_LOWWORD 0
#ವ್ಯಾಖ್ಯಾನಿಸು _QUAD_HIGHWORD 1
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ
