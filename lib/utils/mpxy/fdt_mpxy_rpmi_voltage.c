#ضمن <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ساكن بنية mpxy_rpmi_service_معلومات voltage_services[] = {
{
	.id = RPMI_VOLTAGE_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = حجم(بنية rpmi_enable_notification_req),
	.max_tx_len = حجم(بنية rpmi_enable_notification_req),
	.min_rx_len = حجم(بنية rpmi_enable_notification_resp),
	.max_rx_len = حجم(بنية rpmi_enable_notification_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_انگ_DOMAINS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = حجم(بنية rpmi_voltage_get_انگ_domains_resp),
	.max_rx_len = حجم(بنية rpmi_voltage_get_انگ_domains_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_خاصيتون,
	.min_tx_len = حجم(بنية rpmi_voltage_get_خاصيتون_req),
	.max_tx_len = حجم(بنية rpmi_voltage_get_خاصيتون_req),
	.min_rx_len = حجم(بنية rpmi_voltage_get_خاصيتون_resp),
	.max_rx_len = حجم(بنية rpmi_voltage_get_خاصيتون_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_SUPPORTED_سطحون,
	.min_tx_len = حجم(بنية rpmi_voltage_get_supported_شرح_req),
	.max_tx_len = حجم(بنية rpmi_voltage_get_supported_شرح_req),
	.min_rx_len = حجم(بنية rpmi_voltage_get_supported_شرح_resp),
	.max_rx_len = -1U,
},
{
	.id = RPMI_VOLTAGE_SRV_سيٽ_CONFIG,
	.min_tx_len = حجم(بنية rpmi_voltage_سيٽ_config_req),
	.max_tx_len = حجم(بنية rpmi_voltage_سيٽ_config_req),
	.min_rx_len = حجم(بنية rpmi_voltage_سيٽ_config_resp),
	.max_rx_len = حجم(بنية rpmi_voltage_سيٽ_config_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_CONFIG,
	.min_tx_len = حجم(بنية rpmi_voltage_get_config_req),
	.max_tx_len = حجم(بنية rpmi_voltage_get_config_req),
	.min_rx_len = حجم(بنية rpmi_voltage_get_config_resp),
	.max_rx_len = حجم(بنية rpmi_voltage_get_config_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_سيٽ_LEVEL,
	.min_tx_len = حجم(بنية rpmi_voltage_سيٽ_level_req),
	.max_tx_len = حجم(بنية rpmi_voltage_سيٽ_level_req),
	.min_rx_len = حجم(بنية rpmi_voltage_سيٽ_level_resp),
	.max_rx_len = حجم(بنية rpmi_voltage_سيٽ_level_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_LEVEL,
	.min_tx_len = حجم(بنية rpmi_voltage_get_level_req),
	.max_tx_len = حجم(بنية rpmi_voltage_get_level_req),
	.min_rx_len = حجم(بنية rpmi_voltage_get_level_resp),
	.max_rx_len = حجم(بنية rpmi_voltage_get_level_resp),
},
};

ساكن ثابت بنية mpxy_rpmi_mbox_معلومات voltage_معلومات = {
	.servicegrp_id = RPMI_SRVGRP_VOLTAGE,
	.انگ_services = RPMI_VOLTAGE_SRV_MAX_ڳڻپ,
	.service_معلومات = voltage_services,
};

ساكن ثابت بنية fdt_ميلاپ voltage_ميلاپ[] = {
	{ .compatible = "riscv,rpmi-mpxy-voltage", .معلومات = &voltage_معلومات },
	{ },
};

ثابت بنية fdt_driver fdt_mpxy_rpmi_voltage = {
	.experimental = true,
	.ميلاپ_جدول = voltage_ميلاپ,
	.init = mpxy_rpmi_mbox_init,
};
