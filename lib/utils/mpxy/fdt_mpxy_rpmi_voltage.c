#अन्तर्भाव <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

स्थिर संरचना mpxy_rpmi_service_दत्त voltage_services[] = {
{
	.id = RPMI_VOLTAGE_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = परिमाण(संरचना rpmi_enable_notification_req),
	.max_tx_len = परिमाण(संरचना rpmi_enable_notification_req),
	.min_rx_len = परिमाण(संरचना rpmi_enable_notification_resp),
	.max_rx_len = परिमाण(संरचना rpmi_enable_notification_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_अङ्कः_DOMAINS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = परिमाण(संरचना rpmi_voltage_get_अङ्कः_domains_resp),
	.max_rx_len = परिमाण(संरचना rpmi_voltage_get_अङ्कः_domains_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_गुणाः,
	.min_tx_len = परिमाण(संरचना rpmi_voltage_get_गुणाः_req),
	.max_tx_len = परिमाण(संरचना rpmi_voltage_get_गुणाः_req),
	.min_rx_len = परिमाण(संरचना rpmi_voltage_get_गुणाः_resp),
	.max_rx_len = परिमाण(संरचना rpmi_voltage_get_गुणाः_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_SUPPORTED_स्तराः,
	.min_tx_len = परिमाण(संरचना rpmi_voltage_get_supported_दरः_req),
	.max_tx_len = परिमाण(संरचना rpmi_voltage_get_supported_दरः_req),
	.min_rx_len = परिमाण(संरचना rpmi_voltage_get_supported_दरः_resp),
	.max_rx_len = -1U,
},
{
	.id = RPMI_VOLTAGE_SRV_समुच्चयः_CONFIG,
	.min_tx_len = परिमाण(संरचना rpmi_voltage_समुच्चयः_config_req),
	.max_tx_len = परिमाण(संरचना rpmi_voltage_समुच्चयः_config_req),
	.min_rx_len = परिमाण(संरचना rpmi_voltage_समुच्चयः_config_resp),
	.max_rx_len = परिमाण(संरचना rpmi_voltage_समुच्चयः_config_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_CONFIG,
	.min_tx_len = परिमाण(संरचना rpmi_voltage_get_config_req),
	.max_tx_len = परिमाण(संरचना rpmi_voltage_get_config_req),
	.min_rx_len = परिमाण(संरचना rpmi_voltage_get_config_resp),
	.max_rx_len = परिमाण(संरचना rpmi_voltage_get_config_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_समुच्चयः_LEVEL,
	.min_tx_len = परिमाण(संरचना rpmi_voltage_समुच्चयः_level_req),
	.max_tx_len = परिमाण(संरचना rpmi_voltage_समुच्चयः_level_req),
	.min_rx_len = परिमाण(संरचना rpmi_voltage_समुच्चयः_level_resp),
	.max_rx_len = परिमाण(संरचना rpmi_voltage_समुच्चयः_level_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_LEVEL,
	.min_tx_len = परिमाण(संरचना rpmi_voltage_get_level_req),
	.max_tx_len = परिमाण(संरचना rpmi_voltage_get_level_req),
	.min_rx_len = परिमाण(संरचना rpmi_voltage_get_level_resp),
	.max_rx_len = परिमाण(संरचना rpmi_voltage_get_level_resp),
},
};

स्थिर ध्रुव संरचना mpxy_rpmi_mbox_दत्त voltage_दत्त = {
	.servicegrp_id = RPMI_SRVGRP_VOLTAGE,
	.अङ्कः_services = RPMI_VOLTAGE_SRV_MAX_गणनम्,
	.service_दत्त = voltage_services,
};

स्थिर ध्रुव संरचना fdt_मेलनम् voltage_मेलनम्[] = {
	{ .compatible = "riscv,rpmi-mpxy-voltage", .दत्त = &voltage_दत्त },
	{ },
};

ध्रुव संरचना fdt_driver fdt_mpxy_rpmi_voltage = {
	.experimental = true,
	.मेलनम्_सारणी = voltage_मेलनम्,
	.init = mpxy_rpmi_mbox_init,
};
