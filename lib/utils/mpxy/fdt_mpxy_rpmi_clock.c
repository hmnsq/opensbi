/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 *   Anup Patel <apatel@ventanamicro.com>
 */

#شامل <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ساکن ساخت mpxy_rpmi_service_ڈیٹا clock_services[] = {
{
	.id = RPMI_CLOCK_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = حجم(ساخت rpmi_enable_notification_req),
	.max_tx_len = حجم(ساخت rpmi_enable_notification_req),
	.min_rx_len = حجم(ساخت rpmi_enable_notification_resp),
	.max_rx_len = حجم(ساخت rpmi_enable_notification_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_عدد_CLOCKS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = حجم(ساخت rpmi_clock_get_عدد_clocks_resp),
	.max_rx_len = حجم(ساخت rpmi_clock_get_عدد_clocks_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_اوصاف,
	.min_tx_len = حجم(ساخت rpmi_clock_get_اوصاف_req),
	.max_tx_len = حجم(ساخت rpmi_clock_get_اوصاف_req),
	.min_rx_len = حجم(ساخت rpmi_clock_get_اوصاف_resp),
	.max_rx_len = حجم(ساخت rpmi_clock_get_اوصاف_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_SUPPORTED_RATES,
	.min_tx_len = حجم(ساخت rpmi_clock_get_supported_rates_req),
	.max_tx_len = حجم(ساخت rpmi_clock_get_supported_rates_req),
	.min_rx_len = حجم(ساخت rpmi_clock_get_supported_rates_resp),
	.max_rx_len = -1U,
},
{
	.id = RPMI_CLOCK_SRV_سیٹ_CONFIG,
	.min_tx_len = حجم(ساخت rpmi_clock_سیٹ_config_req),
	.max_tx_len = حجم(ساخت rpmi_clock_سیٹ_config_req),
	.min_rx_len = حجم(ساخت rpmi_clock_سیٹ_config_resp),
	.max_rx_len = حجم(ساخت rpmi_clock_سیٹ_config_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_CONFIG,
	.min_tx_len = حجم(ساخت rpmi_clock_get_config_req),
	.max_tx_len = حجم(ساخت rpmi_clock_get_config_req),
	.min_rx_len = حجم(ساخت rpmi_clock_get_config_resp),
	.max_rx_len = حجم(ساخت rpmi_clock_get_config_resp),
},
{
	.id = RPMI_CLOCK_SRV_سیٹ_شرح,
	.min_tx_len = حجم(ساخت rpmi_clock_سیٹ_شرح_req),
	.max_tx_len = حجم(ساخت rpmi_clock_سیٹ_شرح_req),
	.min_rx_len = حجم(ساخت rpmi_clock_سیٹ_شرح_resp),
	.max_rx_len = حجم(ساخت rpmi_clock_سیٹ_شرح_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_شرح,
	.min_tx_len = حجم(ساخت rpmi_clock_get_شرح_req),
	.max_tx_len = حجم(ساخت rpmi_clock_get_شرح_req),
	.min_rx_len = حجم(ساخت rpmi_clock_get_شرح_resp),
	.max_rx_len = حجم(ساخت rpmi_clock_get_شرح_resp),
},
};

ساکن ثابت ساخت mpxy_rpmi_mbox_ڈیٹا clock_ڈیٹا = {
	.servicegrp_id = RPMI_SRVGRP_CLOCK,
	.عدد_services = RPMI_CLOCK_SRV_MAX_گنتی,
	.service_ڈیٹا = clock_services,
};

ساکن ثابت ساخت fdt_مطابقت clock_مطابقت[] = {
	{ .compatible = "riscv,rpmi-mpxy-clock", .ڈیٹا = &clock_ڈیٹا },
	{ },
};

ثابت ساخت fdt_driver fdt_mpxy_rpmi_clock = {
	.مطابقت_جدول = clock_مطابقت,
	.init = mpxy_rpmi_mbox_init,
};
