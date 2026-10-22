/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#અન્તર્ભાવ <sbi/sbi_bitmap.h>
#અન્તર્ભાવ <sbi/sbi_domain.h>
#અન્તર્ભાવ <sbi/sbi_byteorder.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi_utils/mailbox/rpmi_mailbox.h>
#અન્તર્ભાવ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

સંરચના mpxy_rpmi_sysmsi {
	u32 sys_અંક_msi;
	unsigned long *sys_msi_denied_bmap;
};

સ્થિર પૂર્ણાંક mpxy_rpmi_sysmis_xfer(શૂન્ય *context, સંરચના mbox_chan *chan,
				 સંરચના mbox_xfer *xfer)
{
	સંરચના rpmi_message_args *args = xfer->args;
	સંરચના mpxy_rpmi_sysmsi *smg = context;
	u64 sys_msi_address;
	u32 sys_msi_index;
	પૂર્ણાંક rc = 0;

	જો (!xfer->rx || args->type != RPMI_સંદેશ_NORMAL_REQUEST)
		પાછા 0;

	પસંદગી (args->service_id) {
	સ્થિતિ RPMI_SYSMSI_SRV_GET_ATTRIBUTES:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_to_le32(smg->sys_અંક_msi);
		((u32 *)xfer->rx)[2] = 0;
		((u32 *)xfer->rx)[3] = 0;
		args->rx_data_len = 4 * કદ(u32);
		તોડો;
	સ્થિતિ RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES:
	સ્થિતિ RPMI_SYSMSI_SRV_SET_MSI_STATE:
	સ્થિતિ RPMI_SYSMSI_SRV_GET_MSI_STATE:
	સ્થિતિ RPMI_SYSMSI_SRV_SET_MSI_TARGET:
	સ્થિતિ RPMI_SYSMSI_SRV_GET_MSI_TARGET:
		sys_msi_index = le32_to_cpu(((u32 *)xfer->tx)[0]);
		જો (smg->sys_અંક_msi <= sys_msi_index) {
			((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_INVALID_PARAM);
			args->rx_data_len = કદ(u32);
			તોડો;
		}
		જો (bitmap_test(smg->sys_msi_denied_bmap, sys_msi_index)) {
			((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_DENIED);
			args->rx_data_len = કદ(u32);
			તોડો;
		}
		જો (args->service_id == RPMI_SYSMSI_SRV_SET_MSI_TARGET) {
			sys_msi_address = le32_to_cpu(((u32 *)xfer->tx)[1]);
			sys_msi_address |= ((u64)le32_to_cpu(((u32 *)xfer->tx)[2])) << 32;
			જો (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
							 sys_msi_address, 0x4, PRV_S,
							 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE |
							 SBI_DOMAIN_MMIO)) {
				((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_INVALID_ADDR);
				args->rx_data_len = કદ(u32);
				તોડો;
			}
		}
		rc = mbox_chan_xfer(chan, xfer);
		તોડો;
	મૂળભૂત:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_NOTSUPP);
		args->rx_data_len = કદ(u32);
		તોડો;
	};

	પાછા rc;
}

સ્થિર શૂન્ય mpxy_rpmi_sysmsi_cleanup(શૂન્ય *context)
{
	સંરચના mpxy_rpmi_sysmsi *smg = context;

	sbi_free(smg->sys_msi_denied_bmap);
	sbi_free(smg);
}

સ્થિર પૂર્ણાંક mpxy_rpmi_sysmsi_setup(શૂન્ય **context, સંરચના mbox_chan *chan,
				  અચલ સંરચના mpxy_rpmi_mbox_data *data)
{
	સંરચના rpmi_sysmsi_get_msi_attributes_resp gmaresp;
	સંરચના rpmi_sysmsi_get_msi_attributes_req gmareq;
	સંરચના rpmi_sysmsi_get_attributes_resp garesp;
	સંરચના mpxy_rpmi_sysmsi *smg;
	u32 p2a_db_index;
	પૂર્ણાંક rc, i;

	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_P2A_DOORBELL_SYSMSI_INDEX,
				     &p2a_db_index);
	જો (rc)
		પાછા rc;

	rc = rpmi_normal_request_with_status(chan, RPMI_SYSMSI_SRV_GET_ATTRIBUTES,
					     ખાલી, 0, 0, &garesp, rpmi_u32_ગણતરી(garesp),
					     rpmi_u32_ગણતરી(garesp));
	જો (rc)
		પાછા rc;

	smg = sbi_zalloc(કદ(*smg));
	જો (!smg)
		પાછા SBI_ENOMEM;

	smg->sys_અંક_msi = garesp.sys_અંક_msi;
	smg->sys_msi_denied_bmap = sbi_zalloc(bitmap_estimate_size(smg->sys_અંક_msi));
	જો (!smg->sys_msi_denied_bmap) {
		sbi_free(smg);
		પાછા SBI_ENOMEM;
	}

	માટે (i = 0; i < smg->sys_અંક_msi; i++) {
		gmareq.sys_msi_index = i;
		rc = rpmi_normal_request_with_status(chan,
						     RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES,
						     &gmareq, rpmi_u32_ગણતરી(gmareq),
						     rpmi_u32_ગણતરી(gmareq),
						     &gmaresp, rpmi_u32_ગણતરી(gmaresp),
						     rpmi_u32_ગણતરી(gmaresp));
		જો (rc) {
			mpxy_rpmi_sysmsi_cleanup(smg);
			પાછા rc;
		}

		જો (p2a_db_index == i ||
		    (gmaresp.flag0 & RPMI_SYSMSI_MSI_ATTRIBUTES_FLAG0_PREF_PRIV))
			bitmap_set(smg->sys_msi_denied_bmap, i, 1);
	}

	*context = smg;
	પાછા 0;
}

સ્થિર સંરચના mpxy_rpmi_service_data sysmsi_services[] = {
{
	.id = RPMI_SYSMSI_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = કદ(સંરચના rpmi_enable_notification_req),
	.max_tx_len = કદ(સંરચના rpmi_enable_notification_req),
	.min_rx_len = કદ(સંરચના rpmi_enable_notification_resp),
	.max_rx_len = કદ(સંરચના rpmi_enable_notification_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_ATTRIBUTES,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = કદ(સંરચના rpmi_sysmsi_get_attributes_resp),
	.max_rx_len = કદ(સંરચના rpmi_sysmsi_get_attributes_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES,
	.min_tx_len = કદ(સંરચના rpmi_sysmsi_get_msi_attributes_req),
	.max_tx_len = કદ(સંરચના rpmi_sysmsi_get_msi_attributes_req),
	.min_rx_len = કદ(સંરચના rpmi_sysmsi_get_msi_attributes_resp),
	.max_rx_len = કદ(સંરચના rpmi_sysmsi_get_msi_attributes_resp),
},
{
	.id = RPMI_SYSMSI_SRV_SET_MSI_STATE,
	.min_tx_len = કદ(સંરચના rpmi_sysmsi_set_msi_state_req),
	.max_tx_len = કદ(સંરચના rpmi_sysmsi_set_msi_state_req),
	.min_rx_len = કદ(સંરચના rpmi_sysmsi_set_msi_state_resp),
	.max_rx_len = કદ(સંરચના rpmi_sysmsi_set_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_STATE,
	.min_tx_len = કદ(સંરચના rpmi_sysmsi_get_msi_state_req),
	.max_tx_len = કદ(સંરચના rpmi_sysmsi_get_msi_state_req),
	.min_rx_len = કદ(સંરચના rpmi_sysmsi_get_msi_state_resp),
	.max_rx_len = કદ(સંરચના rpmi_sysmsi_get_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_SET_MSI_TARGET,
	.min_tx_len = કદ(સંરચના rpmi_sysmsi_set_msi_target_req),
	.max_tx_len = કદ(સંરચના rpmi_sysmsi_set_msi_target_req),
	.min_rx_len = કદ(સંરચના rpmi_sysmsi_set_msi_target_resp),
	.max_rx_len = કદ(સંરચના rpmi_sysmsi_set_msi_target_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_TARGET,
	.min_tx_len = કદ(સંરચના rpmi_sysmsi_get_msi_target_req),
	.max_tx_len = કદ(સંરચના rpmi_sysmsi_get_msi_target_req),
	.min_rx_len = કદ(સંરચના rpmi_sysmsi_get_msi_target_resp),
	.max_rx_len = કદ(સંરચના rpmi_sysmsi_get_msi_target_resp),
},
};

સ્થિર અચલ સંરચના mpxy_rpmi_mbox_data sysmsi_data = {
	.servicegrp_id = RPMI_SRVGRP_SYSTEM_MSI,
	.અંક_services = RPMI_SYSMSI_SRV_ID_MAX_ગણતરી,
	.service_data = sysmsi_services,
	.xfer_group = mpxy_rpmi_sysmis_xfer,
	.setup_group = mpxy_rpmi_sysmsi_setup,
	.cleanup_group = mpxy_rpmi_sysmsi_cleanup,
};

સ્થિર અચલ સંરચના fdt_match sysmsi_match[] = {
	{ .compatible = "riscv,rpmi-mpxy-system-msi", .data = &sysmsi_data },
	{ },
};

અચલ સંરચના fdt_driver fdt_mpxy_rpmi_sysmsi = {
	.match_table = sysmsi_match,
	.init = mpxy_rpmi_mbox_init,
};
