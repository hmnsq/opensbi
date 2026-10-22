/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Qualcomm Technologies, Inc.
 *
 * Authors:
 *   Ranbir Singh <ranbir.singh@oss.qualcomm.com>
 *   Sunil V L <sunilvl@oss.qualcomm.com>
 */

#અન્તર્ભાવ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>
#અન્તર્ભાવ <sbi_utils/mailbox/rpmi_mailbox.h>

સ્થિર સંરચના rpmi_mm_get_attributes_rsp rsp;

સ્થિર સંરચના mpxy_rpmi_service_data mm_srvcdata[] = {
	[0] = {
		.id = RPMI_MM_SRV_ENABLE_NOTIFICATION,
		.min_tx_len = કદ(સંરચના rpmi_enable_notification_req),
		.max_tx_len = કદ(સંરચના rpmi_enable_notification_req),
		.min_rx_len = કદ(સંરચના rpmi_enable_notification_resp),
		.max_rx_len = કદ(સંરચના rpmi_enable_notification_resp),
	},
	[1] = {
		.id = RPMI_MM_SRV_GET_ATTRIBUTES,
		.min_tx_len = 0,
		.max_tx_len = 0,
		.min_rx_len = કદ(સંરચના rpmi_mm_get_attributes_rsp),
		.max_rx_len = કદ(સંરચના rpmi_mm_get_attributes_rsp),
	},
	[2] = {
		.id = RPMI_MM_SRV_COMMUNICATE,
		.min_tx_len = કદ(સંરચના rpmi_mm_communicate_req),
		.max_tx_len = કદ(સંરચના rpmi_mm_communicate_req),
		.min_rx_len = કદ(સંરચના rpmi_mm_communicate_rsp),
		.max_rx_len = કદ(સંરચના rpmi_mm_communicate_rsp),
	},
};

સ્થિર પૂર્ણાંક mpxy_rpmi_mm_setup(શૂન્ય **context, સંરચના mbox_chan *chan,
			      અચલ સંરચના mpxy_rpmi_mbox_data *data)
{
	unsigned long mm_region_addr = 0;
	unsigned long mm_region_size = 0;
	unsigned long mm_region_flags;
	પૂર્ણાંક rc = 0;

	rc = rpmi_normal_request_with_status(chan, RPMI_MM_SRV_GET_ATTRIBUTES,
					     ખાલી, 0, 0, &rsp,
					     rpmi_u32_ગણતરી(rsp),
					     rpmi_u32_ગણતરી(rsp));
	જો (rc)
		પાછા rc;

#જો __riscv_xlen == 32
	mm_region_addr = rsp.mma.shmem_addr_lo;
#નહીંતર
	mm_region_addr = ((unsigned long)(rsp.mma.shmem_addr_hi) << 32) |
	    rsp.mma.shmem_addr_lo;
#endif

	mm_region_size = rsp.mma.shmem_size;
	mm_region_flags = SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW;

	rc = sbi_domain_મૂળ_add_memrange(mm_region_addr, mm_region_size,
					  PAGE_SIZE, mm_region_flags);
	પાછા rc;
}

સ્થિર પૂર્ણાંક mpxy_rpmi_mm_xfer(શૂન્ય *context, સંરચના mbox_chan *chan,
			     સંરચના mbox_xfer *xfer)
{
	સંરચના rpmi_message_args *args = xfer->args;
	પૂર્ણાંક rc = 0;

	જો (!xfer->rx || (args->type != RPMI_સંદેશ_NORMAL_REQUEST))
		પાછા 0;

	પસંદગી (args->service_id) {
	સ્થિતિ RPMI_MM_SRV_GET_ATTRIBUTES:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_to_le32(rsp.mma.mm_version);
		((u32 *)xfer->rx)[2] = cpu_to_le32(rsp.mma.shmem_addr_lo);
		((u32 *)xfer->rx)[3] = cpu_to_le32(rsp.mma.shmem_addr_hi);
		((u32 *)xfer->rx)[4] = cpu_to_le32(rsp.mma.shmem_size);
		args->rx_data_len = 5 * કદ(u32);
		તોડો;

	સ્થિતિ RPMI_MM_SRV_COMMUNICATE:
		rc = mbox_chan_xfer(chan, xfer);
		તોડો;

	મૂળભૂત:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_NOTSUPP);
		args->rx_data_len = કદ(u32);
		તોડો;
	};

	પાછા rc;
}

સ્થિર અચલ સંરચના mpxy_rpmi_mbox_data mm_data = {
	.servicegrp_id = RPMI_SRVGRP_MANAGEMENT_MODE,
	.અંક_services = RPMI_MM_SRV_MAX_ગણતરી,
	.service_data = mm_srvcdata,
	.setup_group = mpxy_rpmi_mm_setup,
	.xfer_group = mpxy_rpmi_mm_xfer,
};

/* one extra blank entry for loop termination while matching */
સ્થિર અચલ સંરચના fdt_match mm_match[] = {
	{
		.compatible = "riscv,rpmi-mpxy-mm",
		.data = &mm_data,
	},
	{},
};

અચલ સંરચના fdt_driver fdt_mpxy_rpmi_mm = {
	.experimental = true,
	.match_table = mm_match,
	.init = mpxy_rpmi_mbox_init,
};
