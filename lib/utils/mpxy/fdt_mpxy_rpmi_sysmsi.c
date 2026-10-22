/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#অন্তর্ভাৱ <sbi/sbi_bitmap.h>
#অন্তর্ভাৱ <sbi/sbi_domain.h>
#অন্তর্ভাৱ <sbi/sbi_byteorder.h>
#অন্তর্ভাৱ <sbi/sbi_heap.h>
#অন্তর্ভাৱ <sbi_utils/mailbox/rpmi_mailbox.h>
#অন্তর্ভাৱ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

সংরচনা mpxy_rpmi_sysmsi {
	u32 sys_অঙ্ক_msi;
	unsigned long *sys_msi_denied_bmap;
};

স্থির পূর্ণাঙ্ক mpxy_rpmi_sysmis_xfer(শূন্য *context, সংরচনা mbox_chan *chan,
				 সংরচনা mbox_xfer *xfer)
{
	সংরচনা rpmi_message_args *args = xfer->args;
	সংরচনা mpxy_rpmi_sysmsi *smg = context;
	u64 sys_msi_address;
	u32 sys_msi_index;
	পূর্ণাঙ্ক rc = 0;

	যদি (!xfer->rx || args->type != RPMI_বাৰ্তা_NORMAL_REQUEST)
		ঘূৰাই 0;

	ৱিকল্প (args->service_id) {
	পক্ষ RPMI_SYSMSI_SRV_GET_ATTRIBUTES:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_to_le32(smg->sys_অঙ্ক_msi);
		((u32 *)xfer->rx)[2] = 0;
		((u32 *)xfer->rx)[3] = 0;
		args->rx_data_len = 4 * পরিমাণ(u32);
		ভঙ্গ;
	পক্ষ RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES:
	পক্ষ RPMI_SYSMSI_SRV_SET_MSI_STATE:
	পক্ষ RPMI_SYSMSI_SRV_GET_MSI_STATE:
	পক্ষ RPMI_SYSMSI_SRV_SET_MSI_TARGET:
	পক্ষ RPMI_SYSMSI_SRV_GET_MSI_TARGET:
		sys_msi_index = le32_to_cpu(((u32 *)xfer->tx)[0]);
		যদি (smg->sys_অঙ্ক_msi <= sys_msi_index) {
			((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_INVALID_PARAM);
			args->rx_data_len = পরিমাণ(u32);
			ভঙ্গ;
		}
		যদি (bitmap_test(smg->sys_msi_denied_bmap, sys_msi_index)) {
			((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_DENIED);
			args->rx_data_len = পরিমাণ(u32);
			ভঙ্গ;
		}
		যদি (args->service_id == RPMI_SYSMSI_SRV_SET_MSI_TARGET) {
			sys_msi_address = le32_to_cpu(((u32 *)xfer->tx)[1]);
			sys_msi_address |= ((u64)le32_to_cpu(((u32 *)xfer->tx)[2])) << 32;
			যদি (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
							 sys_msi_address, 0x4, PRV_S,
							 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE |
							 SBI_DOMAIN_MMIO)) {
				((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_INVALID_ADDR);
				args->rx_data_len = পরিমাণ(u32);
				ভঙ্গ;
			}
		}
		rc = mbox_chan_xfer(chan, xfer);
		ভঙ্গ;
	স্ৱভাৱ:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_NOTSUPP);
		args->rx_data_len = পরিমাণ(u32);
		ভঙ্গ;
	};

	ঘূৰাই rc;
}

স্থির শূন্য mpxy_rpmi_sysmsi_cleanup(শূন্য *context)
{
	সংরচনা mpxy_rpmi_sysmsi *smg = context;

	sbi_free(smg->sys_msi_denied_bmap);
	sbi_free(smg);
}

স্থির পূর্ণাঙ্ক mpxy_rpmi_sysmsi_setup(শূন্য **context, সংরচনা mbox_chan *chan,
				  ধ্রুৱ সংরচনা mpxy_rpmi_mbox_data *data)
{
	সংরচনা rpmi_sysmsi_get_msi_attributes_resp gmaresp;
	সংরচনা rpmi_sysmsi_get_msi_attributes_req gmareq;
	সংরচনা rpmi_sysmsi_get_attributes_resp garesp;
	সংরচনা mpxy_rpmi_sysmsi *smg;
	u32 p2a_db_index;
	পূর্ণাঙ্ক rc, i;

	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_P2A_DOORBELL_SYSMSI_INDEX,
				     &p2a_db_index);
	যদি (rc)
		ঘূৰাই rc;

	rc = rpmi_normal_request_with_status(chan, RPMI_SYSMSI_SRV_GET_ATTRIBUTES,
					     রিক্ত, 0, 0, &garesp, rpmi_u32_গুনতি(garesp),
					     rpmi_u32_গুনতি(garesp));
	যদি (rc)
		ঘূৰাই rc;

	smg = sbi_zalloc(পরিমাণ(*smg));
	যদি (!smg)
		ঘূৰাই SBI_ENOMEM;

	smg->sys_অঙ্ক_msi = garesp.sys_অঙ্ক_msi;
	smg->sys_msi_denied_bmap = sbi_zalloc(bitmap_estimate_size(smg->sys_অঙ্ক_msi));
	যদি (!smg->sys_msi_denied_bmap) {
		sbi_free(smg);
		ঘূৰাই SBI_ENOMEM;
	}

	বাবে (i = 0; i < smg->sys_অঙ্ক_msi; i++) {
		gmareq.sys_msi_index = i;
		rc = rpmi_normal_request_with_status(chan,
						     RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES,
						     &gmareq, rpmi_u32_গুনতি(gmareq),
						     rpmi_u32_গুনতি(gmareq),
						     &gmaresp, rpmi_u32_গুনতি(gmaresp),
						     rpmi_u32_গুনতি(gmaresp));
		যদি (rc) {
			mpxy_rpmi_sysmsi_cleanup(smg);
			ঘূৰাই rc;
		}

		যদি (p2a_db_index == i ||
		    (gmaresp.flag0 & RPMI_SYSMSI_MSI_ATTRIBUTES_FLAG0_PREF_PRIV))
			bitmap_set(smg->sys_msi_denied_bmap, i, 1);
	}

	*context = smg;
	ঘূৰাই 0;
}

স্থির সংরচনা mpxy_rpmi_service_data sysmsi_services[] = {
{
	.id = RPMI_SYSMSI_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = পরিমাণ(সংরচনা rpmi_enable_notification_req),
	.max_tx_len = পরিমাণ(সংরচনা rpmi_enable_notification_req),
	.min_rx_len = পরিমাণ(সংরচনা rpmi_enable_notification_resp),
	.max_rx_len = পরিমাণ(সংরচনা rpmi_enable_notification_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_ATTRIBUTES,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = পরিমাণ(সংরচনা rpmi_sysmsi_get_attributes_resp),
	.max_rx_len = পরিমাণ(সংরচনা rpmi_sysmsi_get_attributes_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES,
	.min_tx_len = পরিমাণ(সংরচনা rpmi_sysmsi_get_msi_attributes_req),
	.max_tx_len = পরিমাণ(সংরচনা rpmi_sysmsi_get_msi_attributes_req),
	.min_rx_len = পরিমাণ(সংরচনা rpmi_sysmsi_get_msi_attributes_resp),
	.max_rx_len = পরিমাণ(সংরচনা rpmi_sysmsi_get_msi_attributes_resp),
},
{
	.id = RPMI_SYSMSI_SRV_SET_MSI_STATE,
	.min_tx_len = পরিমাণ(সংরচনা rpmi_sysmsi_set_msi_state_req),
	.max_tx_len = পরিমাণ(সংরচনা rpmi_sysmsi_set_msi_state_req),
	.min_rx_len = পরিমাণ(সংরচনা rpmi_sysmsi_set_msi_state_resp),
	.max_rx_len = পরিমাণ(সংরচনা rpmi_sysmsi_set_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_STATE,
	.min_tx_len = পরিমাণ(সংরচনা rpmi_sysmsi_get_msi_state_req),
	.max_tx_len = পরিমাণ(সংরচনা rpmi_sysmsi_get_msi_state_req),
	.min_rx_len = পরিমাণ(সংরচনা rpmi_sysmsi_get_msi_state_resp),
	.max_rx_len = পরিমাণ(সংরচনা rpmi_sysmsi_get_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_SET_MSI_TARGET,
	.min_tx_len = পরিমাণ(সংরচনা rpmi_sysmsi_set_msi_target_req),
	.max_tx_len = পরিমাণ(সংরচনা rpmi_sysmsi_set_msi_target_req),
	.min_rx_len = পরিমাণ(সংরচনা rpmi_sysmsi_set_msi_target_resp),
	.max_rx_len = পরিমাণ(সংরচনা rpmi_sysmsi_set_msi_target_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_TARGET,
	.min_tx_len = পরিমাণ(সংরচনা rpmi_sysmsi_get_msi_target_req),
	.max_tx_len = পরিমাণ(সংরচনা rpmi_sysmsi_get_msi_target_req),
	.min_rx_len = পরিমাণ(সংরচনা rpmi_sysmsi_get_msi_target_resp),
	.max_rx_len = পরিমাণ(সংরচনা rpmi_sysmsi_get_msi_target_resp),
},
};

স্থির ধ্রুৱ সংরচনা mpxy_rpmi_mbox_data sysmsi_data = {
	.servicegrp_id = RPMI_SRVGRP_SYSTEM_MSI,
	.অঙ্ক_services = RPMI_SYSMSI_SRV_ID_MAX_গুনতি,
	.service_data = sysmsi_services,
	.xfer_group = mpxy_rpmi_sysmis_xfer,
	.setup_group = mpxy_rpmi_sysmsi_setup,
	.cleanup_group = mpxy_rpmi_sysmsi_cleanup,
};

স্থির ধ্রুৱ সংরচনা fdt_match sysmsi_match[] = {
	{ .compatible = "riscv,rpmi-mpxy-system-msi", .data = &sysmsi_data },
	{ },
};

ধ্রুৱ সংরচনা fdt_driver fdt_mpxy_rpmi_sysmsi = {
	.match_table = sysmsi_match,
	.init = mpxy_rpmi_mbox_init,
};
