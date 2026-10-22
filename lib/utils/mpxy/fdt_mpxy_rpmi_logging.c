/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Qualcomm, Inc
 *
 * Authors:
 *   Subrahmanya Lingappa <subrahmanya.lingappa@oss.qualcomm.com>
 */

#شامل <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ساکن ساخت mpxy_rpmi_service_ڈیٹا logging_services[] = {
	[0] = {
		.id = RPMI_LOGGING_SRV_ENABLE_NOTIFICATION,
		.min_tx_len = حجم(ساخت rpmi_enable_notification_req),
		.max_tx_len = حجم(ساخت rpmi_enable_notification_req),
		.min_rx_len = حجم(ساخت rpmi_enable_notification_resp),
		.max_rx_len = حجم(ساخت rpmi_enable_notification_resp),
	},
	[1] = {
		.id = RPMI_LOGGING_SRV_LOG_ڈیٹا,
		.min_tx_len = حجم(u32) * 2,
		.max_tx_len = حجم(ساخت rpmi_logging_log_ڈیٹا_req),
		.min_rx_len = حجم(ساخت rpmi_logging_log_ڈیٹا_resp),
		.max_rx_len = حجم(ساخت rpmi_logging_log_ڈیٹا_resp),
	},
	/*
	 * Keep a local terminator for safe lookup because only service ID 0x02
	 * is intentionally exposed by this MPXY service group handler.
	 */
	[2] = {
		.id = RPMI_LOGGING_SRV_MAX_گنتی,
	},
};

ساکن ثابت ساخت mpxy_rpmi_mbox_ڈیٹا logging_ڈیٹا = {
	.servicegrp_id = RPMI_SRVGRP_LOGGING,
	.عدد_services = RPMI_LOGGING_SRV_MAX_گنتی,
	.service_ڈیٹا = logging_services,
};

ساکن ثابت ساخت fdt_مطابقت logging_مطابقت[] = {
	{ .compatible = "riscv,rpmi-mpxy-logging", .ڈیٹا = &logging_ڈیٹا },
	{ },
};

ثابت ساخت fdt_driver fdt_mpxy_rpmi_logging = {
	.مطابقت_جدول = logging_مطابقت,
	.init = mpxy_rpmi_mbox_init,
	.experimental = true,
};
