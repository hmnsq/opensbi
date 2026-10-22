#ضمن <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ساكن بنية mpxy_rpmi_service_داده performance_services[] = {
{
	.id = RPMI_PERF_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = حجم(بنية rpmi_enable_notification_req),
	.max_tx_len = حجم(بنية rpmi_enable_notification_req),
	.min_rx_len = حجم(بنية rpmi_enable_notification_resp),
	.max_rx_len = حجم(بنية rpmi_enable_notification_resp),
},
{
	.id = RPMI_PERF_SRV_GET_رقم_DOMAINS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = حجم(بنية rpmi_perf_get_رقم_domain_resp),
	.max_rx_len = حجم(بنية rpmi_perf_get_رقم_domain_resp),
},
{
	.id = RPMI_PERF_SRV_GET_صفات,
	.min_tx_len = حجم(بنية rpmi_perf_get_attrs_req),
	.max_tx_len = حجم(بنية rpmi_perf_get_attrs_req),
	.min_rx_len = حجم(بنية rpmi_perf_get_attrs_resp),
	.max_rx_len = حجم(بنية rpmi_perf_get_attrs_resp),
},
{
	.id = RPMI_PERF_SRV_GET_SUPPORTED_سطوح,
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
	.id = RPMI_PERF_SRV_مجموعه_LEVEL,
	.min_tx_len = حجم(بنية rpmi_perf_مجموعه_level_req),
	.max_tx_len = حجم(بنية rpmi_perf_مجموعه_level_req),
	.min_rx_len = حجم(بنية rpmi_perf_مجموعه_level_resp),
	.max_rx_len = حجم(بنية rpmi_perf_مجموعه_level_resp),
},
{
	.id = RPMI_PERF_SRV_GET_حد,
	.min_tx_len = حجم(بنية rpmi_perf_get_حد_req),
	.max_tx_len = حجم(بنية rpmi_perf_get_حد_req),
	.min_rx_len = حجم(بنية rpmi_perf_get_حد_resp),
	.max_rx_len = حجم(بنية rpmi_perf_get_حد_resp),
},
{
	.id = RPMI_PERF_SRV_مجموعه_حد,
	.min_tx_len = حجم(بنية rpmi_perf_مجموعه_حد_req),
	.max_tx_len = حجم(بنية rpmi_perf_مجموعه_حد_req),
	.min_rx_len = حجم(بنية rpmi_perf_مجموعه_حد_resp),
	.max_rx_len = حجم(بنية rpmi_perf_مجموعه_حد_resp),
},
{
	.id = RPMI_PERF_SRV_GET_FAST_CHANNEL_REGION,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = حجم(بنية rpmi_perf_get_fast_chn_region_resp),
	.max_rx_len = حجم(بنية rpmi_perf_get_fast_chn_region_resp),
},
{
	.id = RPMI_PERF_SRV_GET_FAST_CHANNEL_صفات,
	.min_tx_len = حجم(بنية rpmi_perf_get_fast_chn_attr_req),
	.max_tx_len = حجم(بنية rpmi_perf_get_fast_chn_attr_req),
	.min_rx_len = حجم(بنية rpmi_perf_get_fast_chn_attr_resp),
	.max_rx_len = حجم(بنية rpmi_perf_get_fast_chn_attr_resp),
},
};

ساكن ثابت بنية mpxy_rpmi_mbox_داده performance_داده = {
	.servicegrp_id = RPMI_SRVGRP_PERFORMANCE ,
	.رقم_services = RPMI_PERF_SRV_MAX_شمار,
	.service_داده = performance_services,
};

ساكن ثابت بنية fdt_تطابق performance_تطابق[] = {
	{ .compatible = "riscv,rpmi-mpxy-performance", .داده = &performance_داده },
	{ },
};

ثابت بنية fdt_driver fdt_mpxy_rpmi_performance = {
	.experimental = true,
	.تطابق_جدول = performance_تطابق,
	.init = mpxy_rpmi_mbox_init,
};
