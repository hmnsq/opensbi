/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Qualcomm Technologies, Inc.
 *
 * Authors:
 *   Ranbir Singh <ranbir.singh@oss.qualcomm.com>
 *   Sunil V L <sunilvl@oss.qualcomm.com>
 */

#അന്തര്ഭാവ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>
#അന്തര്ഭാവ <sbi_utils/mailbox/rpmi_mailbox.h>

സ്ഥിതം ഘടന rpmi_mm_get_attributes_rsp rsp;

സ്ഥിതം ഘടന mpxy_rpmi_service_data mm_srvcdata[] = {
	[0] = {
		.id = RPMI_MM_SRV_ENABLE_NOTIFICATION,
		.min_tx_len = വലുപ്പം(ഘടന rpmi_enable_notification_req),
		.max_tx_len = വലുപ്പം(ഘടന rpmi_enable_notification_req),
		.min_rx_len = വലുപ്പം(ഘടന rpmi_enable_notification_resp),
		.max_rx_len = വലുപ്പം(ഘടന rpmi_enable_notification_resp),
	},
	[1] = {
		.id = RPMI_MM_SRV_GET_ATTRIBUTES,
		.min_tx_len = 0,
		.max_tx_len = 0,
		.min_rx_len = വലുപ്പം(ഘടന rpmi_mm_get_attributes_rsp),
		.max_rx_len = വലുപ്പം(ഘടന rpmi_mm_get_attributes_rsp),
	},
	[2] = {
		.id = RPMI_MM_SRV_COMMUNICATE,
		.min_tx_len = വലുപ്പം(ഘടന rpmi_mm_communicate_req),
		.max_tx_len = വലുപ്പം(ഘടന rpmi_mm_communicate_req),
		.min_rx_len = വലുപ്പം(ഘടന rpmi_mm_communicate_rsp),
		.max_rx_len = വലുപ്പം(ഘടന rpmi_mm_communicate_rsp),
	},
};

സ്ഥിതം പൂർണ്ണസംഖ്യ mpxy_rpmi_mm_setup(ശൂന്യം **context, ഘടന mbox_chan *chan,
			      സ്ഥിരം ഘടന mpxy_rpmi_mbox_data *data)
{
	unsigned long mm_region_addr = 0;
	unsigned long mm_region_size = 0;
	unsigned long mm_region_flags;
	പൂർണ്ണസംഖ്യ rc = 0;

	rc = rpmi_normal_request_with_status(chan, RPMI_MM_SRV_GET_ATTRIBUTES,
					     ശൂന്യ, 0, 0, &rsp,
					     rpmi_u32_എണ്ണം(rsp),
					     rpmi_u32_എണ്ണം(rsp));
	എങ്കിൽ (rc)
		തിരികെ rc;

#എങ്കിൽ __riscv_xlen == 32
	mm_region_addr = rsp.mma.shmem_addr_lo;
#അല്ലെങ്കിൽ
	mm_region_addr = ((unsigned long)(rsp.mma.shmem_addr_hi) << 32) |
	    rsp.mma.shmem_addr_lo;
#endif

	mm_region_size = rsp.mma.shmem_size;
	mm_region_flags = SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW;

	rc = sbi_domain_വേര്_add_memrange(mm_region_addr, mm_region_size,
					  PAGE_SIZE, mm_region_flags);
	തിരികെ rc;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ mpxy_rpmi_mm_xfer(ശൂന്യം *context, ഘടന mbox_chan *chan,
			     ഘടന mbox_xfer *xfer)
{
	ഘടന rpmi_message_args *args = xfer->args;
	പൂർണ്ണസംഖ്യ rc = 0;

	എങ്കിൽ (!xfer->rx || (args->type != RPMI_സന്ദേശം_NORMAL_REQUEST))
		തിരികെ 0;

	തിരഞ്ഞെടുപ്പ് (args->service_id) {
	അവസ്ഥ RPMI_MM_SRV_GET_ATTRIBUTES:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_to_le32(rsp.mma.mm_version);
		((u32 *)xfer->rx)[2] = cpu_to_le32(rsp.mma.shmem_addr_lo);
		((u32 *)xfer->rx)[3] = cpu_to_le32(rsp.mma.shmem_addr_hi);
		((u32 *)xfer->rx)[4] = cpu_to_le32(rsp.mma.shmem_size);
		args->rx_data_len = 5 * വലുപ്പം(u32);
		നിർത്തുക;

	അവസ്ഥ RPMI_MM_SRV_COMMUNICATE:
		rc = mbox_chan_xfer(chan, xfer);
		നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_NOTSUPP);
		args->rx_data_len = വലുപ്പം(u32);
		നിർത്തുക;
	};

	തിരികെ rc;
}

സ്ഥിതം സ്ഥിരം ഘടന mpxy_rpmi_mbox_data mm_data = {
	.servicegrp_id = RPMI_SRVGRP_MANAGEMENT_MODE,
	.അക്കം_services = RPMI_MM_SRV_MAX_എണ്ണം,
	.service_data = mm_srvcdata,
	.setup_group = mpxy_rpmi_mm_setup,
	.xfer_group = mpxy_rpmi_mm_xfer,
};

/* one extra blank entry for loop termination while matching */
സ്ഥിതം സ്ഥിരം ഘടന fdt_match mm_match[] = {
	{
		.compatible = "riscv,rpmi-mpxy-mm",
		.data = &mm_data,
	},
	{},
};

സ്ഥിരം ഘടന fdt_driver fdt_mpxy_rpmi_mm = {
	.experimental = true,
	.match_table = mm_match,
	.init = mpxy_rpmi_mbox_init,
};
