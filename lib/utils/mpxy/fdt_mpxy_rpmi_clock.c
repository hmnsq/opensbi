/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 *   Anup Patel <apatel@ventanamicro.com>
 */

#अन्तर्भाव <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

स्थिर संरचना mpxy_rpmi_service_दत्त clock_services[] = {
{
	.id = RPMI_CLOCK_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = परिमाण(संरचना rpmi_enable_notification_req),
	.max_tx_len = परिमाण(संरचना rpmi_enable_notification_req),
	.min_rx_len = परिमाण(संरचना rpmi_enable_notification_resp),
	.max_rx_len = परिमाण(संरचना rpmi_enable_notification_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_अङ्कः_CLOCKS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = परिमाण(संरचना rpmi_clock_get_अङ्कः_clocks_resp),
	.max_rx_len = परिमाण(संरचना rpmi_clock_get_अङ्कः_clocks_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_गुणाः,
	.min_tx_len = परिमाण(संरचना rpmi_clock_get_गुणाः_req),
	.max_tx_len = परिमाण(संरचना rpmi_clock_get_गुणाः_req),
	.min_rx_len = परिमाण(संरचना rpmi_clock_get_गुणाः_resp),
	.max_rx_len = परिमाण(संरचना rpmi_clock_get_गुणाः_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_SUPPORTED_RATES,
	.min_tx_len = परिमाण(संरचना rpmi_clock_get_supported_rates_req),
	.max_tx_len = परिमाण(संरचना rpmi_clock_get_supported_rates_req),
	.min_rx_len = परिमाण(संरचना rpmi_clock_get_supported_rates_resp),
	.max_rx_len = -1U,
},
{
	.id = RPMI_CLOCK_SRV_समुच्चयः_CONFIG,
	.min_tx_len = परिमाण(संरचना rpmi_clock_समुच्चयः_config_req),
	.max_tx_len = परिमाण(संरचना rpmi_clock_समुच्चयः_config_req),
	.min_rx_len = परिमाण(संरचना rpmi_clock_समुच्चयः_config_resp),
	.max_rx_len = परिमाण(संरचना rpmi_clock_समुच्चयः_config_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_CONFIG,
	.min_tx_len = परिमाण(संरचना rpmi_clock_get_config_req),
	.max_tx_len = परिमाण(संरचना rpmi_clock_get_config_req),
	.min_rx_len = परिमाण(संरचना rpmi_clock_get_config_resp),
	.max_rx_len = परिमाण(संरचना rpmi_clock_get_config_resp),
},
{
	.id = RPMI_CLOCK_SRV_समुच्चयः_दरः,
	.min_tx_len = परिमाण(संरचना rpmi_clock_समुच्चयः_दरः_req),
	.max_tx_len = परिमाण(संरचना rpmi_clock_समुच्चयः_दरः_req),
	.min_rx_len = परिमाण(संरचना rpmi_clock_समुच्चयः_दरः_resp),
	.max_rx_len = परिमाण(संरचना rpmi_clock_समुच्चयः_दरः_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_दरः,
	.min_tx_len = परिमाण(संरचना rpmi_clock_get_दरः_req),
	.max_tx_len = परिमाण(संरचना rpmi_clock_get_दरः_req),
	.min_rx_len = परिमाण(संरचना rpmi_clock_get_दरः_resp),
	.max_rx_len = परिमाण(संरचना rpmi_clock_get_दरः_resp),
},
};

स्थिर ध्रुव संरचना mpxy_rpmi_mbox_दत्त clock_दत्त = {
	.servicegrp_id = RPMI_SRVGRP_CLOCK,
	.अङ्कः_services = RPMI_CLOCK_SRV_MAX_गणनम्,
	.service_दत्त = clock_services,
};

स्थिर ध्रुव संरचना fdt_मेलनम् clock_मेलनम्[] = {
	{ .compatible = "riscv,rpmi-mpxy-clock", .दत्त = &clock_दत्त },
	{ },
};

ध्रुव संरचना fdt_driver fdt_mpxy_rpmi_clock = {
	.मेलनम्_सारणी = clock_मेलनम्,
	.init = mpxy_rpmi_mbox_init,
};
