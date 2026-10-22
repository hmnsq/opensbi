/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Qualcomm, Inc
 *
 * Authors:
 *   Subrahmanya Lingappa <subrahmanya.lingappa@oss.qualcomm.com>
 */

#समावेश <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

अटल काष्ठा mpxy_rpmi_service_तथ्य logging_services[] = {
	[0] = {
		.id = RPMI_LOGGING_SRV_ENABLE_NOTIFICATION,
		.min_tx_len = माप(काष्ठा rpmi_enable_notification_req),
		.max_tx_len = माप(काष्ठा rpmi_enable_notification_req),
		.min_rx_len = माप(काष्ठा rpmi_enable_notification_resp),
		.max_rx_len = माप(काष्ठा rpmi_enable_notification_resp),
	},
	[1] = {
		.id = RPMI_LOGGING_SRV_LOG_तथ्य,
		.min_tx_len = माप(u32) * 2,
		.max_tx_len = माप(काष्ठा rpmi_logging_log_तथ्य_req),
		.min_rx_len = माप(काष्ठा rpmi_logging_log_तथ्य_resp),
		.max_rx_len = माप(काष्ठा rpmi_logging_log_तथ्य_resp),
	},
	/*
	 * Keep a local terminator for safe lookup because only service ID 0x02
	 * is intentionally exposed by this MPXY service group handler.
	 */
	[2] = {
		.id = RPMI_LOGGING_SRV_MAX_गिनती,
	},
};

अटल स्थिर काष्ठा mpxy_rpmi_mbox_तथ्य logging_तथ्य = {
	.servicegrp_id = RPMI_SRVGRP_LOGGING,
	.अंक_services = RPMI_LOGGING_SRV_MAX_गिनती,
	.service_तथ्य = logging_services,
};

अटल स्थिर काष्ठा fdt_match logging_match[] = {
	{ .compatible = "riscv,rpmi-mpxy-logging", .तथ्य = &logging_तथ्य },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_mpxy_rpmi_logging = {
	.match_table = logging_match,
	.init = mpxy_rpmi_mbox_init,
	.experimental = true,
};
