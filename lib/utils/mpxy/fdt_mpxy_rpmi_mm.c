/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Qualcomm Technologies, Inc.
 *
 * Authors:
 *   Ranbir Singh <ranbir.singh@oss.qualcomm.com>
 *   Sunil V L <sunilvl@oss.qualcomm.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/mailbox/rpmi_mailbox.h>

ସ୍ଥିର ସଂରଚନା rpmi_mm_get_attributes_rsp rsp;

ସ୍ଥିର ସଂରଚନା mpxy_rpmi_service_data mm_srvcdata[] = {
	[0] = {
		.id = RPMI_MM_SRV_ENABLE_NOTIFICATION,
		.min_tx_len = ଆକାର(ସଂରଚନା rpmi_enable_notification_req),
		.max_tx_len = ଆକାର(ସଂରଚନା rpmi_enable_notification_req),
		.min_rx_len = ଆକାର(ସଂରଚନା rpmi_enable_notification_resp),
		.max_rx_len = ଆକାର(ସଂରଚନା rpmi_enable_notification_resp),
	},
	[1] = {
		.id = RPMI_MM_SRV_GET_ATTRIBUTES,
		.min_tx_len = 0,
		.max_tx_len = 0,
		.min_rx_len = ଆକାର(ସଂରଚନା rpmi_mm_get_attributes_rsp),
		.max_rx_len = ଆକାର(ସଂରଚନା rpmi_mm_get_attributes_rsp),
	},
	[2] = {
		.id = RPMI_MM_SRV_COMMUNICATE,
		.min_tx_len = ଆକାର(ସଂରଚନା rpmi_mm_communicate_req),
		.max_tx_len = ଆକାର(ସଂରଚନା rpmi_mm_communicate_req),
		.min_rx_len = ଆକାର(ସଂରଚନା rpmi_mm_communicate_rsp),
		.max_rx_len = ଆକାର(ସଂରଚନା rpmi_mm_communicate_rsp),
	},
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mpxy_rpmi_mm_setup(ଶୂନ୍ୟ **context, ସଂରଚନା mbox_chan *chan,
			      ଅଚଳ ସଂରଚନା mpxy_rpmi_mbox_data *data)
{
	unsigned long mm_region_addr = 0;
	unsigned long mm_region_size = 0;
	unsigned long mm_region_flags;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;

	rc = rpmi_normal_request_with_status(chan, RPMI_MM_SRV_GET_ATTRIBUTES,
					     ଖାଲି, 0, 0, &rsp,
					     rpmi_u32_ଗଣନା(rsp),
					     rpmi_u32_ଗଣନା(rsp));
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

#ଯଦି __riscv_xlen == 32
	mm_region_addr = rsp.mma.shmem_addr_lo;
#ଅନ୍ୟଥା
	mm_region_addr = ((unsigned long)(rsp.mma.shmem_addr_hi) << 32) |
	    rsp.mma.shmem_addr_lo;
#endif

	mm_region_size = rsp.mma.shmem_size;
	mm_region_flags = SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW;

	rc = sbi_domain_ମୂଳ_add_memrange(mm_region_addr, mm_region_size,
					  PAGE_SIZE, mm_region_flags);
	ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mpxy_rpmi_mm_xfer(ଶୂନ୍ୟ *context, ସଂରଚନା mbox_chan *chan,
			     ସଂରଚନା mbox_xfer *xfer)
{
	ସଂରଚନା rpmi_message_args *args = xfer->args;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;

	ଯଦି (!xfer->rx || (args->type != RPMI_ସନ୍ଦେଶ_NORMAL_REQUEST))
		ଫେରନ୍ତୁ 0;

	ବାଛ (args->service_id) {
	ସ୍ଥିତି RPMI_MM_SRV_GET_ATTRIBUTES:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_to_le32(rsp.mma.mm_version);
		((u32 *)xfer->rx)[2] = cpu_to_le32(rsp.mma.shmem_addr_lo);
		((u32 *)xfer->rx)[3] = cpu_to_le32(rsp.mma.shmem_addr_hi);
		((u32 *)xfer->rx)[4] = cpu_to_le32(rsp.mma.shmem_size);
		args->rx_data_len = 5 * ଆକାର(u32);
		ଭାଙ୍ଗ;

	ସ୍ଥିତି RPMI_MM_SRV_COMMUNICATE:
		rc = mbox_chan_xfer(chan, xfer);
		ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_NOTSUPP);
		args->rx_data_len = ଆକାର(u32);
		ଭାଙ୍ଗ;
	};

	ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା mpxy_rpmi_mbox_data mm_data = {
	.servicegrp_id = RPMI_SRVGRP_MANAGEMENT_MODE,
	.ଅଙ୍କ_services = RPMI_MM_SRV_MAX_ଗଣନା,
	.service_data = mm_srvcdata,
	.setup_group = mpxy_rpmi_mm_setup,
	.xfer_group = mpxy_rpmi_mm_xfer,
};

/* one extra blank entry for loop termination while matching */
ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match mm_match[] = {
	{
		.compatible = "riscv,rpmi-mpxy-mm",
		.data = &mm_data,
	},
	{},
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_mpxy_rpmi_mm = {
	.experimental = true,
	.match_table = mm_match,
	.init = mpxy_rpmi_mbox_init,
};
