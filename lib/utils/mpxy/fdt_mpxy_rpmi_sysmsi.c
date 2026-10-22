/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#ضمن <sbi/sbi_bitmap.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/sbi_byteorder.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi_utils/mailbox/rpmi_mailbox.h>
#ضمن <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

بنية mpxy_rpmi_sysmsi {
	u32 sys_انگ_msi;
	بنانشان ڊگهو *sys_msi_denied_bmap;
};

ساكن صحيح mpxy_rpmi_sysmis_xfer(فراغ *context, بنية mbox_chan *chan,
				 بنية mbox_xfer *xfer)
{
	بنية rpmi_پيغام_دليلون *دليلون = xfer->دليلون;
	بنية mpxy_rpmi_sysmsi *smg = context;
	u64 sys_msi_پتو;
	u32 sys_msi_اشاريو;
	صحيح rc = 0;

	إذا (!xfer->rx || دليلون->type != RPMI_نياپو_عام_REQUEST)
		أرجع 0;

	بدل (دليلون->service_id) {
	حالة RPMI_SYSMSI_SRV_GET_خاصيتون:
		((u32 *)xfer->rx)[0] = cpu_منزل_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_منزل_le32(smg->sys_انگ_msi);
		((u32 *)xfer->rx)[2] = 0;
		((u32 *)xfer->rx)[3] = 0;
		دليلون->rx_معلومات_len = 4 * حجم(u32);
		اكسر;
	حالة RPMI_SYSMSI_SRV_GET_MSI_خاصيتون:
	حالة RPMI_SYSMSI_SRV_سيٽ_MSI_STATE:
	حالة RPMI_SYSMSI_SRV_GET_MSI_STATE:
	حالة RPMI_SYSMSI_SRV_سيٽ_MSI_هدف:
	حالة RPMI_SYSMSI_SRV_GET_MSI_هدف:
		sys_msi_اشاريو = le32_منزل_cpu(((u32 *)xfer->tx)[0]);
		إذا (smg->sys_انگ_msi <= sys_msi_اشاريو) {
			((u32 *)xfer->rx)[0] = cpu_منزل_le32(RPMI_ERR_INVALID_PARAM);
			دليلون->rx_معلومات_len = حجم(u32);
			اكسر;
		}
		إذا (bitmap_test(smg->sys_msi_denied_bmap, sys_msi_اشاريو)) {
			((u32 *)xfer->rx)[0] = cpu_منزل_le32(RPMI_ERR_DENIED);
			دليلون->rx_معلومات_len = حجم(u32);
			اكسر;
		}
		إذا (دليلون->service_id == RPMI_SYSMSI_SRV_سيٽ_MSI_هدف) {
			sys_msi_پتو = le32_منزل_cpu(((u32 *)xfer->tx)[1]);
			sys_msi_پتو |= ((u64)le32_منزل_cpu(((u32 *)xfer->tx)[2])) << 32;
			إذا (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
							 sys_msi_پتو, 0x4, PRV_S,
							 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE |
							 SBI_DOMAIN_MMIO)) {
				((u32 *)xfer->rx)[0] = cpu_منزل_le32(RPMI_ERR_INVALID_ADDR);
				دليلون->rx_معلومات_len = حجم(u32);
				اكسر;
			}
		}
		rc = mbox_chan_xfer(chan, xfer);
		اكسر;
	افتراضي:
		((u32 *)xfer->rx)[0] = cpu_منزل_le32(RPMI_ERR_NOTSUPP);
		دليلون->rx_معلومات_len = حجم(u32);
		اكسر;
	};

	أرجع rc;
}

ساكن فراغ mpxy_rpmi_sysmsi_cleanup(فراغ *context)
{
	بنية mpxy_rpmi_sysmsi *smg = context;

	sbi_free(smg->sys_msi_denied_bmap);
	sbi_free(smg);
}

ساكن صحيح mpxy_rpmi_sysmsi_تياري(فراغ **context, بنية mbox_chan *chan,
				  ثابت بنية mpxy_rpmi_mbox_معلومات *معلومات)
{
	بنية rpmi_sysmsi_get_msi_خاصيتون_resp gmaresp;
	بنية rpmi_sysmsi_get_msi_خاصيتون_req gmareq;
	بنية rpmi_sysmsi_get_خاصيتون_resp garesp;
	بنية mpxy_rpmi_sysmsi *smg;
	u32 p2a_db_اشاريو;
	صحيح rc, i;

	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_P2_A_DOORBELL_SYSMSI_اشاريو,
				     &p2a_db_اشاريو);
	إذا (rc)
		أرجع rc;

	rc = rpmi_عام_request_with_حالت(chan, RPMI_SYSMSI_SRV_GET_خاصيتون,
					     عدم, 0, 0, &garesp, rpmi_u32_ڳڻپ(garesp),
					     rpmi_u32_ڳڻپ(garesp));
	إذا (rc)
		أرجع rc;

	smg = sbi_zalloc(حجم(*smg));
	إذا (!smg)
		أرجع SBI_ENOMEM;

	smg->sys_انگ_msi = garesp.sys_انگ_msi;
	smg->sys_msi_denied_bmap = sbi_zalloc(bitmap_estimate_سائيز(smg->sys_انگ_msi));
	إذا (!smg->sys_msi_denied_bmap) {
		sbi_free(smg);
		أرجع SBI_ENOMEM;
	}

	لكل (i = 0; i < smg->sys_انگ_msi; i++) {
		gmareq.sys_msi_اشاريو = i;
		rc = rpmi_عام_request_with_حالت(chan,
						     RPMI_SYSMSI_SRV_GET_MSI_خاصيتون,
						     &gmareq, rpmi_u32_ڳڻپ(gmareq),
						     rpmi_u32_ڳڻپ(gmareq),
						     &gmaresp, rpmi_u32_ڳڻپ(gmaresp),
						     rpmi_u32_ڳڻپ(gmaresp));
		إذا (rc) {
			mpxy_rpmi_sysmsi_cleanup(smg);
			أرجع rc;
		}

		إذا (p2a_db_اشاريو == i ||
		    (gmaresp.flag0 & RPMI_SYSMSI_MSI_خاصيتون_FLAG0_PREF_PRIV))
			bitmap_سيٽ(smg->sys_msi_denied_bmap, i, 1);
	}

	*context = smg;
	أرجع 0;
}

ساكن بنية mpxy_rpmi_service_معلومات sysmsi_services[] = {
{
	.id = RPMI_SYSMSI_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = حجم(بنية rpmi_enable_notification_req),
	.max_tx_len = حجم(بنية rpmi_enable_notification_req),
	.min_rx_len = حجم(بنية rpmi_enable_notification_resp),
	.max_rx_len = حجم(بنية rpmi_enable_notification_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_خاصيتون,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = حجم(بنية rpmi_sysmsi_get_خاصيتون_resp),
	.max_rx_len = حجم(بنية rpmi_sysmsi_get_خاصيتون_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_خاصيتون,
	.min_tx_len = حجم(بنية rpmi_sysmsi_get_msi_خاصيتون_req),
	.max_tx_len = حجم(بنية rpmi_sysmsi_get_msi_خاصيتون_req),
	.min_rx_len = حجم(بنية rpmi_sysmsi_get_msi_خاصيتون_resp),
	.max_rx_len = حجم(بنية rpmi_sysmsi_get_msi_خاصيتون_resp),
},
{
	.id = RPMI_SYSMSI_SRV_سيٽ_MSI_STATE,
	.min_tx_len = حجم(بنية rpmi_sysmsi_سيٽ_msi_state_req),
	.max_tx_len = حجم(بنية rpmi_sysmsi_سيٽ_msi_state_req),
	.min_rx_len = حجم(بنية rpmi_sysmsi_سيٽ_msi_state_resp),
	.max_rx_len = حجم(بنية rpmi_sysmsi_سيٽ_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_STATE,
	.min_tx_len = حجم(بنية rpmi_sysmsi_get_msi_state_req),
	.max_tx_len = حجم(بنية rpmi_sysmsi_get_msi_state_req),
	.min_rx_len = حجم(بنية rpmi_sysmsi_get_msi_state_resp),
	.max_rx_len = حجم(بنية rpmi_sysmsi_get_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_سيٽ_MSI_هدف,
	.min_tx_len = حجم(بنية rpmi_sysmsi_سيٽ_msi_هدف_req),
	.max_tx_len = حجم(بنية rpmi_sysmsi_سيٽ_msi_هدف_req),
	.min_rx_len = حجم(بنية rpmi_sysmsi_سيٽ_msi_هدف_resp),
	.max_rx_len = حجم(بنية rpmi_sysmsi_سيٽ_msi_هدف_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_هدف,
	.min_tx_len = حجم(بنية rpmi_sysmsi_get_msi_هدف_req),
	.max_tx_len = حجم(بنية rpmi_sysmsi_get_msi_هدف_req),
	.min_rx_len = حجم(بنية rpmi_sysmsi_get_msi_هدف_resp),
	.max_rx_len = حجم(بنية rpmi_sysmsi_get_msi_هدف_resp),
},
};

ساكن ثابت بنية mpxy_rpmi_mbox_معلومات sysmsi_معلومات = {
	.servicegrp_id = RPMI_SRVGRP_SYSTEM_MSI,
	.انگ_services = RPMI_SYSMSI_SRV_ID_MAX_ڳڻپ,
	.service_معلومات = sysmsi_services,
	.xfer_گروهه = mpxy_rpmi_sysmis_xfer,
	.تياري_گروهه = mpxy_rpmi_sysmsi_تياري,
	.cleanup_گروهه = mpxy_rpmi_sysmsi_cleanup,
};

ساكن ثابت بنية fdt_ميلاپ sysmsi_ميلاپ[] = {
	{ .compatible = "riscv,rpmi-mpxy-system-msi", .معلومات = &sysmsi_معلومات },
	{ },
};

ثابت بنية fdt_driver fdt_mpxy_rpmi_sysmsi = {
	.ميلاپ_جدول = sysmsi_ميلاپ,
	.init = mpxy_rpmi_mbox_init,
};
