#অন্তর্ভাৱ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

স্থির সংরচনা mpxy_rpmi_service_data voltage_services[] = {
{
	.id = RPMI_VOLTAGE_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = পরিমাণ(সংরচনা rpmi_enable_notification_req),
	.max_tx_len = পরিমাণ(সংরচনা rpmi_enable_notification_req),
	.min_rx_len = পরিমাণ(সংরচনা rpmi_enable_notification_resp),
	.max_rx_len = পরিমাণ(সংরচনা rpmi_enable_notification_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_অঙ্ক_DOMAINS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = পরিমাণ(সংরচনা rpmi_voltage_get_অঙ্ক_domains_resp),
	.max_rx_len = পরিমাণ(সংরচনা rpmi_voltage_get_অঙ্ক_domains_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_ATTRIBUTES,
	.min_tx_len = পরিমাণ(সংরচনা rpmi_voltage_get_attributes_req),
	.max_tx_len = পরিমাণ(সংরচনা rpmi_voltage_get_attributes_req),
	.min_rx_len = পরিমাণ(সংরচনা rpmi_voltage_get_attributes_resp),
	.max_rx_len = পরিমাণ(সংরচনা rpmi_voltage_get_attributes_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_SUPPORTED_LEVELS,
	.min_tx_len = পরিমাণ(সংরচনা rpmi_voltage_get_supported_rate_req),
	.max_tx_len = পরিমাণ(সংরচনা rpmi_voltage_get_supported_rate_req),
	.min_rx_len = পরিমাণ(সংরচনা rpmi_voltage_get_supported_rate_resp),
	.max_rx_len = -1U,
},
{
	.id = RPMI_VOLTAGE_SRV_SET_CONFIG,
	.min_tx_len = পরিমাণ(সংরচনা rpmi_voltage_set_config_req),
	.max_tx_len = পরিমাণ(সংরচনা rpmi_voltage_set_config_req),
	.min_rx_len = পরিমাণ(সংরচনা rpmi_voltage_set_config_resp),
	.max_rx_len = পরিমাণ(সংরচনা rpmi_voltage_set_config_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_CONFIG,
	.min_tx_len = পরিমাণ(সংরচনা rpmi_voltage_get_config_req),
	.max_tx_len = পরিমাণ(সংরচনা rpmi_voltage_get_config_req),
	.min_rx_len = পরিমাণ(সংরচনা rpmi_voltage_get_config_resp),
	.max_rx_len = পরিমাণ(সংরচনা rpmi_voltage_get_config_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_SET_LEVEL,
	.min_tx_len = পরিমাণ(সংরচনা rpmi_voltage_set_level_req),
	.max_tx_len = পরিমাণ(সংরচনা rpmi_voltage_set_level_req),
	.min_rx_len = পরিমাণ(সংরচনা rpmi_voltage_set_level_resp),
	.max_rx_len = পরিমাণ(সংরচনা rpmi_voltage_set_level_resp),
},
{
	.id = RPMI_VOLTAGE_SRV_GET_LEVEL,
	.min_tx_len = পরিমাণ(সংরচনা rpmi_voltage_get_level_req),
	.max_tx_len = পরিমাণ(সংরচনা rpmi_voltage_get_level_req),
	.min_rx_len = পরিমাণ(সংরচনা rpmi_voltage_get_level_resp),
	.max_rx_len = পরিমাণ(সংরচনা rpmi_voltage_get_level_resp),
},
};

স্থির ধ্রুৱ সংরচনা mpxy_rpmi_mbox_data voltage_data = {
	.servicegrp_id = RPMI_SRVGRP_VOLTAGE,
	.অঙ্ক_services = RPMI_VOLTAGE_SRV_MAX_গুনতি,
	.service_data = voltage_services,
};

স্থির ধ্রুৱ সংরচনা fdt_match voltage_match[] = {
	{ .compatible = "riscv,rpmi-mpxy-voltage", .data = &voltage_data },
	{ },
};

ধ্রুৱ সংরচনা fdt_driver fdt_mpxy_rpmi_voltage = {
	.experimental = true,
	.match_table = voltage_match,
	.init = mpxy_rpmi_mbox_init,
};
