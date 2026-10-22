#ضمن <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ساكن بنية mpxy_rpmi_service_داده dpwr_services[] = {
{
	.id = RPMI_DPWR_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = حجم(بنية rpmi_enable_notification_req),
	.max_tx_len = حجم(بنية rpmi_enable_notification_req),
	.min_rx_len = حجم(بنية rpmi_enable_notification_resp),
	.max_rx_len = حجم(بنية rpmi_enable_notification_resp),
},
{
	.id = RPMI_DPWR_SRV_GET_رقم_DOMAINS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = حجم(بنية rpmi_dpwr_get_رقم_domain_resp),
	.max_rx_len = حجم(بنية rpmi_dpwr_get_رقم_domain_resp),
},
{
	.id = RPMI_DPWR_SRV_GET_صفات,
	.min_tx_len = حجم(بنية rpmi_dpwr_get_attrs_req),
	.max_tx_len = حجم(بنية rpmi_dpwr_get_attrs_req),
	.min_rx_len = حجم(بنية rpmi_dpwr_get_attrs_resp),
	.max_rx_len = حجم(بنية rpmi_dpwr_get_attrs_resp),
},
{
	.id = RPMI_DPWR_SRV_مجموعه_STATE,
	.min_tx_len = حجم(بنية rpmi_dpwr_مجموعه_state_req),
	.max_tx_len = حجم(بنية rpmi_dpwr_مجموعه_state_req),
	.min_rx_len = حجم(بنية rpmi_dpwr_مجموعه_state_resp),
	.max_rx_len = حجم(بنية rpmi_dpwr_مجموعه_state_resp),
},
{
	.id = RPMI_DPWR_SRV_GET_STATE,
	.min_tx_len = حجم(بنية rpmi_dpwr_get_state_req),
	.max_tx_len = حجم(بنية rpmi_dpwr_get_state_req),
	.min_rx_len = حجم(بنية rpmi_dpwr_get_state_resp),
	.max_rx_len = حجم(بنية rpmi_dpwr_get_state_resp),
},
};

ساكن ثابت بنية mpxy_rpmi_mbox_داده dpwr_داده = {
	.servicegrp_id = RPMI_SRVGRP_DEVICE_POWER,
	.رقم_services = RPMI_DPWR_SRV_MAX_شمار,
	.service_داده = dpwr_services,
};

ساكن ثابت بنية fdt_تطابق dpwr_تطابق[] = {
	{ .compatible = "riscv,rpmi-mpxy-device-power", .داده = &dpwr_داده },
	{ },
};

ثابت بنية fdt_driver fdt_mpxy_rpmi_device_power = {
	.experimental = true,
	.تطابق_جدول = dpwr_تطابق,
	.init = mpxy_rpmi_mbox_init,
};
