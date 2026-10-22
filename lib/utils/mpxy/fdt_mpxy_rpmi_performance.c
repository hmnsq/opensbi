#ಅನ್ತರ್ಭಾವ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ಸ್ಥಿರ ರಚನೆ mpxy_rpmi_service_ದತ್ತಾಂಶ performance_services[] = {
{
	.id = RPMI_PERF_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_enable_notification_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_enable_notification_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_enable_notification_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_enable_notification_resp),
},
{
	.id = RPMI_PERF_SRV_GET_ಅಂಕೆ_DOMAINS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_get_ಅಂಕೆ_domain_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_get_ಅಂಕೆ_domain_resp),
},
{
	.id = RPMI_PERF_SRV_GET_ಗುಣಲಕ್ಷಣಗಳು,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_get_attrs_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_get_attrs_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_get_attrs_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_get_attrs_resp),
},
{
	.id = RPMI_PERF_SRV_GET_SUPPORTED_ಮಟ್ಟಗಳು,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_get_supported_level_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_get_supported_level_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_get_supported_level_resp),
	.max_rx_len = -1U,
},
{
	.id = RPMI_PERF_SRV_GET_LEVEL,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_get_level_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_get_level_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_get_level_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_get_level_resp),
},
{
	.id = RPMI_PERF_SRV_ಗಣ_LEVEL,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_ಗಣ_level_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_ಗಣ_level_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_ಗಣ_level_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_ಗಣ_level_resp),
},
{
	.id = RPMI_PERF_SRV_GET_ಎಲ್ಲೆ,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_get_ಎಲ್ಲೆ_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_get_ಎಲ್ಲೆ_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_get_ಎಲ್ಲೆ_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_get_ಎಲ್ಲೆ_resp),
},
{
	.id = RPMI_PERF_SRV_ಗಣ_ಎಲ್ಲೆ,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_ಗಣ_ಎಲ್ಲೆ_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_ಗಣ_ಎಲ್ಲೆ_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_ಗಣ_ಎಲ್ಲೆ_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_ಗಣ_ಎಲ್ಲೆ_resp),
},
{
	.id = RPMI_PERF_SRV_GET_FAST_CHANNEL_REGION,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_get_fast_chn_region_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_get_fast_chn_region_resp),
},
{
	.id = RPMI_PERF_SRV_GET_FAST_CHANNEL_ಗುಣಲಕ್ಷಣಗಳು,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_get_fast_chn_attr_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_get_fast_chn_attr_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_get_fast_chn_attr_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_perf_get_fast_chn_attr_resp),
},
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ mpxy_rpmi_mbox_ದತ್ತಾಂಶ performance_ದತ್ತಾಂಶ = {
	.servicegrp_id = RPMI_SRVGRP_PERFORMANCE ,
	.ಅಂಕೆ_services = RPMI_PERF_SRV_MAX_ಎಣಿಕೆ,
	.service_ದತ್ತಾಂಶ = performance_services,
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ performance_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "riscv,rpmi-mpxy-performance", .ದತ್ತಾಂಶ = &performance_ದತ್ತಾಂಶ },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_mpxy_rpmi_performance = {
	.experimental = true,
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = performance_ಹೊಂದಿಕೆ,
	.init = mpxy_rpmi_mbox_init,
};
