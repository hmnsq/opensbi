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

ಸ್ಥಿರ ಸಂರಚನಾ rpmi_mm_get_attributes_rsp rsp;

ಸ್ಥಿರ ಸಂರಚನಾ mpxy_rpmi_service_data mm_srvcdata[] = {
	[0] = {
		.id = RPMI_MM_SRV_ENABLE_NOTIFICATION,
		.min_tx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_enable_notification_req),
		.max_tx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_enable_notification_req),
		.min_rx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_enable_notification_resp),
		.max_rx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_enable_notification_resp),
	},
	[1] = {
		.id = RPMI_MM_SRV_GET_ATTRIBUTES,
		.min_tx_len = 0,
		.max_tx_len = 0,
		.min_rx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_mm_get_attributes_rsp),
		.max_rx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_mm_get_attributes_rsp),
	},
	[2] = {
		.id = RPMI_MM_SRV_COMMUNICATE,
		.min_tx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_mm_communicate_req),
		.max_tx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_mm_communicate_req),
		.min_rx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_mm_communicate_rsp),
		.max_rx_len = ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_mm_communicate_rsp),
	},
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mpxy_rpmi_mm_setup(ಶೂನ್ಯ **context, ಸಂರಚನಾ mbox_chan *chan,
			      ಧ್ರುವ ಸಂರಚನಾ mpxy_rpmi_mbox_data *data)
{
	unsigned long mm_region_addr = 0;
	unsigned long mm_region_size = 0;
	unsigned long mm_region_flags;
	ಪೂರ್ಣಾಙ್ಕ rc = 0;

	rc = rpmi_normal_request_with_status(chan, RPMI_MM_SRV_GET_ATTRIBUTES,
					     ರಿಕ್ತ, 0, 0, &rsp,
					     rpmi_u32_ಎಣಿಕೆ(rsp),
					     rpmi_u32_ಎಣಿಕೆ(rsp));
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

#ಯದಿ __riscv_xlen == 32
	mm_region_addr = rsp.mma.shmem_addr_lo;
#ಅನ್ಯಥಾ
	mm_region_addr = ((unsigned long)(rsp.mma.shmem_addr_hi) << 32) |
	    rsp.mma.shmem_addr_lo;
#endif

	mm_region_size = rsp.mma.shmem_size;
	mm_region_flags = SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW;

	rc = sbi_domain_ಬೇರು_add_memrange(mm_region_addr, mm_region_size,
					  PAGE_SIZE, mm_region_flags);
	ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mpxy_rpmi_mm_xfer(ಶೂನ್ಯ *context, ಸಂರಚನಾ mbox_chan *chan,
			     ಸಂರಚನಾ mbox_xfer *xfer)
{
	ಸಂರಚನಾ rpmi_message_args *args = xfer->args;
	ಪೂರ್ಣಾಙ್ಕ rc = 0;

	ಯದಿ (!xfer->rx || (args->type != RPMI_ಸಂದೇಶ_NORMAL_REQUEST))
		ಪ್ರತ್ಯಾಗಮ 0;

	ವಿಕಲ್ಪ (args->service_id) {
	ಪಕ್ಷ RPMI_MM_SRV_GET_ATTRIBUTES:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_to_le32(rsp.mma.mm_version);
		((u32 *)xfer->rx)[2] = cpu_to_le32(rsp.mma.shmem_addr_lo);
		((u32 *)xfer->rx)[3] = cpu_to_le32(rsp.mma.shmem_addr_hi);
		((u32 *)xfer->rx)[4] = cpu_to_le32(rsp.mma.shmem_size);
		args->rx_data_len = 5 * ಪರಿಮಾಣ(u32);
		ಭಙ್ಗ;

	ಪಕ್ಷ RPMI_MM_SRV_COMMUNICATE:
		rc = mbox_chan_xfer(chan, xfer);
		ಭಙ್ಗ;

	ಸ್ವಭಾವ:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_NOTSUPP);
		args->rx_data_len = ಪರಿಮಾಣ(u32);
		ಭಙ್ಗ;
	};

	ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ mpxy_rpmi_mbox_data mm_data = {
	.servicegrp_id = RPMI_SRVGRP_MANAGEMENT_MODE,
	.ಅಂಕೆ_services = RPMI_MM_SRV_MAX_ಎಣಿಕೆ,
	.service_data = mm_srvcdata,
	.setup_group = mpxy_rpmi_mm_setup,
	.xfer_group = mpxy_rpmi_mm_xfer,
};

/* one extra blank entry for loop termination while matching */
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match mm_match[] = {
	{
		.compatible = "riscv,rpmi-mpxy-mm",
		.data = &mm_data,
	},
	{},
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_mpxy_rpmi_mm = {
	.experimental = true,
	.match_table = mm_match,
	.init = mpxy_rpmi_mbox_init,
};
