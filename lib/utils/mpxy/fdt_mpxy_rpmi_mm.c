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

ساكن بنية rpmi_mm_get_گن_rsp rsp;

ساكن بنية mpxy_rpmi_service_انکڑے mm_srvcdata[] = {
	[0] = {
		.id = RPMI_MM_SRV_ENABLE_NOTIFICATION,
		.min_tx_len = حجم(بنية rpmi_enable_notification_req),
		.max_tx_len = حجم(بنية rpmi_enable_notification_req),
		.min_rx_len = حجم(بنية rpmi_enable_notification_resp),
		.max_rx_len = حجم(بنية rpmi_enable_notification_resp),
	},
	[1] = {
		.id = RPMI_MM_SRV_GET_گن,
		.min_tx_len = 0,
		.max_tx_len = 0,
		.min_rx_len = حجم(بنية rpmi_mm_get_گن_rsp),
		.max_rx_len = حجم(بنية rpmi_mm_get_گن_rsp),
	},
	[2] = {
		.id = RPMI_MM_SRV_COMMUNICATE,
		.min_tx_len = حجم(بنية rpmi_mm_communicate_req),
		.max_tx_len = حجم(بنية rpmi_mm_communicate_req),
		.min_rx_len = حجم(بنية rpmi_mm_communicate_rsp),
		.max_rx_len = حجم(بنية rpmi_mm_communicate_rsp),
	},
};

ساكن صحيح mpxy_rpmi_mm_تیاری(فراغ **context, بنية mbox_chan *chan,
			      ثابت بنية mpxy_rpmi_mbox_انکڑے *انکڑے)
{
	بنانشانی لما mm_region_addr = 0;
	بنانشانی لما mm_region_ناپ = 0;
	بنانشانی لما mm_region_flags;
	صحيح rc = 0;

	rc = rpmi_عام_request_with_حالت(chan, RPMI_MM_SRV_GET_گن,
					     عدم, 0, 0, &rsp,
					     rpmi_u32_گنتی(rsp),
					     rpmi_u32_گنتی(rsp));
	إذا (rc)
		أرجع rc;

#إذا __riscv_xlen == 32
	mm_region_addr = rsp.mma.shmem_addr_lo;
#وإلا
	mm_region_addr = ((بنانشانی لما)(rsp.mma.shmem_addr_hi) << 32) |
	    rsp.mma.shmem_addr_lo;
#جے_انت

	mm_region_ناپ = rsp.mma.shmem_ناپ;
	mm_region_flags = SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW;

	rc = sbi_domain_جڑ_جوڑو_memrange(mm_region_addr, mm_region_ناپ,
					  ورقہ_ناپ, mm_region_flags);
	أرجع rc;
}

ساكن صحيح mpxy_rpmi_mm_xfer(فراغ *context, بنية mbox_chan *chan,
			     بنية mbox_xfer *xfer)
{
	بنية rpmi_سنیہا_دلیلاں *دلیلاں = xfer->دلیلاں;
	صحيح rc = 0;

	إذا (!xfer->rx || (دلیلاں->type != RPMI_پیغام_عام_REQUEST))
		أرجع 0;

	بدل (دلیلاں->service_id) {
	حالة RPMI_MM_SRV_GET_گن:
		((u32 *)xfer->rx)[0] = cpu_منزل_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_منزل_le32(rsp.mma.mm_نسخہ);
		((u32 *)xfer->rx)[2] = cpu_منزل_le32(rsp.mma.shmem_addr_lo);
		((u32 *)xfer->rx)[3] = cpu_منزل_le32(rsp.mma.shmem_addr_hi);
		((u32 *)xfer->rx)[4] = cpu_منزل_le32(rsp.mma.shmem_ناپ);
		دلیلاں->rx_انکڑے_len = 5 * حجم(u32);
		اكسر;

	حالة RPMI_MM_SRV_COMMUNICATE:
		rc = mbox_chan_xfer(chan, xfer);
		اكسر;

	افتراضي:
		((u32 *)xfer->rx)[0] = cpu_منزل_le32(RPMI_ERR_NOTSUPP);
		دلیلاں->rx_انکڑے_len = حجم(u32);
		اكسر;
	};

	أرجع rc;
}

ساكن ثابت بنية mpxy_rpmi_mbox_انکڑے mm_انکڑے = {
	.servicegrp_id = RPMI_SRVGRP_MANAGEMENT_منوال,
	.عدد_services = RPMI_MM_SRV_MAX_گنتی,
	.service_انکڑے = mm_srvcdata,
	.تیاری_گروہ = mpxy_rpmi_mm_تیاری,
	.xfer_گروہ = mpxy_rpmi_mm_xfer,
};

/* one extra blank entry for loop termination while matching */
ساكن ثابت بنية fdt_میل mm_میل[] = {
	{
		.compatible = "riscv,rpmi-mpxy-mm",
		.انکڑے = &mm_انکڑے,
	},
	{},
};

ثابت بنية fdt_driver fdt_mpxy_rpmi_mm = {
	.experimental = true,
	.میل_جدول = mm_میل,
	.init = mpxy_rpmi_mbox_init,
};
