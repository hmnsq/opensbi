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

ساكن بنية mpxy_rpmi_service_معلومات clock_services[] = {
{
	.id = RPMI_CLOCK_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = حجم(بنية rpmi_enable_notification_req),
	.max_tx_len = حجم(بنية rpmi_enable_notification_req),
	.min_rx_len = حجم(بنية rpmi_enable_notification_resp),
	.max_rx_len = حجم(بنية rpmi_enable_notification_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_عدد_CLOCKS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = حجم(بنية rpmi_clock_get_عدد_clocks_resp),
	.max_rx_len = حجم(بنية rpmi_clock_get_عدد_clocks_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_ځانګړتیاوې,
	.min_tx_len = حجم(بنية rpmi_clock_get_ځانګړتیاوې_req),
	.max_tx_len = حجم(بنية rpmi_clock_get_ځانګړتیاوې_req),
	.min_rx_len = حجم(بنية rpmi_clock_get_ځانګړتیاوې_resp),
	.max_rx_len = حجم(بنية rpmi_clock_get_ځانګړتیاوې_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_SUPPORTED_RATES,
	.min_tx_len = حجم(بنية rpmi_clock_get_supported_rates_req),
	.max_tx_len = حجم(بنية rpmi_clock_get_supported_rates_req),
	.min_rx_len = حجم(بنية rpmi_clock_get_supported_rates_resp),
	.max_rx_len = -1U,
},
{
	.id = RPMI_CLOCK_SRV_سېټ_CONFIG,
	.min_tx_len = حجم(بنية rpmi_clock_سېټ_config_req),
	.max_tx_len = حجم(بنية rpmi_clock_سېټ_config_req),
	.min_rx_len = حجم(بنية rpmi_clock_سېټ_config_resp),
	.max_rx_len = حجم(بنية rpmi_clock_سېټ_config_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_CONFIG,
	.min_tx_len = حجم(بنية rpmi_clock_get_config_req),
	.max_tx_len = حجم(بنية rpmi_clock_get_config_req),
	.min_rx_len = حجم(بنية rpmi_clock_get_config_resp),
	.max_rx_len = حجم(بنية rpmi_clock_get_config_resp),
},
{
	.id = RPMI_CLOCK_SRV_سېټ_شرح,
	.min_tx_len = حجم(بنية rpmi_clock_سېټ_شرح_req),
	.max_tx_len = حجم(بنية rpmi_clock_سېټ_شرح_req),
	.min_rx_len = حجم(بنية rpmi_clock_سېټ_شرح_resp),
	.max_rx_len = حجم(بنية rpmi_clock_سېټ_شرح_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_شرح,
	.min_tx_len = حجم(بنية rpmi_clock_get_شرح_req),
	.max_tx_len = حجم(بنية rpmi_clock_get_شرح_req),
	.min_rx_len = حجم(بنية rpmi_clock_get_شرح_resp),
	.max_rx_len = حجم(بنية rpmi_clock_get_شرح_resp),
},
};

ساكن ثابت بنية mpxy_rpmi_mbox_معلومات clock_معلومات = {
	.servicegrp_id = RPMI_SRVGRP_CLOCK,
	.عدد_services = RPMI_CLOCK_SRV_MAX_شمېر,
	.service_معلومات = clock_services,
};

ساكن ثابت بنية fdt_سمون clock_سمون[] = {
	{ .compatible = "riscv,rpmi-mpxy-clock", .معلومات = &clock_معلومات },
	{ },
};

ثابت بنية fdt_driver fdt_mpxy_rpmi_clock = {
	.سمون_جدول = clock_سمون,
	.init = mpxy_rpmi_mbox_init,
};
