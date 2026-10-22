/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Qualcomm Technologies, Inc.
 *
 * Authors:
 *   Ranbir Singh <ranbir.singh@oss.qualcomm.com>
 *   Sunil V L <sunilvl@oss.qualcomm.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/mailbox/rpmi_mailbox.h>

ಸ್ಥಿರ ರಚನೆ rpmi_mm_get_ಗುಣಲಕ್ಷಣಗಳು_rsp rsp;

ಸ್ಥಿರ ರಚನೆ mpxy_rpmi_service_ದತ್ತಾಂಶ mm_srvcdata[] = {
	[0] = {
		.id = RPMI_MM_SRV_ENABLE_NOTIFICATION,
		.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_enable_notification_req),
		.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_enable_notification_req),
		.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_enable_notification_resp),
		.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_enable_notification_resp),
	},
	[1] = {
		.id = RPMI_MM_SRV_GET_ಗುಣಲಕ್ಷಣಗಳು,
		.min_tx_len = 0,
		.max_tx_len = 0,
		.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_mm_get_ಗುಣಲಕ್ಷಣಗಳು_rsp),
		.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_mm_get_ಗುಣಲಕ್ಷಣಗಳು_rsp),
	},
	[2] = {
		.id = RPMI_MM_SRV_COMMUNICATE,
		.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_mm_communicate_req),
		.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_mm_communicate_req),
		.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_mm_communicate_rsp),
		.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_mm_communicate_rsp),
	},
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mpxy_rpmi_mm_ಸಿದ್ಧತೆ(ಶೂನ್ಯ **context, ರಚನೆ mbox_chan *chan,
			      ಸ್ಥಿರಾಂಕ ರಚನೆ mpxy_rpmi_mbox_ದತ್ತಾಂಶ *ದತ್ತಾಂಶ)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ mm_region_addr = 0;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ mm_region_ಗಾತ್ರ = 0;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ mm_region_flags;
	ಪೂರ್ಣಾಂಕ rc = 0;

	rc = rpmi_ಸಾಮಾನ್ಯ_request_with_ಸ್ಥಿತಿ(chan, RPMI_MM_SRV_GET_ಗುಣಲಕ್ಷಣಗಳು,
					     ಖಾಲಿ, 0, 0, &rsp,
					     rpmi_u32_ಎಣಿಕೆ(rsp),
					     rpmi_u32_ಎಣಿಕೆ(rsp));
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

#ಆದರೆ __riscv_xlen == 32
	mm_region_addr = rsp.mma.shmem_addr_lo;
#ಇಲ್ಲದಿದ್ದರೆ
	mm_region_addr = ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)(rsp.mma.shmem_addr_hi) << 32) |
	    rsp.mma.shmem_addr_lo;
#ಆದರೆ_ಅಂತ್ಯ

	mm_region_ಗಾತ್ರ = rsp.mma.shmem_ಗಾತ್ರ;
	mm_region_flags = SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW;

	rc = sbi_domain_ಬೇರು_ಕೂಡಿಸು_memrange(mm_region_addr, mm_region_ಗಾತ್ರ,
					  ಪುಟ_ಗಾತ್ರ, mm_region_flags);
	ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mpxy_rpmi_mm_xfer(ಶೂನ್ಯ *context, ರಚನೆ mbox_chan *chan,
			     ರಚನೆ mbox_xfer *xfer)
{
	ರಚನೆ rpmi_ಸಂದೇಶ_ವಾದಗಳು *ವಾದಗಳು = xfer->ವಾದಗಳು;
	ಪೂರ್ಣಾಂಕ rc = 0;

	ಆದರೆ (!xfer->rx || (ವಾದಗಳು->type != RPMI_ಸಂದೇಶ_ಸಾಮಾನ್ಯ_REQUEST))
		ಹಿಂತಿರುಗಿ 0;

	ಆಯ್ಕೆ (ವಾದಗಳು->service_id) {
	ಸ್ಥಿತಿ RPMI_MM_SRV_GET_ಗುಣಲಕ್ಷಣಗಳು:
		((u32 *)xfer->rx)[0] = cpu_ಗಮ್ಯ_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_ಗಮ್ಯ_le32(rsp.mma.mm_ಆವೃತ್ತಿ);
		((u32 *)xfer->rx)[2] = cpu_ಗಮ್ಯ_le32(rsp.mma.shmem_addr_lo);
		((u32 *)xfer->rx)[3] = cpu_ಗಮ್ಯ_le32(rsp.mma.shmem_addr_hi);
		((u32 *)xfer->rx)[4] = cpu_ಗಮ್ಯ_le32(rsp.mma.shmem_ಗಾತ್ರ);
		ವಾದಗಳು->rx_ದತ್ತಾಂಶ_len = 5 * ಗಾತ್ರ(u32);
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ RPMI_MM_SRV_COMMUNICATE:
		rc = mbox_chan_xfer(chan, xfer);
		ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
		((u32 *)xfer->rx)[0] = cpu_ಗಮ್ಯ_le32(RPMI_ERR_NOTSUPP);
		ವಾದಗಳು->rx_ದತ್ತಾಂಶ_len = ಗಾತ್ರ(u32);
		ನಿಲ್ಲಿಸು;
	};

	ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ mpxy_rpmi_mbox_ದತ್ತಾಂಶ mm_ದತ್ತಾಂಶ = {
	.servicegrp_id = RPMI_SRVGRP_MANAGEMENT_ಬಹುಲಕ,
	.ಅಂಕೆ_services = RPMI_MM_SRV_MAX_ಎಣಿಕೆ,
	.service_ದತ್ತಾಂಶ = mm_srvcdata,
	.ಸಿದ್ಧತೆ_ಗುಂಪು = mpxy_rpmi_mm_ಸಿದ್ಧತೆ,
	.xfer_ಗುಂಪು = mpxy_rpmi_mm_xfer,
};

/* one extra blank entry for loop termination while matching */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ mm_ಹೊಂದಿಕೆ[] = {
	{
		.compatible = "riscv,rpmi-mpxy-mm",
		.ದತ್ತಾಂಶ = &mm_ದತ್ತಾಂಶ,
	},
	{},
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_mpxy_rpmi_mm = {
	.experimental = true,
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = mm_ಹೊಂದಿಕೆ,
	.init = mpxy_rpmi_mbox_init,
};
