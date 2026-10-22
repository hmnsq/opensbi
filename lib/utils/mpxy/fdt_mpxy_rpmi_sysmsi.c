/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#include <sbi/sbi_bitmap.h>
#include <sbi/sbi_domain.h>
#include <sbi/sbi_byteorder.h>
#include <sbi/sbi_heap.h>
#include <sbi_utils/mailbox/rpmi_mailbox.h>
#include <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

sa_nracanaa mpxy_rpmi_sysmsi {
	u32 sys_num_msi;
	unsigned long *sys_msi_denied_bmap;
};

static จำนวนเต็ม mpxy_rpmi_sysmis_xfer(void *context, sa_nracanaa mbox_chan *chan,
				 sa_nracanaa mbox_xfer *xfer)
{
	sa_nracanaa rpmi_message_args *args = xfer->args;
	sa_nracanaa mpxy_rpmi_sysmsi *smg = context;
	u64 sys_msi_address;
	u32 sys_msi_index;
	จำนวนเต็ม rc = 0;

	ถ้า (!xfer->rx || args->type != RPMI_MSG_NORMAL_REQUEST)
		คืน 0;

	switch (args->service_id) {
	pak_sa RPMI_SYSMSI_SRV_GET_ATTRIBUTES:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_to_le32(smg->sys_num_msi);
		((u32 *)xfer->rx)[2] = 0;
		((u32 *)xfer->rx)[3] = 0;
		args->rx_data_len = 4 * parimaa_na(u32);
		bha_nga;
	pak_sa RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES:
	pak_sa RPMI_SYSMSI_SRV_SET_MSI_STATE:
	pak_sa RPMI_SYSMSI_SRV_GET_MSI_STATE:
	pak_sa RPMI_SYSMSI_SRV_SET_MSI_TARGET:
	pak_sa RPMI_SYSMSI_SRV_GET_MSI_TARGET:
		sys_msi_index = le32_to_cpu(((u32 *)xfer->tx)[0]);
		ถ้า (smg->sys_num_msi <= sys_msi_index) {
			((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_INVALID_PARAM);
			args->rx_data_len = parimaa_na(u32);
			bha_nga;
		}
		ถ้า (bitmap_test(smg->sys_msi_denied_bmap, sys_msi_index)) {
			((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_DENIED);
			args->rx_data_len = parimaa_na(u32);
			bha_nga;
		}
		ถ้า (args->service_id == RPMI_SYSMSI_SRV_SET_MSI_TARGET) {
			sys_msi_address = le32_to_cpu(((u32 *)xfer->tx)[1]);
			sys_msi_address |= ((u64)le32_to_cpu(((u32 *)xfer->tx)[2])) << 32;
			ถ้า (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
							 sys_msi_address, 0x4, PRV_S,
							 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE |
							 SBI_DOMAIN_MMIO)) {
				((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_INVALID_ADDR);
				args->rx_data_len = parimaa_na(u32);
				bha_nga;
			}
		}
		rc = mbox_chan_xfer(chan, xfer);
		bha_nga;
	default:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_NOTSUPP);
		args->rx_data_len = parimaa_na(u32);
		bha_nga;
	};

	คืน rc;
}

static void mpxy_rpmi_sysmsi_cleanup(void *context)
{
	sa_nracanaa mpxy_rpmi_sysmsi *smg = context;

	sbi_free(smg->sys_msi_denied_bmap);
	sbi_free(smg);
}

static จำนวนเต็ม mpxy_rpmi_sysmsi_setup(void **context, sa_nracanaa mbox_chan *chan,
				  const sa_nracanaa mpxy_rpmi_mbox_data *data)
{
	sa_nracanaa rpmi_sysmsi_get_msi_attributes_resp gmaresp;
	sa_nracanaa rpmi_sysmsi_get_msi_attributes_req gmareq;
	sa_nracanaa rpmi_sysmsi_get_attributes_resp garesp;
	sa_nracanaa mpxy_rpmi_sysmsi *smg;
	u32 p2a_db_index;
	จำนวนเต็ม rc, i;

	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_P2A_DOORBELL_SYSMSI_INDEX,
				     &p2a_db_index);
	ถ้า (rc)
		คืน rc;

	rc = rpmi_normal_request_with_status(chan, RPMI_SYSMSI_SRV_GET_ATTRIBUTES,
					     NULL, 0, 0, &garesp, rpmi_u32_count(garesp),
					     rpmi_u32_count(garesp));
	ถ้า (rc)
		คืน rc;

	smg = sbi_zalloc(parimaa_na(*smg));
	ถ้า (!smg)
		คืน SBI_ENOMEM;

	smg->sys_num_msi = garesp.sys_num_msi;
	smg->sys_msi_denied_bmap = sbi_zalloc(bitmap_estimate_size(smg->sys_num_msi));
	ถ้า (!smg->sys_msi_denied_bmap) {
		sbi_free(smg);
		คืน SBI_ENOMEM;
	}

	สำหรับ (i = 0; i < smg->sys_num_msi; i++) {
		gmareq.sys_msi_index = i;
		rc = rpmi_normal_request_with_status(chan,
						     RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES,
						     &gmareq, rpmi_u32_count(gmareq),
						     rpmi_u32_count(gmareq),
						     &gmaresp, rpmi_u32_count(gmaresp),
						     rpmi_u32_count(gmaresp));
		ถ้า (rc) {
			mpxy_rpmi_sysmsi_cleanup(smg);
			คืน rc;
		}

		ถ้า (p2a_db_index == i ||
		    (gmaresp.flag0 & RPMI_SYSMSI_MSI_ATTRIBUTES_FLAG0_PREF_PRIV))
			bitmap_set(smg->sys_msi_denied_bmap, i, 1);
	}

	*context = smg;
	คืน 0;
}

static sa_nracanaa mpxy_rpmi_service_data sysmsi_services[] = {
{
	.id = RPMI_SYSMSI_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = parimaa_na(sa_nracanaa rpmi_enable_notification_req),
	.max_tx_len = parimaa_na(sa_nracanaa rpmi_enable_notification_req),
	.min_rx_len = parimaa_na(sa_nracanaa rpmi_enable_notification_resp),
	.max_rx_len = parimaa_na(sa_nracanaa rpmi_enable_notification_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_ATTRIBUTES,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = parimaa_na(sa_nracanaa rpmi_sysmsi_get_attributes_resp),
	.max_rx_len = parimaa_na(sa_nracanaa rpmi_sysmsi_get_attributes_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES,
	.min_tx_len = parimaa_na(sa_nracanaa rpmi_sysmsi_get_msi_attributes_req),
	.max_tx_len = parimaa_na(sa_nracanaa rpmi_sysmsi_get_msi_attributes_req),
	.min_rx_len = parimaa_na(sa_nracanaa rpmi_sysmsi_get_msi_attributes_resp),
	.max_rx_len = parimaa_na(sa_nracanaa rpmi_sysmsi_get_msi_attributes_resp),
},
{
	.id = RPMI_SYSMSI_SRV_SET_MSI_STATE,
	.min_tx_len = parimaa_na(sa_nracanaa rpmi_sysmsi_set_msi_state_req),
	.max_tx_len = parimaa_na(sa_nracanaa rpmi_sysmsi_set_msi_state_req),
	.min_rx_len = parimaa_na(sa_nracanaa rpmi_sysmsi_set_msi_state_resp),
	.max_rx_len = parimaa_na(sa_nracanaa rpmi_sysmsi_set_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_STATE,
	.min_tx_len = parimaa_na(sa_nracanaa rpmi_sysmsi_get_msi_state_req),
	.max_tx_len = parimaa_na(sa_nracanaa rpmi_sysmsi_get_msi_state_req),
	.min_rx_len = parimaa_na(sa_nracanaa rpmi_sysmsi_get_msi_state_resp),
	.max_rx_len = parimaa_na(sa_nracanaa rpmi_sysmsi_get_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_SET_MSI_TARGET,
	.min_tx_len = parimaa_na(sa_nracanaa rpmi_sysmsi_set_msi_target_req),
	.max_tx_len = parimaa_na(sa_nracanaa rpmi_sysmsi_set_msi_target_req),
	.min_rx_len = parimaa_na(sa_nracanaa rpmi_sysmsi_set_msi_target_resp),
	.max_rx_len = parimaa_na(sa_nracanaa rpmi_sysmsi_set_msi_target_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_TARGET,
	.min_tx_len = parimaa_na(sa_nracanaa rpmi_sysmsi_get_msi_target_req),
	.max_tx_len = parimaa_na(sa_nracanaa rpmi_sysmsi_get_msi_target_req),
	.min_rx_len = parimaa_na(sa_nracanaa rpmi_sysmsi_get_msi_target_resp),
	.max_rx_len = parimaa_na(sa_nracanaa rpmi_sysmsi_get_msi_target_resp),
},
};

static const sa_nracanaa mpxy_rpmi_mbox_data sysmsi_data = {
	.servicegrp_id = RPMI_SRVGRP_SYSTEM_MSI,
	.num_services = RPMI_SYSMSI_SRV_ID_MAX_COUNT,
	.service_data = sysmsi_services,
	.xfer_group = mpxy_rpmi_sysmis_xfer,
	.setup_group = mpxy_rpmi_sysmsi_setup,
	.cleanup_group = mpxy_rpmi_sysmsi_cleanup,
};

static const sa_nracanaa fdt_match sysmsi_match[] = {
	{ .compatible = "riscv,rpmi-mpxy-system-msi", .data = &sysmsi_data },
	{ },
};

const sa_nracanaa fdt_driver fdt_mpxy_rpmi_sysmsi = {
	.match_table = sysmsi_match,
	.init = mpxy_rpmi_mbox_init,
};
