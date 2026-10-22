/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Qualcomm Technologies, Inc.
 *
 * Authors:
 *   Ranbir Singh <ranbir.singh@oss.qualcomm.com>
 *   Sunil V L <sunilvl@oss.qualcomm.com>
 */

#அந்தர்பாவ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>
#அந்தர்பாவ <sbi_utils/mailbox/rpmi_mailbox.h>

நிலையான கட்டமைப்பு rpmi_mm_get_attributes_rsp rsp;

நிலையான கட்டமைப்பு mpxy_rpmi_service_data mm_srvcdata[] = {
	[0] = {
		.id = RPMI_MM_SRV_ENABLE_NOTIFICATION,
		.min_tx_len = அளவு(கட்டமைப்பு rpmi_enable_notification_req),
		.max_tx_len = அளவு(கட்டமைப்பு rpmi_enable_notification_req),
		.min_rx_len = அளவு(கட்டமைப்பு rpmi_enable_notification_resp),
		.max_rx_len = அளவு(கட்டமைப்பு rpmi_enable_notification_resp),
	},
	[1] = {
		.id = RPMI_MM_SRV_GET_ATTRIBUTES,
		.min_tx_len = 0,
		.max_tx_len = 0,
		.min_rx_len = அளவு(கட்டமைப்பு rpmi_mm_get_attributes_rsp),
		.max_rx_len = அளவு(கட்டமைப்பு rpmi_mm_get_attributes_rsp),
	},
	[2] = {
		.id = RPMI_MM_SRV_COMMUNICATE,
		.min_tx_len = அளவு(கட்டமைப்பு rpmi_mm_communicate_req),
		.max_tx_len = அளவு(கட்டமைப்பு rpmi_mm_communicate_req),
		.min_rx_len = அளவு(கட்டமைப்பு rpmi_mm_communicate_rsp),
		.max_rx_len = அளவு(கட்டமைப்பு rpmi_mm_communicate_rsp),
	},
};

நிலையான முழு mpxy_rpmi_mm_setup(வெறுமை **context, கட்டமைப்பு mbox_chan *chan,
			      மாறிலி கட்டமைப்பு mpxy_rpmi_mbox_data *data)
{
	unsigned long mm_region_addr = 0;
	unsigned long mm_region_size = 0;
	unsigned long mm_region_flags;
	முழு rc = 0;

	rc = rpmi_normal_request_with_status(chan, RPMI_MM_SRV_GET_ATTRIBUTES,
					     வெற்று, 0, 0, &rsp,
					     rpmi_u32_மொத்தம்(rsp),
					     rpmi_u32_மொத்தம்(rsp));
	எனில் (rc)
		திரும்பு rc;

#எனில் __riscv_xlen == 32
	mm_region_addr = rsp.mma.shmem_addr_lo;
#இல்லையெனில்
	mm_region_addr = ((unsigned long)(rsp.mma.shmem_addr_hi) << 32) |
	    rsp.mma.shmem_addr_lo;
#endif

	mm_region_size = rsp.mma.shmem_size;
	mm_region_flags = SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW;

	rc = sbi_domain_வேர்_add_memrange(mm_region_addr, mm_region_size,
					  PAGE_SIZE, mm_region_flags);
	திரும்பு rc;
}

நிலையான முழு mpxy_rpmi_mm_xfer(வெறுமை *context, கட்டமைப்பு mbox_chan *chan,
			     கட்டமைப்பு mbox_xfer *xfer)
{
	கட்டமைப்பு rpmi_message_args *args = xfer->args;
	முழு rc = 0;

	எனில் (!xfer->rx || (args->type != RPMI_செய்தி_NORMAL_REQUEST))
		திரும்பு 0;

	தேர்வு (args->service_id) {
	நிலை RPMI_MM_SRV_GET_ATTRIBUTES:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_to_le32(rsp.mma.mm_version);
		((u32 *)xfer->rx)[2] = cpu_to_le32(rsp.mma.shmem_addr_lo);
		((u32 *)xfer->rx)[3] = cpu_to_le32(rsp.mma.shmem_addr_hi);
		((u32 *)xfer->rx)[4] = cpu_to_le32(rsp.mma.shmem_size);
		args->rx_data_len = 5 * அளவு(u32);
		நிறுத்து;

	நிலை RPMI_MM_SRV_COMMUNICATE:
		rc = mbox_chan_xfer(chan, xfer);
		நிறுத்து;

	இயல்பு:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_NOTSUPP);
		args->rx_data_len = அளவு(u32);
		நிறுத்து;
	};

	திரும்பு rc;
}

நிலையான மாறிலி கட்டமைப்பு mpxy_rpmi_mbox_data mm_data = {
	.servicegrp_id = RPMI_SRVGRP_MANAGEMENT_MODE,
	.இலக்கம்_services = RPMI_MM_SRV_MAX_மொத்தம்,
	.service_data = mm_srvcdata,
	.setup_group = mpxy_rpmi_mm_setup,
	.xfer_group = mpxy_rpmi_mm_xfer,
};

/* one extra blank entry for loop termination while matching */
நிலையான மாறிலி கட்டமைப்பு fdt_match mm_match[] = {
	{
		.compatible = "riscv,rpmi-mpxy-mm",
		.data = &mm_data,
	},
	{},
};

மாறிலி கட்டமைப்பு fdt_driver fdt_mpxy_rpmi_mm = {
	.experimental = true,
	.match_table = mm_match,
	.init = mpxy_rpmi_mbox_init,
};
