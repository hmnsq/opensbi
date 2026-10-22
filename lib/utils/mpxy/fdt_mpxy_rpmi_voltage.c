#అన్తర్భావ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

స్థిర నిర్మాణం mpxy_rpmi_service_దత్తాంశం voltage_services[] = {
{
	.id = RPMI_VOLTAGE_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_enable_notification_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_enable_notification_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_enable_notification_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_enable_notification_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_అంకె_DOMAINS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_voltage_get_అంకె_domains_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_voltage_get_అంకె_domains_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_లక్షణాలు,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_voltage_get_లక్షణాలు_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_voltage_get_లక్షణాలు_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_voltage_get_లక్షణాలు_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_voltage_get_లక్షణాలు_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_SUPPORTED_స్థాయిలు,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_voltage_get_supported_రేటు_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_voltage_get_supported_రేటు_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_voltage_get_supported_రేటు_resp),
	.max_rx_len = -1U,
},
{
	.id = RPMI_VOLTAGE_SRV_సమితి_CONFIG,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_voltage_సమితి_config_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_voltage_సమితి_config_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_voltage_సమితి_config_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_voltage_సమితి_config_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_CONFIG,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_voltage_get_config_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_voltage_get_config_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_voltage_get_config_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_voltage_get_config_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_సమితి_LEVEL,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_voltage_సమితి_level_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_voltage_సమితి_level_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_voltage_సమితి_level_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_voltage_సమితి_level_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_LEVEL,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_voltage_get_level_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_voltage_get_level_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_voltage_get_level_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_voltage_get_level_resp),
},
};

స్థిర స్థిరరాశి నిర్మాణం mpxy_rpmi_mbox_దత్తాంశం voltage_దత్తాంశం = {
	.servicegrp_id = RPMI_SRVGRP_VOLTAGE,
	.అంకె_services = RPMI_VOLTAGE_SRV_MAX_లెక్క,
	.service_దత్తాంశం = voltage_services,
};

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక voltage_సరిపోలిక[] = {
	{ .compatible = "riscv,rpmi-mpxy-voltage", .దత్తాంశం = &voltage_దత్తాంశం },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_mpxy_rpmi_voltage = {
	.experimental = true,
	.సరిపోలిక_పట్టిక = voltage_సరిపోలిక,
	.init = mpxy_rpmi_mbox_init,
};
