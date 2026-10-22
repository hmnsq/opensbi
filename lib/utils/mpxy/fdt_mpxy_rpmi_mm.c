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

स्थिर संरचना rpmi_mm_get_गुणाः_rsp rsp;

स्थिर संरचना mpxy_rpmi_service_दत्त mm_srvcdata[] = {
	[0] = {
		.id = RPMI_MM_SRV_ENABLE_NOTIFICATION,
		.min_tx_len = परिमाण(संरचना rpmi_enable_notification_req),
		.max_tx_len = परिमाण(संरचना rpmi_enable_notification_req),
		.min_rx_len = परिमाण(संरचना rpmi_enable_notification_resp),
		.max_rx_len = परिमाण(संरचना rpmi_enable_notification_resp),
	},
	[1] = {
		.id = RPMI_MM_SRV_GET_गुणाः,
		.min_tx_len = 0,
		.max_tx_len = 0,
		.min_rx_len = परिमाण(संरचना rpmi_mm_get_गुणाः_rsp),
		.max_rx_len = परिमाण(संरचना rpmi_mm_get_गुणाः_rsp),
	},
	[2] = {
		.id = RPMI_MM_SRV_COMMUNICATE,
		.min_tx_len = परिमाण(संरचना rpmi_mm_communicate_req),
		.max_tx_len = परिमाण(संरचना rpmi_mm_communicate_req),
		.min_rx_len = परिमाण(संरचना rpmi_mm_communicate_rsp),
		.max_rx_len = परिमाण(संरचना rpmi_mm_communicate_rsp),
	},
};

स्थिर पूर्णाङ्क mpxy_rpmi_mm_सज्जता(शून्य **context, संरचना mbox_chan *chan,
			      ध्रुव संरचना mpxy_rpmi_mbox_दत्त *दत्त)
{
	अचिह्नित दीर्घ mm_region_addr = 0;
	अचिह्नित दीर्घ mm_region_आकार = 0;
	अचिह्नित दीर्घ mm_region_flags;
	पूर्णाङ्क rc = 0;

	rc = rpmi_प्रसामान्यम्_request_with_स्थितिः(chan, RPMI_MM_SRV_GET_गुणाः,
					     रिक्त, 0, 0, &rsp,
					     rpmi_u32_गणनम्(rsp),
					     rpmi_u32_गणनम्(rsp));
	यदि (rc)
		प्रत्यागम rc;

#यदि __riscv_xlen == 32
	mm_region_addr = rsp.mma.shmem_addr_lo;
#अन्यथा
	mm_region_addr = ((अचिह्नित दीर्घ)(rsp.mma.shmem_addr_hi) << 32) |
	    rsp.mma.shmem_addr_lo;
#यदि_अन्तः

	mm_region_आकार = rsp.mma.shmem_आकार;
	mm_region_flags = SBI_DOMAIN_MEMREGION_SHARED_SURW_MRW;

	rc = sbi_domain_मूलम्_सङ्कलय_memrange(mm_region_addr, mm_region_आकार,
					  PAGE_आकार, mm_region_flags);
	प्रत्यागम rc;
}

स्थिर पूर्णाङ्क mpxy_rpmi_mm_xfer(शून्य *context, संरचना mbox_chan *chan,
			     संरचना mbox_xfer *xfer)
{
	संरचना rpmi_सन्देशः_तर्काः *तर्काः = xfer->तर्काः;
	पूर्णाङ्क rc = 0;

	यदि (!xfer->rx || (तर्काः->type != RPMI_सन्देशः_प्रसामान्यम्_REQUEST))
		प्रत्यागम 0;

	विकल्प (तर्काः->service_id) {
	पक्ष RPMI_MM_SRV_GET_गुणाः:
		((u32 *)xfer->rx)[0] = cpu_गन्तव्यम्_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_गन्तव्यम्_le32(rsp.mma.mm_संस्करणम्);
		((u32 *)xfer->rx)[2] = cpu_गन्तव्यम्_le32(rsp.mma.shmem_addr_lo);
		((u32 *)xfer->rx)[3] = cpu_गन्तव्यम्_le32(rsp.mma.shmem_addr_hi);
		((u32 *)xfer->rx)[4] = cpu_गन्तव्यम्_le32(rsp.mma.shmem_आकार);
		तर्काः->rx_दत्त_len = 5 * परिमाण(u32);
		भङ्ग;

	पक्ष RPMI_MM_SRV_COMMUNICATE:
		rc = mbox_chan_xfer(chan, xfer);
		भङ्ग;

	स्वभाव:
		((u32 *)xfer->rx)[0] = cpu_गन्तव्यम्_le32(RPMI_ERR_NOTSUPP);
		तर्काः->rx_दत्त_len = परिमाण(u32);
		भङ्ग;
	};

	प्रत्यागम rc;
}

स्थिर ध्रुव संरचना mpxy_rpmi_mbox_दत्त mm_दत्त = {
	.servicegrp_id = RPMI_SRVGRP_MANAGEMENT_बहुलकम्,
	.अङ्कः_services = RPMI_MM_SRV_MAX_गणनम्,
	.service_दत्त = mm_srvcdata,
	.सज्जता_गणः = mpxy_rpmi_mm_सज्जता,
	.xfer_गणः = mpxy_rpmi_mm_xfer,
};

/* one extra blank entry for loop termination while matching */
स्थिर ध्रुव संरचना fdt_मेलनम् mm_मेलनम्[] = {
	{
		.compatible = "riscv,rpmi-mpxy-mm",
		.दत्त = &mm_दत्त,
	},
	{},
};

ध्रुव संरचना fdt_driver fdt_mpxy_rpmi_mm = {
	.experimental = true,
	.मेलनम्_सारणी = mm_मेलनम्,
	.init = mpxy_rpmi_mbox_init,
};
