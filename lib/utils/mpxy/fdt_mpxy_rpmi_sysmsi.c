/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#అన్తర్భావ <sbi/sbi_bitmap.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi/sbi_byteorder.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi_utils/mailbox/rpmi_mailbox.h>
#అన్తర్భావ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

నిర్మాణం mpxy_rpmi_sysmsi {
	u32 sys_అంకె_msi;
	గుర్తులేని దీర్ఘ *sys_msi_denied_bmap;
};

స్థిర పూర్ణాంకం mpxy_rpmi_sysmis_xfer(శూన్య *context, నిర్మాణం mbox_chan *chan,
				 నిర్మాణం mbox_xfer *xfer)
{
	నిర్మాణం rpmi_సందేశం_వాదనలు *వాదనలు = xfer->వాదనలు;
	నిర్మాణం mpxy_rpmi_sysmsi *smg = context;
	u64 sys_msi_చిరునామా;
	u32 sys_msi_సూచీ;
	పూర్ణాంకం rc = 0;

	ఒకవేళ (!xfer->rx || వాదనలు->type != RPMI_సందేశం_సాధారణ_REQUEST)
		తిరిగి 0;

	ఎంపిక (వాదనలు->service_id) {
	స్థితి RPMI_SYSMSI_SRV_GET_లక్షణాలు:
		((u32 *)xfer->rx)[0] = cpu_గమ్యం_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_గమ్యం_le32(smg->sys_అంకె_msi);
		((u32 *)xfer->rx)[2] = 0;
		((u32 *)xfer->rx)[3] = 0;
		వాదనలు->rx_దత్తాంశం_len = 4 * పరిమాణం(u32);
		ఆపు;
	స్థితి RPMI_SYSMSI_SRV_GET_MSI_లక్షణాలు:
	స్థితి RPMI_SYSMSI_SRV_సమితి_MSI_STATE:
	స్థితి RPMI_SYSMSI_SRV_GET_MSI_STATE:
	స్థితి RPMI_SYSMSI_SRV_సమితి_MSI_లక్ష్యం:
	స్థితి RPMI_SYSMSI_SRV_GET_MSI_లక్ష్యం:
		sys_msi_సూచీ = le32_గమ్యం_cpu(((u32 *)xfer->tx)[0]);
		ఒకవేళ (smg->sys_అంకె_msi <= sys_msi_సూచీ) {
			((u32 *)xfer->rx)[0] = cpu_గమ్యం_le32(RPMI_ERR_INVALID_PARAM);
			వాదనలు->rx_దత్తాంశం_len = పరిమాణం(u32);
			ఆపు;
		}
		ఒకవేళ (bitmap_test(smg->sys_msi_denied_bmap, sys_msi_సూచీ)) {
			((u32 *)xfer->rx)[0] = cpu_గమ్యం_le32(RPMI_ERR_DENIED);
			వాదనలు->rx_దత్తాంశం_len = పరిమాణం(u32);
			ఆపు;
		}
		ఒకవేళ (వాదనలు->service_id == RPMI_SYSMSI_SRV_సమితి_MSI_లక్ష్యం) {
			sys_msi_చిరునామా = le32_గమ్యం_cpu(((u32 *)xfer->tx)[1]);
			sys_msi_చిరునామా |= ((u64)le32_గమ్యం_cpu(((u32 *)xfer->tx)[2])) << 32;
			ఒకవేళ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
							 sys_msi_చిరునామా, 0x4, PRV_S,
							 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE |
							 SBI_DOMAIN_MMIO)) {
				((u32 *)xfer->rx)[0] = cpu_గమ్యం_le32(RPMI_ERR_INVALID_ADDR);
				వాదనలు->rx_దత్తాంశం_len = పరిమాణం(u32);
				ఆపు;
			}
		}
		rc = mbox_chan_xfer(chan, xfer);
		ఆపు;
	అప్రమేయ:
		((u32 *)xfer->rx)[0] = cpu_గమ్యం_le32(RPMI_ERR_NOTSUPP);
		వాదనలు->rx_దత్తాంశం_len = పరిమాణం(u32);
		ఆపు;
	};

	తిరిగి rc;
}

స్థిర శూన్య mpxy_rpmi_sysmsi_cleanup(శూన్య *context)
{
	నిర్మాణం mpxy_rpmi_sysmsi *smg = context;

	sbi_free(smg->sys_msi_denied_bmap);
	sbi_free(smg);
}

స్థిర పూర్ణాంకం mpxy_rpmi_sysmsi_అమరిక(శూన్య **context, నిర్మాణం mbox_chan *chan,
				  స్థిరరాశి నిర్మాణం mpxy_rpmi_mbox_దత్తాంశం *దత్తాంశం)
{
	నిర్మాణం rpmi_sysmsi_get_msi_లక్షణాలు_resp gmaresp;
	నిర్మాణం rpmi_sysmsi_get_msi_లక్షణాలు_req gmareq;
	నిర్మాణం rpmi_sysmsi_get_లక్షణాలు_resp garesp;
	నిర్మాణం mpxy_rpmi_sysmsi *smg;
	u32 p2a_db_సూచీ;
	పూర్ణాంకం rc, i;

	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_P2_A_DOORBELL_SYSMSI_సూచీ,
				     &p2a_db_సూచీ);
	ఒకవేళ (rc)
		తిరిగి rc;

	rc = rpmi_సాధారణ_request_with_స్థితి(chan, RPMI_SYSMSI_SRV_GET_లక్షణాలు,
					     ఖాళీ, 0, 0, &garesp, rpmi_u32_లెక్క(garesp),
					     rpmi_u32_లెక్క(garesp));
	ఒకవేళ (rc)
		తిరిగి rc;

	smg = sbi_zalloc(పరిమాణం(*smg));
	ఒకవేళ (!smg)
		తిరిగి SBI_ENOMEM;

	smg->sys_అంకె_msi = garesp.sys_అంకె_msi;
	smg->sys_msi_denied_bmap = sbi_zalloc(bitmap_estimate_కొలత(smg->sys_అంకె_msi));
	ఒకవేళ (!smg->sys_msi_denied_bmap) {
		sbi_free(smg);
		తిరిగి SBI_ENOMEM;
	}

	కొరకు (i = 0; i < smg->sys_అంకె_msi; i++) {
		gmareq.sys_msi_సూచీ = i;
		rc = rpmi_సాధారణ_request_with_స్థితి(chan,
						     RPMI_SYSMSI_SRV_GET_MSI_లక్షణాలు,
						     &gmareq, rpmi_u32_లెక్క(gmareq),
						     rpmi_u32_లెక్క(gmareq),
						     &gmaresp, rpmi_u32_లెక్క(gmaresp),
						     rpmi_u32_లెక్క(gmaresp));
		ఒకవేళ (rc) {
			mpxy_rpmi_sysmsi_cleanup(smg);
			తిరిగి rc;
		}

		ఒకవేళ (p2a_db_సూచీ == i ||
		    (gmaresp.flag0 & RPMI_SYSMSI_MSI_లక్షణాలు_FLAG0_PREF_PRIV))
			bitmap_సమితి(smg->sys_msi_denied_bmap, i, 1);
	}

	*context = smg;
	తిరిగి 0;
}

స్థిర నిర్మాణం mpxy_rpmi_service_దత్తాంశం sysmsi_services[] = {
{
	.id = RPMI_SYSMSI_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_enable_notification_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_enable_notification_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_enable_notification_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_enable_notification_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_లక్షణాలు,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_sysmsi_get_లక్షణాలు_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_sysmsi_get_లక్షణాలు_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_లక్షణాలు,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_sysmsi_get_msi_లక్షణాలు_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_sysmsi_get_msi_లక్షణాలు_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_sysmsi_get_msi_లక్షణాలు_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_sysmsi_get_msi_లక్షణాలు_resp),
},
{
	.id = RPMI_SYSMSI_SRV_సమితి_MSI_STATE,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_sysmsi_సమితి_msi_state_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_sysmsi_సమితి_msi_state_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_sysmsi_సమితి_msi_state_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_sysmsi_సమితి_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_STATE,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_sysmsi_get_msi_state_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_sysmsi_get_msi_state_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_sysmsi_get_msi_state_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_sysmsi_get_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_సమితి_MSI_లక్ష్యం,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_sysmsi_సమితి_msi_లక్ష్యం_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_sysmsi_సమితి_msi_లక్ష్యం_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_sysmsi_సమితి_msi_లక్ష్యం_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_sysmsi_సమితి_msi_లక్ష్యం_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_లక్ష్యం,
	.min_tx_len = పరిమాణం(నిర్మాణం rpmi_sysmsi_get_msi_లక్ష్యం_req),
	.max_tx_len = పరిమాణం(నిర్మాణం rpmi_sysmsi_get_msi_లక్ష్యం_req),
	.min_rx_len = పరిమాణం(నిర్మాణం rpmi_sysmsi_get_msi_లక్ష్యం_resp),
	.max_rx_len = పరిమాణం(నిర్మాణం rpmi_sysmsi_get_msi_లక్ష్యం_resp),
},
};

స్థిర స్థిరరాశి నిర్మాణం mpxy_rpmi_mbox_దత్తాంశం sysmsi_దత్తాంశం = {
	.servicegrp_id = RPMI_SRVGRP_SYSTEM_MSI,
	.అంకె_services = RPMI_SYSMSI_SRV_ID_MAX_లెక్క,
	.service_దత్తాంశం = sysmsi_services,
	.xfer_సమూహం = mpxy_rpmi_sysmis_xfer,
	.అమరిక_సమూహం = mpxy_rpmi_sysmsi_అమరిక,
	.cleanup_సమూహం = mpxy_rpmi_sysmsi_cleanup,
};

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక sysmsi_సరిపోలిక[] = {
	{ .compatible = "riscv,rpmi-mpxy-system-msi", .దత్తాంశం = &sysmsi_దత్తాంశం },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_mpxy_rpmi_sysmsi = {
	.సరిపోలిక_పట్టిక = sysmsi_సరిపోలిక,
	.init = mpxy_rpmi_mbox_init,
};
