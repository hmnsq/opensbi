#ಅನ್ತರ್ಭಾವ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ಸ್ಥಿರ ರಚನೆ mpxy_rpmi_service_ದತ್ತಾಂಶ dpwr_services[] = {
{
	.id = RPMI_DPWR_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_enable_notification_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_enable_notification_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_enable_notification_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_enable_notification_resp),
},
{
	.id = RPMI_DPWR_SRV_GET_ಅಂಕೆ_DOMAINS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_dpwr_get_ಅಂಕೆ_domain_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_dpwr_get_ಅಂಕೆ_domain_resp),
},
{
	.id = RPMI_DPWR_SRV_GET_ಗುಣಲಕ್ಷಣಗಳು,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_dpwr_get_attrs_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_dpwr_get_attrs_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_dpwr_get_attrs_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_dpwr_get_attrs_resp),
},
{
	.id = RPMI_DPWR_SRV_ಗಣ_STATE,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_dpwr_ಗಣ_state_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_dpwr_ಗಣ_state_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_dpwr_ಗಣ_state_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_dpwr_ಗಣ_state_resp),
},
{
	.id = RPMI_DPWR_SRV_GET_STATE,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_dpwr_get_state_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_dpwr_get_state_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_dpwr_get_state_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_dpwr_get_state_resp),
},
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ mpxy_rpmi_mbox_ದತ್ತಾಂಶ dpwr_ದತ್ತಾಂಶ = {
	.servicegrp_id = RPMI_SRVGRP_DEVICE_POWER,
	.ಅಂಕೆ_services = RPMI_DPWR_SRV_MAX_ಎಣಿಕೆ,
	.service_ದತ್ತಾಂಶ = dpwr_services,
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ dpwr_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "riscv,rpmi-mpxy-device-power", .ದತ್ತಾಂಶ = &dpwr_ದತ್ತಾಂಶ },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_mpxy_rpmi_device_power = {
	.experimental = true,
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = dpwr_ಹೊಂದಿಕೆ,
	.init = mpxy_rpmi_mbox_init,
};
