/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#अन्तर्भाव <sbi/sbi_bitmap.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/sbi_byteorder.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi_utils/mailbox/rpmi_mailbox.h>
#अन्तर्भाव <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

संरचना mpxy_rpmi_sysmsi {
	u32 sys_अङ्कः_msi;
	अचिह्नित दीर्घ *sys_msi_denied_bmap;
};

स्थिर पूर्णाङ्क mpxy_rpmi_sysmis_xfer(शून्य *context, संरचना mbox_chan *chan,
				 संरचना mbox_xfer *xfer)
{
	संरचना rpmi_सन्देशः_तर्काः *तर्काः = xfer->तर्काः;
	संरचना mpxy_rpmi_sysmsi *smg = context;
	u64 sys_msi_स्थानाङ्कः;
	u32 sys_msi_सूचकांक;
	पूर्णाङ्क rc = 0;

	यदि (!xfer->rx || तर्काः->type != RPMI_सन्देशः_प्रसामान्यम्_REQUEST)
		प्रत्यागम 0;

	विकल्प (तर्काः->service_id) {
	पक्ष RPMI_SYSMSI_SRV_GET_गुणाः:
		((u32 *)xfer->rx)[0] = cpu_गन्तव्यम्_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_गन्तव्यम्_le32(smg->sys_अङ्कः_msi);
		((u32 *)xfer->rx)[2] = 0;
		((u32 *)xfer->rx)[3] = 0;
		तर्काः->rx_दत्त_len = 4 * परिमाण(u32);
		भङ्ग;
	पक्ष RPMI_SYSMSI_SRV_GET_MSI_गुणाः:
	पक्ष RPMI_SYSMSI_SRV_समुच्चयः_MSI_STATE:
	पक्ष RPMI_SYSMSI_SRV_GET_MSI_STATE:
	पक्ष RPMI_SYSMSI_SRV_समुच्चयः_MSI_TARGET:
	पक्ष RPMI_SYSMSI_SRV_GET_MSI_TARGET:
		sys_msi_सूचकांक = le32_गन्तव्यम्_cpu(((u32 *)xfer->tx)[0]);
		यदि (smg->sys_अङ्कः_msi <= sys_msi_सूचकांक) {
			((u32 *)xfer->rx)[0] = cpu_गन्तव्यम्_le32(RPMI_ERR_INVALID_PARAM);
			तर्काः->rx_दत्त_len = परिमाण(u32);
			भङ्ग;
		}
		यदि (bitmap_test(smg->sys_msi_denied_bmap, sys_msi_सूचकांक)) {
			((u32 *)xfer->rx)[0] = cpu_गन्तव्यम्_le32(RPMI_ERR_DENIED);
			तर्काः->rx_दत्त_len = परिमाण(u32);
			भङ्ग;
		}
		यदि (तर्काः->service_id == RPMI_SYSMSI_SRV_समुच्चयः_MSI_TARGET) {
			sys_msi_स्थानाङ्कः = le32_गन्तव्यम्_cpu(((u32 *)xfer->tx)[1]);
			sys_msi_स्थानाङ्कः |= ((u64)le32_गन्तव्यम्_cpu(((u32 *)xfer->tx)[2])) << 32;
			यदि (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
							 sys_msi_स्थानाङ्कः, 0x4, PRV_S,
							 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE |
							 SBI_DOMAIN_MMIO)) {
				((u32 *)xfer->rx)[0] = cpu_गन्तव्यम्_le32(RPMI_ERR_INVALID_ADDR);
				तर्काः->rx_दत्त_len = परिमाण(u32);
				भङ्ग;
			}
		}
		rc = mbox_chan_xfer(chan, xfer);
		भङ्ग;
	स्वभाव:
		((u32 *)xfer->rx)[0] = cpu_गन्तव्यम्_le32(RPMI_ERR_NOTSUPP);
		तर्काः->rx_दत्त_len = परिमाण(u32);
		भङ्ग;
	};

	प्रत्यागम rc;
}

स्थिर शून्य mpxy_rpmi_sysmsi_cleanup(शून्य *context)
{
	संरचना mpxy_rpmi_sysmsi *smg = context;

	sbi_free(smg->sys_msi_denied_bmap);
	sbi_free(smg);
}

स्थिर पूर्णाङ्क mpxy_rpmi_sysmsi_सज्जता(शून्य **context, संरचना mbox_chan *chan,
				  ध्रुव संरचना mpxy_rpmi_mbox_दत्त *दत्त)
{
	संरचना rpmi_sysmsi_get_msi_गुणाः_resp gmaresp;
	संरचना rpmi_sysmsi_get_msi_गुणाः_req gmareq;
	संरचना rpmi_sysmsi_get_गुणाः_resp garesp;
	संरचना mpxy_rpmi_sysmsi *smg;
	u32 p2a_db_सूचकांक;
	पूर्णाङ्क rc, i;

	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_P2_A_DOORBELL_SYSMSI_सूचकांक,
				     &p2a_db_सूचकांक);
	यदि (rc)
		प्रत्यागम rc;

	rc = rpmi_प्रसामान्यम्_request_with_स्थितिः(chan, RPMI_SYSMSI_SRV_GET_गुणाः,
					     रिक्त, 0, 0, &garesp, rpmi_u32_गणनम्(garesp),
					     rpmi_u32_गणनम्(garesp));
	यदि (rc)
		प्रत्यागम rc;

	smg = sbi_zalloc(परिमाण(*smg));
	यदि (!smg)
		प्रत्यागम SBI_ENOMEM;

	smg->sys_अङ्कः_msi = garesp.sys_अङ्कः_msi;
	smg->sys_msi_denied_bmap = sbi_zalloc(bitmap_estimate_आकार(smg->sys_अङ्कः_msi));
	यदि (!smg->sys_msi_denied_bmap) {
		sbi_free(smg);
		प्रत्यागम SBI_ENOMEM;
	}

	प्रति (i = 0; i < smg->sys_अङ्कः_msi; i++) {
		gmareq.sys_msi_सूचकांक = i;
		rc = rpmi_प्रसामान्यम्_request_with_स्थितिः(chan,
						     RPMI_SYSMSI_SRV_GET_MSI_गुणाः,
						     &gmareq, rpmi_u32_गणनम्(gmareq),
						     rpmi_u32_गणनम्(gmareq),
						     &gmaresp, rpmi_u32_गणनम्(gmaresp),
						     rpmi_u32_गणनम्(gmaresp));
		यदि (rc) {
			mpxy_rpmi_sysmsi_cleanup(smg);
			प्रत्यागम rc;
		}

		यदि (p2a_db_सूचकांक == i ||
		    (gmaresp.flag0 & RPMI_SYSMSI_MSI_गुणाः_FLAG0_PREF_PRIV))
			bitmap_समुच्चयः(smg->sys_msi_denied_bmap, i, 1);
	}

	*context = smg;
	प्रत्यागम 0;
}

स्थिर संरचना mpxy_rpmi_service_दत्त sysmsi_services[] = {
{
	.id = RPMI_SYSMSI_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = परिमाण(संरचना rpmi_enable_notification_req),
	.max_tx_len = परिमाण(संरचना rpmi_enable_notification_req),
	.min_rx_len = परिमाण(संरचना rpmi_enable_notification_resp),
	.max_rx_len = परिमाण(संरचना rpmi_enable_notification_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_गुणाः,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = परिमाण(संरचना rpmi_sysmsi_get_गुणाः_resp),
	.max_rx_len = परिमाण(संरचना rpmi_sysmsi_get_गुणाः_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_गुणाः,
	.min_tx_len = परिमाण(संरचना rpmi_sysmsi_get_msi_गुणाः_req),
	.max_tx_len = परिमाण(संरचना rpmi_sysmsi_get_msi_गुणाः_req),
	.min_rx_len = परिमाण(संरचना rpmi_sysmsi_get_msi_गुणाः_resp),
	.max_rx_len = परिमाण(संरचना rpmi_sysmsi_get_msi_गुणाः_resp),
},
{
	.id = RPMI_SYSMSI_SRV_समुच्चयः_MSI_STATE,
	.min_tx_len = परिमाण(संरचना rpmi_sysmsi_समुच्चयः_msi_state_req),
	.max_tx_len = परिमाण(संरचना rpmi_sysmsi_समुच्चयः_msi_state_req),
	.min_rx_len = परिमाण(संरचना rpmi_sysmsi_समुच्चयः_msi_state_resp),
	.max_rx_len = परिमाण(संरचना rpmi_sysmsi_समुच्चयः_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_STATE,
	.min_tx_len = परिमाण(संरचना rpmi_sysmsi_get_msi_state_req),
	.max_tx_len = परिमाण(संरचना rpmi_sysmsi_get_msi_state_req),
	.min_rx_len = परिमाण(संरचना rpmi_sysmsi_get_msi_state_resp),
	.max_rx_len = परिमाण(संरचना rpmi_sysmsi_get_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_समुच्चयः_MSI_TARGET,
	.min_tx_len = परिमाण(संरचना rpmi_sysmsi_समुच्चयः_msi_target_req),
	.max_tx_len = परिमाण(संरचना rpmi_sysmsi_समुच्चयः_msi_target_req),
	.min_rx_len = परिमाण(संरचना rpmi_sysmsi_समुच्चयः_msi_target_resp),
	.max_rx_len = परिमाण(संरचना rpmi_sysmsi_समुच्चयः_msi_target_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_TARGET,
	.min_tx_len = परिमाण(संरचना rpmi_sysmsi_get_msi_target_req),
	.max_tx_len = परिमाण(संरचना rpmi_sysmsi_get_msi_target_req),
	.min_rx_len = परिमाण(संरचना rpmi_sysmsi_get_msi_target_resp),
	.max_rx_len = परिमाण(संरचना rpmi_sysmsi_get_msi_target_resp),
},
};

स्थिर ध्रुव संरचना mpxy_rpmi_mbox_दत्त sysmsi_दत्त = {
	.servicegrp_id = RPMI_SRVGRP_SYSTEM_MSI,
	.अङ्कः_services = RPMI_SYSMSI_SRV_ID_MAX_गणनम्,
	.service_दत्त = sysmsi_services,
	.xfer_गणः = mpxy_rpmi_sysmis_xfer,
	.सज्जता_गणः = mpxy_rpmi_sysmsi_सज्जता,
	.cleanup_गणः = mpxy_rpmi_sysmsi_cleanup,
};

स्थिर ध्रुव संरचना fdt_मेलनम् sysmsi_मेलनम्[] = {
	{ .compatible = "riscv,rpmi-mpxy-system-msi", .दत्त = &sysmsi_दत्त },
	{ },
};

ध्रुव संरचना fdt_driver fdt_mpxy_rpmi_sysmsi = {
	.मेलनम्_सारणी = sysmsi_मेलनम्,
	.init = mpxy_rpmi_mbox_init,
};
