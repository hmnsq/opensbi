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
	u32 sys_عدد_msi;
	بنانشانی لما *sys_msi_denied_bmap;
};

ساكن صحيح mpxy_rpmi_sysmis_xfer(فراغ *context, بنية mbox_chan *chan,
				 بنية mbox_xfer *xfer)
{
	بنية rpmi_سنیہا_دلیلاں *دلیلاں = xfer->دلیلاں;
	بنية mpxy_rpmi_sysmsi *smg = context;
	u64 sys_msi_پتہ;
	u32 sys_msi_اشاریہ;
	صحيح rc = 0;

	إذا (!xfer->rx || دلیلاں->type != RPMI_پیغام_عام_REQUEST)
		أرجع 0;

	بدل (دلیلاں->service_id) {
	حالة RPMI_SYSMSI_SRV_GET_گن:
		((u32 *)xfer->rx)[0] = cpu_منزل_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_منزل_le32(smg->sys_عدد_msi);
		((u32 *)xfer->rx)[2] = 0;
		((u32 *)xfer->rx)[3] = 0;
		دلیلاں->rx_انکڑے_len = 4 * حجم(u32);
		اكسر;
	حالة RPMI_SYSMSI_SRV_GET_MSI_گن:
	حالة RPMI_SYSMSI_SRV_سیٹ_MSI_STATE:
	حالة RPMI_SYSMSI_SRV_GET_MSI_STATE:
	حالة RPMI_SYSMSI_SRV_سیٹ_MSI_نشانہ:
	حالة RPMI_SYSMSI_SRV_GET_MSI_نشانہ:
		sys_msi_اشاریہ = le32_منزل_cpu(((u32 *)xfer->tx)[0]);
		إذا (smg->sys_عدد_msi <= sys_msi_اشاریہ) {
			((u32 *)xfer->rx)[0] = cpu_منزل_le32(RPMI_ERR_INVALID_PARAM);
			دلیلاں->rx_انکڑے_len = حجم(u32);
			اكسر;
		}
		إذا (bitmap_test(smg->sys_msi_denied_bmap, sys_msi_اشاریہ)) {
			((u32 *)xfer->rx)[0] = cpu_منزل_le32(RPMI_ERR_DENIED);
			دلیلاں->rx_انکڑے_len = حجم(u32);
			اكسر;
		}
		إذا (دلیلاں->service_id == RPMI_SYSMSI_SRV_سیٹ_MSI_نشانہ) {
			sys_msi_پتہ = le32_منزل_cpu(((u32 *)xfer->tx)[1]);
			sys_msi_پتہ |= ((u64)le32_منزل_cpu(((u32 *)xfer->tx)[2])) << 32;
			إذا (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
							 sys_msi_پتہ, 0x4, PRV_S,
							 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE |
							 SBI_DOMAIN_MMIO)) {
				((u32 *)xfer->rx)[0] = cpu_منزل_le32(RPMI_ERR_INVALID_ADDR);
				دلیلاں->rx_انکڑے_len = حجم(u32);
				اكسر;
			}
		}
		rc = mbox_chan_xfer(chan, xfer);
		اكسر;
	افتراضي:
		((u32 *)xfer->rx)[0] = cpu_منزل_le32(RPMI_ERR_NOTSUPP);
		دلیلاں->rx_انکڑے_len = حجم(u32);
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

ساكن صحيح mpxy_rpmi_sysmsi_تیاری(فراغ **context, بنية mbox_chan *chan,
				  ثابت بنية mpxy_rpmi_mbox_انکڑے *انکڑے)
{
	بنية rpmi_sysmsi_get_msi_گن_resp gmaresp;
	بنية rpmi_sysmsi_get_msi_گن_req gmareq;
	بنية rpmi_sysmsi_get_گن_resp garesp;
	بنية mpxy_rpmi_sysmsi *smg;
	u32 p2a_db_اشاریہ;
	صحيح rc, i;

	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_P2_A_DOORBELL_SYSMSI_اشاریہ,
				     &p2a_db_اشاریہ);
	إذا (rc)
		أرجع rc;

	rc = rpmi_عام_request_with_حالت(chan, RPMI_SYSMSI_SRV_GET_گن,
					     عدم, 0, 0, &garesp, rpmi_u32_گنتی(garesp),
					     rpmi_u32_گنتی(garesp));
	إذا (rc)
		أرجع rc;

	smg = sbi_zalloc(حجم(*smg));
	إذا (!smg)
		أرجع SBI_ENOMEM;

	smg->sys_عدد_msi = garesp.sys_عدد_msi;
	smg->sys_msi_denied_bmap = sbi_zalloc(bitmap_estimate_ناپ(smg->sys_عدد_msi));
	إذا (!smg->sys_msi_denied_bmap) {
		sbi_free(smg);
		أرجع SBI_ENOMEM;
	}

	لكل (i = 0; i < smg->sys_عدد_msi; i++) {
		gmareq.sys_msi_اشاریہ = i;
		rc = rpmi_عام_request_with_حالت(chan,
						     RPMI_SYSMSI_SRV_GET_MSI_گن,
						     &gmareq, rpmi_u32_گنتی(gmareq),
						     rpmi_u32_گنتی(gmareq),
						     &gmaresp, rpmi_u32_گنتی(gmaresp),
						     rpmi_u32_گنتی(gmaresp));
		إذا (rc) {
			mpxy_rpmi_sysmsi_cleanup(smg);
			أرجع rc;
		}

		إذا (p2a_db_اشاریہ == i ||
		    (gmaresp.flag0 & RPMI_SYSMSI_MSI_گن_FLAG0_PREF_PRIV))
			bitmap_سیٹ(smg->sys_msi_denied_bmap, i, 1);
	}

	*context = smg;
	أرجع 0;
}

ساكن بنية mpxy_rpmi_service_انکڑے sysmsi_services[] = {
{
	.id = RPMI_SYSMSI_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = حجم(بنية rpmi_enable_notification_req),
	.max_tx_len = حجم(بنية rpmi_enable_notification_req),
	.min_rx_len = حجم(بنية rpmi_enable_notification_resp),
	.max_rx_len = حجم(بنية rpmi_enable_notification_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_گن,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = حجم(بنية rpmi_sysmsi_get_گن_resp),
	.max_rx_len = حجم(بنية rpmi_sysmsi_get_گن_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_گن,
	.min_tx_len = حجم(بنية rpmi_sysmsi_get_msi_گن_req),
	.max_tx_len = حجم(بنية rpmi_sysmsi_get_msi_گن_req),
	.min_rx_len = حجم(بنية rpmi_sysmsi_get_msi_گن_resp),
	.max_rx_len = حجم(بنية rpmi_sysmsi_get_msi_گن_resp),
},
{
	.id = RPMI_SYSMSI_SRV_سیٹ_MSI_STATE,
	.min_tx_len = حجم(بنية rpmi_sysmsi_سیٹ_msi_state_req),
	.max_tx_len = حجم(بنية rpmi_sysmsi_سیٹ_msi_state_req),
	.min_rx_len = حجم(بنية rpmi_sysmsi_سیٹ_msi_state_resp),
	.max_rx_len = حجم(بنية rpmi_sysmsi_سیٹ_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_STATE,
	.min_tx_len = حجم(بنية rpmi_sysmsi_get_msi_state_req),
	.max_tx_len = حجم(بنية rpmi_sysmsi_get_msi_state_req),
	.min_rx_len = حجم(بنية rpmi_sysmsi_get_msi_state_resp),
	.max_rx_len = حجم(بنية rpmi_sysmsi_get_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_سیٹ_MSI_نشانہ,
	.min_tx_len = حجم(بنية rpmi_sysmsi_سیٹ_msi_نشانہ_req),
	.max_tx_len = حجم(بنية rpmi_sysmsi_سیٹ_msi_نشانہ_req),
	.min_rx_len = حجم(بنية rpmi_sysmsi_سیٹ_msi_نشانہ_resp),
	.max_rx_len = حجم(بنية rpmi_sysmsi_سیٹ_msi_نشانہ_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_نشانہ,
	.min_tx_len = حجم(بنية rpmi_sysmsi_get_msi_نشانہ_req),
	.max_tx_len = حجم(بنية rpmi_sysmsi_get_msi_نشانہ_req),
	.min_rx_len = حجم(بنية rpmi_sysmsi_get_msi_نشانہ_resp),
	.max_rx_len = حجم(بنية rpmi_sysmsi_get_msi_نشانہ_resp),
},
};

ساكن ثابت بنية mpxy_rpmi_mbox_انکڑے sysmsi_انکڑے = {
	.servicegrp_id = RPMI_SRVGRP_SYSTEM_MSI,
	.عدد_services = RPMI_SYSMSI_SRV_ID_MAX_گنتی,
	.service_انکڑے = sysmsi_services,
	.xfer_گروہ = mpxy_rpmi_sysmis_xfer,
	.تیاری_گروہ = mpxy_rpmi_sysmsi_تیاری,
	.cleanup_گروہ = mpxy_rpmi_sysmsi_cleanup,
};

ساكن ثابت بنية fdt_میل sysmsi_میل[] = {
	{ .compatible = "riscv,rpmi-mpxy-system-msi", .انکڑے = &sysmsi_انکڑے },
	{ },
};

ثابت بنية fdt_driver fdt_mpxy_rpmi_sysmsi = {
	.میل_جدول = sysmsi_میل,
	.init = mpxy_rpmi_mbox_init,
};
