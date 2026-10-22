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
	بینښه اوږد *sys_msi_denied_bmap;
};

ساكن صحيح mpxy_rpmi_sysmis_xfer(فراغ *context, بنية mbox_chan *chan,
				 بنية mbox_xfer *xfer)
{
	بنية rpmi_پیغام_دلیلونه *دلیلونه = xfer->دلیلونه;
	بنية mpxy_rpmi_sysmsi *smg = context;
	u64 sys_msi_پته;
	u32 sys_msi_شاخص;
	صحيح rc = 0;

	إذا (!xfer->rx || دلیلونه->type != RPMI_پیغام_نورمال_REQUEST)
		أرجع 0;

	بدل (دلیلونه->service_id) {
	حالة RPMI_SYSMSI_SRV_GET_ځانګړتیاوې:
		((u32 *)xfer->rx)[0] = cpu_منزل_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_منزل_le32(smg->sys_عدد_msi);
		((u32 *)xfer->rx)[2] = 0;
		((u32 *)xfer->rx)[3] = 0;
		دلیلونه->rx_معلومات_len = 4 * حجم(u32);
		اكسر;
	حالة RPMI_SYSMSI_SRV_GET_MSI_ځانګړتیاوې:
	حالة RPMI_SYSMSI_SRV_سېټ_MSI_STATE:
	حالة RPMI_SYSMSI_SRV_GET_MSI_STATE:
	حالة RPMI_SYSMSI_SRV_سېټ_MSI_موخه:
	حالة RPMI_SYSMSI_SRV_GET_MSI_موخه:
		sys_msi_شاخص = le32_منزل_cpu(((u32 *)xfer->tx)[0]);
		إذا (smg->sys_عدد_msi <= sys_msi_شاخص) {
			((u32 *)xfer->rx)[0] = cpu_منزل_le32(RPMI_ERR_INVALID_PARAM);
			دلیلونه->rx_معلومات_len = حجم(u32);
			اكسر;
		}
		إذا (bitmap_test(smg->sys_msi_denied_bmap, sys_msi_شاخص)) {
			((u32 *)xfer->rx)[0] = cpu_منزل_le32(RPMI_ERR_DENIED);
			دلیلونه->rx_معلومات_len = حجم(u32);
			اكسر;
		}
		إذا (دلیلونه->service_id == RPMI_SYSMSI_SRV_سېټ_MSI_موخه) {
			sys_msi_پته = le32_منزل_cpu(((u32 *)xfer->tx)[1]);
			sys_msi_پته |= ((u64)le32_منزل_cpu(((u32 *)xfer->tx)[2])) << 32;
			إذا (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
							 sys_msi_پته, 0x4, PRV_S,
							 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE |
							 SBI_DOMAIN_MMIO)) {
				((u32 *)xfer->rx)[0] = cpu_منزل_le32(RPMI_ERR_INVALID_ADDR);
				دلیلونه->rx_معلومات_len = حجم(u32);
				اكسر;
			}
		}
		rc = mbox_chan_xfer(chan, xfer);
		اكسر;
	افتراضي:
		((u32 *)xfer->rx)[0] = cpu_منزل_le32(RPMI_ERR_NOTSUPP);
		دلیلونه->rx_معلومات_len = حجم(u32);
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
				  ثابت بنية mpxy_rpmi_mbox_معلومات *معلومات)
{
	بنية rpmi_sysmsi_get_msi_ځانګړتیاوې_resp gmaresp;
	بنية rpmi_sysmsi_get_msi_ځانګړتیاوې_req gmareq;
	بنية rpmi_sysmsi_get_ځانګړتیاوې_resp garesp;
	بنية mpxy_rpmi_sysmsi *smg;
	u32 p2a_db_شاخص;
	صحيح rc, i;

	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_P2_A_DOORBELL_SYSMSI_شاخص,
				     &p2a_db_شاخص);
	إذا (rc)
		أرجع rc;

	rc = rpmi_نورمال_request_with_حالت(chan, RPMI_SYSMSI_SRV_GET_ځانګړتیاوې,
					     عدم, 0, 0, &garesp, rpmi_u32_شمېر(garesp),
					     rpmi_u32_شمېر(garesp));
	إذا (rc)
		أرجع rc;

	smg = sbi_zalloc(حجم(*smg));
	إذا (!smg)
		أرجع SBI_ENOMEM;

	smg->sys_عدد_msi = garesp.sys_عدد_msi;
	smg->sys_msi_denied_bmap = sbi_zalloc(bitmap_estimate_اندازه(smg->sys_عدد_msi));
	إذا (!smg->sys_msi_denied_bmap) {
		sbi_free(smg);
		أرجع SBI_ENOMEM;
	}

	لكل (i = 0; i < smg->sys_عدد_msi; i++) {
		gmareq.sys_msi_شاخص = i;
		rc = rpmi_نورمال_request_with_حالت(chan,
						     RPMI_SYSMSI_SRV_GET_MSI_ځانګړتیاوې,
						     &gmareq, rpmi_u32_شمېر(gmareq),
						     rpmi_u32_شمېر(gmareq),
						     &gmaresp, rpmi_u32_شمېر(gmaresp),
						     rpmi_u32_شمېر(gmaresp));
		إذا (rc) {
			mpxy_rpmi_sysmsi_cleanup(smg);
			أرجع rc;
		}

		إذا (p2a_db_شاخص == i ||
		    (gmaresp.flag0 & RPMI_SYSMSI_MSI_ځانګړتیاوې_FLAG0_PREF_PRIV))
			bitmap_سېټ(smg->sys_msi_denied_bmap, i, 1);
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
	.id = RPMI_SYSMSI_SRV_GET_ځانګړتیاوې,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = حجم(بنية rpmi_sysmsi_get_ځانګړتیاوې_resp),
	.max_rx_len = حجم(بنية rpmi_sysmsi_get_ځانګړتیاوې_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_ځانګړتیاوې,
	.min_tx_len = حجم(بنية rpmi_sysmsi_get_msi_ځانګړتیاوې_req),
	.max_tx_len = حجم(بنية rpmi_sysmsi_get_msi_ځانګړتیاوې_req),
	.min_rx_len = حجم(بنية rpmi_sysmsi_get_msi_ځانګړتیاوې_resp),
	.max_rx_len = حجم(بنية rpmi_sysmsi_get_msi_ځانګړتیاوې_resp),
},
{
	.id = RPMI_SYSMSI_SRV_سېټ_MSI_STATE,
	.min_tx_len = حجم(بنية rpmi_sysmsi_سېټ_msi_state_req),
	.max_tx_len = حجم(بنية rpmi_sysmsi_سېټ_msi_state_req),
	.min_rx_len = حجم(بنية rpmi_sysmsi_سېټ_msi_state_resp),
	.max_rx_len = حجم(بنية rpmi_sysmsi_سېټ_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_STATE,
	.min_tx_len = حجم(بنية rpmi_sysmsi_get_msi_state_req),
	.max_tx_len = حجم(بنية rpmi_sysmsi_get_msi_state_req),
	.min_rx_len = حجم(بنية rpmi_sysmsi_get_msi_state_resp),
	.max_rx_len = حجم(بنية rpmi_sysmsi_get_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_سېټ_MSI_موخه,
	.min_tx_len = حجم(بنية rpmi_sysmsi_سېټ_msi_موخه_req),
	.max_tx_len = حجم(بنية rpmi_sysmsi_سېټ_msi_موخه_req),
	.min_rx_len = حجم(بنية rpmi_sysmsi_سېټ_msi_موخه_resp),
	.max_rx_len = حجم(بنية rpmi_sysmsi_سېټ_msi_موخه_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_موخه,
	.min_tx_len = حجم(بنية rpmi_sysmsi_get_msi_موخه_req),
	.max_tx_len = حجم(بنية rpmi_sysmsi_get_msi_موخه_req),
	.min_rx_len = حجم(بنية rpmi_sysmsi_get_msi_موخه_resp),
	.max_rx_len = حجم(بنية rpmi_sysmsi_get_msi_موخه_resp),
},
};

ساكن ثابت بنية mpxy_rpmi_mbox_معلومات sysmsi_معلومات = {
	.servicegrp_id = RPMI_SRVGRP_SYSTEM_MSI,
	.عدد_services = RPMI_SYSMSI_SRV_ID_MAX_شمېر,
	.service_معلومات = sysmsi_services,
	.xfer_ډله = mpxy_rpmi_sysmis_xfer,
	.تیاری_ډله = mpxy_rpmi_sysmsi_تیاری,
	.cleanup_ډله = mpxy_rpmi_sysmsi_cleanup,
};

ساكن ثابت بنية fdt_سمون sysmsi_سمون[] = {
	{ .compatible = "riscv,rpmi-mpxy-system-msi", .معلومات = &sysmsi_معلومات },
	{ },
};

ثابت بنية fdt_driver fdt_mpxy_rpmi_sysmsi = {
	.سمون_جدول = sysmsi_سمون,
	.init = mpxy_rpmi_mbox_init,
};
