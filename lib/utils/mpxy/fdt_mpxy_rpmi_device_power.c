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
	.id = RPMI_DPWR_SRV_GET_عدد_DOMAINS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = حجم(بنية rpmi_dpwr_get_عدد_domain_resp),
	.max_rx_len = حجم(بنية rpmi_dpwr_get_عدد_domain_resp),
},
{
	.id = RPMI_DPWR_SRV_GET_ځانګړتیاوې,
	.min_tx_len = حجم(بنية rpmi_dpwr_get_attrs_req),
	.max_tx_len = حجم(بنية rpmi_dpwr_get_attrs_req),
	.min_rx_len = حجم(بنية rpmi_dpwr_get_attrs_resp),
	.max_rx_len = حجم(بنية rpmi_dpwr_get_attrs_resp),
},
{
	.id = RPMI_DPWR_SRV_سېټ_STATE,
	.min_tx_len = حجم(بنية rpmi_dpwr_سېټ_state_req),
	.max_tx_len = حجم(بنية rpmi_dpwr_سېټ_state_req),
	.min_rx_len = حجم(بنية rpmi_dpwr_سېټ_state_resp),
	.max_rx_len = حجم(بنية rpmi_dpwr_سېټ_state_resp),
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
	.عدد_services = RPMI_DPWR_SRV_MAX_شمېر,
	.service_معلومات = dpwr_services,
};

ساكن ثابت بنية fdt_سمون dpwr_سمون[] = {
	{ .compatible = "riscv,rpmi-mpxy-device-power", .معلومات = &dpwr_معلومات },
	{ },
};

ثابت بنية fdt_driver fdt_mpxy_rpmi_device_power = {
	.experimental = true,
	.سمون_جدول = dpwr_سمون,
	.init = mpxy_rpmi_mbox_init,
};
