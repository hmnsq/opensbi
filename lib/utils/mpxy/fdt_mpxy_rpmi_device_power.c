#अन्तर्भाव <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

स्थिर संरचना mpxy_rpmi_service_दत्त dpwr_services[] = {
{
	.id = RPMI_DPWR_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = परिमाण(संरचना rpmi_enable_notification_req),
	.max_tx_len = परिमाण(संरचना rpmi_enable_notification_req),
	.min_rx_len = परिमाण(संरचना rpmi_enable_notification_resp),
	.max_rx_len = परिमाण(संरचना rpmi_enable_notification_resp),
},
{
	.id = RPMI_DPWR_SRV_GET_अङ्कः_DOMAINS,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = परिमाण(संरचना rpmi_dpwr_get_अङ्कः_domain_resp),
	.max_rx_len = परिमाण(संरचना rpmi_dpwr_get_अङ्कः_domain_resp),
},
{
	.id = RPMI_DPWR_SRV_GET_गुणाः,
	.min_tx_len = परिमाण(संरचना rpmi_dpwr_get_attrs_req),
	.max_tx_len = परिमाण(संरचना rpmi_dpwr_get_attrs_req),
	.min_rx_len = परिमाण(संरचना rpmi_dpwr_get_attrs_resp),
	.max_rx_len = परिमाण(संरचना rpmi_dpwr_get_attrs_resp),
},
{
	.id = RPMI_DPWR_SRV_समुच्चयः_STATE,
	.min_tx_len = परिमाण(संरचना rpmi_dpwr_समुच्चयः_state_req),
	.max_tx_len = परिमाण(संरचना rpmi_dpwr_समुच्चयः_state_req),
	.min_rx_len = परिमाण(संरचना rpmi_dpwr_समुच्चयः_state_resp),
	.max_rx_len = परिमाण(संरचना rpmi_dpwr_समुच्चयः_state_resp),
},
{
	.id = RPMI_DPWR_SRV_GET_STATE,
	.min_tx_len = परिमाण(संरचना rpmi_dpwr_get_state_req),
	.max_tx_len = परिमाण(संरचना rpmi_dpwr_get_state_req),
	.min_rx_len = परिमाण(संरचना rpmi_dpwr_get_state_resp),
	.max_rx_len = परिमाण(संरचना rpmi_dpwr_get_state_resp),
},
};

स्थिर ध्रुव संरचना mpxy_rpmi_mbox_दत्त dpwr_दत्त = {
	.servicegrp_id = RPMI_SRVGRP_DEVICE_POWER,
	.अङ्कः_services = RPMI_DPWR_SRV_MAX_गणनम्,
	.service_दत्त = dpwr_services,
};

स्थिर ध्रुव संरचना fdt_मेलनम् dpwr_मेलनम्[] = {
	{ .compatible = "riscv,rpmi-mpxy-device-power", .दत्त = &dpwr_दत्त },
	{ },
};

ध्रुव संरचना fdt_driver fdt_mpxy_rpmi_device_power = {
	.experimental = true,
	.मेलनम्_सारणी = dpwr_मेलनम्,
	.init = mpxy_rpmi_mbox_init,
};
