/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_bitmap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_byteorder.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/mailbox/rpmi_mailbox.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ਬਣਤਰ mpxy_rpmi_sysmsi {
	u32 sys_ਅੰਕ_msi;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *sys_msi_denied_bmap;
};

ਸਥਿਰ ਪੂਰਨ mpxy_rpmi_sysmis_xfer(ਖਾਲੀ *context, ਬਣਤਰ mbox_chan *chan,
				 ਬਣਤਰ mbox_xfer *xfer)
{
	ਬਣਤਰ rpmi_ਸੁਨੇਹਾ_ਦਲੀਲਾਂ *ਦਲੀਲਾਂ = xfer->ਦਲੀਲਾਂ;
	ਬਣਤਰ mpxy_rpmi_sysmsi *smg = context;
	u64 sys_msi_ਪਤਾ;
	u32 sys_msi_ਸੂਚਕਾਂਕ;
	ਪੂਰਨ rc = 0;

	ਜੇ (!xfer->rx || ਦਲੀਲਾਂ->type != RPMI_ਸੁਨੇਹਾ_ਸਾਧਾਰਨ_REQUEST)
		ਵਾਪਸ 0;

	ਚੋਣ (ਦਲੀਲਾਂ->service_id) {
	ਹਾਲਤ RPMI_SYSMSI_SRV_GET_ਗੁਣ:
		((u32 *)xfer->rx)[0] = cpu_ਮੰਜ਼ਿਲ_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_ਮੰਜ਼ਿਲ_le32(smg->sys_ਅੰਕ_msi);
		((u32 *)xfer->rx)[2] = 0;
		((u32 *)xfer->rx)[3] = 0;
		ਦਲੀਲਾਂ->rx_ਅੰਕੜੇ_len = 4 * ਆਕਾਰ(u32);
		ਰੋਕੋ;
	ਹਾਲਤ RPMI_SYSMSI_SRV_GET_MSI_ਗੁਣ:
	ਹਾਲਤ RPMI_SYSMSI_SRV_ਸੈੱਟ_MSI_STATE:
	ਹਾਲਤ RPMI_SYSMSI_SRV_GET_MSI_STATE:
	ਹਾਲਤ RPMI_SYSMSI_SRV_ਸੈੱਟ_MSI_ਨਿਸ਼ਾਨਾ:
	ਹਾਲਤ RPMI_SYSMSI_SRV_GET_MSI_ਨਿਸ਼ਾਨਾ:
		sys_msi_ਸੂਚਕਾਂਕ = le32_ਮੰਜ਼ਿਲ_cpu(((u32 *)xfer->tx)[0]);
		ਜੇ (smg->sys_ਅੰਕ_msi <= sys_msi_ਸੂਚਕਾਂਕ) {
			((u32 *)xfer->rx)[0] = cpu_ਮੰਜ਼ਿਲ_le32(RPMI_ERR_INVALID_PARAM);
			ਦਲੀਲਾਂ->rx_ਅੰਕੜੇ_len = ਆਕਾਰ(u32);
			ਰੋਕੋ;
		}
		ਜੇ (bitmap_test(smg->sys_msi_denied_bmap, sys_msi_ਸੂਚਕਾਂਕ)) {
			((u32 *)xfer->rx)[0] = cpu_ਮੰਜ਼ਿਲ_le32(RPMI_ERR_DENIED);
			ਦਲੀਲਾਂ->rx_ਅੰਕੜੇ_len = ਆਕਾਰ(u32);
			ਰੋਕੋ;
		}
		ਜੇ (ਦਲੀਲਾਂ->service_id == RPMI_SYSMSI_SRV_ਸੈੱਟ_MSI_ਨਿਸ਼ਾਨਾ) {
			sys_msi_ਪਤਾ = le32_ਮੰਜ਼ਿਲ_cpu(((u32 *)xfer->tx)[1]);
			sys_msi_ਪਤਾ |= ((u64)le32_ਮੰਜ਼ਿਲ_cpu(((u32 *)xfer->tx)[2])) << 32;
			ਜੇ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
							 sys_msi_ਪਤਾ, 0x4, PRV_S,
							 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE |
							 SBI_DOMAIN_MMIO)) {
				((u32 *)xfer->rx)[0] = cpu_ਮੰਜ਼ਿਲ_le32(RPMI_ERR_INVALID_ADDR);
				ਦਲੀਲਾਂ->rx_ਅੰਕੜੇ_len = ਆਕਾਰ(u32);
				ਰੋਕੋ;
			}
		}
		rc = mbox_chan_xfer(chan, xfer);
		ਰੋਕੋ;
	ਮੂਲ:
		((u32 *)xfer->rx)[0] = cpu_ਮੰਜ਼ਿਲ_le32(RPMI_ERR_NOTSUPP);
		ਦਲੀਲਾਂ->rx_ਅੰਕੜੇ_len = ਆਕਾਰ(u32);
		ਰੋਕੋ;
	};

	ਵਾਪਸ rc;
}

ਸਥਿਰ ਖਾਲੀ mpxy_rpmi_sysmsi_cleanup(ਖਾਲੀ *context)
{
	ਬਣਤਰ mpxy_rpmi_sysmsi *smg = context;

	sbi_free(smg->sys_msi_denied_bmap);
	sbi_free(smg);
}

ਸਥਿਰ ਪੂਰਨ mpxy_rpmi_sysmsi_ਤਿਆਰੀ(ਖਾਲੀ **context, ਬਣਤਰ mbox_chan *chan,
				  ਅਚਲ ਬਣਤਰ mpxy_rpmi_mbox_ਅੰਕੜੇ *ਅੰਕੜੇ)
{
	ਬਣਤਰ rpmi_sysmsi_get_msi_ਗੁਣ_resp gmaresp;
	ਬਣਤਰ rpmi_sysmsi_get_msi_ਗੁਣ_req gmareq;
	ਬਣਤਰ rpmi_sysmsi_get_ਗੁਣ_resp garesp;
	ਬਣਤਰ mpxy_rpmi_sysmsi *smg;
	u32 p2a_db_ਸੂਚਕਾਂਕ;
	ਪੂਰਨ rc, i;

	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_P2_A_DOORBELL_SYSMSI_ਸੂਚਕਾਂਕ,
				     &p2a_db_ਸੂਚਕਾਂਕ);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	rc = rpmi_ਸਾਧਾਰਨ_request_with_ਸਥਿਤੀ(chan, RPMI_SYSMSI_SRV_GET_ਗੁਣ,
					     ਨਿਰਾ, 0, 0, &garesp, rpmi_u32_ਗਿਣਤੀ(garesp),
					     rpmi_u32_ਗਿਣਤੀ(garesp));
	ਜੇ (rc)
		ਵਾਪਸ rc;

	smg = sbi_zalloc(ਆਕਾਰ(*smg));
	ਜੇ (!smg)
		ਵਾਪਸ SBI_ENOMEM;

	smg->sys_ਅੰਕ_msi = garesp.sys_ਅੰਕ_msi;
	smg->sys_msi_denied_bmap = sbi_zalloc(bitmap_estimate_ਮਾਪ(smg->sys_ਅੰਕ_msi));
	ਜੇ (!smg->sys_msi_denied_bmap) {
		sbi_free(smg);
		ਵਾਪਸ SBI_ENOMEM;
	}

	ਲਈ (i = 0; i < smg->sys_ਅੰਕ_msi; i++) {
		gmareq.sys_msi_ਸੂਚਕਾਂਕ = i;
		rc = rpmi_ਸਾਧਾਰਨ_request_with_ਸਥਿਤੀ(chan,
						     RPMI_SYSMSI_SRV_GET_MSI_ਗੁਣ,
						     &gmareq, rpmi_u32_ਗਿਣਤੀ(gmareq),
						     rpmi_u32_ਗਿਣਤੀ(gmareq),
						     &gmaresp, rpmi_u32_ਗਿਣਤੀ(gmaresp),
						     rpmi_u32_ਗਿਣਤੀ(gmaresp));
		ਜੇ (rc) {
			mpxy_rpmi_sysmsi_cleanup(smg);
			ਵਾਪਸ rc;
		}

		ਜੇ (p2a_db_ਸੂਚਕਾਂਕ == i ||
		    (gmaresp.flag0 & RPMI_SYSMSI_MSI_ਗੁਣ_FLAG0_PREF_PRIV))
			bitmap_ਸੈੱਟ(smg->sys_msi_denied_bmap, i, 1);
	}

	*context = smg;
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਬਣਤਰ mpxy_rpmi_service_ਅੰਕੜੇ sysmsi_services[] = {
{
	.id = RPMI_SYSMSI_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_enable_notification_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_enable_notification_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_enable_notification_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_enable_notification_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_ਗੁਣ,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_sysmsi_get_ਗੁਣ_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_sysmsi_get_ਗੁਣ_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_ਗੁਣ,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_sysmsi_get_msi_ਗੁਣ_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_sysmsi_get_msi_ਗੁਣ_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_sysmsi_get_msi_ਗੁਣ_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_sysmsi_get_msi_ਗੁਣ_resp),
},
{
	.id = RPMI_SYSMSI_SRV_ਸੈੱਟ_MSI_STATE,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_sysmsi_ਸੈੱਟ_msi_state_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_sysmsi_ਸੈੱਟ_msi_state_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_sysmsi_ਸੈੱਟ_msi_state_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_sysmsi_ਸੈੱਟ_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_STATE,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_sysmsi_get_msi_state_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_sysmsi_get_msi_state_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_sysmsi_get_msi_state_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_sysmsi_get_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_ਸੈੱਟ_MSI_ਨਿਸ਼ਾਨਾ,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_sysmsi_ਸੈੱਟ_msi_ਨਿਸ਼ਾਨਾ_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_sysmsi_ਸੈੱਟ_msi_ਨਿਸ਼ਾਨਾ_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_sysmsi_ਸੈੱਟ_msi_ਨਿਸ਼ਾਨਾ_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_sysmsi_ਸੈੱਟ_msi_ਨਿਸ਼ਾਨਾ_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_ਨਿਸ਼ਾਨਾ,
	.min_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_sysmsi_get_msi_ਨਿਸ਼ਾਨਾ_req),
	.max_tx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_sysmsi_get_msi_ਨਿਸ਼ਾਨਾ_req),
	.min_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_sysmsi_get_msi_ਨਿਸ਼ਾਨਾ_resp),
	.max_rx_len = ਆਕਾਰ(ਬਣਤਰ rpmi_sysmsi_get_msi_ਨਿਸ਼ਾਨਾ_resp),
},
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ mpxy_rpmi_mbox_ਅੰਕੜੇ sysmsi_ਅੰਕੜੇ = {
	.servicegrp_id = RPMI_SRVGRP_SYSTEM_MSI,
	.ਅੰਕ_services = RPMI_SYSMSI_SRV_ID_MAX_ਗਿਣਤੀ,
	.service_ਅੰਕੜੇ = sysmsi_services,
	.xfer_ਸਮੂਹ = mpxy_rpmi_sysmis_xfer,
	.ਤਿਆਰੀ_ਸਮੂਹ = mpxy_rpmi_sysmsi_ਤਿਆਰੀ,
	.cleanup_ਸਮੂਹ = mpxy_rpmi_sysmsi_cleanup,
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ sysmsi_ਮੇਲ[] = {
	{ .compatible = "riscv,rpmi-mpxy-system-msi", .ਅੰਕੜੇ = &sysmsi_ਅੰਕੜੇ },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_mpxy_rpmi_sysmsi = {
	.ਮੇਲ_ਸਾਰਣੀ = sysmsi_ਮੇਲ,
	.init = mpxy_rpmi_mbox_init,
};
