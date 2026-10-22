#समावेश <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

अटल काष्ठा mpxy_rpmi_service_तथ्य dpwr_services[] = {
{
	.id = RPMI_DPWR_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = माप(काष्ठा rpmi_enable_notification_req),
	.max_tx_len = माप(काष्ठा rpmi_enable_notification_req),
	.min_rx_len = माप(काष्ठा rpmi_enable_notification_resp),
	.max_rx_len = माप(काष्ठा rpmi_enable_notification_resp),
},
{
	.id = RPMI_DPWR_SRV_GET_अंक_DOMAINS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = माप(काष्ठा rpmi_dpwr_get_अंक_domain_resp),
	.max_rx_len = माप(काष्ठा rpmi_dpwr_get_अंक_domain_resp),
},
{
	.id = RPMI_DPWR_SRV_GET_ATTRIBUTES,
	.min_tx_len = माप(काष्ठा rpmi_dpwr_get_attrs_req),
	.max_tx_len = माप(काष्ठा rpmi_dpwr_get_attrs_req),
	.min_rx_len = माप(काष्ठा rpmi_dpwr_get_attrs_resp),
	.max_rx_len = माप(काष्ठा rpmi_dpwr_get_attrs_resp),
},
{
	.id = RPMI_DPWR_SRV_SET_STATE,
	.min_tx_len = माप(काष्ठा rpmi_dpwr_set_state_req),
	.max_tx_len = माप(काष्ठा rpmi_dpwr_set_state_req),
	.min_rx_len = माप(काष्ठा rpmi_dpwr_set_state_resp),
	.max_rx_len = माप(काष्ठा rpmi_dpwr_set_state_resp),
},
{
	.id = RPMI_DPWR_SRV_GET_STATE,
	.min_tx_len = माप(काष्ठा rpmi_dpwr_get_state_req),
	.max_tx_len = माप(काष्ठा rpmi_dpwr_get_state_req),
	.min_rx_len = माप(काष्ठा rpmi_dpwr_get_state_resp),
	.max_rx_len = माप(काष्ठा rpmi_dpwr_get_state_resp),
},
};

अटल स्थिर काष्ठा mpxy_rpmi_mbox_तथ्य dpwr_तथ्य = {
	.servicegrp_id = RPMI_SRVGRP_DEVICE_POWER,
	.अंक_services = RPMI_DPWR_SRV_MAX_गिनती,
	.service_तथ्य = dpwr_services,
};

अटल स्थिर काष्ठा fdt_match dpwr_match[] = {
	{ .compatible = "riscv,rpmi-mpxy-device-power", .तथ्य = &dpwr_तथ्य },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_mpxy_rpmi_device_power = {
	.experimental = true,
	.match_table = dpwr_match,
	.init = mpxy_rpmi_mbox_init,
};
