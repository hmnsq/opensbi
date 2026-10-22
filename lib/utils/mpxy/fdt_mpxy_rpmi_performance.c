#شامل <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ساکن ساخت mpxy_rpmi_service_ڈیٹا performance_services[] = {
{
	.id = RPMI_PERF_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = حجم(ساخت rpmi_enable_notification_req),
	.max_tx_len = حجم(ساخت rpmi_enable_notification_req),
	.min_rx_len = حجم(ساخت rpmi_enable_notification_resp),
	.max_rx_len = حجم(ساخت rpmi_enable_notification_resp),
},
{
	.id = RPMI_PERF_SRV_GET_عدد_DOMAINS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = حجم(ساخت rpmi_perf_get_عدد_domain_resp),
	.max_rx_len = حجم(ساخت rpmi_perf_get_عدد_domain_resp),
},
{
	.id = RPMI_PERF_SRV_GET_اوصاف,
	.min_tx_len = حجم(ساخت rpmi_perf_get_attrs_req),
	.max_tx_len = حجم(ساخت rpmi_perf_get_attrs_req),
	.min_rx_len = حجم(ساخت rpmi_perf_get_attrs_resp),
	.max_rx_len = حجم(ساخت rpmi_perf_get_attrs_resp),
},
{
	.id = RPMI_PERF_SRV_GET_SUPPORTED_سطحیں,
	.min_tx_len = حجم(ساخت rpmi_perf_get_supported_level_req),
	.max_tx_len = حجم(ساخت rpmi_perf_get_supported_level_req),
	.min_rx_len = حجم(ساخت rpmi_perf_get_supported_level_resp),
	.max_rx_len = -1U,
},
{
	.id = RPMI_PERF_SRV_GET_LEVEL,
	.min_tx_len = حجم(ساخت rpmi_perf_get_level_req),
	.max_tx_len = حجم(ساخت rpmi_perf_get_level_req),
	.min_rx_len = حجم(ساخت rpmi_perf_get_level_resp),
	.max_rx_len = حجم(ساخت rpmi_perf_get_level_resp),
},
{
	.id = RPMI_PERF_SRV_سیٹ_LEVEL,
	.min_tx_len = حجم(ساخت rpmi_perf_سیٹ_level_req),
	.max_tx_len = حجم(ساخت rpmi_perf_سیٹ_level_req),
	.min_rx_len = حجم(ساخت rpmi_perf_سیٹ_level_resp),
	.max_rx_len = حجم(ساخت rpmi_perf_سیٹ_level_resp),
},
{
	.id = RPMI_PERF_SRV_GET_حد_بندی,
	.min_tx_len = حجم(ساخت rpmi_perf_get_حد_بندی_req),
	.max_tx_len = حجم(ساخت rpmi_perf_get_حد_بندی_req),
	.min_rx_len = حجم(ساخت rpmi_perf_get_حد_بندی_resp),
	.max_rx_len = حجم(ساخت rpmi_perf_get_حد_بندی_resp),
},
{
	.id = RPMI_PERF_SRV_سیٹ_حد_بندی,
	.min_tx_len = حجم(ساخت rpmi_perf_سیٹ_حد_بندی_req),
	.max_tx_len = حجم(ساخت rpmi_perf_سیٹ_حد_بندی_req),
	.min_rx_len = حجم(ساخت rpmi_perf_سیٹ_حد_بندی_resp),
	.max_rx_len = حجم(ساخت rpmi_perf_سیٹ_حد_بندی_resp),
},
{
	.id = RPMI_PERF_SRV_GET_FAST_CHANNEL_REGION,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = حجم(ساخت rpmi_perf_get_fast_chn_region_resp),
	.max_rx_len = حجم(ساخت rpmi_perf_get_fast_chn_region_resp),
},
{
	.id = RPMI_PERF_SRV_GET_FAST_CHANNEL_اوصاف,
	.min_tx_len = حجم(ساخت rpmi_perf_get_fast_chn_attr_req),
	.max_tx_len = حجم(ساخت rpmi_perf_get_fast_chn_attr_req),
	.min_rx_len = حجم(ساخت rpmi_perf_get_fast_chn_attr_resp),
	.max_rx_len = حجم(ساخت rpmi_perf_get_fast_chn_attr_resp),
},
};

ساکن ثابت ساخت mpxy_rpmi_mbox_ڈیٹا performance_ڈیٹا = {
	.servicegrp_id = RPMI_SRVGRP_PERFORMANCE ,
	.عدد_services = RPMI_PERF_SRV_MAX_گنتی,
	.service_ڈیٹا = performance_services,
};

ساکن ثابت ساخت fdt_مطابقت performance_مطابقت[] = {
	{ .compatible = "riscv,rpmi-mpxy-performance", .ڈیٹا = &performance_ڈیٹا },
	{ },
};

ثابت ساخت fdt_driver fdt_mpxy_rpmi_performance = {
	.experimental = true,
	.مطابقت_جدول = performance_مطابقت,
	.init = mpxy_rpmi_mbox_init,
};
