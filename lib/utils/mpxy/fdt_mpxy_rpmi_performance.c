#அந்தர்பாவ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

நிலையான கட்டமைப்பு mpxy_rpmi_service_data performance_services[] = {
{
	.id = RPMI_PERF_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = அளவு(கட்டமைப்பு rpmi_enable_notification_req),
	.max_tx_len = அளவு(கட்டமைப்பு rpmi_enable_notification_req),
	.min_rx_len = அளவு(கட்டமைப்பு rpmi_enable_notification_resp),
	.max_rx_len = அளவு(கட்டமைப்பு rpmi_enable_notification_resp),
},
{
	.id = RPMI_PERF_SRV_GET_இலக்கம்_DOMAINS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = அளவு(கட்டமைப்பு rpmi_perf_get_இலக்கம்_domain_resp),
	.max_rx_len = அளவு(கட்டமைப்பு rpmi_perf_get_இலக்கம்_domain_resp),
},
{
	.id = RPMI_PERF_SRV_GET_ATTRIBUTES,
	.min_tx_len = அளவு(கட்டமைப்பு rpmi_perf_get_attrs_req),
	.max_tx_len = அளவு(கட்டமைப்பு rpmi_perf_get_attrs_req),
	.min_rx_len = அளவு(கட்டமைப்பு rpmi_perf_get_attrs_resp),
	.max_rx_len = அளவு(கட்டமைப்பு rpmi_perf_get_attrs_resp),
},
{
	.id = RPMI_PERF_SRV_GET_SUPPORTED_LEVELS,
	.min_tx_len = அளவு(கட்டமைப்பு rpmi_perf_get_supported_level_req),
	.max_tx_len = அளவு(கட்டமைப்பு rpmi_perf_get_supported_level_req),
	.min_rx_len = அளவு(கட்டமைப்பு rpmi_perf_get_supported_level_resp),
	.max_rx_len = -1U,
},
{
	.id = RPMI_PERF_SRV_GET_LEVEL,
	.min_tx_len = அளவு(கட்டமைப்பு rpmi_perf_get_level_req),
	.max_tx_len = அளவு(கட்டமைப்பு rpmi_perf_get_level_req),
	.min_rx_len = அளவு(கட்டமைப்பு rpmi_perf_get_level_resp),
	.max_rx_len = அளவு(கட்டமைப்பு rpmi_perf_get_level_resp),
},
{
	.id = RPMI_PERF_SRV_SET_LEVEL,
	.min_tx_len = அளவு(கட்டமைப்பு rpmi_perf_set_level_req),
	.max_tx_len = அளவு(கட்டமைப்பு rpmi_perf_set_level_req),
	.min_rx_len = அளவு(கட்டமைப்பு rpmi_perf_set_level_resp),
	.max_rx_len = அளவு(கட்டமைப்பு rpmi_perf_set_level_resp),
},
{
	.id = RPMI_PERF_SRV_GET_LIMIT,
	.min_tx_len = அளவு(கட்டமைப்பு rpmi_perf_get_limit_req),
	.max_tx_len = அளவு(கட்டமைப்பு rpmi_perf_get_limit_req),
	.min_rx_len = அளவு(கட்டமைப்பு rpmi_perf_get_limit_resp),
	.max_rx_len = அளவு(கட்டமைப்பு rpmi_perf_get_limit_resp),
},
{
	.id = RPMI_PERF_SRV_SET_LIMIT,
	.min_tx_len = அளவு(கட்டமைப்பு rpmi_perf_set_limit_req),
	.max_tx_len = அளவு(கட்டமைப்பு rpmi_perf_set_limit_req),
	.min_rx_len = அளவு(கட்டமைப்பு rpmi_perf_set_limit_resp),
	.max_rx_len = அளவு(கட்டமைப்பு rpmi_perf_set_limit_resp),
},
{
	.id = RPMI_PERF_SRV_GET_FAST_CHANNEL_REGION,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = அளவு(கட்டமைப்பு rpmi_perf_get_fast_chn_region_resp),
	.max_rx_len = அளவு(கட்டமைப்பு rpmi_perf_get_fast_chn_region_resp),
},
{
	.id = RPMI_PERF_SRV_GET_FAST_CHANNEL_ATTRIBUTES,
	.min_tx_len = அளவு(கட்டமைப்பு rpmi_perf_get_fast_chn_attr_req),
	.max_tx_len = அளவு(கட்டமைப்பு rpmi_perf_get_fast_chn_attr_req),
	.min_rx_len = அளவு(கட்டமைப்பு rpmi_perf_get_fast_chn_attr_resp),
	.max_rx_len = அளவு(கட்டமைப்பு rpmi_perf_get_fast_chn_attr_resp),
},
};

நிலையான மாறிலி கட்டமைப்பு mpxy_rpmi_mbox_data performance_data = {
	.servicegrp_id = RPMI_SRVGRP_PERFORMANCE ,
	.இலக்கம்_services = RPMI_PERF_SRV_MAX_மொத்தம்,
	.service_data = performance_services,
};

நிலையான மாறிலி கட்டமைப்பு fdt_match performance_match[] = {
	{ .compatible = "riscv,rpmi-mpxy-performance", .data = &performance_data },
	{ },
};

மாறிலி கட்டமைப்பு fdt_driver fdt_mpxy_rpmi_performance = {
	.experimental = true,
	.match_table = performance_match,
	.init = mpxy_rpmi_mbox_init,
};
