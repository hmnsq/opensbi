/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Qualcomm Technologies, Inc.
 *
 * Authors:
 *   Ranbir Singh <ranbir.singh@oss.qualcomm.com>
 *   Sunil V L <sunilvl@oss.qualcomm.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/mailbox/rpmi_mailbox.h>

ਸਥਿਰ ਬਣਤਰ rpmi_mm_get_ਗੁਣ_rsp rsp;

ਸਥਿਰ ਬਣਤਰ mpxy_rpmi_service_ਅੰਕੜੇ mm_srvcdata[] = {
	[0] = {
		.id = RPMI_MM_SRV_ENABLE_NOTIFICATION,
		.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_enable_notification_req),
		.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_enable_notification_req),
		.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_enable_notification_resp),
		.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_enable_notification_resp),
	},
	[1] = {
		.id = RPMI_MM_SRV_GET_ਗੁਣ,
		.min_tx_len = 0,
		.max_tx_len = 0,
		.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_mm_get_ਗੁਣ_rsp),
		.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_mm_get_ਗੁਣ_rsp),
	},
	[2] = {
		.id = RPMI_MM_SRV_COMMUNICATE,
		.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_mm_communicate_req),
		.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_mm_communicate_req),
		.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_mm_communicate_rsp),
		.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_mm_communicate_rsp),
	},
};

ਸਥਿਰ ਪੂਰਨ mpxy_rpmi_mm_ਤਿਆਰੀ(ਖਾਲੀ **context, ਬਣਤਰ mbox_chan *chan,
			      ਅਚਲ ਬਣਤਰ mpxy_rpmi_mbox_ਅੰਕੜੇ *ਅੰਕੜੇ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ mm_region_addr = 0;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ mm_region_ਮਾਪ = 0;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ mm_region_flags;
	ਪੂਰਨ rc = 0;

	rc = rpmi_ਸਾਧਾਰਨ_request_with_ਸਥਿਤੀ(chan, RPMI_MM_SRV_GET_ਗੁਣ,
					     ਨਿਰਾ, 0, 0, &rsp,
					     rpmi_u32_ਗਿਣਤੀ(rsp),
					     rpmi_u32_ਗਿਣਤੀ(rsp));
	ਜੇ (rc)
		ਵਾਪਸ rc;

#ਜੇ __riscv_xlen == 32
	mm_region_addr = rsp.mma.shmem_addr_lo;
#ਨਹੀਂਤਾਂ
	mm_region_addr = ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)(rsp.mma.shmem_addr_hi) << 32) |
	    rsp.mma.shmem_addr_lo;
#ਜੇ_ਅੰਤ

	mm_region_ਮਾਪ = rsp.mma.shmem_ਮਾਪ;
	mm_region_flags = SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW;

	rc = sbi_domain_ਜੜ੍ਹ_ਜੋੜੋ_memrange(mm_region_addr, mm_region_ਮਾਪ,
					  ਪੰਨਾ_ਮਾਪ, mm_region_flags);
	ਵਾਪਸ rc;
}

ਸਥਿਰ ਪੂਰਨ mpxy_rpmi_mm_xfer(ਖਾਲੀ *context, ਬਣਤਰ mbox_chan *chan,
			     ਬਣਤਰ mbox_xfer *xfer)
{
	ਬਣਤਰ rpmi_ਸੁਨੇਹਾ_ਦਲੀਲਾਂ *ਦਲੀਲਾਂ = xfer->ਦਲੀਲਾਂ;
	ਪੂਰਨ rc = 0;

	ਜੇ (!xfer->rx || (ਦਲੀਲਾਂ->type != RPMI_ਸੁਨੇਹਾ_ਸਾਧਾਰਨ_REQUEST))
		ਵਾਪਸ 0;

	ਚੋਣ (ਦਲੀਲਾਂ->service_id) {
	ਹਾਲਤ RPMI_MM_SRV_GET_ਗੁਣ:
		((u32 *)xfer->rx)[0] = cpu_ਮੰਜ਼ਿਲ_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_ਮੰਜ਼ਿਲ_le32(rsp.mma.mm_ਸੰਸਕਰਣ);
		((u32 *)xfer->rx)[2] = cpu_ਮੰਜ਼ਿਲ_le32(rsp.mma.shmem_addr_lo);
		((u32 *)xfer->rx)[3] = cpu_ਮੰਜ਼ਿਲ_le32(rsp.mma.shmem_addr_hi);
		((u32 *)xfer->rx)[4] = cpu_ਮੰਜ਼ਿਲ_le32(rsp.mma.shmem_ਮਾਪ);
		ਦਲੀਲਾਂ->rx_ਅੰਕੜੇ_len = 5 * ਆਕਾਰ(u32);
		ਰੋਕੋ;

	ਹਾਲਤ RPMI_MM_SRV_COMMUNICATE:
		rc = mbox_chan_xfer(chan, xfer);
		ਰੋਕੋ;

	ਮੂਲ:
		((u32 *)xfer->rx)[0] = cpu_ਮੰਜ਼ਿਲ_le32(RPMI_ERR_NOTSUPP);
		ਦਲੀਲਾਂ->rx_ਅੰਕੜੇ_len = ਆਕਾਰ(u32);
		ਰੋਕੋ;
	};

	ਵਾਪਸ rc;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ mpxy_rpmi_mbox_ਅੰਕੜੇ mm_ਅੰਕੜੇ = {
	.servicegrp_id = RPMI_SRVGRP_MANAGEMENT_ਬਹੁਲਕ,
	.ਅੰਕ_services = RPMI_MM_SRV_MAX_ਗਿਣਤੀ,
	.service_ਅੰਕੜੇ = mm_srvcdata,
	.ਤਿਆਰੀ_ਸਮੂਹ = mpxy_rpmi_mm_ਤਿਆਰੀ,
	.xfer_ਸਮੂਹ = mpxy_rpmi_mm_xfer,
};

/* one extra blank entry for loop termination while matching */
ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ mm_ਮੇਲ[] = {
	{
		.compatible = "riscv,rpmi-mpxy-mm",
		.ਅੰਕੜੇ = &mm_ਅੰਕੜੇ,
	},
	{},
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_mpxy_rpmi_mm = {
	.experimental = true,
	.ਮੇਲ_ਸਾਰਣੀ = mm_ਮੇਲ,
	.init = mpxy_rpmi_mbox_init,
};
