/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Qualcomm Technologies, Inc.
 *
 * Authors:
 *   Ranbir Singh <ranbir.singh@oss.qualcomm.com>
 *   Sunil V L <sunilvl@oss.qualcomm.com>
 */

#ضمن <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>
#ضمن <sbi_utils/mailbox/rpmi_mailbox.h>

ساكن بنية rpmi_mm_get_attributes_rsp rsp;

ساكن بنية mpxy_rpmi_service_data mm_srvcdata[] = {
	[0] = {
		.id = RPMI_MM_SRV_ENABLE_NOTIFICATION,
		.min_tx_len = حجم(بنية rpmi_enable_notification_req),
		.max_tx_len = حجم(بنية rpmi_enable_notification_req),
		.min_rx_len = حجم(بنية rpmi_enable_notification_resp),
		.max_rx_len = حجم(بنية rpmi_enable_notification_resp),
	},
	[1] = {
		.id = RPMI_MM_SRV_GET_ATTRIBUTES,
		.min_tx_len = 0,
		.max_tx_len = 0,
		.min_rx_len = حجم(بنية rpmi_mm_get_attributes_rsp),
		.max_rx_len = حجم(بنية rpmi_mm_get_attributes_rsp),
	},
	[2] = {
		.id = RPMI_MM_SRV_COMMUNICATE,
		.min_tx_len = حجم(بنية rpmi_mm_communicate_req),
		.max_tx_len = حجم(بنية rpmi_mm_communicate_req),
		.min_rx_len = حجم(بنية rpmi_mm_communicate_rsp),
		.max_rx_len = حجم(بنية rpmi_mm_communicate_rsp),
	},
};

ساكن صحيح mpxy_rpmi_mm_setup(فراغ **context, بنية mbox_chan *chan,
			      ثابت بنية mpxy_rpmi_mbox_data *data)
{
	unsigned long mm_region_addr = 0;
	unsigned long mm_region_size = 0;
	unsigned long mm_region_flags;
	صحيح rc = 0;

	rc = rpmi_normal_request_with_status(chan, RPMI_MM_SRV_GET_ATTRIBUTES,
					     عدم, 0, 0, &rsp,
					     rpmi_u32_عدد(rsp),
					     rpmi_u32_عدد(rsp));
	إذا (rc)
		أرجع rc;

#إذا __riscv_xlen == 32
	mm_region_addr = rsp.mma.shmem_addr_lo;
#وإلا
	mm_region_addr = ((unsigned long)(rsp.mma.shmem_addr_hi) << 32) |
	    rsp.mma.shmem_addr_lo;
#endif

	mm_region_size = rsp.mma.shmem_size;
	mm_region_flags = SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW;

	rc = sbi_domain_جذر_add_memrange(mm_region_addr, mm_region_size,
					  PAGE_SIZE, mm_region_flags);
	أرجع rc;
}

ساكن صحيح mpxy_rpmi_mm_xfer(فراغ *context, بنية mbox_chan *chan,
			     بنية mbox_xfer *xfer)
{
	بنية rpmi_message_وسائط *وسائط = xfer->وسائط;
	صحيح rc = 0;

	إذا (!xfer->rx || (وسائط->type != RPMI_رسالة_NORMAL_REQUEST))
		أرجع 0;

	بدل (وسائط->service_id) {
	حالة RPMI_MM_SRV_GET_ATTRIBUTES:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_to_le32(rsp.mma.mm_version);
		((u32 *)xfer->rx)[2] = cpu_to_le32(rsp.mma.shmem_addr_lo);
		((u32 *)xfer->rx)[3] = cpu_to_le32(rsp.mma.shmem_addr_hi);
		((u32 *)xfer->rx)[4] = cpu_to_le32(rsp.mma.shmem_size);
		وسائط->rx_data_len = 5 * حجم(u32);
		اكسر;

	حالة RPMI_MM_SRV_COMMUNICATE:
		rc = mbox_chan_xfer(chan, xfer);
		اكسر;

	افتراضي:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_NOTSUPP);
		وسائط->rx_data_len = حجم(u32);
		اكسر;
	};

	أرجع rc;
}

ساكن ثابت بنية mpxy_rpmi_mbox_data mm_data = {
	.servicegrp_id = RPMI_SRVGRP_MANAGEMENT_MODE,
	.رقم_services = RPMI_MM_SRV_MAX_عدد,
	.service_data = mm_srvcdata,
	.setup_group = mpxy_rpmi_mm_setup,
	.xfer_group = mpxy_rpmi_mm_xfer,
};

/* one extra blank entry for loop termination while matching */
ساكن ثابت بنية fdt_match mm_match[] = {
	{
		.compatible = "riscv,rpmi-mpxy-mm",
		.data = &mm_data,
	},
	{},
};

ثابت بنية fdt_driver fdt_mpxy_rpmi_mm = {
	.experimental = true,
	.match_table = mm_match,
	.init = mpxy_rpmi_mbox_init,
};
