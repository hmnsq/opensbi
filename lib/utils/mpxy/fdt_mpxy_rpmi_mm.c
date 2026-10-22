/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Qualcomm Technologies, Inc.
 *
 * Authors:
 *   Ranbir Singh <ranbir.singh@oss.qualcomm.com>
 *   Sunil V L <sunilvl@oss.qualcomm.com>
 */

#अन्तर्भाव <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>
#अन्तर्भाव <sbi_utils/mailbox/rpmi_mailbox.h>

स्थिर संरचना rpmi_mm_get_attributes_rsp rsp;

स्थिर संरचना mpxy_rpmi_service_data mm_srvcdata[] = {
	[0] = {
		.id = RPMI_MM_SRV_ENABLE_NOTIFICATION,
		.min_tx_len = परिमाण(संरचना rpmi_enable_notification_req),
		.max_tx_len = परिमाण(संरचना rpmi_enable_notification_req),
		.min_rx_len = परिमाण(संरचना rpmi_enable_notification_resp),
		.max_rx_len = परिमाण(संरचना rpmi_enable_notification_resp),
	},
	[1] = {
		.id = RPMI_MM_SRV_GET_ATTRIBUTES,
		.min_tx_len = 0,
		.max_tx_len = 0,
		.min_rx_len = परिमाण(संरचना rpmi_mm_get_attributes_rsp),
		.max_rx_len = परिमाण(संरचना rpmi_mm_get_attributes_rsp),
	},
	[2] = {
		.id = RPMI_MM_SRV_COMMUNICATE,
		.min_tx_len = परिमाण(संरचना rpmi_mm_communicate_req),
		.max_tx_len = परिमाण(संरचना rpmi_mm_communicate_req),
		.min_rx_len = परिमाण(संरचना rpmi_mm_communicate_rsp),
		.max_rx_len = परिमाण(संरचना rpmi_mm_communicate_rsp),
	},
};

स्थिर पूर्णाङ्क mpxy_rpmi_mm_setup(शून्य **context, संरचना mbox_chan *chan,
			      अचर संरचना mpxy_rpmi_mbox_data *data)
{
	अचिन्हित दीर्घ mm_region_addr = 0;
	अचिन्हित दीर्घ mm_region_size = 0;
	अचिन्हित दीर्घ mm_region_flags;
	पूर्णाङ्क rc = 0;

	rc = rpmi_normal_request_with_status(chan, RPMI_MM_SRV_GET_ATTRIBUTES,
					     रिक्त, 0, 0, &rsp,
					     rpmi_u32_गन्ती(rsp),
					     rpmi_u32_गन्ती(rsp));
	यदि (rc)
		फर्काऊ rc;

#यदि __riscv_xlen == 32
	mm_region_addr = rsp.mma.shmem_addr_lo;
#अन्यथा
	mm_region_addr = ((अचिन्हित दीर्घ)(rsp.mma.shmem_addr_hi) << 32) |
	    rsp.mma.shmem_addr_lo;
#endif

	mm_region_size = rsp.mma.shmem_size;
	mm_region_flags = SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW;

	rc = sbi_domain_जरा_add_memrange(mm_region_addr, mm_region_size,
					  PAGE_SIZE, mm_region_flags);
	फर्काऊ rc;
}

स्थिर पूर्णाङ्क mpxy_rpmi_mm_xfer(शून्य *context, संरचना mbox_chan *chan,
			     संरचना mbox_xfer *xfer)
{
	संरचना rpmi_message_args *args = xfer->args;
	पूर्णाङ्क rc = 0;

	यदि (!xfer->rx || (args->type != RPMI_सन्देश_NORMAL_REQUEST))
		फर्काऊ 0;

	विकल्प (args->service_id) {
	पक्ष RPMI_MM_SRV_GET_ATTRIBUTES:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_to_le32(rsp.mma.mm_version);
		((u32 *)xfer->rx)[2] = cpu_to_le32(rsp.mma.shmem_addr_lo);
		((u32 *)xfer->rx)[3] = cpu_to_le32(rsp.mma.shmem_addr_hi);
		((u32 *)xfer->rx)[4] = cpu_to_le32(rsp.mma.shmem_size);
		args->rx_data_len = 5 * परिमाण(u32);
		रोक;

	पक्ष RPMI_MM_SRV_COMMUNICATE:
		rc = mbox_chan_xfer(chan, xfer);
		रोक;

	स्वभाव:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_NOTSUPP);
		args->rx_data_len = परिमाण(u32);
		रोक;
	};

	फर्काऊ rc;
}

स्थिर अचर संरचना mpxy_rpmi_mbox_data mm_data = {
	.servicegrp_id = RPMI_SRVGRP_MANAGEMENT_MODE,
	.अङ्क_services = RPMI_MM_SRV_MAX_गन्ती,
	.service_data = mm_srvcdata,
	.setup_group = mpxy_rpmi_mm_setup,
	.xfer_group = mpxy_rpmi_mm_xfer,
};

/* one extra blank entry for loop termination while matching */
स्थिर अचर संरचना fdt_match mm_match[] = {
	{
		.compatible = "riscv,rpmi-mpxy-mm",
		.data = &mm_data,
	},
	{},
};

अचर संरचना fdt_driver fdt_mpxy_rpmi_mm = {
	.experimental = true,
	.match_table = mm_match,
	.init = mpxy_rpmi_mbox_init,
};
