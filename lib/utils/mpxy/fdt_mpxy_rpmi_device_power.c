#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ਸਥਿਰ ਬਣਤਰ mpxy_rpmi_service_ਅੰਕੜੇ dpwr_services[] = {
{
	.id = RPMI_DPWR_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_enable_notification_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_enable_notification_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_enable_notification_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_enable_notification_resp),
},
{
	.id = RPMI_DPWR_SRV_GET_ਅੰਕ_DOMAINS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_dpwr_get_ਅੰਕ_domain_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_dpwr_get_ਅੰਕ_domain_resp),
},
{
	.id = RPMI_DPWR_SRV_GET_ਗੁਣ,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_dpwr_get_attrs_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_dpwr_get_attrs_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_dpwr_get_attrs_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_dpwr_get_attrs_resp),
},
{
	.id = RPMI_DPWR_SRV_ਸੈੱਟ_STATE,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_dpwr_ਸੈੱਟ_state_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_dpwr_ਸੈੱਟ_state_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_dpwr_ਸੈੱਟ_state_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_dpwr_ਸੈੱਟ_state_resp),
},
{
	.id = RPMI_DPWR_SRV_GET_STATE,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_dpwr_get_state_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_dpwr_get_state_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_dpwr_get_state_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_dpwr_get_state_resp),
},
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ mpxy_rpmi_mbox_ਅੰਕੜੇ dpwr_ਅੰਕੜੇ = {
	.servicegrp_id = RPMI_SRVGRP_DEVICE_POWER,
	.ਅੰਕ_services = RPMI_DPWR_SRV_MAX_ਗਿਣਤੀ,
	.service_ਅੰਕੜੇ = dpwr_services,
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ dpwr_ਮੇਲ[] = {
	{ .compatible = "riscv,rpmi-mpxy-device-power", .ਅੰਕੜੇ = &dpwr_ਅੰਕੜੇ },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_mpxy_rpmi_device_power = {
	.experimental = true,
	.ਮੇਲ_ਸਾਰਣੀ = dpwr_ਮੇਲ,
	.init = mpxy_rpmi_mbox_init,
};
