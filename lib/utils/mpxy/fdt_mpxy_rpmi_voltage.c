#شامل <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ساکن ساخت mpxy_rpmi_service_ڈیٹا voltage_services[] = {
{
	.id = RPMI_VOLTAGE_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = حجم(ساخت rpmi_enable_notification_req),
	.max_tx_len = حجم(ساخت rpmi_enable_notification_req),
	.min_rx_len = حجم(ساخت rpmi_enable_notification_resp),
	.max_rx_len = حجم(ساخت rpmi_enable_notification_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_عدد_DOMAINS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = حجم(ساخت rpmi_voltage_get_عدد_domains_resp),
	.max_rx_len = حجم(ساخت rpmi_voltage_get_عدد_domains_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_اوصاف,
	.min_tx_len = حجم(ساخت rpmi_voltage_get_اوصاف_req),
	.max_tx_len = حجم(ساخت rpmi_voltage_get_اوصاف_req),
	.min_rx_len = حجم(ساخت rpmi_voltage_get_اوصاف_resp),
	.max_rx_len = حجم(ساخت rpmi_voltage_get_اوصاف_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_SUPPORTED_سطحیں,
	.min_tx_len = حجم(ساخت rpmi_voltage_get_supported_شرح_req),
	.max_tx_len = حجم(ساخت rpmi_voltage_get_supported_شرح_req),
	.min_rx_len = حجم(ساخت rpmi_voltage_get_supported_شرح_resp),
	.max_rx_len = -1U,
},
{
	.id = RPMI_VOLTAGE_SRV_سیٹ_CONFIG,
	.min_tx_len = حجم(ساخت rpmi_voltage_سیٹ_config_req),
	.max_tx_len = حجم(ساخت rpmi_voltage_سیٹ_config_req),
	.min_rx_len = حجم(ساخت rpmi_voltage_سیٹ_config_resp),
	.max_rx_len = حجم(ساخت rpmi_voltage_سیٹ_config_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_CONFIG,
	.min_tx_len = حجم(ساخت rpmi_voltage_get_config_req),
	.max_tx_len = حجم(ساخت rpmi_voltage_get_config_req),
	.min_rx_len = حجم(ساخت rpmi_voltage_get_config_resp),
	.max_rx_len = حجم(ساخت rpmi_voltage_get_config_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_سیٹ_LEVEL,
	.min_tx_len = حجم(ساخت rpmi_voltage_سیٹ_level_req),
	.max_tx_len = حجم(ساخت rpmi_voltage_سیٹ_level_req),
	.min_rx_len = حجم(ساخت rpmi_voltage_سیٹ_level_resp),
	.max_rx_len = حجم(ساخت rpmi_voltage_سیٹ_level_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_LEVEL,
	.min_tx_len = حجم(ساخت rpmi_voltage_get_level_req),
	.max_tx_len = حجم(ساخت rpmi_voltage_get_level_req),
	.min_rx_len = حجم(ساخت rpmi_voltage_get_level_resp),
	.max_rx_len = حجم(ساخت rpmi_voltage_get_level_resp),
},
};

ساکن ثابت ساخت mpxy_rpmi_mbox_ڈیٹا voltage_ڈیٹا = {
	.servicegrp_id = RPMI_SRVGRP_VOLTAGE,
	.عدد_services = RPMI_VOLTAGE_SRV_MAX_گنتی,
	.service_ڈیٹا = voltage_services,
};

ساکن ثابت ساخت fdt_مطابقت voltage_مطابقت[] = {
	{ .compatible = "riscv,rpmi-mpxy-voltage", .ڈیٹا = &voltage_ڈیٹا },
	{ },
};

ثابت ساخت fdt_driver fdt_mpxy_rpmi_voltage = {
	.experimental = true,
	.مطابقت_جدول = voltage_مطابقت,
	.init = mpxy_rpmi_mbox_init,
};
