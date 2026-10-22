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

ساختار mpxy_rpmi_sysmsi {
	u32 sys_رقم_msi;
	unsigned long *sys_msi_denied_bmap;
};

ایستا صحیح mpxy_rpmi_sysmis_xfer(تهی *context, ساختار mbox_chan *chan,
				 ساختار mbox_xfer *xfer)
{
	ساختار rpmi_message_args *args = xfer->args;
	ساختار mpxy_rpmi_sysmsi *smg = context;
	u64 sys_msi_address;
	u32 sys_msi_index;
	صحیح rc = 0;

	اگر (!xfer->rx || args->type != RPMI_پیام_NORMAL_REQUEST)
		بازگشت 0;

	انتخاب (args->service_id) {
	حالت RPMI_SYSMSI_SRV_GET_ATTRIBUTES:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_to_le32(smg->sys_رقم_msi);
		((u32 *)xfer->rx)[2] = 0;
		((u32 *)xfer->rx)[3] = 0;
		args->rx_data_len = 4 * اندازه(u32);
		بشکن;
	حالت RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES:
	حالت RPMI_SYSMSI_SRV_SET_MSI_STATE:
	حالت RPMI_SYSMSI_SRV_GET_MSI_STATE:
	حالت RPMI_SYSMSI_SRV_SET_MSI_TARGET:
	حالت RPMI_SYSMSI_SRV_GET_MSI_TARGET:
		sys_msi_index = le32_to_cpu(((u32 *)xfer->tx)[0]);
		اگر (smg->sys_رقم_msi <= sys_msi_index) {
			((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_INVALID_PARAM);
			args->rx_data_len = اندازه(u32);
			بشکن;
		}
		اگر (bitmap_test(smg->sys_msi_denied_bmap, sys_msi_index)) {
			((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_DENIED);
			args->rx_data_len = اندازه(u32);
			بشکن;
		}
		اگر (args->service_id == RPMI_SYSMSI_SRV_SET_MSI_TARGET) {
			sys_msi_address = le32_to_cpu(((u32 *)xfer->tx)[1]);
			sys_msi_address |= ((u64)le32_to_cpu(((u32 *)xfer->tx)[2])) << 32;
			اگر (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
							 sys_msi_address, 0x4, PRV_S,
							 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE |
							 SBI_DOMAIN_MMIO)) {
				((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_INVALID_ADDR);
				args->rx_data_len = اندازه(u32);
				بشکن;
			}
		}
		rc = mbox_chan_xfer(chan, xfer);
		بشکن;
	پیشفرض:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_NOTSUPP);
		args->rx_data_len = اندازه(u32);
		بشکن;
	};

	بازگشت rc;
}

ایستا تهی mpxy_rpmi_sysmsi_cleanup(تهی *context)
{
	ساختار mpxy_rpmi_sysmsi *smg = context;

	sbi_free(smg->sys_msi_denied_bmap);
	sbi_free(smg);
}

ایستا صحیح mpxy_rpmi_sysmsi_setup(تهی **context, ساختار mbox_chan *chan,
				  ثابت ساختار mpxy_rpmi_mbox_data *data)
{
	ساختار rpmi_sysmsi_get_msi_attributes_resp gmaresp;
	ساختار rpmi_sysmsi_get_msi_attributes_req gmareq;
	ساختار rpmi_sysmsi_get_attributes_resp garesp;
	ساختار mpxy_rpmi_sysmsi *smg;
	u32 p2a_db_index;
	صحیح rc, i;

	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_P2A_DOORBELL_SYSMSI_INDEX,
				     &p2a_db_index);
	اگر (rc)
		بازگشت rc;

	rc = rpmi_normal_request_with_status(chan, RPMI_SYSMSI_SRV_GET_ATTRIBUTES,
					     پوچ, 0, 0, &garesp, rpmi_u32_شمار(garesp),
					     rpmi_u32_شمار(garesp));
	اگر (rc)
		بازگشت rc;

	smg = sbi_zalloc(اندازه(*smg));
	اگر (!smg)
		بازگشت SBI_ENOMEM;

	smg->sys_رقم_msi = garesp.sys_رقم_msi;
	smg->sys_msi_denied_bmap = sbi_zalloc(bitmap_estimate_size(smg->sys_رقم_msi));
	اگر (!smg->sys_msi_denied_bmap) {
		sbi_free(smg);
		بازگشت SBI_ENOMEM;
	}

	برای (i = 0; i < smg->sys_رقم_msi; i++) {
		gmareq.sys_msi_index = i;
		rc = rpmi_normal_request_with_status(chan,
						     RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES,
						     &gmareq, rpmi_u32_شمار(gmareq),
						     rpmi_u32_شمار(gmareq),
						     &gmaresp, rpmi_u32_شمار(gmaresp),
						     rpmi_u32_شمار(gmaresp));
		اگر (rc) {
			mpxy_rpmi_sysmsi_cleanup(smg);
			بازگشت rc;
		}

		اگر (p2a_db_index == i ||
		    (gmaresp.flag0 & RPMI_SYSMSI_MSI_ATTRIBUTES_FLAG0_PREF_PRIV))
			bitmap_set(smg->sys_msi_denied_bmap, i, 1);
	}

	*context = smg;
	بازگشت 0;
}

ایستا ساختار mpxy_rpmi_service_data sysmsi_services[] = {
{
	.id = RPMI_SYSMSI_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = اندازه(ساختار rpmi_enable_notification_req),
	.max_tx_len = اندازه(ساختار rpmi_enable_notification_req),
	.min_rx_len = اندازه(ساختار rpmi_enable_notification_resp),
	.max_rx_len = اندازه(ساختار rpmi_enable_notification_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_ATTRIBUTES,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = اندازه(ساختار rpmi_sysmsi_get_attributes_resp),
	.max_rx_len = اندازه(ساختار rpmi_sysmsi_get_attributes_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES,
	.min_tx_len = اندازه(ساختار rpmi_sysmsi_get_msi_attributes_req),
	.max_tx_len = اندازه(ساختار rpmi_sysmsi_get_msi_attributes_req),
	.min_rx_len = اندازه(ساختار rpmi_sysmsi_get_msi_attributes_resp),
	.max_rx_len = اندازه(ساختار rpmi_sysmsi_get_msi_attributes_resp),
},
{
	.id = RPMI_SYSMSI_SRV_SET_MSI_STATE,
	.min_tx_len = اندازه(ساختار rpmi_sysmsi_set_msi_state_req),
	.max_tx_len = اندازه(ساختار rpmi_sysmsi_set_msi_state_req),
	.min_rx_len = اندازه(ساختار rpmi_sysmsi_set_msi_state_resp),
	.max_rx_len = اندازه(ساختار rpmi_sysmsi_set_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_STATE,
	.min_tx_len = اندازه(ساختار rpmi_sysmsi_get_msi_state_req),
	.max_tx_len = اندازه(ساختار rpmi_sysmsi_get_msi_state_req),
	.min_rx_len = اندازه(ساختار rpmi_sysmsi_get_msi_state_resp),
	.max_rx_len = اندازه(ساختار rpmi_sysmsi_get_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_SET_MSI_TARGET,
	.min_tx_len = اندازه(ساختار rpmi_sysmsi_set_msi_target_req),
	.max_tx_len = اندازه(ساختار rpmi_sysmsi_set_msi_target_req),
	.min_rx_len = اندازه(ساختار rpmi_sysmsi_set_msi_target_resp),
	.max_rx_len = اندازه(ساختار rpmi_sysmsi_set_msi_target_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_TARGET,
	.min_tx_len = اندازه(ساختار rpmi_sysmsi_get_msi_target_req),
	.max_tx_len = اندازه(ساختار rpmi_sysmsi_get_msi_target_req),
	.min_rx_len = اندازه(ساختار rpmi_sysmsi_get_msi_target_resp),
	.max_rx_len = اندازه(ساختار rpmi_sysmsi_get_msi_target_resp),
},
};

ایستا ثابت ساختار mpxy_rpmi_mbox_data sysmsi_data = {
	.servicegrp_id = RPMI_SRVGRP_SYSTEM_MSI,
	.رقم_services = RPMI_SYSMSI_SRV_ID_MAX_شمار,
	.service_data = sysmsi_services,
	.xfer_group = mpxy_rpmi_sysmis_xfer,
	.setup_group = mpxy_rpmi_sysmsi_setup,
	.cleanup_group = mpxy_rpmi_sysmsi_cleanup,
};

ایستا ثابت ساختار fdt_match sysmsi_match[] = {
	{ .compatible = "riscv,rpmi-mpxy-system-msi", .data = &sysmsi_data },
	{ },
};

ثابت ساختار fdt_driver fdt_mpxy_rpmi_sysmsi = {
	.match_table = sysmsi_match,
	.init = mpxy_rpmi_mbox_init,
};
