#ضمن <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ساكن بنية mpxy_rpmi_service_انکڑے performance_services[] = {
{
	.id = RPMI_PERF_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = حجم(بنية rpmi_enable_notification_req),
	.max_tx_len = حجم(بنية rpmi_enable_notification_req),
	.min_rx_len = حجم(بنية rpmi_enable_notification_resp),
	.max_rx_len = حجم(بنية rpmi_enable_notification_resp),
},
{
	.id = RPMI_PERF_SRV_GET_عدد_DOMAINS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = حجم(بنية rpmi_perf_get_عدد_domain_resp),
	.max_rx_len = حجم(بنية rpmi_perf_get_عدد_domain_resp),
},
{
	.id = RPMI_PERF_SRV_GET_گن,
	.min_tx_len = حجم(بنية rpmi_perf_get_attrs_req),
	.max_tx_len = حجم(بنية rpmi_perf_get_attrs_req),
	.min_rx_len = حجم(بنية rpmi_perf_get_attrs_resp),
	.max_rx_len = حجم(بنية rpmi_perf_get_attrs_resp),
},
{
	.id = RPMI_PERF_SRV_GET_SUPPORTED_پدھراں,
	.min_tx_len = حجم(بنية rpmi_perf_get_supported_level_req),
	.max_tx_len = حجم(بنية rpmi_perf_get_supported_level_req),
	.min_rx_len = حجم(بنية rpmi_perf_get_supported_level_resp),
	.max_rx_len = -1U,
},
{
	.id = RPMI_PERF_SRV_GET_LEVEL,
	.min_tx_len = حجم(بنية rpmi_perf_get_level_req),
	.max_tx_len = حجم(بنية rpmi_perf_get_level_req),
	.min_rx_len = حجم(بنية rpmi_perf_get_level_resp),
	.max_rx_len = حجم(بنية rpmi_perf_get_level_resp),
},
{
	.id = RPMI_PERF_SRV_سیٹ_LEVEL,
	.min_tx_len = حجم(بنية rpmi_perf_سیٹ_level_req),
	.max_tx_len = حجم(بنية rpmi_perf_سیٹ_level_req),
	.min_rx_len = حجم(بنية rpmi_perf_سیٹ_level_resp),
	.max_rx_len = حجم(بنية rpmi_perf_سیٹ_level_resp),
},
{
	.id = RPMI_PERF_SRV_GET_حد_بندی,
	.min_tx_len = حجم(بنية rpmi_perf_get_حد_بندی_req),
	.max_tx_len = حجم(بنية rpmi_perf_get_حد_بندی_req),
	.min_rx_len = حجم(بنية rpmi_perf_get_حد_بندی_resp),
	.max_rx_len = حجم(بنية rpmi_perf_get_حد_بندی_resp),
},
{
	.id = RPMI_PERF_SRV_سیٹ_حد_بندی,
	.min_tx_len = حجم(بنية rpmi_perf_سیٹ_حد_بندی_req),
	.max_tx_len = حجم(بنية rpmi_perf_سیٹ_حد_بندی_req),
	.min_rx_len = حجم(بنية rpmi_perf_سیٹ_حد_بندی_resp),
	.max_rx_len = حجم(بنية rpmi_perf_سیٹ_حد_بندی_resp),
},
{
	.id = RPMI_PERF_SRV_GET_FAST_CHANNEL_REGION,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = حجم(بنية rpmi_perf_get_fast_chn_region_resp),
	.max_rx_len = حجم(بنية rpmi_perf_get_fast_chn_region_resp),
},
{
	.id = RPMI_PERF_SRV_GET_FAST_CHANNEL_گن,
	.min_tx_len = حجم(بنية rpmi_perf_get_fast_chn_attr_req),
	.max_tx_len = حجم(بنية rpmi_perf_get_fast_chn_attr_req),
	.min_rx_len = حجم(بنية rpmi_perf_get_fast_chn_attr_resp),
	.max_rx_len = حجم(بنية rpmi_perf_get_fast_chn_attr_resp),
},
};

ساكن ثابت بنية mpxy_rpmi_mbox_انکڑے performance_انکڑے = {
	.servicegrp_id = RPMI_SRVGRP_PERFORMANCE ,
	.عدد_services = RPMI_PERF_SRV_MAX_گنتی,
	.service_انکڑے = performance_services,
};

ساكن ثابت بنية fdt_میل performance_میل[] = {
	{ .compatible = "riscv,rpmi-mpxy-performance", .انکڑے = &performance_انکڑے },
	{ },
};

ثابت بنية fdt_driver fdt_mpxy_rpmi_performance = {
	.experimental = true,
	.میل_جدول = performance_میل,
	.init = mpxy_rpmi_mbox_init,
};
