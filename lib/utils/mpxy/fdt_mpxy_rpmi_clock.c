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

ಸ್ಥಿರ ರಚನೆ mpxy_rpmi_service_ದತ್ತಾಂಶ clock_services[] = {
{
	.id = RPMI_CLOCK_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_enable_notification_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_enable_notification_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_enable_notification_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_enable_notification_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_ಅಂಕೆ_CLOCKS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_clock_get_ಅಂಕೆ_clocks_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_clock_get_ಅಂಕೆ_clocks_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_ಗುಣಲಕ್ಷಣಗಳು,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_clock_get_ಗುಣಲಕ್ಷಣಗಳು_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_clock_get_ಗುಣಲಕ್ಷಣಗಳು_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_clock_get_ಗುಣಲಕ್ಷಣಗಳು_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_clock_get_ಗುಣಲಕ್ಷಣಗಳು_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_SUPPORTED_RATES,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_clock_get_supported_rates_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_clock_get_supported_rates_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_clock_get_supported_rates_resp),
	.max_rx_len = -1U,
},
{
	.id = RPMI_CLOCK_SRV_ಗಣ_CONFIG,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_clock_ಗಣ_config_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_clock_ಗಣ_config_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_clock_ಗಣ_config_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_clock_ಗಣ_config_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_CONFIG,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_clock_get_config_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_clock_get_config_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_clock_get_config_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_clock_get_config_resp),
},
{
	.id = RPMI_CLOCK_SRV_ಗಣ_ದರ,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_clock_ಗಣ_ದರ_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_clock_ಗಣ_ದರ_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_clock_ಗಣ_ದರ_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_clock_ಗಣ_ದರ_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_ದರ,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_clock_get_ದರ_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_clock_get_ದರ_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_clock_get_ದರ_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_clock_get_ದರ_resp),
},
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ mpxy_rpmi_mbox_ದತ್ತಾಂಶ clock_ದತ್ತಾಂಶ = {
	.servicegrp_id = RPMI_SRVGRP_CLOCK,
	.ಅಂಕೆ_services = RPMI_CLOCK_SRV_MAX_ಎಣಿಕೆ,
	.service_ದತ್ತಾಂಶ = clock_services,
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ clock_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "riscv,rpmi-mpxy-clock", .ದತ್ತಾಂಶ = &clock_ದತ್ತಾಂಶ },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_mpxy_rpmi_clock = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = clock_ಹೊಂದಿಕೆ,
	.init = mpxy_rpmi_mbox_init,
};
