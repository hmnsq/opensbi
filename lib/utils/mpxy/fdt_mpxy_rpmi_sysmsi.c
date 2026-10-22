/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_bitmap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_domain.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_byteorder.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/mailbox/rpmi_mailbox.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

ସଂରଚନା mpxy_rpmi_sysmsi {
	u32 sys_ଅଙ୍କ_msi;
	unsigned long *sys_msi_denied_bmap;
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mpxy_rpmi_sysmis_xfer(ଶୂନ୍ୟ *context, ସଂରଚନା mbox_chan *chan,
				 ସଂରଚନା mbox_xfer *xfer)
{
	ସଂରଚନା rpmi_message_args *args = xfer->args;
	ସଂରଚନା mpxy_rpmi_sysmsi *smg = context;
	u64 sys_msi_address;
	u32 sys_msi_index;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;

	ଯଦି (!xfer->rx || args->type != RPMI_ସନ୍ଦେଶ_NORMAL_REQUEST)
		ଫେରନ୍ତୁ 0;

	ବାଛ (args->service_id) {
	ସ୍ଥିତି RPMI_SYSMSI_SRV_GET_ATTRIBUTES:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_to_le32(smg->sys_ଅଙ୍କ_msi);
		((u32 *)xfer->rx)[2] = 0;
		((u32 *)xfer->rx)[3] = 0;
		args->rx_data_len = 4 * ଆକାର(u32);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES:
	ସ୍ଥିତି RPMI_SYSMSI_SRV_SET_MSI_STATE:
	ସ୍ଥିତି RPMI_SYSMSI_SRV_GET_MSI_STATE:
	ସ୍ଥିତି RPMI_SYSMSI_SRV_SET_MSI_TARGET:
	ସ୍ଥିତି RPMI_SYSMSI_SRV_GET_MSI_TARGET:
		sys_msi_index = le32_to_cpu(((u32 *)xfer->tx)[0]);
		ଯଦି (smg->sys_ଅଙ୍କ_msi <= sys_msi_index) {
			((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_INVALID_PARAM);
			args->rx_data_len = ଆକାର(u32);
			ଭାଙ୍ଗ;
		}
		ଯଦି (bitmap_test(smg->sys_msi_denied_bmap, sys_msi_index)) {
			((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_DENIED);
			args->rx_data_len = ଆକାର(u32);
			ଭାଙ୍ଗ;
		}
		ଯଦି (args->service_id == RPMI_SYSMSI_SRV_SET_MSI_TARGET) {
			sys_msi_address = le32_to_cpu(((u32 *)xfer->tx)[1]);
			sys_msi_address |= ((u64)le32_to_cpu(((u32 *)xfer->tx)[2])) << 32;
			ଯଦି (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
							 sys_msi_address, 0x4, PRV_S,
							 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE |
							 SBI_DOMAIN_MMIO)) {
				((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_INVALID_ADDR);
				args->rx_data_len = ଆକାର(u32);
				ଭାଙ୍ଗ;
			}
		}
		rc = mbox_chan_xfer(chan, xfer);
		ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_NOTSUPP);
		args->rx_data_len = ଆକାର(u32);
		ଭାଙ୍ଗ;
	};

	ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର ଶୂନ୍ୟ mpxy_rpmi_sysmsi_cleanup(ଶୂନ୍ୟ *context)
{
	ସଂରଚନା mpxy_rpmi_sysmsi *smg = context;

	sbi_free(smg->sys_msi_denied_bmap);
	sbi_free(smg);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mpxy_rpmi_sysmsi_setup(ଶୂନ୍ୟ **context, ସଂରଚନା mbox_chan *chan,
				  ଅଚଳ ସଂରଚନା mpxy_rpmi_mbox_data *data)
{
	ସଂରଚନା rpmi_sysmsi_get_msi_attributes_resp gmaresp;
	ସଂରଚନା rpmi_sysmsi_get_msi_attributes_req gmareq;
	ସଂରଚନା rpmi_sysmsi_get_attributes_resp garesp;
	ସଂରଚନା mpxy_rpmi_sysmsi *smg;
	u32 p2a_db_index;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc, i;

	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_P2A_DOORBELL_SYSMSI_INDEX,
				     &p2a_db_index);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	rc = rpmi_normal_request_with_status(chan, RPMI_SYSMSI_SRV_GET_ATTRIBUTES,
					     ଖାଲି, 0, 0, &garesp, rpmi_u32_ଗଣନା(garesp),
					     rpmi_u32_ଗଣନା(garesp));
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	smg = sbi_zalloc(ଆକାର(*smg));
	ଯଦି (!smg)
		ଫେରନ୍ତୁ SBI_ENOMEM;

	smg->sys_ଅଙ୍କ_msi = garesp.sys_ଅଙ୍କ_msi;
	smg->sys_msi_denied_bmap = sbi_zalloc(bitmap_estimate_size(smg->sys_ଅଙ୍କ_msi));
	ଯଦି (!smg->sys_msi_denied_bmap) {
		sbi_free(smg);
		ଫେରନ୍ତୁ SBI_ENOMEM;
	}

	ପାଇଁ (i = 0; i < smg->sys_ଅଙ୍କ_msi; i++) {
		gmareq.sys_msi_index = i;
		rc = rpmi_normal_request_with_status(chan,
						     RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES,
						     &gmareq, rpmi_u32_ଗଣନା(gmareq),
						     rpmi_u32_ଗଣନା(gmareq),
						     &gmaresp, rpmi_u32_ଗଣନା(gmaresp),
						     rpmi_u32_ଗଣନା(gmaresp));
		ଯଦି (rc) {
			mpxy_rpmi_sysmsi_cleanup(smg);
			ଫେରନ୍ତୁ rc;
		}

		ଯଦି (p2a_db_index == i ||
		    (gmaresp.flag0 & RPMI_SYSMSI_MSI_ATTRIBUTES_FLAG0_PREF_PRIV))
			bitmap_set(smg->sys_msi_denied_bmap, i, 1);
	}

	*context = smg;
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ସଂରଚନା mpxy_rpmi_service_data sysmsi_services[] = {
{
	.id = RPMI_SYSMSI_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = ଆକାର(ସଂରଚନା rpmi_enable_notification_req),
	.max_tx_len = ଆକାର(ସଂରଚନା rpmi_enable_notification_req),
	.min_rx_len = ଆକାର(ସଂରଚନା rpmi_enable_notification_resp),
	.max_rx_len = ଆକାର(ସଂରଚନା rpmi_enable_notification_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_ATTRIBUTES,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = ଆକାର(ସଂରଚନା rpmi_sysmsi_get_attributes_resp),
	.max_rx_len = ଆକାର(ସଂରଚନା rpmi_sysmsi_get_attributes_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES,
	.min_tx_len = ଆକାର(ସଂରଚନା rpmi_sysmsi_get_msi_attributes_req),
	.max_tx_len = ଆକାର(ସଂରଚନା rpmi_sysmsi_get_msi_attributes_req),
	.min_rx_len = ଆକାର(ସଂରଚନା rpmi_sysmsi_get_msi_attributes_resp),
	.max_rx_len = ଆକାର(ସଂରଚନା rpmi_sysmsi_get_msi_attributes_resp),
},
{
	.id = RPMI_SYSMSI_SRV_SET_MSI_STATE,
	.min_tx_len = ଆକାର(ସଂରଚନା rpmi_sysmsi_set_msi_state_req),
	.max_tx_len = ଆକାର(ସଂରଚନା rpmi_sysmsi_set_msi_state_req),
	.min_rx_len = ଆକାର(ସଂରଚନା rpmi_sysmsi_set_msi_state_resp),
	.max_rx_len = ଆକାର(ସଂରଚନା rpmi_sysmsi_set_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_STATE,
	.min_tx_len = ଆକାର(ସଂରଚନା rpmi_sysmsi_get_msi_state_req),
	.max_tx_len = ଆକାର(ସଂରଚନା rpmi_sysmsi_get_msi_state_req),
	.min_rx_len = ଆକାର(ସଂରଚନା rpmi_sysmsi_get_msi_state_resp),
	.max_rx_len = ଆକାର(ସଂରଚନା rpmi_sysmsi_get_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_SET_MSI_TARGET,
	.min_tx_len = ଆକାର(ସଂରଚନା rpmi_sysmsi_set_msi_target_req),
	.max_tx_len = ଆକାର(ସଂରଚନା rpmi_sysmsi_set_msi_target_req),
	.min_rx_len = ଆକାର(ସଂରଚନା rpmi_sysmsi_set_msi_target_resp),
	.max_rx_len = ଆକାର(ସଂରଚନା rpmi_sysmsi_set_msi_target_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_TARGET,
	.min_tx_len = ଆକାର(ସଂରଚନା rpmi_sysmsi_get_msi_target_req),
	.max_tx_len = ଆକାର(ସଂରଚନା rpmi_sysmsi_get_msi_target_req),
	.min_rx_len = ଆକାର(ସଂରଚନା rpmi_sysmsi_get_msi_target_resp),
	.max_rx_len = ଆକାର(ସଂରଚନା rpmi_sysmsi_get_msi_target_resp),
},
};

ସ୍ଥିର ଅଚଳ ସଂରଚନା mpxy_rpmi_mbox_data sysmsi_data = {
	.servicegrp_id = RPMI_SRVGRP_SYSTEM_MSI,
	.ଅଙ୍କ_services = RPMI_SYSMSI_SRV_ID_MAX_ଗଣନା,
	.service_data = sysmsi_services,
	.xfer_group = mpxy_rpmi_sysmis_xfer,
	.setup_group = mpxy_rpmi_sysmsi_setup,
	.cleanup_group = mpxy_rpmi_sysmsi_cleanup,
};

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match sysmsi_match[] = {
	{ .compatible = "riscv,rpmi-mpxy-system-msi", .data = &sysmsi_data },
	{ },
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_mpxy_rpmi_sysmsi = {
	.match_table = sysmsi_match,
	.init = mpxy_rpmi_mbox_init,
};
