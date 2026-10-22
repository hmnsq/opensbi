/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Jessica Clarke <jrtc27@jrtc27.com>
 */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __SYS_TYPES_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __SYS_TYPES_H__

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_types.h>

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ u_long;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ int64_t quad_t;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ uint64_t u_quad_t;

#ਜੇ __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _QUAD_LOWWORD 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _QUAD_HIGHWORD 0
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _QUAD_LOWWORD 0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _QUAD_HIGHWORD 1
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ
