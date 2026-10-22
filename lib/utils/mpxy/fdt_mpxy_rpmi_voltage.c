#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ਸਥਿਰ ਬਣਤਰ mpxy_rpmi_service_ਅੰਕੜੇ voltage_services[] = {
{
	.id = RPMI_VOLTAGE_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_enable_notification_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_enable_notification_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_enable_notification_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_enable_notification_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_ਅੰਕ_DOMAINS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_voltage_get_ਅੰਕ_domains_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_voltage_get_ਅੰਕ_domains_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_ਗੁਣ,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_voltage_get_ਗੁਣ_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_voltage_get_ਗੁਣ_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_voltage_get_ਗੁਣ_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_voltage_get_ਗੁਣ_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_SUPPORTED_ਪੱਧਰ,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_voltage_get_supported_ਦਰ_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_voltage_get_supported_ਦਰ_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_voltage_get_supported_ਦਰ_resp),
	.max_rx_len = -1U,
},
{
	.id = RPMI_VOLTAGE_SRV_ਸੈੱਟ_CONFIG,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_voltage_ਸੈੱਟ_config_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_voltage_ਸੈੱਟ_config_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_voltage_ਸੈੱਟ_config_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_voltage_ਸੈੱਟ_config_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_CONFIG,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_voltage_get_config_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_voltage_get_config_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_voltage_get_config_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_voltage_get_config_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_ਸੈੱਟ_LEVEL,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_voltage_ਸੈੱਟ_level_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_voltage_ਸੈੱਟ_level_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_voltage_ਸੈੱਟ_level_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_voltage_ਸੈੱਟ_level_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_LEVEL,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_voltage_get_level_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_voltage_get_level_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_voltage_get_level_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_voltage_get_level_resp),
},
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ mpxy_rpmi_mbox_ਅੰਕੜੇ voltage_ਅੰਕੜੇ = {
	.servicegrp_id = RPMI_SRVGRP_VOLTAGE,
	.ਅੰਕ_services = RPMI_VOLTAGE_SRV_MAX_ਗਿਣਤੀ,
	.service_ਅੰਕੜੇ = voltage_services,
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ voltage_ਮੇਲ[] = {
	{ .compatible = "riscv,rpmi-mpxy-voltage", .ਅੰਕੜੇ = &voltage_ਅੰਕੜੇ },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_mpxy_rpmi_voltage = {
	.experimental = true,
	.ਮੇਲ_ਸਾਰਣੀ = voltage_ਮੇਲ,
	.init = mpxy_rpmi_mbox_init,
};
