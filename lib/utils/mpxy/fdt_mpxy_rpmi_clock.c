/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ضمن <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ساكن بنية mpxy_rpmi_service_داده clock_services[] = {
{
	.id = RPMI_CLOCK_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = حجم(بنية rpmi_enable_notification_req),
	.max_tx_len = حجم(بنية rpmi_enable_notification_req),
	.min_rx_len = حجم(بنية rpmi_enable_notification_resp),
	.max_rx_len = حجم(بنية rpmi_enable_notification_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_رقم_CLOCKS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = حجم(بنية rpmi_clock_get_رقم_clocks_resp),
	.max_rx_len = حجم(بنية rpmi_clock_get_رقم_clocks_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_صفات,
	.min_tx_len = حجم(بنية rpmi_clock_get_صفات_req),
	.max_tx_len = حجم(بنية rpmi_clock_get_صفات_req),
	.min_rx_len = حجم(بنية rpmi_clock_get_صفات_resp),
	.max_rx_len = حجم(بنية rpmi_clock_get_صفات_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_SUPPORTED_RATES,
	.min_tx_len = حجم(بنية rpmi_clock_get_supported_rates_req),
	.max_tx_len = حجم(بنية rpmi_clock_get_supported_rates_req),
	.min_rx_len = حجم(بنية rpmi_clock_get_supported_rates_resp),
	.max_rx_len = -1U,
},
{
	.id = RPMI_CLOCK_SRV_مجموعه_CONFIG,
	.min_tx_len = حجم(بنية rpmi_clock_مجموعه_config_req),
	.max_tx_len = حجم(بنية rpmi_clock_مجموعه_config_req),
	.min_rx_len = حجم(بنية rpmi_clock_مجموعه_config_resp),
	.max_rx_len = حجم(بنية rpmi_clock_مجموعه_config_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_CONFIG,
	.min_tx_len = حجم(بنية rpmi_clock_get_config_req),
	.max_tx_len = حجم(بنية rpmi_clock_get_config_req),
	.min_rx_len = حجم(بنية rpmi_clock_get_config_resp),
	.max_rx_len = حجم(بنية rpmi_clock_get_config_resp),
},
{
	.id = RPMI_CLOCK_SRV_مجموعه_نرخ,
	.min_tx_len = حجم(بنية rpmi_clock_مجموعه_نرخ_req),
	.max_tx_len = حجم(بنية rpmi_clock_مجموعه_نرخ_req),
	.min_rx_len = حجم(بنية rpmi_clock_مجموعه_نرخ_resp),
	.max_rx_len = حجم(بنية rpmi_clock_مجموعه_نرخ_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_نرخ,
	.min_tx_len = حجم(بنية rpmi_clock_get_نرخ_req),
	.max_tx_len = حجم(بنية rpmi_clock_get_نرخ_req),
	.min_rx_len = حجم(بنية rpmi_clock_get_نرخ_resp),
	.max_rx_len = حجم(بنية rpmi_clock_get_نرخ_resp),
},
};

ساكن ثابت بنية mpxy_rpmi_mbox_داده clock_داده = {
	.servicegrp_id = RPMI_SRVGRP_CLOCK,
	.رقم_services = RPMI_CLOCK_SRV_MAX_شمار,
	.service_داده = clock_services,
};

ساكن ثابت بنية fdt_تطابق clock_تطابق[] = {
	{ .compatible = "riscv,rpmi-mpxy-clock", .داده = &clock_داده },
	{ },
};

ثابت بنية fdt_driver fdt_mpxy_rpmi_clock = {
	.تطابق_جدول = clock_تطابق,
	.init = mpxy_rpmi_mbox_init,
};
