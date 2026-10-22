/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#അന്തര്ഭാവ <sbi/sbi_bitmap.h>
#അന്തര്ഭാവ <sbi/sbi_domain.h>
#അന്തര്ഭാവ <sbi/sbi_byteorder.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi_utils/mailbox/rpmi_mailbox.h>
#അന്തര്ഭാവ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ഘടന mpxy_rpmi_sysmsi {
	u32 sys_അക്കം_msi;
	unsigned long *sys_msi_denied_bmap;
};

സ്ഥിതം പൂർണ്ണസംഖ്യ mpxy_rpmi_sysmis_xfer(ശൂന്യം *context, ഘടന mbox_chan *chan,
				 ഘടന mbox_xfer *xfer)
{
	ഘടന rpmi_message_args *args = xfer->args;
	ഘടന mpxy_rpmi_sysmsi *smg = context;
	u64 sys_msi_address;
	u32 sys_msi_index;
	പൂർണ്ണസംഖ്യ rc = 0;

	എങ്കിൽ (!xfer->rx || args->type != RPMI_സന്ദേശം_NORMAL_REQUEST)
		തിരികെ 0;

	തിരഞ്ഞെടുപ്പ് (args->service_id) {
	അവസ്ഥ RPMI_SYSMSI_SRV_GET_ATTRIBUTES:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_to_le32(smg->sys_അക്കം_msi);
		((u32 *)xfer->rx)[2] = 0;
		((u32 *)xfer->rx)[3] = 0;
		args->rx_data_len = 4 * വലുപ്പം(u32);
		നിർത്തുക;
	അവസ്ഥ RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES:
	അവസ്ഥ RPMI_SYSMSI_SRV_SET_MSI_STATE:
	അവസ്ഥ RPMI_SYSMSI_SRV_GET_MSI_STATE:
	അവസ്ഥ RPMI_SYSMSI_SRV_SET_MSI_TARGET:
	അവസ്ഥ RPMI_SYSMSI_SRV_GET_MSI_TARGET:
		sys_msi_index = le32_to_cpu(((u32 *)xfer->tx)[0]);
		എങ്കിൽ (smg->sys_അക്കം_msi <= sys_msi_index) {
			((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_INVALID_PARAM);
			args->rx_data_len = വലുപ്പം(u32);
			നിർത്തുക;
		}
		എങ്കിൽ (bitmap_test(smg->sys_msi_denied_bmap, sys_msi_index)) {
			((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_DENIED);
			args->rx_data_len = വലുപ്പം(u32);
			നിർത്തുക;
		}
		എങ്കിൽ (args->service_id == RPMI_SYSMSI_SRV_SET_MSI_TARGET) {
			sys_msi_address = le32_to_cpu(((u32 *)xfer->tx)[1]);
			sys_msi_address |= ((u64)le32_to_cpu(((u32 *)xfer->tx)[2])) << 32;
			എങ്കിൽ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
							 sys_msi_address, 0x4, PRV_S,
							 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE |
							 SBI_DOMAIN_MMIO)) {
				((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_INVALID_ADDR);
				args->rx_data_len = വലുപ്പം(u32);
				നിർത്തുക;
			}
		}
		rc = mbox_chan_xfer(chan, xfer);
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_NOTSUPP);
		args->rx_data_len = വലുപ്പം(u32);
		നിർത്തുക;
	};

	തിരികെ rc;
}

സ്ഥിതം ശൂന്യം mpxy_rpmi_sysmsi_cleanup(ശൂന്യം *context)
{
	ഘടന mpxy_rpmi_sysmsi *smg = context;

	sbi_free(smg->sys_msi_denied_bmap);
	sbi_free(smg);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ mpxy_rpmi_sysmsi_setup(ശൂന്യം **context, ഘടന mbox_chan *chan,
				  സ്ഥിരം ഘടന mpxy_rpmi_mbox_data *data)
{
	ഘടന rpmi_sysmsi_get_msi_attributes_resp gmaresp;
	ഘടന rpmi_sysmsi_get_msi_attributes_req gmareq;
	ഘടന rpmi_sysmsi_get_attributes_resp garesp;
	ഘടന mpxy_rpmi_sysmsi *smg;
	u32 p2a_db_index;
	പൂർണ്ണസംഖ്യ rc, i;

	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_P2A_DOORBELL_SYSMSI_INDEX,
				     &p2a_db_index);
	എങ്കിൽ (rc)
		തിരികെ rc;

	rc = rpmi_normal_request_with_status(chan, RPMI_SYSMSI_SRV_GET_ATTRIBUTES,
					     ശൂന്യ, 0, 0, &garesp, rpmi_u32_എണ്ണം(garesp),
					     rpmi_u32_എണ്ണം(garesp));
	എങ്കിൽ (rc)
		തിരികെ rc;

	smg = sbi_zalloc(വലുപ്പം(*smg));
	എങ്കിൽ (!smg)
		തിരികെ SBI_ENOMEM;

	smg->sys_അക്കം_msi = garesp.sys_അക്കം_msi;
	smg->sys_msi_denied_bmap = sbi_zalloc(bitmap_estimate_size(smg->sys_അക്കം_msi));
	എങ്കിൽ (!smg->sys_msi_denied_bmap) {
		sbi_free(smg);
		തിരികെ SBI_ENOMEM;
	}

	വേണ്ടി (i = 0; i < smg->sys_അക്കം_msi; i++) {
		gmareq.sys_msi_index = i;
		rc = rpmi_normal_request_with_status(chan,
						     RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES,
						     &gmareq, rpmi_u32_എണ്ണം(gmareq),
						     rpmi_u32_എണ്ണം(gmareq),
						     &gmaresp, rpmi_u32_എണ്ണം(gmaresp),
						     rpmi_u32_എണ്ണം(gmaresp));
		എങ്കിൽ (rc) {
			mpxy_rpmi_sysmsi_cleanup(smg);
			തിരികെ rc;
		}

		എങ്കിൽ (p2a_db_index == i ||
		    (gmaresp.flag0 & RPMI_SYSMSI_MSI_ATTRIBUTES_FLAG0_PREF_PRIV))
			bitmap_set(smg->sys_msi_denied_bmap, i, 1);
	}

	*context = smg;
	തിരികെ 0;
}

സ്ഥിതം ഘടന mpxy_rpmi_service_data sysmsi_services[] = {
{
	.id = RPMI_SYSMSI_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = വലുപ്പം(ഘടന rpmi_enable_notification_req),
	.max_tx_len = വലുപ്പം(ഘടന rpmi_enable_notification_req),
	.min_rx_len = വലുപ്പം(ഘടന rpmi_enable_notification_resp),
	.max_rx_len = വലുപ്പം(ഘടന rpmi_enable_notification_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_ATTRIBUTES,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = വലുപ്പം(ഘടന rpmi_sysmsi_get_attributes_resp),
	.max_rx_len = വലുപ്പം(ഘടന rpmi_sysmsi_get_attributes_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES,
	.min_tx_len = വലുപ്പം(ഘടന rpmi_sysmsi_get_msi_attributes_req),
	.max_tx_len = വലുപ്പം(ഘടന rpmi_sysmsi_get_msi_attributes_req),
	.min_rx_len = വലുപ്പം(ഘടന rpmi_sysmsi_get_msi_attributes_resp),
	.max_rx_len = വലുപ്പം(ഘടന rpmi_sysmsi_get_msi_attributes_resp),
},
{
	.id = RPMI_SYSMSI_SRV_SET_MSI_STATE,
	.min_tx_len = വലുപ്പം(ഘടന rpmi_sysmsi_set_msi_state_req),
	.max_tx_len = വലുപ്പം(ഘടന rpmi_sysmsi_set_msi_state_req),
	.min_rx_len = വലുപ്പം(ഘടന rpmi_sysmsi_set_msi_state_resp),
	.max_rx_len = വലുപ്പം(ഘടന rpmi_sysmsi_set_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_STATE,
	.min_tx_len = വലുപ്പം(ഘടന rpmi_sysmsi_get_msi_state_req),
	.max_tx_len = വലുപ്പം(ഘടന rpmi_sysmsi_get_msi_state_req),
	.min_rx_len = വലുപ്പം(ഘടന rpmi_sysmsi_get_msi_state_resp),
	.max_rx_len = വലുപ്പം(ഘടന rpmi_sysmsi_get_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_SET_MSI_TARGET,
	.min_tx_len = വലുപ്പം(ഘടന rpmi_sysmsi_set_msi_target_req),
	.max_tx_len = വലുപ്പം(ഘടന rpmi_sysmsi_set_msi_target_req),
	.min_rx_len = വലുപ്പം(ഘടന rpmi_sysmsi_set_msi_target_resp),
	.max_rx_len = വലുപ്പം(ഘടന rpmi_sysmsi_set_msi_target_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_TARGET,
	.min_tx_len = വലുപ്പം(ഘടന rpmi_sysmsi_get_msi_target_req),
	.max_tx_len = വലുപ്പം(ഘടന rpmi_sysmsi_get_msi_target_req),
	.min_rx_len = വലുപ്പം(ഘടന rpmi_sysmsi_get_msi_target_resp),
	.max_rx_len = വലുപ്പം(ഘടന rpmi_sysmsi_get_msi_target_resp),
},
};

സ്ഥിതം സ്ഥിരം ഘടന mpxy_rpmi_mbox_data sysmsi_data = {
	.servicegrp_id = RPMI_SRVGRP_SYSTEM_MSI,
	.അക്കം_services = RPMI_SYSMSI_SRV_ID_MAX_എണ്ണം,
	.service_data = sysmsi_services,
	.xfer_group = mpxy_rpmi_sysmis_xfer,
	.setup_group = mpxy_rpmi_sysmsi_setup,
	.cleanup_group = mpxy_rpmi_sysmsi_cleanup,
};

സ്ഥിതം സ്ഥിരം ഘടന fdt_match sysmsi_match[] = {
	{ .compatible = "riscv,rpmi-mpxy-system-msi", .data = &sysmsi_data },
	{ },
};

സ്ഥിരം ഘടന fdt_driver fdt_mpxy_rpmi_sysmsi = {
	.match_table = sysmsi_match,
	.init = mpxy_rpmi_mbox_init,
};
