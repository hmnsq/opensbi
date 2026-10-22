/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Qualcomm Technologies, Inc.
 *
 * Authors:
 *   Ranbir Singh <ranbir.singh@oss.qualcomm.com>
 *   Sunil V L <sunilvl@oss.qualcomm.com>
 */

#অন্তর্ভাব <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>
#অন্তর্ভাব <sbi_utils/mailbox/rpmi_mailbox.h>

স্থির সংরচনা rpmi_mm_get_attributes_rsp rsp;

স্থির সংরচনা mpxy_rpmi_service_data mm_srvcdata[] = {
	[0] = {
		.id = RPMI_MM_SRV_ENABLE_NOTIFICATION,
		.min_tx_len = পরিমাণ(সংরচনা rpmi_enable_notification_req),
		.max_tx_len = পরিমাণ(সংরচনা rpmi_enable_notification_req),
		.min_rx_len = পরিমাণ(সংরচনা rpmi_enable_notification_resp),
		.max_rx_len = পরিমাণ(সংরচনা rpmi_enable_notification_resp),
	},
	[1] = {
		.id = RPMI_MM_SRV_GET_ATTRIBUTES,
		.min_tx_len = 0,
		.max_tx_len = 0,
		.min_rx_len = পরিমাণ(সংরচনা rpmi_mm_get_attributes_rsp),
		.max_rx_len = পরিমাণ(সংরচনা rpmi_mm_get_attributes_rsp),
	},
	[2] = {
		.id = RPMI_MM_SRV_COMMUNICATE,
		.min_tx_len = পরিমাণ(সংরচনা rpmi_mm_communicate_req),
		.max_tx_len = পরিমাণ(সংরচনা rpmi_mm_communicate_req),
		.min_rx_len = পরিমাণ(সংরচনা rpmi_mm_communicate_rsp),
		.max_rx_len = পরিমাণ(সংরচনা rpmi_mm_communicate_rsp),
	},
};

স্থির পূর্ণাঙ্ক mpxy_rpmi_mm_setup(শূন্য **context, সংরচনা mbox_chan *chan,
			      ধ্রুব সংরচনা mpxy_rpmi_mbox_data *data)
{
	unsigned long mm_region_addr = 0;
	unsigned long mm_region_size = 0;
	unsigned long mm_region_flags;
	পূর্ণাঙ্ক rc = 0;

	rc = rpmi_normal_request_with_status(chan, RPMI_MM_SRV_GET_ATTRIBUTES,
					     রিক্ত, 0, 0, &rsp,
					     rpmi_u32_গুনতি(rsp),
					     rpmi_u32_গুনতি(rsp));
	যদি (rc)
		প্রত্যাগম rc;

#যদি __riscv_xlen == 32
	mm_region_addr = rsp.mma.shmem_addr_lo;
#অন্যথা
	mm_region_addr = ((unsigned long)(rsp.mma.shmem_addr_hi) << 32) |
	    rsp.mma.shmem_addr_lo;
#endif

	mm_region_size = rsp.mma.shmem_size;
	mm_region_flags = SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW;

	rc = sbi_domain_মূল_add_memrange(mm_region_addr, mm_region_size,
					  PAGE_SIZE, mm_region_flags);
	প্রত্যাগম rc;
}

স্থির পূর্ণাঙ্ক mpxy_rpmi_mm_xfer(শূন্য *context, সংরচনা mbox_chan *chan,
			     সংরচনা mbox_xfer *xfer)
{
	সংরচনা rpmi_message_args *args = xfer->args;
	পূর্ণাঙ্ক rc = 0;

	যদি (!xfer->rx || (args->type != RPMI_বার্তা_NORMAL_REQUEST))
		প্রত্যাগম 0;

	বিকল্প (args->service_id) {
	পক্ষ RPMI_MM_SRV_GET_ATTRIBUTES:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_to_le32(rsp.mma.mm_version);
		((u32 *)xfer->rx)[2] = cpu_to_le32(rsp.mma.shmem_addr_lo);
		((u32 *)xfer->rx)[3] = cpu_to_le32(rsp.mma.shmem_addr_hi);
		((u32 *)xfer->rx)[4] = cpu_to_le32(rsp.mma.shmem_size);
		args->rx_data_len = 5 * পরিমাণ(u32);
		ভঙ্গ;

	পক্ষ RPMI_MM_SRV_COMMUNICATE:
		rc = mbox_chan_xfer(chan, xfer);
		ভঙ্গ;

	স্বভাব:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_NOTSUPP);
		args->rx_data_len = পরিমাণ(u32);
		ভঙ্গ;
	};

	প্রত্যাগম rc;
}

স্থির ধ্রুব সংরচনা mpxy_rpmi_mbox_data mm_data = {
	.servicegrp_id = RPMI_SRVGRP_MANAGEMENT_MODE,
	.অঙ্ক_services = RPMI_MM_SRV_MAX_গুনতি,
	.service_data = mm_srvcdata,
	.setup_group = mpxy_rpmi_mm_setup,
	.xfer_group = mpxy_rpmi_mm_xfer,
};

/* one extra blank entry for loop termination while matching */
স্থির ধ্রুব সংরচনা fdt_match mm_match[] = {
	{
		.compatible = "riscv,rpmi-mpxy-mm",
		.data = &mm_data,
	},
	{},
};

ধ্রুব সংরচনা fdt_driver fdt_mpxy_rpmi_mm = {
	.experimental = true,
	.match_table = mm_match,
	.init = mpxy_rpmi_mbox_init,
};
