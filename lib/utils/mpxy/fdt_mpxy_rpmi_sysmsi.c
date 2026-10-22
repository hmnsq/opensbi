/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitmap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_byteorder.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/mailbox/rpmi_mailbox.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ರಚನೆ mpxy_rpmi_sysmsi {
	u32 sys_ಅಂಕೆ_msi;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *sys_msi_denied_bmap;
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mpxy_rpmi_sysmis_xfer(ಶೂನ್ಯ *context, ರಚನೆ mbox_chan *chan,
				 ರಚನೆ mbox_xfer *xfer)
{
	ರಚನೆ rpmi_ಸಂದೇಶ_ವಾದಗಳು *ವಾದಗಳು = xfer->ವಾದಗಳು;
	ರಚನೆ mpxy_rpmi_sysmsi *smg = context;
	u64 sys_msi_ವಿಳಾಸ;
	u32 sys_msi_ಸೂಚ್ಯಂಕ;
	ಪೂರ್ಣಾಂಕ rc = 0;

	ಆದರೆ (!xfer->rx || ವಾದಗಳು->type != RPMI_ಸಂದೇಶ_ಸಾಮಾನ್ಯ_REQUEST)
		ಹಿಂತಿರುಗಿ 0;

	ಆಯ್ಕೆ (ವಾದಗಳು->service_id) {
	ಸ್ಥಿತಿ RPMI_SYSMSI_SRV_GET_ಗುಣಲಕ್ಷಣಗಳು:
		((u32 *)xfer->rx)[0] = cpu_ಗಮ್ಯ_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_ಗಮ್ಯ_le32(smg->sys_ಅಂಕೆ_msi);
		((u32 *)xfer->rx)[2] = 0;
		((u32 *)xfer->rx)[3] = 0;
		ವಾದಗಳು->rx_ದತ್ತಾಂಶ_len = 4 * ಗಾತ್ರ(u32);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ RPMI_SYSMSI_SRV_GET_MSI_ಗುಣಲಕ್ಷಣಗಳು:
	ಸ್ಥಿತಿ RPMI_SYSMSI_SRV_ಗಣ_MSI_STATE:
	ಸ್ಥಿತಿ RPMI_SYSMSI_SRV_GET_MSI_STATE:
	ಸ್ಥಿತಿ RPMI_SYSMSI_SRV_ಗಣ_MSI_ಗುರಿ:
	ಸ್ಥಿತಿ RPMI_SYSMSI_SRV_GET_MSI_ಗುರಿ:
		sys_msi_ಸೂಚ್ಯಂಕ = le32_ಗಮ್ಯ_cpu(((u32 *)xfer->tx)[0]);
		ಆದರೆ (smg->sys_ಅಂಕೆ_msi <= sys_msi_ಸೂಚ್ಯಂಕ) {
			((u32 *)xfer->rx)[0] = cpu_ಗಮ್ಯ_le32(RPMI_ERR_INVALID_PARAM);
			ವಾದಗಳು->rx_ದತ್ತಾಂಶ_len = ಗಾತ್ರ(u32);
			ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (bitmap_test(smg->sys_msi_denied_bmap, sys_msi_ಸೂಚ್ಯಂಕ)) {
			((u32 *)xfer->rx)[0] = cpu_ಗಮ್ಯ_le32(RPMI_ERR_DENIED);
			ವಾದಗಳು->rx_ದತ್ತಾಂಶ_len = ಗಾತ್ರ(u32);
			ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (ವಾದಗಳು->service_id == RPMI_SYSMSI_SRV_ಗಣ_MSI_ಗುರಿ) {
			sys_msi_ವಿಳಾಸ = le32_ಗಮ್ಯ_cpu(((u32 *)xfer->tx)[1]);
			sys_msi_ವಿಳಾಸ |= ((u64)le32_ಗಮ್ಯ_cpu(((u32 *)xfer->tx)[2])) << 32;
			ಆದರೆ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
							 sys_msi_ವಿಳಾಸ, 0x4, PRV_S,
							 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE |
							 SBI_DOMAIN_MMIO)) {
				((u32 *)xfer->rx)[0] = cpu_ಗಮ್ಯ_le32(RPMI_ERR_INVALID_ADDR);
				ವಾದಗಳು->rx_ದತ್ತಾಂಶ_len = ಗಾತ್ರ(u32);
				ನಿಲ್ಲಿಸು;
			}
		}
		rc = mbox_chan_xfer(chan, xfer);
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		((u32 *)xfer->rx)[0] = cpu_ಗಮ್ಯ_le32(RPMI_ERR_NOTSUPP);
		ವಾದಗಳು->rx_ದತ್ತಾಂಶ_len = ಗಾತ್ರ(u32);
		ನಿಲ್ಲಿಸು;
	};

	ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ ಶೂನ್ಯ mpxy_rpmi_sysmsi_cleanup(ಶೂನ್ಯ *context)
{
	ರಚನೆ mpxy_rpmi_sysmsi *smg = context;

	sbi_free(smg->sys_msi_denied_bmap);
	sbi_free(smg);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mpxy_rpmi_sysmsi_ಸಿದ್ಧತೆ(ಶೂನ್ಯ **context, ರಚನೆ mbox_chan *chan,
				  ಸ್ಥಿರಾಂಕ ರಚನೆ mpxy_rpmi_mbox_ದತ್ತಾಂಶ *ದತ್ತಾಂಶ)
{
	ರಚನೆ rpmi_sysmsi_get_msi_ಗುಣಲಕ್ಷಣಗಳು_resp gmaresp;
	ರಚನೆ rpmi_sysmsi_get_msi_ಗುಣಲಕ್ಷಣಗಳು_req gmareq;
	ರಚನೆ rpmi_sysmsi_get_ಗುಣಲಕ್ಷಣಗಳು_resp garesp;
	ರಚನೆ mpxy_rpmi_sysmsi *smg;
	u32 p2a_db_ಸೂಚ್ಯಂಕ;
	ಪೂರ್ಣಾಂಕ rc, i;

	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_P2_A_DOORBELL_SYSMSI_ಸೂಚ್ಯಂಕ,
				     &p2a_db_ಸೂಚ್ಯಂಕ);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	rc = rpmi_ಸಾಮಾನ್ಯ_request_with_ಸ್ಥಿತಿ(chan, RPMI_SYSMSI_SRV_GET_ಗುಣಲಕ್ಷಣಗಳು,
					     ಖಾಲಿ, 0, 0, &garesp, rpmi_u32_ಎಣಿಕೆ(garesp),
					     rpmi_u32_ಎಣಿಕೆ(garesp));
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	smg = sbi_zalloc(ಗಾತ್ರ(*smg));
	ಆದರೆ (!smg)
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;

	smg->sys_ಅಂಕೆ_msi = garesp.sys_ಅಂಕೆ_msi;
	smg->sys_msi_denied_bmap = sbi_zalloc(bitmap_estimate_ಗಾತ್ರ(smg->sys_ಅಂಕೆ_msi));
	ಆದರೆ (!smg->sys_msi_denied_bmap) {
		sbi_free(smg);
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;
	}

	ಗಾಗಿ (i = 0; i < smg->sys_ಅಂಕೆ_msi; i++) {
		gmareq.sys_msi_ಸೂಚ್ಯಂಕ = i;
		rc = rpmi_ಸಾಮಾನ್ಯ_request_with_ಸ್ಥಿತಿ(chan,
						     RPMI_SYSMSI_SRV_GET_MSI_ಗುಣಲಕ್ಷಣಗಳು,
						     &gmareq, rpmi_u32_ಎಣಿಕೆ(gmareq),
						     rpmi_u32_ಎಣಿಕೆ(gmareq),
						     &gmaresp, rpmi_u32_ಎಣಿಕೆ(gmaresp),
						     rpmi_u32_ಎಣಿಕೆ(gmaresp));
		ಆದರೆ (rc) {
			mpxy_rpmi_sysmsi_cleanup(smg);
			ಹಿಂತಿರುಗಿ rc;
		}

		ಆದರೆ (p2a_db_ಸೂಚ್ಯಂಕ == i ||
		    (gmaresp.flag0 & RPMI_SYSMSI_MSI_ಗುಣಲಕ್ಷಣಗಳು_FLAG0_PREF_PRIV))
			bitmap_ಗಣ(smg->sys_msi_denied_bmap, i, 1);
	}

	*context = smg;
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ರಚನೆ mpxy_rpmi_service_ದತ್ತಾಂಶ sysmsi_services[] = {
{
	.id = RPMI_SYSMSI_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_enable_notification_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_enable_notification_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_enable_notification_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_enable_notification_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_ಗುಣಲಕ್ಷಣಗಳು,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_sysmsi_get_ಗುಣಲಕ್ಷಣಗಳು_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_sysmsi_get_ಗುಣಲಕ್ಷಣಗಳು_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_ಗುಣಲಕ್ಷಣಗಳು,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_sysmsi_get_msi_ಗುಣಲಕ್ಷಣಗಳು_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_sysmsi_get_msi_ಗುಣಲಕ್ಷಣಗಳು_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_sysmsi_get_msi_ಗುಣಲಕ್ಷಣಗಳು_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_sysmsi_get_msi_ಗುಣಲಕ್ಷಣಗಳು_resp),
},
{
	.id = RPMI_SYSMSI_SRV_ಗಣ_MSI_STATE,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_sysmsi_ಗಣ_msi_state_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_sysmsi_ಗಣ_msi_state_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_sysmsi_ಗಣ_msi_state_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_sysmsi_ಗಣ_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_STATE,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_sysmsi_get_msi_state_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_sysmsi_get_msi_state_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_sysmsi_get_msi_state_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_sysmsi_get_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_ಗಣ_MSI_ಗುರಿ,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_sysmsi_ಗಣ_msi_ಗುರಿ_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_sysmsi_ಗಣ_msi_ಗುರಿ_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_sysmsi_ಗಣ_msi_ಗುರಿ_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_sysmsi_ಗಣ_msi_ಗುರಿ_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_ಗುರಿ,
	.min_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_sysmsi_get_msi_ಗುರಿ_req),
	.max_tx_len = ಗಾತ್ರ(ರಚನೆ rpmi_sysmsi_get_msi_ಗುರಿ_req),
	.min_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_sysmsi_get_msi_ಗುರಿ_resp),
	.max_rx_len = ಗಾತ್ರ(ರಚನೆ rpmi_sysmsi_get_msi_ಗುರಿ_resp),
},
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ mpxy_rpmi_mbox_ದತ್ತಾಂಶ sysmsi_ದತ್ತಾಂಶ = {
	.servicegrp_id = RPMI_SRVGRP_SYSTEM_MSI,
	.ಅಂಕೆ_services = RPMI_SYSMSI_SRV_ID_MAX_ಎಣಿಕೆ,
	.service_ದತ್ತಾಂಶ = sysmsi_services,
	.xfer_ಗುಂಪು = mpxy_rpmi_sysmis_xfer,
	.ಸಿದ್ಧತೆ_ಗುಂಪು = mpxy_rpmi_sysmsi_ಸಿದ್ಧತೆ,
	.cleanup_ಗುಂಪು = mpxy_rpmi_sysmsi_cleanup,
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ sysmsi_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "riscv,rpmi-mpxy-system-msi", .ದತ್ತಾಂಶ = &sysmsi_ದತ್ತಾಂಶ },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_mpxy_rpmi_sysmsi = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = sysmsi_ಹೊಂದಿಕೆ,
	.init = mpxy_rpmi_mbox_init,
};
