#ضمن <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ساكن بنية mpxy_rpmi_service_داده voltage_services[] = {
{
	.id = RPMI_VOLTAGE_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = حجم(بنية rpmi_enable_notification_req),
	.max_tx_len = حجم(بنية rpmi_enable_notification_req),
	.min_rx_len = حجم(بنية rpmi_enable_notification_resp),
	.max_rx_len = حجم(بنية rpmi_enable_notification_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_رقم_DOMAINS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = حجم(بنية rpmi_voltage_get_رقم_domains_resp),
	.max_rx_len = حجم(بنية rpmi_voltage_get_رقم_domains_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_صفات,
	.min_tx_len = حجم(بنية rpmi_voltage_get_صفات_req),
	.max_tx_len = حجم(بنية rpmi_voltage_get_صفات_req),
	.min_rx_len = حجم(بنية rpmi_voltage_get_صفات_resp),
	.max_rx_len = حجم(بنية rpmi_voltage_get_صفات_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_SUPPORTED_سطوح,
	.min_tx_len = حجم(بنية rpmi_voltage_get_supported_نرخ_req),
	.max_tx_len = حجم(بنية rpmi_voltage_get_supported_نرخ_req),
	.min_rx_len = حجم(بنية rpmi_voltage_get_supported_نرخ_resp),
	.max_rx_len = -1U,
},
{
	.id = RPMI_VOLTAGE_SRV_مجموعه_CONFIG,
	.min_tx_len = حجم(بنية rpmi_voltage_مجموعه_config_req),
	.max_tx_len = حجم(بنية rpmi_voltage_مجموعه_config_req),
	.min_rx_len = حجم(بنية rpmi_voltage_مجموعه_config_resp),
	.max_rx_len = حجم(بنية rpmi_voltage_مجموعه_config_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_CONFIG,
	.min_tx_len = حجم(بنية rpmi_voltage_get_config_req),
	.max_tx_len = حجم(بنية rpmi_voltage_get_config_req),
	.min_rx_len = حجم(بنية rpmi_voltage_get_config_resp),
	.max_rx_len = حجم(بنية rpmi_voltage_get_config_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_مجموعه_LEVEL,
	.min_tx_len = حجم(بنية rpmi_voltage_مجموعه_level_req),
	.max_tx_len = حجم(بنية rpmi_voltage_مجموعه_level_req),
	.min_rx_len = حجم(بنية rpmi_voltage_مجموعه_level_resp),
	.max_rx_len = حجم(بنية rpmi_voltage_مجموعه_level_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_LEVEL,
	.min_tx_len = حجم(بنية rpmi_voltage_get_level_req),
	.max_tx_len = حجم(بنية rpmi_voltage_get_level_req),
	.min_rx_len = حجم(بنية rpmi_voltage_get_level_resp),
	.max_rx_len = حجم(بنية rpmi_voltage_get_level_resp),
},
};

ساكن ثابت بنية mpxy_rpmi_mbox_داده voltage_داده = {
	.servicegrp_id = RPMI_SRVGRP_VOLTAGE,
	.رقم_services = RPMI_VOLTAGE_SRV_MAX_شمار,
	.service_داده = voltage_services,
};

ساكن ثابت بنية fdt_تطابق voltage_تطابق[] = {
	{ .compatible = "riscv,rpmi-mpxy-voltage", .داده = &voltage_داده },
	{ },
};

ثابت بنية fdt_driver fdt_mpxy_rpmi_voltage = {
	.experimental = true,
	.تطابق_جدول = voltage_تطابق,
	.init = mpxy_rpmi_mbox_init,
};
