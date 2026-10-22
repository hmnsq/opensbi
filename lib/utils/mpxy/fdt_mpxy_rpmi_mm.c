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

ساكن بنية rpmi_mm_get_صفات_rsp rsp;

ساكن بنية mpxy_rpmi_service_داده mm_srvcdata[] = {
	[0] = {
		.id = RPMI_MM_SRV_ENABLE_NOTIFICATION,
		.min_tx_len = حجم(بنية rpmi_enable_notification_req),
		.max_tx_len = حجم(بنية rpmi_enable_notification_req),
		.min_rx_len = حجم(بنية rpmi_enable_notification_resp),
		.max_rx_len = حجم(بنية rpmi_enable_notification_resp),
	},
	[1] = {
		.id = RPMI_MM_SRV_GET_صفات,
		.min_tx_len = 0,
		.max_tx_len = 0,
		.min_rx_len = حجم(بنية rpmi_mm_get_صفات_rsp),
		.max_rx_len = حجم(بنية rpmi_mm_get_صفات_rsp),
	},
	[2] = {
		.id = RPMI_MM_SRV_COMMUNICATE,
		.min_tx_len = حجم(بنية rpmi_mm_communicate_req),
		.max_tx_len = حجم(بنية rpmi_mm_communicate_req),
		.min_rx_len = حجم(بنية rpmi_mm_communicate_rsp),
		.max_rx_len = حجم(بنية rpmi_mm_communicate_rsp),
	},
};

ساكن صحيح mpxy_rpmi_mm_راهاندازی(فراغ **context, بنية mbox_chan *chan,
			      ثابت بنية mpxy_rpmi_mbox_داده *داده)
{
	بیعلامت دراز mm_region_addr = 0;
	بیعلامت دراز mm_region_اندازه = 0;
	بیعلامت دراز mm_region_flags;
	صحيح rc = 0;

	rc = rpmi_نرمال_request_with_وضعیت(chan, RPMI_MM_SRV_GET_صفات,
					     عدم, 0, 0, &rsp,
					     rpmi_u32_شمار(rsp),
					     rpmi_u32_شمار(rsp));
	إذا (rc)
		أرجع rc;

#إذا __riscv_xlen == 32
	mm_region_addr = rsp.mma.shmem_addr_lo;
#وإلا
	mm_region_addr = ((بیعلامت دراز)(rsp.mma.shmem_addr_hi) << 32) |
	    rsp.mma.shmem_addr_lo;
#پایان_اگر

	mm_region_اندازه = rsp.mma.shmem_اندازه;
	mm_region_flags = SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW;

	rc = sbi_domain_ریشه_جمع_کن_memrange(mm_region_addr, mm_region_اندازه,
					  صفحه_اندازه, mm_region_flags);
	أرجع rc;
}

ساكن صحيح mpxy_rpmi_mm_xfer(فراغ *context, بنية mbox_chan *chan,
			     بنية mbox_xfer *xfer)
{
	بنية rpmi_پیام_آرگومانها *آرگومانها = xfer->آرگومانها;
	صحيح rc = 0;

	إذا (!xfer->rx || (آرگومانها->type != RPMI_پیام_نرمال_REQUEST))
		أرجع 0;

	بدل (آرگومانها->service_id) {
	حالة RPMI_MM_SRV_GET_صفات:
		((u32 *)xfer->rx)[0] = cpu_مقصد_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_مقصد_le32(rsp.mma.mm_نسخه);
		((u32 *)xfer->rx)[2] = cpu_مقصد_le32(rsp.mma.shmem_addr_lo);
		((u32 *)xfer->rx)[3] = cpu_مقصد_le32(rsp.mma.shmem_addr_hi);
		((u32 *)xfer->rx)[4] = cpu_مقصد_le32(rsp.mma.shmem_اندازه);
		آرگومانها->rx_داده_len = 5 * حجم(u32);
		اكسر;

	حالة RPMI_MM_SRV_COMMUNICATE:
		rc = mbox_chan_xfer(chan, xfer);
		اكسر;

	افتراضي:
		((u32 *)xfer->rx)[0] = cpu_مقصد_le32(RPMI_ERR_NOTSUPP);
		آرگومانها->rx_داده_len = حجم(u32);
		اكسر;
	};

	أرجع rc;
}

ساكن ثابت بنية mpxy_rpmi_mbox_داده mm_داده = {
	.servicegrp_id = RPMI_SRVGRP_MANAGEMENT_نما,
	.رقم_services = RPMI_MM_SRV_MAX_شمار,
	.service_داده = mm_srvcdata,
	.راهاندازی_گروه = mpxy_rpmi_mm_راهاندازی,
	.xfer_گروه = mpxy_rpmi_mm_xfer,
};

/* one extra blank entry for loop termination while matching */
ساكن ثابت بنية fdt_تطابق mm_تطابق[] = {
	{
		.compatible = "riscv,rpmi-mpxy-mm",
		.داده = &mm_داده,
	},
	{},
};

ثابت بنية fdt_driver fdt_mpxy_rpmi_mm = {
	.experimental = true,
	.تطابق_جدول = mm_تطابق,
	.init = mpxy_rpmi_mbox_init,
};
