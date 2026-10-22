/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Qualcomm, Inc
 *
 * Authors:
 *   Subrahmanya Lingappa <subrahmanya.lingappa@oss.qualcomm.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ಸ್ಥಿರ ರಚನೆ mpxy_rpmi_service_ದತ್ತಾಂಶ logging_services[] = {
	[0] = {
		.id = RPMI_LOGGING_SRV_ENABLE_NOTIFICATION,
		.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_enable_notification_req),
		.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_enable_notification_req),
		.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_enable_notification_resp),
		.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_enable_notification_resp),
	},
	[1] = {
		.id = RPMI_LOGGING_SRV_LOG_ದತ್ತಾಂಶ,
		.min_tx_len = ಗಾತ್ರ(u32) * 2,
		.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_logging_log_ದತ್ತಾಂಶ_req),
		.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_logging_log_ದತ್ತಾಂಶ_resp),
		.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_logging_log_ದತ್ತಾಂಶ_resp),
	},
	/*
	 * Keep a local terminator for safe lookup because only service ID 0x02
	 * is intentionally exposed by this MPXY service group handler.
	 */
	[2] = {
		.id = RPMI_LOGGING_SRV_MAX_ಎಣಿಕೆ,
	},
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ mpxy_rpmi_mbox_ದತ್ತಾಂಶ logging_ದತ್ತಾಂಶ = {
	.servicegrp_id = RPMI_SRVGRP_LOGGING,
	.ಅಂಕೆ_services = RPMI_LOGGING_SRV_MAX_ಎಣಿಕೆ,
	.service_ದತ್ತಾಂಶ = logging_services,
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ logging_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "riscv,rpmi-mpxy-logging", .ದತ್ತಾಂಶ = &logging_ದತ್ತಾಂಶ },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_mpxy_rpmi_logging = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = logging_ಹೊಂದಿಕೆ,
	.init = mpxy_rpmi_mbox_init,
	.experimental = true,
};
