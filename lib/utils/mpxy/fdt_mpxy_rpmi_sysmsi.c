/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#अन्तर्भाव <sbi/sbi_bitmap.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/sbi_byteorder.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi_utils/mailbox/rpmi_mailbox.h>
#अन्तर्भाव <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

संरचना mpxy_rpmi_sysmsi {
	u32 sys_अंक_msi;
	अचिन्हित दीर्घ *sys_msi_denied_bmap;
};

स्थिर पूर्णांक mpxy_rpmi_sysmis_xfer(शून्य *context, संरचना mbox_chan *chan,
				 संरचना mbox_xfer *xfer)
{
	संरचना rpmi_message_args *args = xfer->args;
	संरचना mpxy_rpmi_sysmsi *smg = context;
	u64 sys_msi_address;
	u32 sys_msi_index;
	पूर्णांक rc = 0;

	जर (!xfer->rx || args->type != RPMI_संदेश_NORMAL_REQUEST)
		परत 0;

	निवड (args->service_id) {
	स्थिती RPMI_SYSMSI_SRV_GET_ATTRIBUTES:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_to_le32(smg->sys_अंक_msi);
		((u32 *)xfer->rx)[2] = 0;
		((u32 *)xfer->rx)[3] = 0;
		args->rx_data_len = 4 * आकार(u32);
		थांबा;
	स्थिती RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES:
	स्थिती RPMI_SYSMSI_SRV_SET_MSI_STATE:
	स्थिती RPMI_SYSMSI_SRV_GET_MSI_STATE:
	स्थिती RPMI_SYSMSI_SRV_SET_MSI_TARGET:
	स्थिती RPMI_SYSMSI_SRV_GET_MSI_TARGET:
		sys_msi_index = le32_to_cpu(((u32 *)xfer->tx)[0]);
		जर (smg->sys_अंक_msi <= sys_msi_index) {
			((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_INVALID_PARAM);
			args->rx_data_len = आकार(u32);
			थांबा;
		}
		जर (bitmap_test(smg->sys_msi_denied_bmap, sys_msi_index)) {
			((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_DENIED);
			args->rx_data_len = आकार(u32);
			थांबा;
		}
		जर (args->service_id == RPMI_SYSMSI_SRV_SET_MSI_TARGET) {
			sys_msi_address = le32_to_cpu(((u32 *)xfer->tx)[1]);
			sys_msi_address |= ((u64)le32_to_cpu(((u32 *)xfer->tx)[2])) << 32;
			जर (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
							 sys_msi_address, 0x4, PRV_S,
							 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE |
							 SBI_DOMAIN_MMIO)) {
				((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_INVALID_ADDR);
				args->rx_data_len = आकार(u32);
				थांबा;
			}
		}
		rc = mbox_chan_xfer(chan, xfer);
		थांबा;
	पूर्वनिर्धारित:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_NOTSUPP);
		args->rx_data_len = आकार(u32);
		थांबा;
	};

	परत rc;
}

स्थिर शून्य mpxy_rpmi_sysmsi_cleanup(शून्य *context)
{
	संरचना mpxy_rpmi_sysmsi *smg = context;

	sbi_free(smg->sys_msi_denied_bmap);
	sbi_free(smg);
}

स्थिर पूर्णांक mpxy_rpmi_sysmsi_setup(शून्य **context, संरचना mbox_chan *chan,
				  स्थिरांक संरचना mpxy_rpmi_mbox_data *data)
{
	संरचना rpmi_sysmsi_get_msi_attributes_resp gmaresp;
	संरचना rpmi_sysmsi_get_msi_attributes_req gmareq;
	संरचना rpmi_sysmsi_get_attributes_resp garesp;
	संरचना mpxy_rpmi_sysmsi *smg;
	u32 p2a_db_index;
	पूर्णांक rc, i;

	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_P2A_DOORBELL_SYSMSI_INDEX,
				     &p2a_db_index);
	जर (rc)
		परत rc;

	rc = rpmi_normal_request_with_status(chan, RPMI_SYSMSI_SRV_GET_ATTRIBUTES,
					     रिक्त, 0, 0, &garesp, rpmi_u32_मोजणी(garesp),
					     rpmi_u32_मोजणी(garesp));
	जर (rc)
		परत rc;

	smg = sbi_zalloc(आकार(*smg));
	जर (!smg)
		परत SBI_ENOMEM;

	smg->sys_अंक_msi = garesp.sys_अंक_msi;
	smg->sys_msi_denied_bmap = sbi_zalloc(bitmap_estimate_size(smg->sys_अंक_msi));
	जर (!smg->sys_msi_denied_bmap) {
		sbi_free(smg);
		परत SBI_ENOMEM;
	}

	साठी (i = 0; i < smg->sys_अंक_msi; i++) {
		gmareq.sys_msi_index = i;
		rc = rpmi_normal_request_with_status(chan,
						     RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES,
						     &gmareq, rpmi_u32_मोजणी(gmareq),
						     rpmi_u32_मोजणी(gmareq),
						     &gmaresp, rpmi_u32_मोजणी(gmaresp),
						     rpmi_u32_मोजणी(gmaresp));
		जर (rc) {
			mpxy_rpmi_sysmsi_cleanup(smg);
			परत rc;
		}

		जर (p2a_db_index == i ||
		    (gmaresp.flag0 & RPMI_SYSMSI_MSI_ATTRIBUTES_FLAG0_PREF_PRIV))
			bitmap_set(smg->sys_msi_denied_bmap, i, 1);
	}

	*context = smg;
	परत 0;
}

स्थिर संरचना mpxy_rpmi_service_data sysmsi_services[] = {
{
	.id = RPMI_SYSMSI_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = आकार(संरचना rpmi_enable_notification_req),
	.max_tx_len = आकार(संरचना rpmi_enable_notification_req),
	.min_rx_len = आकार(संरचना rpmi_enable_notification_resp),
	.max_rx_len = आकार(संरचना rpmi_enable_notification_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_ATTRIBUTES,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = आकार(संरचना rpmi_sysmsi_get_attributes_resp),
	.max_rx_len = आकार(संरचना rpmi_sysmsi_get_attributes_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES,
	.min_tx_len = आकार(संरचना rpmi_sysmsi_get_msi_attributes_req),
	.max_tx_len = आकार(संरचना rpmi_sysmsi_get_msi_attributes_req),
	.min_rx_len = आकार(संरचना rpmi_sysmsi_get_msi_attributes_resp),
	.max_rx_len = आकार(संरचना rpmi_sysmsi_get_msi_attributes_resp),
},
{
	.id = RPMI_SYSMSI_SRV_SET_MSI_STATE,
	.min_tx_len = आकार(संरचना rpmi_sysmsi_set_msi_state_req),
	.max_tx_len = आकार(संरचना rpmi_sysmsi_set_msi_state_req),
	.min_rx_len = आकार(संरचना rpmi_sysmsi_set_msi_state_resp),
	.max_rx_len = आकार(संरचना rpmi_sysmsi_set_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_STATE,
	.min_tx_len = आकार(संरचना rpmi_sysmsi_get_msi_state_req),
	.max_tx_len = आकार(संरचना rpmi_sysmsi_get_msi_state_req),
	.min_rx_len = आकार(संरचना rpmi_sysmsi_get_msi_state_resp),
	.max_rx_len = आकार(संरचना rpmi_sysmsi_get_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_SET_MSI_TARGET,
	.min_tx_len = आकार(संरचना rpmi_sysmsi_set_msi_target_req),
	.max_tx_len = आकार(संरचना rpmi_sysmsi_set_msi_target_req),
	.min_rx_len = आकार(संरचना rpmi_sysmsi_set_msi_target_resp),
	.max_rx_len = आकार(संरचना rpmi_sysmsi_set_msi_target_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_TARGET,
	.min_tx_len = आकार(संरचना rpmi_sysmsi_get_msi_target_req),
	.max_tx_len = आकार(संरचना rpmi_sysmsi_get_msi_target_req),
	.min_rx_len = आकार(संरचना rpmi_sysmsi_get_msi_target_resp),
	.max_rx_len = आकार(संरचना rpmi_sysmsi_get_msi_target_resp),
},
};

स्थिर स्थिरांक संरचना mpxy_rpmi_mbox_data sysmsi_data = {
	.servicegrp_id = RPMI_SRVGRP_SYSTEM_MSI,
	.अंक_services = RPMI_SYSMSI_SRV_ID_MAX_मोजणी,
	.service_data = sysmsi_services,
	.xfer_group = mpxy_rpmi_sysmis_xfer,
	.setup_group = mpxy_rpmi_sysmsi_setup,
	.cleanup_group = mpxy_rpmi_sysmsi_cleanup,
};

स्थिर स्थिरांक संरचना fdt_match sysmsi_match[] = {
	{ .compatible = "riscv,rpmi-mpxy-system-msi", .data = &sysmsi_data },
	{ },
};

स्थिरांक संरचना fdt_driver fdt_mpxy_rpmi_sysmsi = {
	.match_table = sysmsi_match,
	.init = mpxy_rpmi_mbox_init,
};
