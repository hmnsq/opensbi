/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Qualcomm Technologies, Inc.
 *
 * Authors:
 *   Ranbir Singh <ranbir.singh@oss.qualcomm.com>
 *   Sunil V L <sunilvl@oss.qualcomm.com>
 */

#شامل <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>
#شامل <sbi_utils/mailbox/rpmi_mailbox.h>

ایستا ساختار rpmi_mm_get_attributes_rsp rsp;

ایستا ساختار mpxy_rpmi_service_data mm_srvcdata[] = {
	[0] = {
		.id = RPMI_MM_SRV_ENABLE_NOTIFICATION,
		.min_tx_len = اندازه(ساختار rpmi_enable_notification_req),
		.max_tx_len = اندازه(ساختار rpmi_enable_notification_req),
		.min_rx_len = اندازه(ساختار rpmi_enable_notification_resp),
		.max_rx_len = اندازه(ساختار rpmi_enable_notification_resp),
	},
	[1] = {
		.id = RPMI_MM_SRV_GET_ATTRIBUTES,
		.min_tx_len = 0,
		.max_tx_len = 0,
		.min_rx_len = اندازه(ساختار rpmi_mm_get_attributes_rsp),
		.max_rx_len = اندازه(ساختار rpmi_mm_get_attributes_rsp),
	},
	[2] = {
		.id = RPMI_MM_SRV_COMMUNICATE,
		.min_tx_len = اندازه(ساختار rpmi_mm_communicate_req),
		.max_tx_len = اندازه(ساختار rpmi_mm_communicate_req),
		.min_rx_len = اندازه(ساختار rpmi_mm_communicate_rsp),
		.max_rx_len = اندازه(ساختار rpmi_mm_communicate_rsp),
	},
};

ایستا صحیح mpxy_rpmi_mm_setup(تهی **context, ساختار mbox_chan *chan,
			      ثابت ساختار mpxy_rpmi_mbox_data *data)
{
	unsigned long mm_region_addr = 0;
	unsigned long mm_region_size = 0;
	unsigned long mm_region_flags;
	صحیح rc = 0;

	rc = rpmi_normal_request_with_status(chan, RPMI_MM_SRV_GET_ATTRIBUTES,
					     پوچ, 0, 0, &rsp,
					     rpmi_u32_شمار(rsp),
					     rpmi_u32_شمار(rsp));
	اگر (rc)
		بازگشت rc;

#اگر __riscv_xlen == 32
	mm_region_addr = rsp.mma.shmem_addr_lo;
#وگرنه
	mm_region_addr = ((unsigned long)(rsp.mma.shmem_addr_hi) << 32) |
	    rsp.mma.shmem_addr_lo;
#endif

	mm_region_size = rsp.mma.shmem_size;
	mm_region_flags = SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW;

	rc = sbi_domain_ریشه_add_memrange(mm_region_addr, mm_region_size,
					  PAGE_SIZE, mm_region_flags);
	بازگشت rc;
}

ایستا صحیح mpxy_rpmi_mm_xfer(تهی *context, ساختار mbox_chan *chan,
			     ساختار mbox_xfer *xfer)
{
	ساختار rpmi_message_args *args = xfer->args;
	صحیح rc = 0;

	اگر (!xfer->rx || (args->type != RPMI_پیام_NORMAL_REQUEST))
		بازگشت 0;

	انتخاب (args->service_id) {
	حالت RPMI_MM_SRV_GET_ATTRIBUTES:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_to_le32(rsp.mma.mm_version);
		((u32 *)xfer->rx)[2] = cpu_to_le32(rsp.mma.shmem_addr_lo);
		((u32 *)xfer->rx)[3] = cpu_to_le32(rsp.mma.shmem_addr_hi);
		((u32 *)xfer->rx)[4] = cpu_to_le32(rsp.mma.shmem_size);
		args->rx_data_len = 5 * اندازه(u32);
		بشکن;

	حالت RPMI_MM_SRV_COMMUNICATE:
		rc = mbox_chan_xfer(chan, xfer);
		بشکن;

	پیشفرض:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_NOTSUPP);
		args->rx_data_len = اندازه(u32);
		بشکن;
	};

	بازگشت rc;
}

ایستا ثابت ساختار mpxy_rpmi_mbox_data mm_data = {
	.servicegrp_id = RPMI_SRVGRP_MANAGEMENT_MODE,
	.رقم_services = RPMI_MM_SRV_MAX_شمار,
	.service_data = mm_srvcdata,
	.setup_group = mpxy_rpmi_mm_setup,
	.xfer_group = mpxy_rpmi_mm_xfer,
};

/* one extra blank entry for loop termination while matching */
ایستا ثابت ساختار fdt_match mm_match[] = {
	{
		.compatible = "riscv,rpmi-mpxy-mm",
		.data = &mm_data,
	},
	{},
};

ثابت ساختار fdt_driver fdt_mpxy_rpmi_mm = {
	.experimental = true,
	.match_table = mm_match,
	.init = mpxy_rpmi_mbox_init,
};
