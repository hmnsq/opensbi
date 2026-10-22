/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Qualcomm, Inc
 *
 * Authors:
 *   Subrahmanya Lingappa <subrahmanya.lingappa@oss.qualcomm.com>
 */

#ضمن <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ساكن بنية mpxy_rpmi_service_data logging_services[] = {
	[0] = {
		.id = RPMI_LOGGING_SRV_ENABLE_NOTIFICATION,
		.min_tx_len = حجم(بنية rpmi_enable_notification_req),
		.max_tx_len = حجم(بنية rpmi_enable_notification_req),
		.min_rx_len = حجم(بنية rpmi_enable_notification_resp),
		.max_rx_len = حجم(بنية rpmi_enable_notification_resp),
	},
	[1] = {
		.id = RPMI_LOGGING_SRV_LOG_DATA,
		.min_tx_len = حجم(u32) * 2,
		.max_tx_len = حجم(بنية rpmi_logging_log_data_req),
		.min_rx_len = حجم(بنية rpmi_logging_log_data_resp),
		.max_rx_len = حجم(بنية rpmi_logging_log_data_resp),
	},
	/*
	 * Keep a local terminator for safe lookup because only service ID 0x02
	 * is intentionally exposed by this MPXY service group handler.
	 */
	[2] = {
		.id = RPMI_LOGGING_SRV_MAX_كدي,
	},
};

ساكن ثابت بنية mpxy_rpmi_mbox_data logging_data = {
	.servicegrp_id = RPMI_SRVGRP_LOGGING,
	.لمب_services = RPMI_LOGGING_SRV_MAX_كدي,
	.service_data = logging_services,
};

ساكن ثابت بنية fdt_match logging_match[] = {
	{ .compatible = "riscv,rpmi-mpxy-logging", .data = &logging_data },
	{ },
};

ثابت بنية fdt_driver fdt_mpxy_rpmi_logging = {
	.match_table = logging_match,
	.init = mpxy_rpmi_mbox_init,
	.experimental = true,
};
