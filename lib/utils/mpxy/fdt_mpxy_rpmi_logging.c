/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Qualcomm, Inc
 *
 * Authors:
 *   Subrahmanya Lingappa <subrahmanya.lingappa@oss.qualcomm.com>
 */

#అన్తర్భావ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

స్థిర నిర్మాణం mpxy_rpmi_service_దత్తాంశం logging_services[] = {
	[0] = {
		.id = RPMI_LOGGING_SRV_ENABLE_NOTIFICATION,
		.min_tx_len = పరిమాణం(నిర్మాణం rpmi_enable_notification_req),
		.max_tx_len = పరిమాణం(నిర్మాణం rpmi_enable_notification_req),
		.min_rx_len = పరిమాణం(నిర్మాణం rpmi_enable_notification_resp),
		.max_rx_len = పరిమాణం(నిర్మాణం rpmi_enable_notification_resp),
	},
	[1] = {
		.id = RPMI_LOGGING_SRV_LOG_దత్తాంశం,
		.min_tx_len = పరిమాణం(u32) * 2,
		.max_tx_len = పరిమాణం(నిర్మాణం rpmi_logging_log_దత్తాంశం_req),
		.min_rx_len = పరిమాణం(నిర్మాణం rpmi_logging_log_దత్తాంశం_resp),
		.max_rx_len = పరిమాణం(నిర్మాణం rpmi_logging_log_దత్తాంశం_resp),
	},
	/*
	 * Keep a local terminator for safe lookup because only service ID 0x02
	 * is intentionally exposed by this MPXY service group handler.
	 */
	[2] = {
		.id = RPMI_LOGGING_SRV_MAX_లెక్క,
	},
};

స్థిర స్థిరరాశి నిర్మాణం mpxy_rpmi_mbox_దత్తాంశం logging_దత్తాంశం = {
	.servicegrp_id = RPMI_SRVGRP_LOGGING,
	.అంకె_services = RPMI_LOGGING_SRV_MAX_లెక్క,
	.service_దత్తాంశం = logging_services,
};

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక logging_సరిపోలిక[] = {
	{ .compatible = "riscv,rpmi-mpxy-logging", .దత్తాంశం = &logging_దత్తాంశం },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_mpxy_rpmi_logging = {
	.సరిపోలిక_పట్టిక = logging_సరిపోలిక,
	.init = mpxy_rpmi_mbox_init,
	.experimental = true,
};
