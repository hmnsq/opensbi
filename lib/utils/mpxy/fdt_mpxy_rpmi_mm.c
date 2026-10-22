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

ساكن بنية rpmi_mm_get_خاصيتون_rsp rsp;

ساكن بنية mpxy_rpmi_service_معلومات mm_srvcdata[] = {
	[0] = {
		.id = RPMI_MM_SRV_ENABLE_NOTIFICATION,
		.min_tx_len = حجم(بنية rpmi_enable_notification_req),
		.max_tx_len = حجم(بنية rpmi_enable_notification_req),
		.min_rx_len = حجم(بنية rpmi_enable_notification_resp),
		.max_rx_len = حجم(بنية rpmi_enable_notification_resp),
	},
	[1] = {
		.id = RPMI_MM_SRV_GET_خاصيتون,
		.min_tx_len = 0,
		.max_tx_len = 0,
		.min_rx_len = حجم(بنية rpmi_mm_get_خاصيتون_rsp),
		.max_rx_len = حجم(بنية rpmi_mm_get_خاصيتون_rsp),
	},
	[2] = {
		.id = RPMI_MM_SRV_COMMUNICATE,
		.min_tx_len = حجم(بنية rpmi_mm_communicate_req),
		.max_tx_len = حجم(بنية rpmi_mm_communicate_req),
		.min_rx_len = حجم(بنية rpmi_mm_communicate_rsp),
		.max_rx_len = حجم(بنية rpmi_mm_communicate_rsp),
	},
};

ساكن صحيح mpxy_rpmi_mm_تياري(فراغ **context, بنية mbox_chan *chan,
			      ثابت بنية mpxy_rpmi_mbox_معلومات *معلومات)
{
	بنانشان ڊگهو mm_region_addr = 0;
	بنانشان ڊگهو mm_region_سائيز = 0;
	بنانشان ڊگهو mm_region_flags;
	صحيح rc = 0;

	rc = rpmi_عام_request_with_حالت(chan, RPMI_MM_SRV_GET_خاصيتون,
					     عدم, 0, 0, &rsp,
					     rpmi_u32_ڳڻپ(rsp),
					     rpmi_u32_ڳڻپ(rsp));
	إذا (rc)
		أرجع rc;

#إذا __riscv_xlen == 32
	mm_region_addr = rsp.mma.shmem_addr_lo;
#وإلا
	mm_region_addr = ((بنانشان ڊگهو)(rsp.mma.shmem_addr_hi) << 32) |
	    rsp.mma.shmem_addr_lo;
#جيڪڏهن_پورو

	mm_region_سائيز = rsp.mma.shmem_سائيز;
	mm_region_flags = SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW;

	rc = sbi_domain_پاڙ_جوڙيو_memrange(mm_region_addr, mm_region_سائيز,
					  صفحو_سائيز, mm_region_flags);
	أرجع rc;
}

ساكن صحيح mpxy_rpmi_mm_xfer(فراغ *context, بنية mbox_chan *chan,
			     بنية mbox_xfer *xfer)
{
	بنية rpmi_پيغام_دليلون *دليلون = xfer->دليلون;
	صحيح rc = 0;

	إذا (!xfer->rx || (دليلون->type != RPMI_نياپو_عام_REQUEST))
		أرجع 0;

	بدل (دليلون->service_id) {
	حالة RPMI_MM_SRV_GET_خاصيتون:
		((u32 *)xfer->rx)[0] = cpu_منزل_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_منزل_le32(rsp.mma.mm_نسخو);
		((u32 *)xfer->rx)[2] = cpu_منزل_le32(rsp.mma.shmem_addr_lo);
		((u32 *)xfer->rx)[3] = cpu_منزل_le32(rsp.mma.shmem_addr_hi);
		((u32 *)xfer->rx)[4] = cpu_منزل_le32(rsp.mma.shmem_سائيز);
		دليلون->rx_معلومات_len = 5 * حجم(u32);
		اكسر;

	حالة RPMI_MM_SRV_COMMUNICATE:
		rc = mbox_chan_xfer(chan, xfer);
		اكسر;

	افتراضي:
		((u32 *)xfer->rx)[0] = cpu_منزل_le32(RPMI_ERR_NOTSUPP);
		دليلون->rx_معلومات_len = حجم(u32);
		اكسر;
	};

	أرجع rc;
}

ساكن ثابت بنية mpxy_rpmi_mbox_معلومات mm_معلومات = {
	.servicegrp_id = RPMI_SRVGRP_MANAGEMENT_گهڻائي,
	.انگ_services = RPMI_MM_SRV_MAX_ڳڻپ,
	.service_معلومات = mm_srvcdata,
	.تياري_گروهه = mpxy_rpmi_mm_تياري,
	.xfer_گروهه = mpxy_rpmi_mm_xfer,
};

/* one extra blank entry for loop termination while matching */
ساكن ثابت بنية fdt_ميلاپ mm_ميلاپ[] = {
	{
		.compatible = "riscv,rpmi-mpxy-mm",
		.معلومات = &mm_معلومات,
	},
	{},
};

ثابت بنية fdt_driver fdt_mpxy_rpmi_mm = {
	.experimental = true,
	.ميلاپ_جدول = mm_ميلاپ,
	.init = mpxy_rpmi_mbox_init,
};
