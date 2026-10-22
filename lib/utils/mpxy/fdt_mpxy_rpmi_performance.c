#ضمن <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ساكن بنية mpxy_rpmi_service_معلومات performance_services[] = {
{
	.id = RPMI_PERF_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = حجم(بنية rpmi_enable_notification_req),
	.max_tx_len = حجم(بنية rpmi_enable_notification_req),
	.min_rx_len = حجم(بنية rpmi_enable_notification_resp),
	.max_rx_len = حجم(بنية rpmi_enable_notification_resp),
},
{
	.id = RPMI_PERF_SRV_GET_انگ_DOMAINS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = حجم(بنية rpmi_perf_get_انگ_domain_resp),
	.max_rx_len = حجم(بنية rpmi_perf_get_انگ_domain_resp),
},
{
	.id = RPMI_PERF_SRV_GET_خاصيتون,
	.min_tx_len = حجم(بنية rpmi_perf_get_attrs_req),
	.max_tx_len = حجم(بنية rpmi_perf_get_attrs_req),
	.min_rx_len = حجم(بنية rpmi_perf_get_attrs_resp),
	.max_rx_len = حجم(بنية rpmi_perf_get_attrs_resp),
},
{
	.id = RPMI_PERF_SRV_GET_SUPPORTED_سطحون,
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
	.id = RPMI_PERF_SRV_سيٽ_LEVEL,
	.min_tx_len = حجم(بنية rpmi_perf_سيٽ_level_req),
	.max_tx_len = حجم(بنية rpmi_perf_سيٽ_level_req),
	.min_rx_len = حجم(بنية rpmi_perf_سيٽ_level_resp),
	.max_rx_len = حجم(بنية rpmi_perf_سيٽ_level_resp),
},
{
	.id = RPMI_PERF_SRV_GET_حد_بندي,
	.min_tx_len = حجم(بنية rpmi_perf_get_حد_بندي_req),
	.max_tx_len = حجم(بنية rpmi_perf_get_حد_بندي_req),
	.min_rx_len = حجم(بنية rpmi_perf_get_حد_بندي_resp),
	.max_rx_len = حجم(بنية rpmi_perf_get_حد_بندي_resp),
},
{
	.id = RPMI_PERF_SRV_سيٽ_حد_بندي,
	.min_tx_len = حجم(بنية rpmi_perf_سيٽ_حد_بندي_req),
	.max_tx_len = حجم(بنية rpmi_perf_سيٽ_حد_بندي_req),
	.min_rx_len = حجم(بنية rpmi_perf_سيٽ_حد_بندي_resp),
	.max_rx_len = حجم(بنية rpmi_perf_سيٽ_حد_بندي_resp),
},
{
	.id = RPMI_PERF_SRV_GET_FAST_CHANNEL_REGION,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = حجم(بنية rpmi_perf_get_fast_chn_region_resp),
	.max_rx_len = حجم(بنية rpmi_perf_get_fast_chn_region_resp),
},
{
	.id = RPMI_PERF_SRV_GET_FAST_CHANNEL_خاصيتون,
	.min_tx_len = حجم(بنية rpmi_perf_get_fast_chn_attr_req),
	.max_tx_len = حجم(بنية rpmi_perf_get_fast_chn_attr_req),
	.min_rx_len = حجم(بنية rpmi_perf_get_fast_chn_attr_resp),
	.max_rx_len = حجم(بنية rpmi_perf_get_fast_chn_attr_resp),
},
};

ساكن ثابت بنية mpxy_rpmi_mbox_معلومات performance_معلومات = {
	.servicegrp_id = RPMI_SRVGRP_PERFORMANCE ,
	.انگ_services = RPMI_PERF_SRV_MAX_ڳڻپ,
	.service_معلومات = performance_services,
};

ساكن ثابت بنية fdt_ميلاپ performance_ميلاپ[] = {
	{ .compatible = "riscv,rpmi-mpxy-performance", .معلومات = &performance_معلومات },
	{ },
};

ثابت بنية fdt_driver fdt_mpxy_rpmi_performance = {
	.experimental = true,
	.ميلاپ_جدول = performance_ميلاپ,
	.init = mpxy_rpmi_mbox_init,
};
