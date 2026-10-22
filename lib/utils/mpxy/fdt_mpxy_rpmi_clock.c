/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ਸਥਿਰ ਬਣਤਰ mpxy_rpmi_service_ਅੰਕੜੇ clock_services[] = {
{
	.id = RPMI_CLOCK_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_enable_notification_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_enable_notification_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_enable_notification_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_enable_notification_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_ਅੰਕ_CLOCKS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_clock_get_ਅੰਕ_clocks_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_clock_get_ਅੰਕ_clocks_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_ਗੁਣ,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_clock_get_ਗੁਣ_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_clock_get_ਗੁਣ_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_clock_get_ਗੁਣ_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_clock_get_ਗੁਣ_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_SUPPORTED_RATES,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_clock_get_supported_rates_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_clock_get_supported_rates_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_clock_get_supported_rates_resp),
	.max_rx_len = -1U,
},
{
	.id = RPMI_CLOCK_SRV_ਸੈੱਟ_CONFIG,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_clock_ਸੈੱਟ_config_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_clock_ਸੈੱਟ_config_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_clock_ਸੈੱਟ_config_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_clock_ਸੈੱਟ_config_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_CONFIG,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_clock_get_config_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_clock_get_config_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_clock_get_config_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_clock_get_config_resp),
},
{
	.id = RPMI_CLOCK_SRV_ਸੈੱਟ_ਦਰ,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_clock_ਸੈੱਟ_ਦਰ_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_clock_ਸੈੱਟ_ਦਰ_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_clock_ਸੈੱਟ_ਦਰ_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_clock_ਸੈੱਟ_ਦਰ_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_ਦਰ,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_clock_get_ਦਰ_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_clock_get_ਦਰ_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_clock_get_ਦਰ_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_clock_get_ਦਰ_resp),
},
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ mpxy_rpmi_mbox_ਅੰਕੜੇ clock_ਅੰਕੜੇ = {
	.servicegrp_id = RPMI_SRVGRP_CLOCK,
	.ਅੰਕ_services = RPMI_CLOCK_SRV_MAX_ਗਿਣਤੀ,
	.service_ਅੰਕੜੇ = clock_services,
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ clock_ਮੇਲ[] = {
	{ .compatible = "riscv,rpmi-mpxy-clock", .ਅੰਕੜੇ = &clock_ਅੰਕੜੇ },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_mpxy_rpmi_clock = {
	.ਮੇਲ_ਸਾਰਣੀ = clock_ਮੇਲ,
	.init = mpxy_rpmi_mbox_init,
};
