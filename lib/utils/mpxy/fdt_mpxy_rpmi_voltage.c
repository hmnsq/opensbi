#ಅನ್ತರ್ಭಾವ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ಸ್ಥಿರ ರಚನೆ mpxy_rpmi_service_ದತ್ತಾಂಶ voltage_services[] = {
{
	.id = RPMI_VOLTAGE_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_enable_notification_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_enable_notification_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_enable_notification_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_enable_notification_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_ಅಂಕೆ_DOMAINS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_voltage_get_ಅಂಕೆ_domains_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_voltage_get_ಅಂಕೆ_domains_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_ಗುಣಲಕ್ಷಣಗಳು,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_voltage_get_ಗುಣಲಕ್ಷಣಗಳು_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_voltage_get_ಗುಣಲಕ್ಷಣಗಳು_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_voltage_get_ಗುಣಲಕ್ಷಣಗಳು_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_voltage_get_ಗುಣಲಕ್ಷಣಗಳು_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_SUPPORTED_ಮಟ್ಟಗಳು,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_voltage_get_supported_ದರ_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_voltage_get_supported_ದರ_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_voltage_get_supported_ದರ_resp),
	.max_rx_len = -1U,
},
{
	.id = RPMI_VOLTAGE_SRV_ಗಣ_CONFIG,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_voltage_ಗಣ_config_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_voltage_ಗಣ_config_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_voltage_ಗಣ_config_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_voltage_ಗಣ_config_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_CONFIG,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_voltage_get_config_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_voltage_get_config_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_voltage_get_config_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_voltage_get_config_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_ಗಣ_LEVEL,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_voltage_ಗಣ_level_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_voltage_ಗಣ_level_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_voltage_ಗಣ_level_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_voltage_ಗಣ_level_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_LEVEL,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_voltage_get_level_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_voltage_get_level_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_voltage_get_level_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_voltage_get_level_resp),
},
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ mpxy_rpmi_mbox_ದತ್ತಾಂಶ voltage_ದತ್ತಾಂಶ = {
	.servicegrp_id = RPMI_SRVGRP_VOLTAGE,
	.ಅಂಕೆ_services = RPMI_VOLTAGE_SRV_MAX_ಎಣಿಕೆ,
	.service_ದತ್ತಾಂಶ = voltage_services,
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ voltage_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "riscv,rpmi-mpxy-voltage", .ದತ್ತಾಂಶ = &voltage_ದತ್ತಾಂಶ },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_mpxy_rpmi_voltage = {
	.experimental = true,
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = voltage_ಹೊಂದಿಕೆ,
	.init = mpxy_rpmi_mbox_init,
};
