#अन्तर्भाव <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

स्थिर संरचना mpxy_rpmi_service_दत्त performance_services[] = {
{
	.id = RPMI_PERF_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = परिमाण(संरचना rpmi_enable_notification_req),
	.max_tx_len = परिमाण(संरचना rpmi_enable_notification_req),
	.min_rx_len = परिमाण(संरचना rpmi_enable_notification_resp),
	.max_rx_len = परिमाण(संरचना rpmi_enable_notification_resp),
},
{
	.id = RPMI_PERF_SRV_GET_अङ्कः_DOMAINS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = परिमाण(संरचना rpmi_perf_get_अङ्कः_domain_resp),
	.max_rx_len = परिमाण(संरचना rpmi_perf_get_अङ्कः_domain_resp),
},
{
	.id = RPMI_PERF_SRV_GET_गुणाः,
	.min_tx_len = परिमाण(संरचना rpmi_perf_get_attrs_req),
	.max_tx_len = परिमाण(संरचना rpmi_perf_get_attrs_req),
	.min_rx_len = परिमाण(संरचना rpmi_perf_get_attrs_resp),
	.max_rx_len = परिमाण(संरचना rpmi_perf_get_attrs_resp),
},
{
	.id = RPMI_PERF_SRV_GET_SUPPORTED_स्तराः,
	.min_tx_len = परिमाण(संरचना rpmi_perf_get_supported_level_req),
	.max_tx_len = परिमाण(संरचना rpmi_perf_get_supported_level_req),
	.min_rx_len = परिमाण(संरचना rpmi_perf_get_supported_level_resp),
	.max_rx_len = -1U,
},
{
	.id = RPMI_PERF_SRV_GET_LEVEL,
	.min_tx_len = परिमाण(संरचना rpmi_perf_get_level_req),
	.max_tx_len = परिमाण(संरचना rpmi_perf_get_level_req),
	.min_rx_len = परिमाण(संरचना rpmi_perf_get_level_resp),
	.max_rx_len = परिमाण(संरचना rpmi_perf_get_level_resp),
},
{
	.id = RPMI_PERF_SRV_समुच्चयः_LEVEL,
	.min_tx_len = परिमाण(संरचना rpmi_perf_समुच्चयः_level_req),
	.max_tx_len = परिमाण(संरचना rpmi_perf_समुच्चयः_level_req),
	.min_rx_len = परिमाण(संरचना rpmi_perf_समुच्चयः_level_resp),
	.max_rx_len = परिमाण(संरचना rpmi_perf_समुच्चयः_level_resp),
},
{
	.id = RPMI_PERF_SRV_GET_सीमा,
	.min_tx_len = परिमाण(संरचना rpmi_perf_get_सीमा_req),
	.max_tx_len = परिमाण(संरचना rpmi_perf_get_सीमा_req),
	.min_rx_len = परिमाण(संरचना rpmi_perf_get_सीमा_resp),
	.max_rx_len = परिमाण(संरचना rpmi_perf_get_सीमा_resp),
},
{
	.id = RPMI_PERF_SRV_समुच्चयः_सीमा,
	.min_tx_len = परिमाण(संरचना rpmi_perf_समुच्चयः_सीमा_req),
	.max_tx_len = परिमाण(संरचना rpmi_perf_समुच्चयः_सीमा_req),
	.min_rx_len = परिमाण(संरचना rpmi_perf_समुच्चयः_सीमा_resp),
	.max_rx_len = परिमाण(संरचना rpmi_perf_समुच्चयः_सीमा_resp),
},
{
	.id = RPMI_PERF_SRV_GET_FAST_CHANNEL_REGION,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = परिमाण(संरचना rpmi_perf_get_fast_chn_region_resp),
	.max_rx_len = परिमाण(संरचना rpmi_perf_get_fast_chn_region_resp),
},
{
	.id = RPMI_PERF_SRV_GET_FAST_CHANNEL_गुणाः,
	.min_tx_len = परिमाण(संरचना rpmi_perf_get_fast_chn_attr_req),
	.max_tx_len = परिमाण(संरचना rpmi_perf_get_fast_chn_attr_req),
	.min_rx_len = परिमाण(संरचना rpmi_perf_get_fast_chn_attr_resp),
	.max_rx_len = परिमाण(संरचना rpmi_perf_get_fast_chn_attr_resp),
},
};

स्थिर ध्रुव संरचना mpxy_rpmi_mbox_दत्त performance_दत्त = {
	.servicegrp_id = RPMI_SRVGRP_PERFORMANCE ,
	.अङ्कः_services = RPMI_PERF_SRV_MAX_गणनम्,
	.service_दत्त = performance_services,
};

स्थिर ध्रुव संरचना fdt_मेलनम् performance_मेलनम्[] = {
	{ .compatible = "riscv,rpmi-mpxy-performance", .दत्त = &performance_दत्त },
	{ },
};

ध्रुव संरचना fdt_driver fdt_mpxy_rpmi_performance = {
	.experimental = true,
	.मेलनम्_सारणी = performance_मेलनम्,
	.init = mpxy_rpmi_mbox_init,
};
