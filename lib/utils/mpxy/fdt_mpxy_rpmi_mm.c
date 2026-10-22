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

স্থির কাঠামো rpmi_mm_get_attributes_rsp rsp;

স্থির কাঠামো mpxy_rpmi_service_তথ্য mm_srvcdata[] = {
	[0] = {
		.id = RPMI_MM_SRV_ENABLE_NOTIFICATION,
		.min_tx_len = আকার(কাঠামো rpmi_enable_notification_req),
		.max_tx_len = আকার(কাঠামো rpmi_enable_notification_req),
		.min_rx_len = আকার(কাঠামো rpmi_enable_notification_resp),
		.max_rx_len = আকার(কাঠামো rpmi_enable_notification_resp),
	},
	[1] = {
		.id = RPMI_MM_SRV_GET_ATTRIBUTES,
		.min_tx_len = 0,
		.max_tx_len = 0,
		.min_rx_len = আকার(কাঠামো rpmi_mm_get_attributes_rsp),
		.max_rx_len = আকার(কাঠামো rpmi_mm_get_attributes_rsp),
	},
	[2] = {
		.id = RPMI_MM_SRV_COMMUNICATE,
		.min_tx_len = আকার(কাঠামো rpmi_mm_communicate_req),
		.max_tx_len = আকার(কাঠামো rpmi_mm_communicate_req),
		.min_rx_len = আকার(কাঠামো rpmi_mm_communicate_rsp),
		.max_rx_len = আকার(কাঠামো rpmi_mm_communicate_rsp),
	},
};

স্থির পূর্ণসংখ্যা mpxy_rpmi_mm_setup(শূন্য **context, কাঠামো mbox_chan *chan,
			      ধ্রুবক কাঠামো mpxy_rpmi_mbox_তথ্য *তথ্য)
{
	unsigned long mm_region_addr = 0;
	unsigned long mm_region_আকার = 0;
	unsigned long mm_region_flags;
	পূর্ণসংখ্যা rc = 0;

	rc = rpmi_normal_request_with_status(chan, RPMI_MM_SRV_GET_ATTRIBUTES,
					     ফাঁকা, 0, 0, &rsp,
					     rpmi_u32_গুনতি(rsp),
					     rpmi_u32_গুনতি(rsp));
	যদি (rc)
		ফেরত rc;

#যদি __riscv_xlen == 32
	mm_region_addr = rsp.mma.shmem_addr_lo;
#অন্যথা
	mm_region_addr = ((unsigned long)(rsp.mma.shmem_addr_hi) << 32) |
	    rsp.mma.shmem_addr_lo;
#endif

	mm_region_আকার = rsp.mma.shmem_আকার;
	mm_region_flags = SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW;

	rc = sbi_domain_মূল_add_memrange(mm_region_addr, mm_region_আকার,
					  PAGE_আকার, mm_region_flags);
	ফেরত rc;
}

স্থির পূর্ণসংখ্যা mpxy_rpmi_mm_xfer(শূন্য *context, কাঠামো mbox_chan *chan,
			     কাঠামো mbox_xfer *xfer)
{
	কাঠামো rpmi_message_args *args = xfer->args;
	পূর্ণসংখ্যা rc = 0;

	যদি (!xfer->rx || (args->type != RPMI_বার্তা_NORMAL_REQUEST))
		ফেরত 0;

	নির্বাচন (args->service_id) {
	ক্ষেত্র RPMI_MM_SRV_GET_ATTRIBUTES:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_to_le32(rsp.mma.mm_version);
		((u32 *)xfer->rx)[2] = cpu_to_le32(rsp.mma.shmem_addr_lo);
		((u32 *)xfer->rx)[3] = cpu_to_le32(rsp.mma.shmem_addr_hi);
		((u32 *)xfer->rx)[4] = cpu_to_le32(rsp.mma.shmem_আকার);
		args->rx_তথ্য_len = 5 * আকার(u32);
		বিরতি;

	ক্ষেত্র RPMI_MM_SRV_COMMUNICATE:
		rc = mbox_chan_xfer(chan, xfer);
		বিরতি;

	পূর্বনির্ধারিত:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_NOTSUPP);
		args->rx_তথ্য_len = আকার(u32);
		বিরতি;
	};

	ফেরত rc;
}

স্থির ধ্রুবক কাঠামো mpxy_rpmi_mbox_তথ্য mm_তথ্য = {
	.servicegrp_id = RPMI_SRVGRP_MANAGEMENT_MODE,
	.অঙ্ক_services = RPMI_MM_SRV_MAX_গুনতি,
	.service_তথ্য = mm_srvcdata,
	.setup_group = mpxy_rpmi_mm_setup,
	.xfer_group = mpxy_rpmi_mm_xfer,
};

/* one extra blank entry for loop termination while matching */
স্থির ধ্রুবক কাঠামো fdt_match mm_match[] = {
	{
		.compatible = "riscv,rpmi-mpxy-mm",
		.তথ্য = &mm_তথ্য,
	},
	{},
};

ধ্রুবক কাঠামো fdt_driver fdt_mpxy_rpmi_mm = {
	.experimental = true,
	.match_table = mm_match,
	.init = mpxy_rpmi_mbox_init,
};
