/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Qualcomm, Inc
 *
 * Authors:
 *   Subrahmanya Lingappa <subrahmanya.lingappa@oss.qualcomm.com>
 */

#ضمن <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ساكن بنية mpxy_rpmi_service_داده logging_services[] = {
	[0] = {
		.id = RPMI_LOGGING_SRV_ENABLE_NOTIFICATION,
		.min_tx_len = حجم(بنية rpmi_enable_notification_req),
		.max_tx_len = حجم(بنية rpmi_enable_notification_req),
		.min_rx_len = حجم(بنية rpmi_enable_notification_resp),
		.max_rx_len = حجم(بنية rpmi_enable_notification_resp),
	},
	[1] = {
		.id = RPMI_LOGGING_SRV_LOG_داده,
		.min_tx_len = حجم(u32) * 2,
		.max_tx_len = حجم(بنية rpmi_logging_log_داده_req),
		.min_rx_len = حجم(بنية rpmi_logging_log_داده_resp),
		.max_rx_len = حجم(بنية rpmi_logging_log_داده_resp),
	},
	/*
	 * Keep a local terminator for safe lookup because only service ID 0x02
	 * is intentionally exposed by this MPXY service group handler.
	 */
	[2] = {
		.id = RPMI_LOGGING_SRV_MAX_شمار,
	},
};

ساكن ثابت بنية mpxy_rpmi_mbox_داده logging_داده = {
	.servicegrp_id = RPMI_SRVGRP_LOGGING,
	.رقم_services = RPMI_LOGGING_SRV_MAX_شمار,
	.service_داده = logging_services,
};

ساكن ثابت بنية fdt_تطابق logging_تطابق[] = {
	{ .compatible = "riscv,rpmi-mpxy-logging", .داده = &logging_داده },
	{ },
};

ثابت بنية fdt_driver fdt_mpxy_rpmi_logging = {
	.تطابق_جدول = logging_تطابق,
	.init = mpxy_rpmi_mbox_init,
	.experimental = true,
};
