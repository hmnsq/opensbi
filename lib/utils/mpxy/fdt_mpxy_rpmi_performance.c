#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ਸਥਿਰ ਬਣਤਰ mpxy_rpmi_service_ਅੰਕੜੇ performance_services[] = {
{
	.id = RPMI_PERF_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_enable_notification_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_enable_notification_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_enable_notification_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_enable_notification_resp),
},
{
	.id = RPMI_PERF_SRV_GET_ਅੰਕ_DOMAINS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_get_ਅੰਕ_domain_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_get_ਅੰਕ_domain_resp),
},
{
	.id = RPMI_PERF_SRV_GET_ਗੁਣ,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_get_attrs_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_get_attrs_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_get_attrs_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_get_attrs_resp),
},
{
	.id = RPMI_PERF_SRV_GET_SUPPORTED_ਪੱਧਰ,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_get_supported_level_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_get_supported_level_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_get_supported_level_resp),
	.max_rx_len = -1U,
},
{
	.id = RPMI_PERF_SRV_GET_LEVEL,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_get_level_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_get_level_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_get_level_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_get_level_resp),
},
{
	.id = RPMI_PERF_SRV_ਸੈੱਟ_LEVEL,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_ਸੈੱਟ_level_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_ਸੈੱਟ_level_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_ਸੈੱਟ_level_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_ਸੈੱਟ_level_resp),
},
{
	.id = RPMI_PERF_SRV_GET_ਹੱਦਬੰਦੀ,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_get_ਹੱਦਬੰਦੀ_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_get_ਹੱਦਬੰਦੀ_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_get_ਹੱਦਬੰਦੀ_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_get_ਹੱਦਬੰਦੀ_resp),
},
{
	.id = RPMI_PERF_SRV_ਸੈੱਟ_ਹੱਦਬੰਦੀ,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_ਸੈੱਟ_ਹੱਦਬੰਦੀ_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_ਸੈੱਟ_ਹੱਦਬੰਦੀ_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_ਸੈੱਟ_ਹੱਦਬੰਦੀ_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_ਸੈੱਟ_ਹੱਦਬੰਦੀ_resp),
},
{
	.id = RPMI_PERF_SRV_GET_FAST_CHANNEL_REGION,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_get_fast_chn_region_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_get_fast_chn_region_resp),
},
{
	.id = RPMI_PERF_SRV_GET_FAST_CHANNEL_ਗੁਣ,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_get_fast_chn_attr_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_get_fast_chn_attr_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_get_fast_chn_attr_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_perf_get_fast_chn_attr_resp),
},
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ mpxy_rpmi_mbox_ਅੰਕੜੇ performance_ਅੰਕੜੇ = {
	.servicegrp_id = RPMI_SRVGRP_PERFORMANCE ,
	.ਅੰਕ_services = RPMI_PERF_SRV_MAX_ਗਿਣਤੀ,
	.service_ਅੰਕੜੇ = performance_services,
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ performance_ਮੇਲ[] = {
	{ .compatible = "riscv,rpmi-mpxy-performance", .ਅੰਕੜੇ = &performance_ਅੰਕੜੇ },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_mpxy_rpmi_performance = {
	.experimental = true,
	.ਮੇਲ_ਸਾਰਣੀ = performance_ਮੇਲ,
	.init = mpxy_rpmi_mbox_init,
};
