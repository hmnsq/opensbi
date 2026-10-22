/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#شامل <sbi/sbi_bitmap.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_byteorder.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi_utils/mailbox/rpmi_mailbox.h>
#شامل <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ساخت mpxy_rpmi_sysmsi {
	u32 sys_عدد_msi;
	غیرعلامتی طویل *sys_msi_denied_bmap;
};

ساکن صحیح mpxy_rpmi_sysmis_xfer(خالی *context, ساخت mbox_chan *chan,
				 ساخت mbox_xfer *xfer)
{
	ساخت rpmi_پیغام_دلائل *دلائل = xfer->دلائل;
	ساخت mpxy_rpmi_sysmsi *smg = context;
	u64 sys_msi_پتہ;
	u32 sys_msi_اشاریہ;
	صحیح rc = 0;

	اگر (!xfer->rx || دلائل->type != RPMI_پیغام_معمول_REQUEST)
		واپس 0;

	انتخاب (دلائل->service_id) {
	حالت RPMI_SYSMSI_SRV_GET_اوصاف:
		((u32 *)xfer->rx)[0] = cpu_مقصد_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_مقصد_le32(smg->sys_عدد_msi);
		((u32 *)xfer->rx)[2] = 0;
		((u32 *)xfer->rx)[3] = 0;
		دلائل->rx_ڈیٹا_len = 4 * حجم(u32);
		توڑو;
	حالت RPMI_SYSMSI_SRV_GET_MSI_اوصاف:
	حالت RPMI_SYSMSI_SRV_سیٹ_MSI_STATE:
	حالت RPMI_SYSMSI_SRV_GET_MSI_STATE:
	حالت RPMI_SYSMSI_SRV_سیٹ_MSI_ہدف:
	حالت RPMI_SYSMSI_SRV_GET_MSI_ہدف:
		sys_msi_اشاریہ = le32_مقصد_cpu(((u32 *)xfer->tx)[0]);
		اگر (smg->sys_عدد_msi <= sys_msi_اشاریہ) {
			((u32 *)xfer->rx)[0] = cpu_مقصد_le32(RPMI_ERR_INVALID_PARAM);
			دلائل->rx_ڈیٹا_len = حجم(u32);
			توڑو;
		}
		اگر (bitmap_test(smg->sys_msi_denied_bmap, sys_msi_اشاریہ)) {
			((u32 *)xfer->rx)[0] = cpu_مقصد_le32(RPMI_ERR_DENIED);
			دلائل->rx_ڈیٹا_len = حجم(u32);
			توڑو;
		}
		اگر (دلائل->service_id == RPMI_SYSMSI_SRV_سیٹ_MSI_ہدف) {
			sys_msi_پتہ = le32_مقصد_cpu(((u32 *)xfer->tx)[1]);
			sys_msi_پتہ |= ((u64)le32_مقصد_cpu(((u32 *)xfer->tx)[2])) << 32;
			اگر (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
							 sys_msi_پتہ, 0x4, PRV_S,
							 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE |
							 SBI_DOMAIN_MMIO)) {
				((u32 *)xfer->rx)[0] = cpu_مقصد_le32(RPMI_ERR_INVALID_ADDR);
				دلائل->rx_ڈیٹا_len = حجم(u32);
				توڑو;
			}
		}
		rc = mbox_chan_xfer(chan, xfer);
		توڑو;
	طےشدہ:
		((u32 *)xfer->rx)[0] = cpu_مقصد_le32(RPMI_ERR_NOTSUPP);
		دلائل->rx_ڈیٹا_len = حجم(u32);
		توڑو;
	};

	واپس rc;
}

ساکن خالی mpxy_rpmi_sysmsi_cleanup(خالی *context)
{
	ساخت mpxy_rpmi_sysmsi *smg = context;

	sbi_free(smg->sys_msi_denied_bmap);
	sbi_free(smg);
}

ساکن صحیح mpxy_rpmi_sysmsi_تیاری(خالی **context, ساخت mbox_chan *chan,
				  ثابت ساخت mpxy_rpmi_mbox_ڈیٹا *ڈیٹا)
{
	ساخت rpmi_sysmsi_get_msi_اوصاف_resp gmaresp;
	ساخت rpmi_sysmsi_get_msi_اوصاف_req gmareq;
	ساخت rpmi_sysmsi_get_اوصاف_resp garesp;
	ساخت mpxy_rpmi_sysmsi *smg;
	u32 p2a_db_اشاریہ;
	صحیح rc, i;

	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_P2_A_DOORBELL_SYSMSI_اشاریہ,
				     &p2a_db_اشاریہ);
	اگر (rc)
		واپس rc;

	rc = rpmi_معمول_request_with_حالت(chan, RPMI_SYSMSI_SRV_GET_اوصاف,
					     تہی, 0, 0, &garesp, rpmi_u32_گنتی(garesp),
					     rpmi_u32_گنتی(garesp));
	اگر (rc)
		واپس rc;

	smg = sbi_zalloc(حجم(*smg));
	اگر (!smg)
		واپس SBI_ENOMEM;

	smg->sys_عدد_msi = garesp.sys_عدد_msi;
	smg->sys_msi_denied_bmap = sbi_zalloc(bitmap_estimate_سائز(smg->sys_عدد_msi));
	اگر (!smg->sys_msi_denied_bmap) {
		sbi_free(smg);
		واپس SBI_ENOMEM;
	}

	برائے (i = 0; i < smg->sys_عدد_msi; i++) {
		gmareq.sys_msi_اشاریہ = i;
		rc = rpmi_معمول_request_with_حالت(chan,
						     RPMI_SYSMSI_SRV_GET_MSI_اوصاف,
						     &gmareq, rpmi_u32_گنتی(gmareq),
						     rpmi_u32_گنتی(gmareq),
						     &gmaresp, rpmi_u32_گنتی(gmaresp),
						     rpmi_u32_گنتی(gmaresp));
		اگر (rc) {
			mpxy_rpmi_sysmsi_cleanup(smg);
			واپس rc;
		}

		اگر (p2a_db_اشاریہ == i ||
		    (gmaresp.flag0 & RPMI_SYSMSI_MSI_اوصاف_FLAG0_PREF_PRIV))
			bitmap_سیٹ(smg->sys_msi_denied_bmap, i, 1);
	}

	*context = smg;
	واپس 0;
}

ساکن ساخت mpxy_rpmi_service_ڈیٹا sysmsi_services[] = {
{
	.id = RPMI_SYSMSI_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = حجم(ساخت rpmi_enable_notification_req),
	.max_tx_len = حجم(ساخت rpmi_enable_notification_req),
	.min_rx_len = حجم(ساخت rpmi_enable_notification_resp),
	.max_rx_len = حجم(ساخت rpmi_enable_notification_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_اوصاف,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = حجم(ساخت rpmi_sysmsi_get_اوصاف_resp),
	.max_rx_len = حجم(ساخت rpmi_sysmsi_get_اوصاف_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_اوصاف,
	.min_tx_len = حجم(ساخت rpmi_sysmsi_get_msi_اوصاف_req),
	.max_tx_len = حجم(ساخت rpmi_sysmsi_get_msi_اوصاف_req),
	.min_rx_len = حجم(ساخت rpmi_sysmsi_get_msi_اوصاف_resp),
	.max_rx_len = حجم(ساخت rpmi_sysmsi_get_msi_اوصاف_resp),
},
{
	.id = RPMI_SYSMSI_SRV_سیٹ_MSI_STATE,
	.min_tx_len = حجم(ساخت rpmi_sysmsi_سیٹ_msi_state_req),
	.max_tx_len = حجم(ساخت rpmi_sysmsi_سیٹ_msi_state_req),
	.min_rx_len = حجم(ساخت rpmi_sysmsi_سیٹ_msi_state_resp),
	.max_rx_len = حجم(ساخت rpmi_sysmsi_سیٹ_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_STATE,
	.min_tx_len = حجم(ساخت rpmi_sysmsi_get_msi_state_req),
	.max_tx_len = حجم(ساخت rpmi_sysmsi_get_msi_state_req),
	.min_rx_len = حجم(ساخت rpmi_sysmsi_get_msi_state_resp),
	.max_rx_len = حجم(ساخت rpmi_sysmsi_get_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_سیٹ_MSI_ہدف,
	.min_tx_len = حجم(ساخت rpmi_sysmsi_سیٹ_msi_ہدف_req),
	.max_tx_len = حجم(ساخت rpmi_sysmsi_سیٹ_msi_ہدف_req),
	.min_rx_len = حجم(ساخت rpmi_sysmsi_سیٹ_msi_ہدف_resp),
	.max_rx_len = حجم(ساخت rpmi_sysmsi_سیٹ_msi_ہدف_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_ہدف,
	.min_tx_len = حجم(ساخت rpmi_sysmsi_get_msi_ہدف_req),
	.max_tx_len = حجم(ساخت rpmi_sysmsi_get_msi_ہدف_req),
	.min_rx_len = حجم(ساخت rpmi_sysmsi_get_msi_ہدف_resp),
	.max_rx_len = حجم(ساخت rpmi_sysmsi_get_msi_ہدف_resp),
},
};

ساکن ثابت ساخت mpxy_rpmi_mbox_ڈیٹا sysmsi_ڈیٹا = {
	.servicegrp_id = RPMI_SRVGRP_SYSTEM_MSI,
	.عدد_services = RPMI_SYSMSI_SRV_ID_MAX_گنتی,
	.service_ڈیٹا = sysmsi_services,
	.xfer_گروہ = mpxy_rpmi_sysmis_xfer,
	.تیاری_گروہ = mpxy_rpmi_sysmsi_تیاری,
	.cleanup_گروہ = mpxy_rpmi_sysmsi_cleanup,
};

ساکن ثابت ساخت fdt_مطابقت sysmsi_مطابقت[] = {
	{ .compatible = "riscv,rpmi-mpxy-system-msi", .ڈیٹا = &sysmsi_ڈیٹا },
	{ },
};

ثابت ساخت fdt_driver fdt_mpxy_rpmi_sysmsi = {
	.مطابقت_جدول = sysmsi_مطابقت,
	.init = mpxy_rpmi_mbox_init,
};
