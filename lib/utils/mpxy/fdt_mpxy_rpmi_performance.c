#شامل <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ایستا ساختار mpxy_rpmi_service_data performance_services[] = {
{
	.id = RPMI_PERF_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = اندازه(ساختار rpmi_enable_notification_req),
	.max_tx_len = اندازه(ساختار rpmi_enable_notification_req),
	.min_rx_len = اندازه(ساختار rpmi_enable_notification_resp),
	.max_rx_len = اندازه(ساختار rpmi_enable_notification_resp),
},
{
	.id = RPMI_PERF_SRV_GET_رقم_DOMAINS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = اندازه(ساختار rpmi_perf_get_رقم_domain_resp),
	.max_rx_len = اندازه(ساختار rpmi_perf_get_رقم_domain_resp),
},
{
	.id = RPMI_PERF_SRV_GET_ATTRIBUTES,
	.min_tx_len = اندازه(ساختار rpmi_perf_get_attrs_req),
	.max_tx_len = اندازه(ساختار rpmi_perf_get_attrs_req),
	.min_rx_len = اندازه(ساختار rpmi_perf_get_attrs_resp),
	.max_rx_len = اندازه(ساختار rpmi_perf_get_attrs_resp),
},
{
	.id = RPMI_PERF_SRV_GET_SUPPORTED_LEVELS,
	.min_tx_len = اندازه(ساختار rpmi_perf_get_supported_level_req),
	.max_tx_len = اندازه(ساختار rpmi_perf_get_supported_level_req),
	.min_rx_len = اندازه(ساختار rpmi_perf_get_supported_level_resp),
	.max_rx_len = -1U,
},
{
	.id = RPMI_PERF_SRV_GET_LEVEL,
	.min_tx_len = اندازه(ساختار rpmi_perf_get_level_req),
	.max_tx_len = اندازه(ساختار rpmi_perf_get_level_req),
	.min_rx_len = اندازه(ساختار rpmi_perf_get_level_resp),
	.max_rx_len = اندازه(ساختار rpmi_perf_get_level_resp),
},
{
	.id = RPMI_PERF_SRV_SET_LEVEL,
	.min_tx_len = اندازه(ساختار rpmi_perf_set_level_req),
	.max_tx_len = اندازه(ساختار rpmi_perf_set_level_req),
	.min_rx_len = اندازه(ساختار rpmi_perf_set_level_resp),
	.max_rx_len = اندازه(ساختار rpmi_perf_set_level_resp),
},
{
	.id = RPMI_PERF_SRV_GET_LIMIT,
	.min_tx_len = اندازه(ساختار rpmi_perf_get_limit_req),
	.max_tx_len = اندازه(ساختار rpmi_perf_get_limit_req),
	.min_rx_len = اندازه(ساختار rpmi_perf_get_limit_resp),
	.max_rx_len = اندازه(ساختار rpmi_perf_get_limit_resp),
},
{
	.id = RPMI_PERF_SRV_SET_LIMIT,
	.min_tx_len = اندازه(ساختار rpmi_perf_set_limit_req),
	.max_tx_len = اندازه(ساختار rpmi_perf_set_limit_req),
	.min_rx_len = اندازه(ساختار rpmi_perf_set_limit_resp),
	.max_rx_len = اندازه(ساختار rpmi_perf_set_limit_resp),
},
{
	.id = RPMI_PERF_SRV_GET_FAST_CHANNEL_REGION,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = اندازه(ساختار rpmi_perf_get_fast_chn_region_resp),
	.max_rx_len = اندازه(ساختار rpmi_perf_get_fast_chn_region_resp),
},
{
	.id = RPMI_PERF_SRV_GET_FAST_CHANNEL_ATTRIBUTES,
	.min_tx_len = اندازه(ساختار rpmi_perf_get_fast_chn_attr_req),
	.max_tx_len = اندازه(ساختار rpmi_perf_get_fast_chn_attr_req),
	.min_rx_len = اندازه(ساختار rpmi_perf_get_fast_chn_attr_resp),
	.max_rx_len = اندازه(ساختار rpmi_perf_get_fast_chn_attr_resp),
},
};

ایستا ثابت ساختار mpxy_rpmi_mbox_data performance_data = {
	.servicegrp_id = RPMI_SRVGRP_PERFORMANCE ,
	.رقم_services = RPMI_PERF_SRV_MAX_شمار,
	.service_data = performance_services,
};

ایستا ثابت ساختار fdt_match performance_match[] = {
	{ .compatible = "riscv,rpmi-mpxy-performance", .data = &performance_data },
	{ },
};

ثابت ساختار fdt_driver fdt_mpxy_rpmi_performance = {
	.experimental = true,
	.match_table = performance_match,
	.init = mpxy_rpmi_mbox_init,
};
