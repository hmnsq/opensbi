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
	u32 sys_num_msi;
	unsigned long *sys_msi_denied_bmap;
};

ساكن صحيح mpxy_rpmi_sysmis_xfer(فراغ *context, بنية mbox_chan *chan,
				 بنية mbox_xfer *xfer)
{
	بنية rpmi_message_args *args = xfer->args;
	بنية mpxy_rpmi_sysmsi *smg = context;
	u64 sys_msi_address;
	u32 sys_msi_index;
	صحيح rc = 0;

	אם (!xfer->rx || args->type != RPMI_MSG_NORMAL_REQUEST)
		החזר 0;

	بدل (args->service_id) {
	حالة RPMI_SYSMSI_SRV_GET_ATTRIBUTES:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_to_le32(smg->sys_num_msi);
		((u32 *)xfer->rx)[2] = 0;
		((u32 *)xfer->rx)[3] = 0;
		args->rx_data_len = 4 * حجم(u32);
		اكسر;
	حالة RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES:
	حالة RPMI_SYSMSI_SRV_SET_MSI_STATE:
	حالة RPMI_SYSMSI_SRV_GET_MSI_STATE:
	حالة RPMI_SYSMSI_SRV_SET_MSI_TARGET:
	حالة RPMI_SYSMSI_SRV_GET_MSI_TARGET:
		sys_msi_index = le32_to_cpu(((u32 *)xfer->tx)[0]);
		אם (smg->sys_num_msi <= sys_msi_index) {
			((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_INVALID_PARAM);
			args->rx_data_len = حجم(u32);
			اكسر;
		}
		אם (bitmap_test(smg->sys_msi_denied_bmap, sys_msi_index)) {
			((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_DENIED);
			args->rx_data_len = حجم(u32);
			اكسر;
		}
		אם (args->service_id == RPMI_SYSMSI_SRV_SET_MSI_TARGET) {
			sys_msi_address = le32_to_cpu(((u32 *)xfer->tx)[1]);
			sys_msi_address |= ((u64)le32_to_cpu(((u32 *)xfer->tx)[2])) << 32;
			אם (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
							 sys_msi_address, 0x4, PRV_S,
							 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE |
							 SBI_DOMAIN_MMIO)) {
				((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_INVALID_ADDR);
				args->rx_data_len = حجم(u32);
				اكسر;
			}
		}
		rc = mbox_chan_xfer(chan, xfer);
		اكسر;
	افتراضي:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_NOTSUPP);
		args->rx_data_len = حجم(u32);
		اكسر;
	};

	החזר rc;
}

ساكن فراغ mpxy_rpmi_sysmsi_cleanup(فراغ *context)
{
	بنية mpxy_rpmi_sysmsi *smg = context;

	sbi_free(smg->sys_msi_denied_bmap);
	sbi_free(smg);
}

ساكن صحيح mpxy_rpmi_sysmsi_setup(فراغ **context, بنية mbox_chan *chan,
				  ثابت بنية mpxy_rpmi_mbox_data *data)
{
	بنية rpmi_sysmsi_get_msi_attributes_resp gmaresp;
	بنية rpmi_sysmsi_get_msi_attributes_req gmareq;
	بنية rpmi_sysmsi_get_attributes_resp garesp;
	بنية mpxy_rpmi_sysmsi *smg;
	u32 p2a_db_index;
	صحيح rc, i;

	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_P2A_DOORBELL_SYSMSI_INDEX,
				     &p2a_db_index);
	אם (rc)
		החזר rc;

	rc = rpmi_normal_request_with_status(chan, RPMI_SYSMSI_SRV_GET_ATTRIBUTES,
					     عدم, 0, 0, &garesp, rpmi_u32_count(garesp),
					     rpmi_u32_count(garesp));
	אם (rc)
		החזר rc;

	smg = sbi_zalloc(حجم(*smg));
	אם (!smg)
		החזר SBI_ENOMEM;

	smg->sys_num_msi = garesp.sys_num_msi;
	smg->sys_msi_denied_bmap = sbi_zalloc(bitmap_estimate_size(smg->sys_num_msi));
	אם (!smg->sys_msi_denied_bmap) {
		sbi_free(smg);
		החזר SBI_ENOMEM;
	}

	עבור (i = 0; i < smg->sys_num_msi; i++) {
		gmareq.sys_msi_index = i;
		rc = rpmi_normal_request_with_status(chan,
						     RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES,
						     &gmareq, rpmi_u32_count(gmareq),
						     rpmi_u32_count(gmareq),
						     &gmaresp, rpmi_u32_count(gmaresp),
						     rpmi_u32_count(gmaresp));
		אם (rc) {
			mpxy_rpmi_sysmsi_cleanup(smg);
			החזר rc;
		}

		אם (p2a_db_index == i ||
		    (gmaresp.flag0 & RPMI_SYSMSI_MSI_ATTRIBUTES_FLAG0_PREF_PRIV))
			bitmap_set(smg->sys_msi_denied_bmap, i, 1);
	}

	*context = smg;
	החזר 0;
}

ساكن بنية mpxy_rpmi_service_data sysmsi_services[] = {
{
	.id = RPMI_SYSMSI_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = حجم(بنية rpmi_enable_notification_req),
	.max_tx_len = حجم(بنية rpmi_enable_notification_req),
	.min_rx_len = حجم(بنية rpmi_enable_notification_resp),
	.max_rx_len = حجم(بنية rpmi_enable_notification_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_ATTRIBUTES,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = حجم(بنية rpmi_sysmsi_get_attributes_resp),
	.max_rx_len = حجم(بنية rpmi_sysmsi_get_attributes_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES,
	.min_tx_len = حجم(بنية rpmi_sysmsi_get_msi_attributes_req),
	.max_tx_len = حجم(بنية rpmi_sysmsi_get_msi_attributes_req),
	.min_rx_len = حجم(بنية rpmi_sysmsi_get_msi_attributes_resp),
	.max_rx_len = حجم(بنية rpmi_sysmsi_get_msi_attributes_resp),
},
{
	.id = RPMI_SYSMSI_SRV_SET_MSI_STATE,
	.min_tx_len = حجم(بنية rpmi_sysmsi_set_msi_state_req),
	.max_tx_len = حجم(بنية rpmi_sysmsi_set_msi_state_req),
	.min_rx_len = حجم(بنية rpmi_sysmsi_set_msi_state_resp),
	.max_rx_len = حجم(بنية rpmi_sysmsi_set_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_STATE,
	.min_tx_len = حجم(بنية rpmi_sysmsi_get_msi_state_req),
	.max_tx_len = حجم(بنية rpmi_sysmsi_get_msi_state_req),
	.min_rx_len = حجم(بنية rpmi_sysmsi_get_msi_state_resp),
	.max_rx_len = حجم(بنية rpmi_sysmsi_get_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_SET_MSI_TARGET,
	.min_tx_len = حجم(بنية rpmi_sysmsi_set_msi_target_req),
	.max_tx_len = حجم(بنية rpmi_sysmsi_set_msi_target_req),
	.min_rx_len = حجم(بنية rpmi_sysmsi_set_msi_target_resp),
	.max_rx_len = حجم(بنية rpmi_sysmsi_set_msi_target_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_TARGET,
	.min_tx_len = حجم(بنية rpmi_sysmsi_get_msi_target_req),
	.max_tx_len = حجم(بنية rpmi_sysmsi_get_msi_target_req),
	.min_rx_len = حجم(بنية rpmi_sysmsi_get_msi_target_resp),
	.max_rx_len = حجم(بنية rpmi_sysmsi_get_msi_target_resp),
},
};

ساكن ثابت بنية mpxy_rpmi_mbox_data sysmsi_data = {
	.servicegrp_id = RPMI_SRVGRP_SYSTEM_MSI,
	.num_services = RPMI_SYSMSI_SRV_ID_MAX_COUNT,
	.service_data = sysmsi_services,
	.xfer_group = mpxy_rpmi_sysmis_xfer,
	.setup_group = mpxy_rpmi_sysmsi_setup,
	.cleanup_group = mpxy_rpmi_sysmsi_cleanup,
};

ساكن ثابت بنية fdt_match sysmsi_match[] = {
	{ .compatible = "riscv,rpmi-mpxy-system-msi", .data = &sysmsi_data },
	{ },
};

ثابت بنية fdt_driver fdt_mpxy_rpmi_sysmsi = {
	.match_table = sysmsi_match,
	.init = mpxy_rpmi_mbox_init,
};
