/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Qualcomm Technologies, Inc.
 *
 * Authors:
 *   Ranbir Singh <ranbir.singh@oss.qualcomm.com>
 *   Sunil V L <sunilvl@oss.qualcomm.com>
 */

#అన్తర్భావ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>
#అన్తర్భావ <sbi_utils/mailbox/rpmi_mailbox.h>

స్థిర నిర్మాణం rpmi_mm_get_లక్షణాలు_rsp rsp;

స్థిర నిర్మాణం mpxy_rpmi_service_దత్తాంశం mm_srvcdata[] = {
	[0] = {
		.id = RPMI_MM_SRV_ENABLE_NOTIFICATION,
		.min_tx_len = పరిమాణం(నిర్మాణం rpmi_enable_notification_req),
		.max_tx_len = పరిమాణం(నిర్మాణం rpmi_enable_notification_req),
		.min_rx_len = పరిమాణం(నిర్మాణం rpmi_enable_notification_resp),
		.max_rx_len = పరిమాణం(నిర్మాణం rpmi_enable_notification_resp),
	},
	[1] = {
		.id = RPMI_MM_SRV_GET_లక్షణాలు,
		.min_tx_len = 0,
		.max_tx_len = 0,
		.min_rx_len = పరిమాణం(నిర్మాణం rpmi_mm_get_లక్షణాలు_rsp),
		.max_rx_len = పరిమాణం(నిర్మాణం rpmi_mm_get_లక్షణాలు_rsp),
	},
	[2] = {
		.id = RPMI_MM_SRV_COMMUNICATE,
		.min_tx_len = పరిమాణం(నిర్మాణం rpmi_mm_communicate_req),
		.max_tx_len = పరిమాణం(నిర్మాణం rpmi_mm_communicate_req),
		.min_rx_len = పరిమాణం(నిర్మాణం rpmi_mm_communicate_rsp),
		.max_rx_len = పరిమాణం(నిర్మాణం rpmi_mm_communicate_rsp),
	},
};

స్థిర పూర్ణాంకం mpxy_rpmi_mm_అమరిక(శూన్య **context, నిర్మాణం mbox_chan *chan,
			      స్థిరరాశి నిర్మాణం mpxy_rpmi_mbox_దత్తాంశం *దత్తాంశం)
{
	గుర్తులేని దీర్ఘ mm_region_addr = 0;
	గుర్తులేని దీర్ఘ mm_region_కొలత = 0;
	గుర్తులేని దీర్ఘ mm_region_flags;
	పూర్ణాంకం rc = 0;

	rc = rpmi_సాధారణ_request_with_స్థితి(chan, RPMI_MM_SRV_GET_లక్షణాలు,
					     ఖాళీ, 0, 0, &rsp,
					     rpmi_u32_లెక్క(rsp),
					     rpmi_u32_లెక్క(rsp));
	ఒకవేళ (rc)
		తిరిగి rc;

#ఒకవేళ __riscv_xlen == 32
	mm_region_addr = rsp.mma.shmem_addr_lo;
#లేకపోతే
	mm_region_addr = ((గుర్తులేని దీర్ఘ)(rsp.mma.shmem_addr_hi) << 32) |
	    rsp.mma.shmem_addr_lo;
#అయితే_ముగింపు

	mm_region_కొలత = rsp.mma.shmem_కొలత;
	mm_region_flags = SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW;

	rc = sbi_domain_మూలం_కూడు_memrange(mm_region_addr, mm_region_కొలత,
					  పుట_కొలత, mm_region_flags);
	తిరిగి rc;
}

స్థిర పూర్ణాంకం mpxy_rpmi_mm_xfer(శూన్య *context, నిర్మాణం mbox_chan *chan,
			     నిర్మాణం mbox_xfer *xfer)
{
	నిర్మాణం rpmi_సందేశం_వాదనలు *వాదనలు = xfer->వాదనలు;
	పూర్ణాంకం rc = 0;

	ఒకవేళ (!xfer->rx || (వాదనలు->type != RPMI_సందేశం_సాధారణ_REQUEST))
		తిరిగి 0;

	ఎంపిక (వాదనలు->service_id) {
	స్థితి RPMI_MM_SRV_GET_లక్షణాలు:
		((u32 *)xfer->rx)[0] = cpu_గమ్యం_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_గమ్యం_le32(rsp.mma.mm_సంస్కరణ);
		((u32 *)xfer->rx)[2] = cpu_గమ్యం_le32(rsp.mma.shmem_addr_lo);
		((u32 *)xfer->rx)[3] = cpu_గమ్యం_le32(rsp.mma.shmem_addr_hi);
		((u32 *)xfer->rx)[4] = cpu_గమ్యం_le32(rsp.mma.shmem_కొలత);
		వాదనలు->rx_దత్తాంశం_len = 5 * పరిమాణం(u32);
		ఆపు;

	స్థితి RPMI_MM_SRV_COMMUNICATE:
		rc = mbox_chan_xfer(chan, xfer);
		ఆపు;

	అప్రమేయ:
		((u32 *)xfer->rx)[0] = cpu_గమ్యం_le32(RPMI_ERR_NOTSUPP);
		వాదనలు->rx_దత్తాంశం_len = పరిమాణం(u32);
		ఆపు;
	};

	తిరిగి rc;
}

స్థిర స్థిరరాశి నిర్మాణం mpxy_rpmi_mbox_దత్తాంశం mm_దత్తాంశం = {
	.servicegrp_id = RPMI_SRVGRP_MANAGEMENT_బహుళకం,
	.అంకె_services = RPMI_MM_SRV_MAX_లెక్క,
	.service_దత్తాంశం = mm_srvcdata,
	.అమరిక_సమూహం = mpxy_rpmi_mm_అమరిక,
	.xfer_సమూహం = mpxy_rpmi_mm_xfer,
};

/* one extra blank entry for loop termination while matching */
స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక mm_సరిపోలిక[] = {
	{
		.compatible = "riscv,rpmi-mpxy-mm",
		.దత్తాంశం = &mm_దత్తాంశం,
	},
	{},
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_mpxy_rpmi_mm = {
	.experimental = true,
	.సరిపోలిక_పట్టిక = mm_సరిపోలిక,
	.init = mpxy_rpmi_mbox_init,
};
