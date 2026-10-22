/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ಸ್ಥಿರ ಸಂರಚನಾ mpxy_rpmi_service_data clock_services[] = {
{
	.id = RPMI_CLOCK_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_enable_notification_req),
	.max_tx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_enable_notification_req),
	.min_rx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_enable_notification_resp),
	.max_rx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_enable_notification_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_ಅಂಕೆ_CLOCKS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_clock_get_ಅಂಕೆ_clocks_resp),
	.max_rx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_clock_get_ಅಂಕೆ_clocks_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_ATTRIBUTES,
	.min_tx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_clock_get_attributes_req),
	.max_tx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_clock_get_attributes_req),
	.min_rx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_clock_get_attributes_resp),
	.max_rx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_clock_get_attributes_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_SUPPORTED_RATES,
	.min_tx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_clock_get_supported_rates_req),
	.max_tx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_clock_get_supported_rates_req),
	.min_rx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_clock_get_supported_rates_resp),
	.max_rx_len = -1U,
},
{
	.id = RPMI_CLOCK_SRV_SET_CONFIG,
	.min_tx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_clock_set_config_req),
	.max_tx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_clock_set_config_req),
	.min_rx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_clock_set_config_resp),
	.max_rx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_clock_set_config_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_CONFIG,
	.min_tx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_clock_get_config_req),
	.max_tx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_clock_get_config_req),
	.min_rx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_clock_get_config_resp),
	.max_rx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_clock_get_config_resp),
},
{
	.id = RPMI_CLOCK_SRV_SET_RATE,
	.min_tx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_clock_set_rate_req),
	.max_tx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_clock_set_rate_req),
	.min_rx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_clock_set_rate_resp),
	.max_rx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_clock_set_rate_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_RATE,
	.min_tx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_clock_get_rate_req),
	.max_tx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_clock_get_rate_req),
	.min_rx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_clock_get_rate_resp),
	.max_rx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_clock_get_rate_resp),
},
};

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ mpxy_rpmi_mbox_data clock_data = {
	.servicegrp_id = RPMI_SRVGRP_CLOCK,
	.ಅಂಕೆ_services = RPMI_CLOCK_SRV_MAX_ಎಣಿಕೆ,
	.service_data = clock_services,
};

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match clock_match[] = {
	{ .compatible = "riscv,rpmi-mpxy-clock", .data = &clock_data },
	{ },
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_mpxy_rpmi_clock = {
	.match_table = clock_match,
	.init = mpxy_rpmi_mbox_init,
};
