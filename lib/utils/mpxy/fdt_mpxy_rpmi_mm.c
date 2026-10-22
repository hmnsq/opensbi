/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Qualcomm Technologies, Inc.
 *
 * Authors:
 *   Ranbir Singh <ranbir.singh@oss.qualcomm.com>
 *   Sunil V L <sunilvl@oss.qualcomm.com>
 */

#समावेश <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>
#समावेश <sbi_utils/mailbox/rpmi_mailbox.h>

अटल काष्ठा rpmi_mm_get_attributes_rsp rsp;

अटल काष्ठा mpxy_rpmi_service_तथ्य mm_srvcdata[] = {
	[0] = {
		.id = RPMI_MM_SRV_ENABLE_NOTIFICATION,
		.min_tx_len = माप(काष्ठा rpmi_enable_notification_req),
		.max_tx_len = माप(काष्ठा rpmi_enable_notification_req),
		.min_rx_len = माप(काष्ठा rpmi_enable_notification_resp),
		.max_rx_len = माप(काष्ठा rpmi_enable_notification_resp),
	},
	[1] = {
		.id = RPMI_MM_SRV_GET_ATTRIBUTES,
		.min_tx_len = 0,
		.max_tx_len = 0,
		.min_rx_len = माप(काष्ठा rpmi_mm_get_attributes_rsp),
		.max_rx_len = माप(काष्ठा rpmi_mm_get_attributes_rsp),
	},
	[2] = {
		.id = RPMI_MM_SRV_COMMUNICATE,
		.min_tx_len = माप(काष्ठा rpmi_mm_communicate_req),
		.max_tx_len = माप(काष्ठा rpmi_mm_communicate_req),
		.min_rx_len = माप(काष्ठा rpmi_mm_communicate_rsp),
		.max_rx_len = माप(काष्ठा rpmi_mm_communicate_rsp),
	},
};

अटल पूर्णांक mpxy_rpmi_mm_setup(व्योम **context, काष्ठा mbox_chan *chan,
			      स्थिर काष्ठा mpxy_rpmi_mbox_तथ्य *तथ्य)
{
	अचिन्हित दीर्घ mm_region_addr = 0;
	अचिन्हित दीर्घ mm_region_आकार = 0;
	अचिन्हित दीर्घ mm_region_flags;
	पूर्णांक rc = 0;

	rc = rpmi_normal_request_with_status(chan, RPMI_MM_SRV_GET_ATTRIBUTES,
					     शून्य, 0, 0, &rsp,
					     rpmi_u32_गिनती(rsp),
					     rpmi_u32_गिनती(rsp));
	अगर (rc)
		वापस rc;

#अगर __riscv_xlen == 32
	mm_region_addr = rsp.mma.shmem_addr_lo;
#अन्यथा
	mm_region_addr = ((अचिन्हित दीर्घ)(rsp.mma.shmem_addr_hi) << 32) |
	    rsp.mma.shmem_addr_lo;
#पूर्ण_अगर

	mm_region_आकार = rsp.mma.shmem_आकार;
	mm_region_flags = SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW;

	rc = sbi_domain_मूल_add_memrange(mm_region_addr, mm_region_आकार,
					  PAGE_आकार, mm_region_flags);
	वापस rc;
}

अटल पूर्णांक mpxy_rpmi_mm_xfer(व्योम *context, काष्ठा mbox_chan *chan,
			     काष्ठा mbox_xfer *xfer)
{
	काष्ठा rpmi_message_args *args = xfer->args;
	पूर्णांक rc = 0;

	अगर (!xfer->rx || (args->type != RPMI_संदेश_NORMAL_REQUEST))
		वापस 0;

	चयन (args->service_id) {
	हाल RPMI_MM_SRV_GET_ATTRIBUTES:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_to_le32(rsp.mma.mm_version);
		((u32 *)xfer->rx)[2] = cpu_to_le32(rsp.mma.shmem_addr_lo);
		((u32 *)xfer->rx)[3] = cpu_to_le32(rsp.mma.shmem_addr_hi);
		((u32 *)xfer->rx)[4] = cpu_to_le32(rsp.mma.shmem_आकार);
		args->rx_तथ्य_len = 5 * माप(u32);
		अवरोध;

	हाल RPMI_MM_SRV_COMMUNICATE:
		rc = mbox_chan_xfer(chan, xfer);
		अवरोध;

	शेष:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_NOTSUPP);
		args->rx_तथ्य_len = माप(u32);
		अवरोध;
	};

	वापस rc;
}

अटल स्थिर काष्ठा mpxy_rpmi_mbox_तथ्य mm_तथ्य = {
	.servicegrp_id = RPMI_SRVGRP_MANAGEMENT_MODE,
	.अंक_services = RPMI_MM_SRV_MAX_गिनती,
	.service_तथ्य = mm_srvcdata,
	.setup_group = mpxy_rpmi_mm_setup,
	.xfer_group = mpxy_rpmi_mm_xfer,
};

/* one extra blank entry for loop termination while matching */
अटल स्थिर काष्ठा fdt_match mm_match[] = {
	{
		.compatible = "riscv,rpmi-mpxy-mm",
		.तथ्य = &mm_तथ्य,
	},
	{},
};

स्थिर काष्ठा fdt_driver fdt_mpxy_rpmi_mm = {
	.experimental = true,
	.match_table = mm_match,
	.init = mpxy_rpmi_mbox_init,
};
