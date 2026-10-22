#ضمن <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ساكن بنية mpxy_rpmi_service_انکڑے voltage_services[] = {
{
	.id = RPMI_VOLTAGE_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = حجم(بنية rpmi_enable_notification_req),
	.max_tx_len = حجم(بنية rpmi_enable_notification_req),
	.min_rx_len = حجم(بنية rpmi_enable_notification_resp),
	.max_rx_len = حجم(بنية rpmi_enable_notification_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_عدد_DOMAINS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = حجم(بنية rpmi_voltage_get_عدد_domains_resp),
	.max_rx_len = حجم(بنية rpmi_voltage_get_عدد_domains_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_گن,
	.min_tx_len = حجم(بنية rpmi_voltage_get_گن_req),
	.max_tx_len = حجم(بنية rpmi_voltage_get_گن_req),
	.min_rx_len = حجم(بنية rpmi_voltage_get_گن_resp),
	.max_rx_len = حجم(بنية rpmi_voltage_get_گن_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_SUPPORTED_پدھراں,
	.min_tx_len = حجم(بنية rpmi_voltage_get_supported_شرح_req),
	.max_tx_len = حجم(بنية rpmi_voltage_get_supported_شرح_req),
	.min_rx_len = حجم(بنية rpmi_voltage_get_supported_شرح_resp),
	.max_rx_len = -1U,
},
{
	.id = RPMI_VOLTAGE_SRV_سیٹ_CONFIG,
	.min_tx_len = حجم(بنية rpmi_voltage_سیٹ_config_req),
	.max_tx_len = حجم(بنية rpmi_voltage_سیٹ_config_req),
	.min_rx_len = حجم(بنية rpmi_voltage_سیٹ_config_resp),
	.max_rx_len = حجم(بنية rpmi_voltage_سیٹ_config_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_CONFIG,
	.min_tx_len = حجم(بنية rpmi_voltage_get_config_req),
	.max_tx_len = حجم(بنية rpmi_voltage_get_config_req),
	.min_rx_len = حجم(بنية rpmi_voltage_get_config_resp),
	.max_rx_len = حجم(بنية rpmi_voltage_get_config_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_سیٹ_LEVEL,
	.min_tx_len = حجم(بنية rpmi_voltage_سیٹ_level_req),
	.max_tx_len = حجم(بنية rpmi_voltage_سیٹ_level_req),
	.min_rx_len = حجم(بنية rpmi_voltage_سیٹ_level_resp),
	.max_rx_len = حجم(بنية rpmi_voltage_سیٹ_level_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_LEVEL,
	.min_tx_len = حجم(بنية rpmi_voltage_get_level_req),
	.max_tx_len = حجم(بنية rpmi_voltage_get_level_req),
	.min_rx_len = حجم(بنية rpmi_voltage_get_level_resp),
	.max_rx_len = حجم(بنية rpmi_voltage_get_level_resp),
},
};

ساكن ثابت بنية mpxy_rpmi_mbox_انکڑے voltage_انکڑے = {
	.servicegrp_id = RPMI_SRVGRP_VOLTAGE,
	.عدد_services = RPMI_VOLTAGE_SRV_MAX_گنتی,
	.service_انکڑے = voltage_services,
};

ساكن ثابت بنية fdt_میل voltage_میل[] = {
	{ .compatible = "riscv,rpmi-mpxy-voltage", .انکڑے = &voltage_انکڑے },
	{ },
};

ثابت بنية fdt_driver fdt_mpxy_rpmi_voltage = {
	.experimental = true,
	.میل_جدول = voltage_میل,
	.init = mpxy_rpmi_mbox_init,
};
