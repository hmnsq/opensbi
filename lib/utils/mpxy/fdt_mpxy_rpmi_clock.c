/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 *   Anup Patel <apatel@ventanamicro.com>
 */

#అన్తర్భావ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

స్థిర నిర్మాణం mpxy_rpmi_service_దత్తాంశం clock_services[] = {
{
	.id = RPMI_CLOCK_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_enable_notification_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_enable_notification_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_enable_notification_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_enable_notification_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_అంకె_CLOCKS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_clock_get_అంకె_clocks_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_clock_get_అంకె_clocks_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_లక్షణాలు,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_clock_get_లక్షణాలు_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_clock_get_లక్షణాలు_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_clock_get_లక్షణాలు_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_clock_get_లక్షణాలు_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_SUPPORTED_RATES,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_clock_get_supported_rates_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_clock_get_supported_rates_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_clock_get_supported_rates_resp),
	.max_rx_len = -1U,
},
{
	.id = RPMI_CLOCK_SRV_సమితి_CONFIG,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_clock_సమితి_config_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_clock_సమితి_config_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_clock_సమితి_config_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_clock_సమితి_config_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_CONFIG,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_clock_get_config_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_clock_get_config_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_clock_get_config_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_clock_get_config_resp),
},
{
	.id = RPMI_CLOCK_SRV_సమితి_రేటు,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_clock_సమితి_రేటు_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_clock_సమితి_రేటు_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_clock_సమితి_రేటు_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_clock_సమితి_రేటు_resp),
},
{
	.id = RPMI_CLOCK_SRV_GET_రేటు,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_clock_get_రేటు_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_clock_get_రేటు_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_clock_get_రేటు_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_clock_get_రేటు_resp),
},
};

స్థిర స్థిరరాశి నిర్మాణం mpxy_rpmi_mbox_దత్తాంశం clock_దత్తాంశం = {
	.servicegrp_id = RPMI_SRVGRP_CLOCK,
	.అంకె_services = RPMI_CLOCK_SRV_MAX_లెక్క,
	.service_దత్తాంశం = clock_services,
};

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక clock_సరిపోలిక[] = {
	{ .compatible = "riscv,rpmi-mpxy-clock", .దత్తాంశం = &clock_దత్తాంశం },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_mpxy_rpmi_clock = {
	.సరిపోలిక_పట్టిక = clock_సరిపోలిక,
	.init = mpxy_rpmi_mbox_init,
};
