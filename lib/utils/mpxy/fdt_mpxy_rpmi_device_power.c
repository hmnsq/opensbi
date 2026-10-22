#شامل <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ساکن ساخت mpxy_rpmi_service_ڈیٹا dpwr_services[] = {
{
	.id = RPMI_DPWR_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = حجم(ساخت rpmi_enable_notification_req),
	.max_tx_len = حجم(ساخت rpmi_enable_notification_req),
	.min_rx_len = حجم(ساخت rpmi_enable_notification_resp),
	.max_rx_len = حجم(ساخت rpmi_enable_notification_resp),
},
{
	.id = RPMI_DPWR_SRV_GET_عدد_DOMAINS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = حجم(ساخت rpmi_dpwr_get_عدد_domain_resp),
	.max_rx_len = حجم(ساخت rpmi_dpwr_get_عدد_domain_resp),
},
{
	.id = RPMI_DPWR_SRV_GET_اوصاف,
	.min_tx_len = حجم(ساخت rpmi_dpwr_get_attrs_req),
	.max_tx_len = حجم(ساخت rpmi_dpwr_get_attrs_req),
	.min_rx_len = حجم(ساخت rpmi_dpwr_get_attrs_resp),
	.max_rx_len = حجم(ساخت rpmi_dpwr_get_attrs_resp),
},
{
	.id = RPMI_DPWR_SRV_سیٹ_STATE,
	.min_tx_len = حجم(ساخت rpmi_dpwr_سیٹ_state_req),
	.max_tx_len = حجم(ساخت rpmi_dpwr_سیٹ_state_req),
	.min_rx_len = حجم(ساخت rpmi_dpwr_سیٹ_state_resp),
	.max_rx_len = حجم(ساخت rpmi_dpwr_سیٹ_state_resp),
},
{
	.id = RPMI_DPWR_SRV_GET_STATE,
	.min_tx_len = حجم(ساخت rpmi_dpwr_get_state_req),
	.max_tx_len = حجم(ساخت rpmi_dpwr_get_state_req),
	.min_rx_len = حجم(ساخت rpmi_dpwr_get_state_resp),
	.max_rx_len = حجم(ساخت rpmi_dpwr_get_state_resp),
},
};

ساکن ثابت ساخت mpxy_rpmi_mbox_ڈیٹا dpwr_ڈیٹا = {
	.servicegrp_id = RPMI_SRVGRP_DEVICE_POWER,
	.عدد_services = RPMI_DPWR_SRV_MAX_گنتی,
	.service_ڈیٹا = dpwr_services,
};

ساکن ثابت ساخت fdt_مطابقت dpwr_مطابقت[] = {
	{ .compatible = "riscv,rpmi-mpxy-device-power", .ڈیٹا = &dpwr_ڈیٹا },
	{ },
};

ثابت ساخت fdt_driver fdt_mpxy_rpmi_device_power = {
	.experimental = true,
	.مطابقت_جدول = dpwr_مطابقت,
	.init = mpxy_rpmi_mbox_init,
};
