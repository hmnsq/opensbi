/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Qualcomm, Inc
 *
 * Authors:
 *   Subrahmanya Lingappa <subrahmanya.lingappa@oss.qualcomm.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ਸਥਿਰ ਬਣਤਰ mpxy_rpmi_service_ਅੰਕੜੇ logging_services[] = {
	[0] = {
		.id = RPMI_LOGGING_SRV_ENABLE_NOTIFICATION,
		.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_enable_notification_req),
		.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_enable_notification_req),
		.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_enable_notification_resp),
		.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_enable_notification_resp),
	},
	[1] = {
		.id = RPMI_LOGGING_SRV_LOG_ਅੰਕੜੇ,
		.min_tx_len = ਆਕਾਰ(u32) * 2,
		.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_logging_log_ਅੰਕੜੇ_req),
		.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_logging_log_ਅੰਕੜੇ_resp),
		.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_logging_log_ਅੰਕੜੇ_resp),
	},
	/*
	 * Keep a local terminator for safe lookup because only service ID 0x02
	 * is intentionally exposed by this MPXY service group handler.
	 */
	[2] = {
		.id = RPMI_LOGGING_SRV_MAX_ਗਿਣਤੀ,
	},
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ mpxy_rpmi_mbox_ਅੰਕੜੇ logging_ਅੰਕੜੇ = {
	.servicegrp_id = RPMI_SRVGRP_LOGGING,
	.ਅੰਕ_services = RPMI_LOGGING_SRV_MAX_ਗਿਣਤੀ,
	.service_ਅੰਕੜੇ = logging_services,
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ logging_ਮੇਲ[] = {
	{ .compatible = "riscv,rpmi-mpxy-logging", .ਅੰਕੜੇ = &logging_ਅੰਕੜੇ },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_mpxy_rpmi_logging = {
	.ਮੇਲ_ਸਾਰਣੀ = logging_ਮੇਲ,
	.init = mpxy_rpmi_mbox_init,
	.experimental = true,
};
