#అన్తర్భావ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

స్థిర నిర్మాణం mpxy_rpmi_service_దత్తాంశం dpwr_services[] = {
{
	.id = RPMI_DPWR_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_enable_notification_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_enable_notification_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_enable_notification_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_enable_notification_resp),
},
{
	.id = RPMI_DPWR_SRV_GET_అంకె_DOMAINS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_dpwr_get_అంకె_domain_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_dpwr_get_అంకె_domain_resp),
},
{
	.id = RPMI_DPWR_SRV_GET_లక్షణాలు,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_dpwr_get_attrs_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_dpwr_get_attrs_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_dpwr_get_attrs_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_dpwr_get_attrs_resp),
},
{
	.id = RPMI_DPWR_SRV_సమితి_STATE,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_dpwr_సమితి_state_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_dpwr_సమితి_state_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_dpwr_సమితి_state_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_dpwr_సమితి_state_resp),
},
{
	.id = RPMI_DPWR_SRV_GET_STATE,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_dpwr_get_state_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_dpwr_get_state_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_dpwr_get_state_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_dpwr_get_state_resp),
},
};

స్థిర స్థిరరాశి నిర్మాణం mpxy_rpmi_mbox_దత్తాంశం dpwr_దత్తాంశం = {
	.servicegrp_id = RPMI_SRVGRP_DEVICE_POWER,
	.అంకె_services = RPMI_DPWR_SRV_MAX_లెక్క,
	.service_దత్తాంశం = dpwr_services,
};

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక dpwr_సరిపోలిక[] = {
	{ .compatible = "riscv,rpmi-mpxy-device-power", .దత్తాంశం = &dpwr_దత్తాంశం },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_mpxy_rpmi_device_power = {
	.experimental = true,
	.సరిపోలిక_పట్టిక = dpwr_సరిపోలిక,
	.init = mpxy_rpmi_mbox_init,
};
