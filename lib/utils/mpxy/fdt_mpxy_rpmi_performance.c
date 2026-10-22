#అన్తర్భావ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

స్థిర నిర్మాణం mpxy_rpmi_service_దత్తాంశం performance_services[] = {
{
	.id = RPMI_PERF_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_enable_notification_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_enable_notification_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_enable_notification_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_enable_notification_resp),
},
{
	.id = RPMI_PERF_SRV_GET_అంకె_DOMAINS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_perf_get_అంకె_domain_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_perf_get_అంకె_domain_resp),
},
{
	.id = RPMI_PERF_SRV_GET_లక్షణాలు,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_perf_get_attrs_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_perf_get_attrs_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_perf_get_attrs_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_perf_get_attrs_resp),
},
{
	.id = RPMI_PERF_SRV_GET_SUPPORTED_స్థాయిలు,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_perf_get_supported_level_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_perf_get_supported_level_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_perf_get_supported_level_resp),
	.max_rx_len = -1U,
},
{
	.id = RPMI_PERF_SRV_GET_LEVEL,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_perf_get_level_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_perf_get_level_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_perf_get_level_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_perf_get_level_resp),
},
{
	.id = RPMI_PERF_SRV_సమితి_LEVEL,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_perf_సమితి_level_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_perf_సమితి_level_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_perf_సమితి_level_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_perf_సమితి_level_resp),
},
{
	.id = RPMI_PERF_SRV_GET_హద్దు,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_perf_get_హద్దు_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_perf_get_హద్దు_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_perf_get_హద్దు_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_perf_get_హద్దు_resp),
},
{
	.id = RPMI_PERF_SRV_సమితి_హద్దు,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_perf_సమితి_హద్దు_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_perf_సమితి_హద్దు_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_perf_సమితి_హద్దు_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_perf_సమితి_హద్దు_resp),
},
{
	.id = RPMI_PERF_SRV_GET_FAST_CHANNEL_REGION,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_perf_get_fast_chn_region_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_perf_get_fast_chn_region_resp),
},
{
	.id = RPMI_PERF_SRV_GET_FAST_CHANNEL_లక్షణాలు,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_perf_get_fast_chn_attr_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_perf_get_fast_chn_attr_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_perf_get_fast_chn_attr_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_perf_get_fast_chn_attr_resp),
},
};

స్థిర స్థిరరాశి నిర్మాణం mpxy_rpmi_mbox_దత్తాంశం performance_దత్తాంశం = {
	.servicegrp_id = RPMI_SRVGRP_PERFORMANCE ,
	.అంకె_services = RPMI_PERF_SRV_MAX_లెక్క,
	.service_దత్తాంశం = performance_services,
};

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక performance_సరిపోలిక[] = {
	{ .compatible = "riscv,rpmi-mpxy-performance", .దత్తాంశం = &performance_దత్తాంశం },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_mpxy_rpmi_performance = {
	.experimental = true,
	.సరిపోలిక_పట్టిక = performance_సరిపోలిక,
	.init = mpxy_rpmi_mbox_init,
};
