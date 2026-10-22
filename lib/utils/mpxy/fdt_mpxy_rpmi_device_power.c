#ضمن <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ساكن بنية mpxy_rpmi_service_data dpwr_services[] = {
{
	.id = RPMI_DPWR_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = حجم(بنية rpmi_enable_notification_req),
	.max_tx_len = حجم(بنية rpmi_enable_notification_req),
	.min_rx_len = حجم(بنية rpmi_enable_notification_resp),
	.max_rx_len = حجم(بنية rpmi_enable_notification_resp),
},
{
	.id = RPMI_DPWR_SRV_GET_رەقەم_DOMAINS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = حجم(بنية rpmi_dpwr_get_رەقەم_domain_resp),
	.max_rx_len = حجم(بنية rpmi_dpwr_get_رەقەم_domain_resp),
},
{
	.id = RPMI_DPWR_SRV_GET_ATTRIBUTES,
	.min_tx_len = حجم(بنية rpmi_dpwr_get_attrs_req),
	.max_tx_len = حجم(بنية rpmi_dpwr_get_attrs_req),
	.min_rx_len = حجم(بنية rpmi_dpwr_get_attrs_resp),
	.max_rx_len = حجم(بنية rpmi_dpwr_get_attrs_resp),
},
{
	.id = RPMI_DPWR_SRV_SET_STATE,
	.min_tx_len = حجم(بنية rpmi_dpwr_set_state_req),
	.max_tx_len = حجم(بنية rpmi_dpwr_set_state_req),
	.min_rx_len = حجم(بنية rpmi_dpwr_set_state_resp),
	.max_rx_len = حجم(بنية rpmi_dpwr_set_state_resp),
},
{
	.id = RPMI_DPWR_SRV_GET_STATE,
	.min_tx_len = حجم(بنية rpmi_dpwr_get_state_req),
	.max_tx_len = حجم(بنية rpmi_dpwr_get_state_req),
	.min_rx_len = حجم(بنية rpmi_dpwr_get_state_resp),
	.max_rx_len = حجم(بنية rpmi_dpwr_get_state_resp),
},
};

ساكن ثابت بنية mpxy_rpmi_mbox_data dpwr_data = {
	.servicegrp_id = RPMI_SRVGRP_DEVICE_POWER,
	.رەقەم_services = RPMI_DPWR_SRV_MAX_سانى,
	.service_data = dpwr_services,
};

ساكن ثابت بنية fdt_match dpwr_match[] = {
	{ .compatible = "riscv,rpmi-mpxy-device-power", .data = &dpwr_data },
	{ },
};

ثابت بنية fdt_driver fdt_mpxy_rpmi_device_power = {
	.experimental = true,
	.match_table = dpwr_match,
	.init = mpxy_rpmi_mbox_init,
};
