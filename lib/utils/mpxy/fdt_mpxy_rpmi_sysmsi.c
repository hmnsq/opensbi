/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#অন্তর্ভাব <sbi/sbi_bitmap.h>
#অন্তর্ভাব <sbi/sbi_domain.h>
#অন্তর্ভাব <sbi/sbi_byteorder.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi_utils/mailbox/rpmi_mailbox.h>
#অন্তর্ভাব <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

কাঠামো mpxy_rpmi_sysmsi {
	u32 sys_অঙ্ক_msi;
	unsigned long *sys_msi_denied_bmap;
};

স্থির পূর্ণসংখ্যা mpxy_rpmi_sysmis_xfer(শূন্য *context, কাঠামো mbox_chan *chan,
				 কাঠামো mbox_xfer *xfer)
{
	কাঠামো rpmi_message_args *args = xfer->args;
	কাঠামো mpxy_rpmi_sysmsi *smg = context;
	u64 sys_msi_address;
	u32 sys_msi_সূচক;
	পূর্ণসংখ্যা rc = 0;

	যদি (!xfer->rx || args->type != RPMI_বার্তা_NORMAL_REQUEST)
		ফেরত 0;

	নির্বাচন (args->service_id) {
	ক্ষেত্র RPMI_SYSMSI_SRV_GET_ATTRIBUTES:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_to_le32(smg->sys_অঙ্ক_msi);
		((u32 *)xfer->rx)[2] = 0;
		((u32 *)xfer->rx)[3] = 0;
		args->rx_তথ্য_len = 4 * আকার(u32);
		বিরতি;
	ক্ষেত্র RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES:
	ক্ষেত্র RPMI_SYSMSI_SRV_SET_MSI_STATE:
	ক্ষেত্র RPMI_SYSMSI_SRV_GET_MSI_STATE:
	ক্ষেত্র RPMI_SYSMSI_SRV_SET_MSI_TARGET:
	ক্ষেত্র RPMI_SYSMSI_SRV_GET_MSI_TARGET:
		sys_msi_সূচক = le32_to_cpu(((u32 *)xfer->tx)[0]);
		যদি (smg->sys_অঙ্ক_msi <= sys_msi_সূচক) {
			((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_INVALID_PARAM);
			args->rx_তথ্য_len = আকার(u32);
			বিরতি;
		}
		যদি (bitmap_test(smg->sys_msi_denied_bmap, sys_msi_সূচক)) {
			((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_DENIED);
			args->rx_তথ্য_len = আকার(u32);
			বিরতি;
		}
		যদি (args->service_id == RPMI_SYSMSI_SRV_SET_MSI_TARGET) {
			sys_msi_address = le32_to_cpu(((u32 *)xfer->tx)[1]);
			sys_msi_address |= ((u64)le32_to_cpu(((u32 *)xfer->tx)[2])) << 32;
			যদি (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
							 sys_msi_address, 0x4, PRV_S,
							 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE |
							 SBI_DOMAIN_MMIO)) {
				((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_INVALID_ADDR);
				args->rx_তথ্য_len = আকার(u32);
				বিরতি;
			}
		}
		rc = mbox_chan_xfer(chan, xfer);
		বিরতি;
	পূর্বনির্ধারিত:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_NOTSUPP);
		args->rx_তথ্য_len = আকার(u32);
		বিরতি;
	};

	ফেরত rc;
}

স্থির শূন্য mpxy_rpmi_sysmsi_cleanup(শূন্য *context)
{
	কাঠামো mpxy_rpmi_sysmsi *smg = context;

	sbi_free(smg->sys_msi_denied_bmap);
	sbi_free(smg);
}

স্থির পূর্ণসংখ্যা mpxy_rpmi_sysmsi_setup(শূন্য **context, কাঠামো mbox_chan *chan,
				  ধ্রুবক কাঠামো mpxy_rpmi_mbox_তথ্য *তথ্য)
{
	কাঠামো rpmi_sysmsi_get_msi_attributes_resp gmaresp;
	কাঠামো rpmi_sysmsi_get_msi_attributes_req gmareq;
	কাঠামো rpmi_sysmsi_get_attributes_resp garesp;
	কাঠামো mpxy_rpmi_sysmsi *smg;
	u32 p2a_db_সূচক;
	পূর্ণসংখ্যা rc, i;

	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_P2_A_DOORBELL_SYSMSI_সূচক,
				     &p2a_db_সূচক);
	যদি (rc)
		ফেরত rc;

	rc = rpmi_normal_request_with_status(chan, RPMI_SYSMSI_SRV_GET_ATTRIBUTES,
					     ফাঁকা, 0, 0, &garesp, rpmi_u32_গুনতি(garesp),
					     rpmi_u32_গুনতি(garesp));
	যদি (rc)
		ফেরত rc;

	smg = sbi_zalloc(আকার(*smg));
	যদি (!smg)
		ফেরত SBI_ENOMEM;

	smg->sys_অঙ্ক_msi = garesp.sys_অঙ্ক_msi;
	smg->sys_msi_denied_bmap = sbi_zalloc(bitmap_estimate_আকার(smg->sys_অঙ্ক_msi));
	যদি (!smg->sys_msi_denied_bmap) {
		sbi_free(smg);
		ফেরত SBI_ENOMEM;
	}

	জন্য (i = 0; i < smg->sys_অঙ্ক_msi; i++) {
		gmareq.sys_msi_সূচক = i;
		rc = rpmi_normal_request_with_status(chan,
						     RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES,
						     &gmareq, rpmi_u32_গুনতি(gmareq),
						     rpmi_u32_গুনতি(gmareq),
						     &gmaresp, rpmi_u32_গুনতি(gmaresp),
						     rpmi_u32_গুনতি(gmaresp));
		যদি (rc) {
			mpxy_rpmi_sysmsi_cleanup(smg);
			ফেরত rc;
		}

		যদি (p2a_db_সূচক == i ||
		    (gmaresp.flag0 & RPMI_SYSMSI_MSI_ATTRIBUTES_FLAG0_PREF_PRIV))
			bitmap_set(smg->sys_msi_denied_bmap, i, 1);
	}

	*context = smg;
	ফেরত 0;
}

স্থির কাঠামো mpxy_rpmi_service_তথ্য sysmsi_services[] = {
{
	.id = RPMI_SYSMSI_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = আকার(কাঠামো rpmi_enable_notification_req),
	.max_tx_len = আকার(কাঠামো rpmi_enable_notification_req),
	.min_rx_len = আকার(কাঠামো rpmi_enable_notification_resp),
	.max_rx_len = আকার(কাঠামো rpmi_enable_notification_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_ATTRIBUTES,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = আকার(কাঠামো rpmi_sysmsi_get_attributes_resp),
	.max_rx_len = আকার(কাঠামো rpmi_sysmsi_get_attributes_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES,
	.min_tx_len = আকার(কাঠামো rpmi_sysmsi_get_msi_attributes_req),
	.max_tx_len = আকার(কাঠামো rpmi_sysmsi_get_msi_attributes_req),
	.min_rx_len = আকার(কাঠামো rpmi_sysmsi_get_msi_attributes_resp),
	.max_rx_len = আকার(কাঠামো rpmi_sysmsi_get_msi_attributes_resp),
},
{
	.id = RPMI_SYSMSI_SRV_SET_MSI_STATE,
	.min_tx_len = আকার(কাঠামো rpmi_sysmsi_set_msi_state_req),
	.max_tx_len = আকার(কাঠামো rpmi_sysmsi_set_msi_state_req),
	.min_rx_len = আকার(কাঠামো rpmi_sysmsi_set_msi_state_resp),
	.max_rx_len = আকার(কাঠামো rpmi_sysmsi_set_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_STATE,
	.min_tx_len = আকার(কাঠামো rpmi_sysmsi_get_msi_state_req),
	.max_tx_len = আকার(কাঠামো rpmi_sysmsi_get_msi_state_req),
	.min_rx_len = আকার(কাঠামো rpmi_sysmsi_get_msi_state_resp),
	.max_rx_len = আকার(কাঠামো rpmi_sysmsi_get_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_SET_MSI_TARGET,
	.min_tx_len = আকার(কাঠামো rpmi_sysmsi_set_msi_target_req),
	.max_tx_len = আকার(কাঠামো rpmi_sysmsi_set_msi_target_req),
	.min_rx_len = আকার(কাঠামো rpmi_sysmsi_set_msi_target_resp),
	.max_rx_len = আকার(কাঠামো rpmi_sysmsi_set_msi_target_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_TARGET,
	.min_tx_len = আকার(কাঠামো rpmi_sysmsi_get_msi_target_req),
	.max_tx_len = আকার(কাঠামো rpmi_sysmsi_get_msi_target_req),
	.min_rx_len = আকার(কাঠামো rpmi_sysmsi_get_msi_target_resp),
	.max_rx_len = আকার(কাঠামো rpmi_sysmsi_get_msi_target_resp),
},
};

স্থির ধ্রুবক কাঠামো mpxy_rpmi_mbox_তথ্য sysmsi_তথ্য = {
	.servicegrp_id = RPMI_SRVGRP_SYSTEM_MSI,
	.অঙ্ক_services = RPMI_SYSMSI_SRV_ID_MAX_গুনতি,
	.service_তথ্য = sysmsi_services,
	.xfer_group = mpxy_rpmi_sysmis_xfer,
	.setup_group = mpxy_rpmi_sysmsi_setup,
	.cleanup_group = mpxy_rpmi_sysmsi_cleanup,
};

স্থির ধ্রুবক কাঠামো fdt_match sysmsi_match[] = {
	{ .compatible = "riscv,rpmi-mpxy-system-msi", .তথ্য = &sysmsi_তথ্য },
	{ },
};

ধ্রুবক কাঠামো fdt_driver fdt_mpxy_rpmi_sysmsi = {
	.match_table = sysmsi_match,
	.init = mpxy_rpmi_mbox_init,
};
