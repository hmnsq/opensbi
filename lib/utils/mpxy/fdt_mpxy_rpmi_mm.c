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

ساکن ساخت rpmi_mm_get_اوصاف_rsp rsp;

ساکن ساخت mpxy_rpmi_service_ڈیٹا mm_srvcdata[] = {
	[0] = {
		.id = RPMI_MM_SRV_ENABLE_NOTIFICATION,
		.min_tx_len = حجم(ساخت rpmi_enable_notification_req),
		.max_tx_len = حجم(ساخت rpmi_enable_notification_req),
		.min_rx_len = حجم(ساخت rpmi_enable_notification_resp),
		.max_rx_len = حجم(ساخت rpmi_enable_notification_resp),
	},
	[1] = {
		.id = RPMI_MM_SRV_GET_اوصاف,
		.min_tx_len = 0,
		.max_tx_len = 0,
		.min_rx_len = حجم(ساخت rpmi_mm_get_اوصاف_rsp),
		.max_rx_len = حجم(ساخت rpmi_mm_get_اوصاف_rsp),
	},
	[2] = {
		.id = RPMI_MM_SRV_COMMUNICATE,
		.min_tx_len = حجم(ساخت rpmi_mm_communicate_req),
		.max_tx_len = حجم(ساخت rpmi_mm_communicate_req),
		.min_rx_len = حجم(ساخت rpmi_mm_communicate_rsp),
		.max_rx_len = حجم(ساخت rpmi_mm_communicate_rsp),
	},
};

ساکن صحیح mpxy_rpmi_mm_تیاری(خالی **context, ساخت mbox_chan *chan,
			      ثابت ساخت mpxy_rpmi_mbox_ڈیٹا *ڈیٹا)
{
	غیرعلامتی طویل mm_region_addr = 0;
	غیرعلامتی طویل mm_region_سائز = 0;
	غیرعلامتی طویل mm_region_flags;
	صحیح rc = 0;

	rc = rpmi_معمول_request_with_حالت(chan, RPMI_MM_SRV_GET_اوصاف,
					     تہی, 0, 0, &rsp,
					     rpmi_u32_گنتی(rsp),
					     rpmi_u32_گنتی(rsp));
	اگر (rc)
		واپس rc;

#اگر __riscv_xlen == 32
	mm_region_addr = rsp.mma.shmem_addr_lo;
#ورنہ
	mm_region_addr = ((غیرعلامتی طویل)(rsp.mma.shmem_addr_hi) << 32) |
	    rsp.mma.shmem_addr_lo;
#اگر_ختم

	mm_region_سائز = rsp.mma.shmem_سائز;
	mm_region_flags = SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW;

	rc = sbi_domain_جڑ_جوڑو_memrange(mm_region_addr, mm_region_سائز,
					  صفحہ_سائز, mm_region_flags);
	واپس rc;
}

ساکن صحیح mpxy_rpmi_mm_xfer(خالی *context, ساخت mbox_chan *chan,
			     ساخت mbox_xfer *xfer)
{
	ساخت rpmi_پیغام_دلائل *دلائل = xfer->دلائل;
	صحیح rc = 0;

	اگر (!xfer->rx || (دلائل->type != RPMI_پیغام_معمول_REQUEST))
		واپس 0;

	انتخاب (دلائل->service_id) {
	حالت RPMI_MM_SRV_GET_اوصاف:
		((u32 *)xfer->rx)[0] = cpu_مقصد_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_مقصد_le32(rsp.mma.mm_نسخہ);
		((u32 *)xfer->rx)[2] = cpu_مقصد_le32(rsp.mma.shmem_addr_lo);
		((u32 *)xfer->rx)[3] = cpu_مقصد_le32(rsp.mma.shmem_addr_hi);
		((u32 *)xfer->rx)[4] = cpu_مقصد_le32(rsp.mma.shmem_سائز);
		دلائل->rx_ڈیٹا_len = 5 * حجم(u32);
		توڑو;

	حالت RPMI_MM_SRV_COMMUNICATE:
		rc = mbox_chan_xfer(chan, xfer);
		توڑو;

	طےشدہ:
		((u32 *)xfer->rx)[0] = cpu_مقصد_le32(RPMI_ERR_NOTSUPP);
		دلائل->rx_ڈیٹا_len = حجم(u32);
		توڑو;
	};

	واپس rc;
}

ساکن ثابت ساخت mpxy_rpmi_mbox_ڈیٹا mm_ڈیٹا = {
	.servicegrp_id = RPMI_SRVGRP_MANAGEMENT_منوال,
	.عدد_services = RPMI_MM_SRV_MAX_گنتی,
	.service_ڈیٹا = mm_srvcdata,
	.تیاری_گروہ = mpxy_rpmi_mm_تیاری,
	.xfer_گروہ = mpxy_rpmi_mm_xfer,
};

/* one extra blank entry for loop termination while matching */
ساکن ثابت ساخت fdt_مطابقت mm_مطابقت[] = {
	{
		.compatible = "riscv,rpmi-mpxy-mm",
		.ڈیٹا = &mm_ڈیٹا,
	},
	{},
};

ثابت ساخت fdt_driver fdt_mpxy_rpmi_mm = {
	.experimental = true,
	.مطابقت_جدول = mm_مطابقت,
	.init = mpxy_rpmi_mbox_init,
};
