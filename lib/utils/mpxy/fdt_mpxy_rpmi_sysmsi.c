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
	u32 sys_رقم_msi;
	بیعلامت دراز *sys_msi_denied_bmap;
};

ساكن صحيح mpxy_rpmi_sysmis_xfer(فراغ *context, بنية mbox_chan *chan,
				 بنية mbox_xfer *xfer)
{
	بنية rpmi_پیام_آرگومانها *آرگومانها = xfer->آرگومانها;
	بنية mpxy_rpmi_sysmsi *smg = context;
	u64 sys_msi_آدرس;
	u32 sys_msi_شاخص;
	صحيح rc = 0;

	إذا (!xfer->rx || آرگومانها->type != RPMI_پیام_نرمال_REQUEST)
		أرجع 0;

	بدل (آرگومانها->service_id) {
	حالة RPMI_SYSMSI_SRV_GET_صفات:
		((u32 *)xfer->rx)[0] = cpu_مقصد_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_مقصد_le32(smg->sys_رقم_msi);
		((u32 *)xfer->rx)[2] = 0;
		((u32 *)xfer->rx)[3] = 0;
		آرگومانها->rx_داده_len = 4 * حجم(u32);
		اكسر;
	حالة RPMI_SYSMSI_SRV_GET_MSI_صفات:
	حالة RPMI_SYSMSI_SRV_مجموعه_MSI_STATE:
	حالة RPMI_SYSMSI_SRV_GET_MSI_STATE:
	حالة RPMI_SYSMSI_SRV_مجموعه_MSI_هدف:
	حالة RPMI_SYSMSI_SRV_GET_MSI_هدف:
		sys_msi_شاخص = le32_مقصد_cpu(((u32 *)xfer->tx)[0]);
		إذا (smg->sys_رقم_msi <= sys_msi_شاخص) {
			((u32 *)xfer->rx)[0] = cpu_مقصد_le32(RPMI_ERR_INVALID_PARAM);
			آرگومانها->rx_داده_len = حجم(u32);
			اكسر;
		}
		إذا (bitmap_test(smg->sys_msi_denied_bmap, sys_msi_شاخص)) {
			((u32 *)xfer->rx)[0] = cpu_مقصد_le32(RPMI_ERR_DENIED);
			آرگومانها->rx_داده_len = حجم(u32);
			اكسر;
		}
		إذا (آرگومانها->service_id == RPMI_SYSMSI_SRV_مجموعه_MSI_هدف) {
			sys_msi_آدرس = le32_مقصد_cpu(((u32 *)xfer->tx)[1]);
			sys_msi_آدرس |= ((u64)le32_مقصد_cpu(((u32 *)xfer->tx)[2])) << 32;
			إذا (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
							 sys_msi_آدرس, 0x4, PRV_S,
							 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE |
							 SBI_DOMAIN_MMIO)) {
				((u32 *)xfer->rx)[0] = cpu_مقصد_le32(RPMI_ERR_INVALID_ADDR);
				آرگومانها->rx_داده_len = حجم(u32);
				اكسر;
			}
		}
		rc = mbox_chan_xfer(chan, xfer);
		اكسر;
	افتراضي:
		((u32 *)xfer->rx)[0] = cpu_مقصد_le32(RPMI_ERR_NOTSUPP);
		آرگومانها->rx_داده_len = حجم(u32);
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

ساكن صحيح mpxy_rpmi_sysmsi_راهاندازی(فراغ **context, بنية mbox_chan *chan,
				  ثابت بنية mpxy_rpmi_mbox_داده *داده)
{
	بنية rpmi_sysmsi_get_msi_صفات_resp gmaresp;
	بنية rpmi_sysmsi_get_msi_صفات_req gmareq;
	بنية rpmi_sysmsi_get_صفات_resp garesp;
	بنية mpxy_rpmi_sysmsi *smg;
	u32 p2a_db_شاخص;
	صحيح rc, i;

	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_P2_A_DOORBELL_SYSMSI_شاخص,
				     &p2a_db_شاخص);
	إذا (rc)
		أرجع rc;

	rc = rpmi_نرمال_request_with_وضعیت(chan, RPMI_SYSMSI_SRV_GET_صفات,
					     عدم, 0, 0, &garesp, rpmi_u32_شمار(garesp),
					     rpmi_u32_شمار(garesp));
	إذا (rc)
		أرجع rc;

	smg = sbi_zalloc(حجم(*smg));
	إذا (!smg)
		أرجع SBI_ENOMEM;

	smg->sys_رقم_msi = garesp.sys_رقم_msi;
	smg->sys_msi_denied_bmap = sbi_zalloc(bitmap_estimate_اندازه(smg->sys_رقم_msi));
	إذا (!smg->sys_msi_denied_bmap) {
		sbi_free(smg);
		أرجع SBI_ENOMEM;
	}

	لكل (i = 0; i < smg->sys_رقم_msi; i++) {
		gmareq.sys_msi_شاخص = i;
		rc = rpmi_نرمال_request_with_وضعیت(chan,
						     RPMI_SYSMSI_SRV_GET_MSI_صفات,
						     &gmareq, rpmi_u32_شمار(gmareq),
						     rpmi_u32_شمار(gmareq),
						     &gmaresp, rpmi_u32_شمار(gmaresp),
						     rpmi_u32_شمار(gmaresp));
		إذا (rc) {
			mpxy_rpmi_sysmsi_cleanup(smg);
			أرجع rc;
		}

		إذا (p2a_db_شاخص == i ||
		    (gmaresp.flag0 & RPMI_SYSMSI_MSI_صفات_FLAG0_PREF_PRIV))
			bitmap_مجموعه(smg->sys_msi_denied_bmap, i, 1);
	}

	*context = smg;
	أرجع 0;
}

ساكن بنية mpxy_rpmi_service_داده sysmsi_services[] = {
{
	.id = RPMI_SYSMSI_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = حجم(بنية rpmi_enable_notification_req),
	.max_tx_len = حجم(بنية rpmi_enable_notification_req),
	.min_rx_len = حجم(بنية rpmi_enable_notification_resp),
	.max_rx_len = حجم(بنية rpmi_enable_notification_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_صفات,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = حجم(بنية rpmi_sysmsi_get_صفات_resp),
	.max_rx_len = حجم(بنية rpmi_sysmsi_get_صفات_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_صفات,
	.min_tx_len = حجم(بنية rpmi_sysmsi_get_msi_صفات_req),
	.max_tx_len = حجم(بنية rpmi_sysmsi_get_msi_صفات_req),
	.min_rx_len = حجم(بنية rpmi_sysmsi_get_msi_صفات_resp),
	.max_rx_len = حجم(بنية rpmi_sysmsi_get_msi_صفات_resp),
},
{
	.id = RPMI_SYSMSI_SRV_مجموعه_MSI_STATE,
	.min_tx_len = حجم(بنية rpmi_sysmsi_مجموعه_msi_state_req),
	.max_tx_len = حجم(بنية rpmi_sysmsi_مجموعه_msi_state_req),
	.min_rx_len = حجم(بنية rpmi_sysmsi_مجموعه_msi_state_resp),
	.max_rx_len = حجم(بنية rpmi_sysmsi_مجموعه_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_STATE,
	.min_tx_len = حجم(بنية rpmi_sysmsi_get_msi_state_req),
	.max_tx_len = حجم(بنية rpmi_sysmsi_get_msi_state_req),
	.min_rx_len = حجم(بنية rpmi_sysmsi_get_msi_state_resp),
	.max_rx_len = حجم(بنية rpmi_sysmsi_get_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_مجموعه_MSI_هدف,
	.min_tx_len = حجم(بنية rpmi_sysmsi_مجموعه_msi_هدف_req),
	.max_tx_len = حجم(بنية rpmi_sysmsi_مجموعه_msi_هدف_req),
	.min_rx_len = حجم(بنية rpmi_sysmsi_مجموعه_msi_هدف_resp),
	.max_rx_len = حجم(بنية rpmi_sysmsi_مجموعه_msi_هدف_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_هدف,
	.min_tx_len = حجم(بنية rpmi_sysmsi_get_msi_هدف_req),
	.max_tx_len = حجم(بنية rpmi_sysmsi_get_msi_هدف_req),
	.min_rx_len = حجم(بنية rpmi_sysmsi_get_msi_هدف_resp),
	.max_rx_len = حجم(بنية rpmi_sysmsi_get_msi_هدف_resp),
},
};

ساكن ثابت بنية mpxy_rpmi_mbox_داده sysmsi_داده = {
	.servicegrp_id = RPMI_SRVGRP_SYSTEM_MSI,
	.رقم_services = RPMI_SYSMSI_SRV_ID_MAX_شمار,
	.service_داده = sysmsi_services,
	.xfer_گروه = mpxy_rpmi_sysmis_xfer,
	.راهاندازی_گروه = mpxy_rpmi_sysmsi_راهاندازی,
	.cleanup_گروه = mpxy_rpmi_sysmsi_cleanup,
};

ساكن ثابت بنية fdt_تطابق sysmsi_تطابق[] = {
	{ .compatible = "riscv,rpmi-mpxy-system-msi", .داده = &sysmsi_داده },
	{ },
};

ثابت بنية fdt_driver fdt_mpxy_rpmi_sysmsi = {
	.تطابق_جدول = sysmsi_تطابق,
	.init = mpxy_rpmi_mbox_init,
};
