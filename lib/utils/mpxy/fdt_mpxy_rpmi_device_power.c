#ضمن <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ساكن بنية mpxy_rpmi_service_معلومات dpwr_services[] = {
{
	.id = RPMI_DPWR_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = حجم(بنية rpmi_enable_notification_req),
	.max_tx_len = حجم(بنية rpmi_enable_notification_req),
	.min_rx_len = حجم(بنية rpmi_enable_notification_resp),
	.max_rx_len = حجم(بنية rpmi_enable_notification_resp),
},
{
	.id = RPMI_DPWR_SRV_GET_انگ_DOMAINS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = حجم(بنية rpmi_dpwr_get_انگ_domain_resp),
	.max_rx_len = حجم(بنية rpmi_dpwr_get_انگ_domain_resp),
},
{
	.id = RPMI_DPWR_SRV_GET_خاصيتون,
	.min_tx_len = حجم(بنية rpmi_dpwr_get_attrs_req),
	.max_tx_len = حجم(بنية rpmi_dpwr_get_attrs_req),
	.min_rx_len = حجم(بنية rpmi_dpwr_get_attrs_resp),
	.max_rx_len = حجم(بنية rpmi_dpwr_get_attrs_resp),
},
{
	.id = RPMI_DPWR_SRV_سيٽ_STATE,
	.min_tx_len = حجم(بنية rpmi_dpwr_سيٽ_state_req),
	.max_tx_len = حجم(بنية rpmi_dpwr_سيٽ_state_req),
	.min_rx_len = حجم(بنية rpmi_dpwr_سيٽ_state_resp),
	.max_rx_len = حجم(بنية rpmi_dpwr_سيٽ_state_resp),
},
{
	.id = RPMI_DPWR_SRV_GET_STATE,
	.min_tx_len = حجم(بنية rpmi_dpwr_get_state_req),
	.max_tx_len = حجم(بنية rpmi_dpwr_get_state_req),
	.min_rx_len = حجم(بنية rpmi_dpwr_get_state_resp),
	.max_rx_len = حجم(بنية rpmi_dpwr_get_state_resp),
},
};

ساكن ثابت بنية mpxy_rpmi_mbox_معلومات dpwr_معلومات = {
	.servicegrp_id = RPMI_SRVGRP_DEVICE_POWER,
	.انگ_services = RPMI_DPWR_SRV_MAX_ڳڻپ,
	.service_معلومات = dpwr_services,
};

ساكن ثابت بنية fdt_ميلاپ dpwr_ميلاپ[] = {
	{ .compatible = "riscv,rpmi-mpxy-device-power", .معلومات = &dpwr_معلومات },
	{ },
};

ثابت بنية fdt_driver fdt_mpxy_rpmi_device_power = {
	.experimental = true,
	.ميلاپ_جدول = dpwr_ميلاپ,
	.init = mpxy_rpmi_mbox_init,
};
