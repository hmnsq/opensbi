/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#அந்தர்பாவ <sbi/sbi_bitmap.h>
#அந்தர்பாவ <sbi/sbi_domain.h>
#அந்தர்பாவ <sbi/sbi_byteorder.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi_utils/mailbox/rpmi_mailbox.h>
#அந்தர்பாவ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

கட்டமைப்பு mpxy_rpmi_sysmsi {
	u32 sys_இலக்கம்_msi;
	unsigned long *sys_msi_denied_bmap;
};

நிலையான முழு mpxy_rpmi_sysmis_xfer(வெறுமை *context, கட்டமைப்பு mbox_chan *chan,
				 கட்டமைப்பு mbox_xfer *xfer)
{
	கட்டமைப்பு rpmi_message_args *args = xfer->args;
	கட்டமைப்பு mpxy_rpmi_sysmsi *smg = context;
	u64 sys_msi_address;
	u32 sys_msi_index;
	முழு rc = 0;

	எனில் (!xfer->rx || args->type != RPMI_செய்தி_NORMAL_REQUEST)
		திரும்பு 0;

	தேர்வு (args->service_id) {
	நிலை RPMI_SYSMSI_SRV_GET_ATTRIBUTES:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_to_le32(smg->sys_இலக்கம்_msi);
		((u32 *)xfer->rx)[2] = 0;
		((u32 *)xfer->rx)[3] = 0;
		args->rx_data_len = 4 * அளவு(u32);
		நிறுத்து;
	நிலை RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES:
	நிலை RPMI_SYSMSI_SRV_SET_MSI_STATE:
	நிலை RPMI_SYSMSI_SRV_GET_MSI_STATE:
	நிலை RPMI_SYSMSI_SRV_SET_MSI_TARGET:
	நிலை RPMI_SYSMSI_SRV_GET_MSI_TARGET:
		sys_msi_index = le32_to_cpu(((u32 *)xfer->tx)[0]);
		எனில் (smg->sys_இலக்கம்_msi <= sys_msi_index) {
			((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_INVALID_PARAM);
			args->rx_data_len = அளவு(u32);
			நிறுத்து;
		}
		எனில் (bitmap_test(smg->sys_msi_denied_bmap, sys_msi_index)) {
			((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_DENIED);
			args->rx_data_len = அளவு(u32);
			நிறுத்து;
		}
		எனில் (args->service_id == RPMI_SYSMSI_SRV_SET_MSI_TARGET) {
			sys_msi_address = le32_to_cpu(((u32 *)xfer->tx)[1]);
			sys_msi_address |= ((u64)le32_to_cpu(((u32 *)xfer->tx)[2])) << 32;
			எனில் (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
							 sys_msi_address, 0x4, PRV_S,
							 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE |
							 SBI_DOMAIN_MMIO)) {
				((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_INVALID_ADDR);
				args->rx_data_len = அளவு(u32);
				நிறுத்து;
			}
		}
		rc = mbox_chan_xfer(chan, xfer);
		நிறுத்து;
	இயல்பு:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_NOTSUPP);
		args->rx_data_len = அளவு(u32);
		நிறுத்து;
	};

	திரும்பு rc;
}

நிலையான வெறுமை mpxy_rpmi_sysmsi_cleanup(வெறுமை *context)
{
	கட்டமைப்பு mpxy_rpmi_sysmsi *smg = context;

	sbi_free(smg->sys_msi_denied_bmap);
	sbi_free(smg);
}

நிலையான முழு mpxy_rpmi_sysmsi_setup(வெறுமை **context, கட்டமைப்பு mbox_chan *chan,
				  மாறிலி கட்டமைப்பு mpxy_rpmi_mbox_data *data)
{
	கட்டமைப்பு rpmi_sysmsi_get_msi_attributes_resp gmaresp;
	கட்டமைப்பு rpmi_sysmsi_get_msi_attributes_req gmareq;
	கட்டமைப்பு rpmi_sysmsi_get_attributes_resp garesp;
	கட்டமைப்பு mpxy_rpmi_sysmsi *smg;
	u32 p2a_db_index;
	முழு rc, i;

	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_P2A_DOORBELL_SYSMSI_INDEX,
				     &p2a_db_index);
	எனில் (rc)
		திரும்பு rc;

	rc = rpmi_normal_request_with_status(chan, RPMI_SYSMSI_SRV_GET_ATTRIBUTES,
					     வெற்று, 0, 0, &garesp, rpmi_u32_மொத்தம்(garesp),
					     rpmi_u32_மொத்தம்(garesp));
	எனில் (rc)
		திரும்பு rc;

	smg = sbi_zalloc(அளவு(*smg));
	எனில் (!smg)
		திரும்பு SBI_ENOMEM;

	smg->sys_இலக்கம்_msi = garesp.sys_இலக்கம்_msi;
	smg->sys_msi_denied_bmap = sbi_zalloc(bitmap_estimate_size(smg->sys_இலக்கம்_msi));
	எனில் (!smg->sys_msi_denied_bmap) {
		sbi_free(smg);
		திரும்பு SBI_ENOMEM;
	}

	க்கு (i = 0; i < smg->sys_இலக்கம்_msi; i++) {
		gmareq.sys_msi_index = i;
		rc = rpmi_normal_request_with_status(chan,
						     RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES,
						     &gmareq, rpmi_u32_மொத்தம்(gmareq),
						     rpmi_u32_மொத்தம்(gmareq),
						     &gmaresp, rpmi_u32_மொத்தம்(gmaresp),
						     rpmi_u32_மொத்தம்(gmaresp));
		எனில் (rc) {
			mpxy_rpmi_sysmsi_cleanup(smg);
			திரும்பு rc;
		}

		எனில் (p2a_db_index == i ||
		    (gmaresp.flag0 & RPMI_SYSMSI_MSI_ATTRIBUTES_FLAG0_PREF_PRIV))
			bitmap_set(smg->sys_msi_denied_bmap, i, 1);
	}

	*context = smg;
	திரும்பு 0;
}

நிலையான கட்டமைப்பு mpxy_rpmi_service_data sysmsi_services[] = {
{
	.id = RPMI_SYSMSI_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = அளவு(கட்டமைப்பு rpmi_enable_notification_req),
	.max_tx_len = அளவு(கட்டமைப்பு rpmi_enable_notification_req),
	.min_rx_len = அளவு(கட்டமைப்பு rpmi_enable_notification_resp),
	.max_rx_len = அளவு(கட்டமைப்பு rpmi_enable_notification_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_ATTRIBUTES,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = அளவு(கட்டமைப்பு rpmi_sysmsi_get_attributes_resp),
	.max_rx_len = அளவு(கட்டமைப்பு rpmi_sysmsi_get_attributes_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES,
	.min_tx_len = அளவு(கட்டமைப்பு rpmi_sysmsi_get_msi_attributes_req),
	.max_tx_len = அளவு(கட்டமைப்பு rpmi_sysmsi_get_msi_attributes_req),
	.min_rx_len = அளவு(கட்டமைப்பு rpmi_sysmsi_get_msi_attributes_resp),
	.max_rx_len = அளவு(கட்டமைப்பு rpmi_sysmsi_get_msi_attributes_resp),
},
{
	.id = RPMI_SYSMSI_SRV_SET_MSI_STATE,
	.min_tx_len = அளவு(கட்டமைப்பு rpmi_sysmsi_set_msi_state_req),
	.max_tx_len = அளவு(கட்டமைப்பு rpmi_sysmsi_set_msi_state_req),
	.min_rx_len = அளவு(கட்டமைப்பு rpmi_sysmsi_set_msi_state_resp),
	.max_rx_len = அளவு(கட்டமைப்பு rpmi_sysmsi_set_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_STATE,
	.min_tx_len = அளவு(கட்டமைப்பு rpmi_sysmsi_get_msi_state_req),
	.max_tx_len = அளவு(கட்டமைப்பு rpmi_sysmsi_get_msi_state_req),
	.min_rx_len = அளவு(கட்டமைப்பு rpmi_sysmsi_get_msi_state_resp),
	.max_rx_len = அளவு(கட்டமைப்பு rpmi_sysmsi_get_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_SET_MSI_TARGET,
	.min_tx_len = அளவு(கட்டமைப்பு rpmi_sysmsi_set_msi_target_req),
	.max_tx_len = அளவு(கட்டமைப்பு rpmi_sysmsi_set_msi_target_req),
	.min_rx_len = அளவு(கட்டமைப்பு rpmi_sysmsi_set_msi_target_resp),
	.max_rx_len = அளவு(கட்டமைப்பு rpmi_sysmsi_set_msi_target_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_TARGET,
	.min_tx_len = அளவு(கட்டமைப்பு rpmi_sysmsi_get_msi_target_req),
	.max_tx_len = அளவு(கட்டமைப்பு rpmi_sysmsi_get_msi_target_req),
	.min_rx_len = அளவு(கட்டமைப்பு rpmi_sysmsi_get_msi_target_resp),
	.max_rx_len = அளவு(கட்டமைப்பு rpmi_sysmsi_get_msi_target_resp),
},
};

நிலையான மாறிலி கட்டமைப்பு mpxy_rpmi_mbox_data sysmsi_data = {
	.servicegrp_id = RPMI_SRVGRP_SYSTEM_MSI,
	.இலக்கம்_services = RPMI_SYSMSI_SRV_ID_MAX_மொத்தம்,
	.service_data = sysmsi_services,
	.xfer_group = mpxy_rpmi_sysmis_xfer,
	.setup_group = mpxy_rpmi_sysmsi_setup,
	.cleanup_group = mpxy_rpmi_sysmsi_cleanup,
};

நிலையான மாறிலி கட்டமைப்பு fdt_match sysmsi_match[] = {
	{ .compatible = "riscv,rpmi-mpxy-system-msi", .data = &sysmsi_data },
	{ },
};

மாறிலி கட்டமைப்பு fdt_driver fdt_mpxy_rpmi_sysmsi = {
	.match_table = sysmsi_match,
	.init = mpxy_rpmi_mbox_init,
};
