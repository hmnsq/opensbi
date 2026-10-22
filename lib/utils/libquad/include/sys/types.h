/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Jessica Clarke <jrtc27@jrtc27.com>
 */

#నిర్వచితం_కాకపోతే __SYS_TYPES_H__
#నిర్వచించు __SYS_TYPES_H__

#అన్తర్భావ <sbi/sbi_types.h>

రకనిర్వచనం గుర్తులేని దీర్ఘ u_long;

రకనిర్వచనం int64_t quad_t;
రకనిర్వచనం uint64_t u_quad_t;

#ఒకవేళ __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#నిర్వచించు _QUAD_LOWWORD 1
#నిర్వచించు _QUAD_HIGHWORD 0
#లేకపోతే
#నిర్వచించు _QUAD_LOWWORD 0
#నిర్వచించు _QUAD_HIGHWORD 1
#అయితే_ముగింపు

#అయితే_ముగింపు
