/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Ventana Micro Systems Inc.
 */

#समावेश <sbi/sbi_bitmap.h>
#समावेश <sbi/sbi_domain.h>
#समावेश <sbi/sbi_byteorder.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi_utils/mailbox/rpmi_mailbox.h>
#समावेश <sbi_utils/mpxy/fdt_mpxy_rpmi_mbox.h>

काष्ठा mpxy_rpmi_sysmsi {
	u32 sys_अंक_msi;
	अचिन्हित दीर्घ *sys_msi_denied_bmap;
};

अटल पूर्णांक mpxy_rpmi_sysmis_xfer(व्योम *context, काष्ठा mbox_chan *chan,
				 काष्ठा mbox_xfer *xfer)
{
	काष्ठा rpmi_message_args *args = xfer->args;
	काष्ठा mpxy_rpmi_sysmsi *smg = context;
	u64 sys_msi_address;
	u32 sys_msi_सूचकांक;
	पूर्णांक rc = 0;

	अगर (!xfer->rx || args->type != RPMI_संदेश_NORMAL_REQUEST)
		वापस 0;

	चयन (args->service_id) {
	हाल RPMI_SYSMSI_SRV_GET_ATTRIBUTES:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_SUCCESS);
		((u32 *)xfer->rx)[1] = cpu_to_le32(smg->sys_अंक_msi);
		((u32 *)xfer->rx)[2] = 0;
		((u32 *)xfer->rx)[3] = 0;
		args->rx_तथ्य_len = 4 * माप(u32);
		अवरोध;
	हाल RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES:
	हाल RPMI_SYSMSI_SRV_SET_MSI_STATE:
	हाल RPMI_SYSMSI_SRV_GET_MSI_STATE:
	हाल RPMI_SYSMSI_SRV_SET_MSI_TARGET:
	हाल RPMI_SYSMSI_SRV_GET_MSI_TARGET:
		sys_msi_सूचकांक = le32_to_cpu(((u32 *)xfer->tx)[0]);
		अगर (smg->sys_अंक_msi <= sys_msi_सूचकांक) {
			((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_INVALID_PARAM);
			args->rx_तथ्य_len = माप(u32);
			अवरोध;
		}
		अगर (bitmap_test(smg->sys_msi_denied_bmap, sys_msi_सूचकांक)) {
			((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_DENIED);
			args->rx_तथ्य_len = माप(u32);
			अवरोध;
		}
		अगर (args->service_id == RPMI_SYSMSI_SRV_SET_MSI_TARGET) {
			sys_msi_address = le32_to_cpu(((u32 *)xfer->tx)[1]);
			sys_msi_address |= ((u64)le32_to_cpu(((u32 *)xfer->tx)[2])) << 32;
			अगर (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
							 sys_msi_address, 0x4, PRV_S,
							 SBI_DOMAIN_READ | SBI_DOMAIN_WRITE |
							 SBI_DOMAIN_MMIO)) {
				((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_INVALID_ADDR);
				args->rx_तथ्य_len = माप(u32);
				अवरोध;
			}
		}
		rc = mbox_chan_xfer(chan, xfer);
		अवरोध;
	शेष:
		((u32 *)xfer->rx)[0] = cpu_to_le32(RPMI_ERR_NOTSUPP);
		args->rx_तथ्य_len = माप(u32);
		अवरोध;
	};

	वापस rc;
}

अटल व्योम mpxy_rpmi_sysmsi_cleanup(व्योम *context)
{
	काष्ठा mpxy_rpmi_sysmsi *smg = context;

	sbi_free(smg->sys_msi_denied_bmap);
	sbi_free(smg);
}

अटल पूर्णांक mpxy_rpmi_sysmsi_setup(व्योम **context, काष्ठा mbox_chan *chan,
				  स्थिर काष्ठा mpxy_rpmi_mbox_तथ्य *तथ्य)
{
	काष्ठा rpmi_sysmsi_get_msi_attributes_resp gmaresp;
	काष्ठा rpmi_sysmsi_get_msi_attributes_req gmareq;
	काष्ठा rpmi_sysmsi_get_attributes_resp garesp;
	काष्ठा mpxy_rpmi_sysmsi *smg;
	u32 p2a_db_सूचकांक;
	पूर्णांक rc, i;

	rc = mbox_chan_get_attribute(chan, RPMI_CHANNEL_ATTR_P2_A_DOORBELL_SYSMSI_सूचकांक,
				     &p2a_db_सूचकांक);
	अगर (rc)
		वापस rc;

	rc = rpmi_normal_request_with_status(chan, RPMI_SYSMSI_SRV_GET_ATTRIBUTES,
					     शून्य, 0, 0, &garesp, rpmi_u32_गिनती(garesp),
					     rpmi_u32_गिनती(garesp));
	अगर (rc)
		वापस rc;

	smg = sbi_zalloc(माप(*smg));
	अगर (!smg)
		वापस SBI_ENOMEM;

	smg->sys_अंक_msi = garesp.sys_अंक_msi;
	smg->sys_msi_denied_bmap = sbi_zalloc(bitmap_estimate_आकार(smg->sys_अंक_msi));
	अगर (!smg->sys_msi_denied_bmap) {
		sbi_free(smg);
		वापस SBI_ENOMEM;
	}

	क्रम (i = 0; i < smg->sys_अंक_msi; i++) {
		gmareq.sys_msi_सूचकांक = i;
		rc = rpmi_normal_request_with_status(chan,
						     RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES,
						     &gmareq, rpmi_u32_गिनती(gmareq),
						     rpmi_u32_गिनती(gmareq),
						     &gmaresp, rpmi_u32_गिनती(gmaresp),
						     rpmi_u32_गिनती(gmaresp));
		अगर (rc) {
			mpxy_rpmi_sysmsi_cleanup(smg);
			वापस rc;
		}

		अगर (p2a_db_सूचकांक == i ||
		    (gmaresp.flag0 & RPMI_SYSMSI_MSI_ATTRIBUTES_FLAG0_PREF_PRIV))
			bitmap_set(smg->sys_msi_denied_bmap, i, 1);
	}

	*context = smg;
	वापस 0;
}

अटल काष्ठा mpxy_rpmi_service_तथ्य sysmsi_services[] = {
{
	.id = RPMI_SYSMSI_SRV_ENABLE_NOTIFICATION,
	.min_tx_len = माप(काष्ठा rpmi_enable_notification_req),
	.max_tx_len = माप(काष्ठा rpmi_enable_notification_req),
	.min_rx_len = माप(काष्ठा rpmi_enable_notification_resp),
	.max_rx_len = माप(काष्ठा rpmi_enable_notification_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_ATTRIBUTES,
	.min_tx_len = 0,
	.max_tx_len = 0,
	.min_rx_len = माप(काष्ठा rpmi_sysmsi_get_attributes_resp),
	.max_rx_len = माप(काष्ठा rpmi_sysmsi_get_attributes_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_ATTRIBUTES,
	.min_tx_len = माप(काष्ठा rpmi_sysmsi_get_msi_attributes_req),
	.max_tx_len = माप(काष्ठा rpmi_sysmsi_get_msi_attributes_req),
	.min_rx_len = माप(काष्ठा rpmi_sysmsi_get_msi_attributes_resp),
	.max_rx_len = माप(काष्ठा rpmi_sysmsi_get_msi_attributes_resp),
},
{
	.id = RPMI_SYSMSI_SRV_SET_MSI_STATE,
	.min_tx_len = माप(काष्ठा rpmi_sysmsi_set_msi_state_req),
	.max_tx_len = माप(काष्ठा rpmi_sysmsi_set_msi_state_req),
	.min_rx_len = माप(काष्ठा rpmi_sysmsi_set_msi_state_resp),
	.max_rx_len = माप(काष्ठा rpmi_sysmsi_set_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_STATE,
	.min_tx_len = माप(काष्ठा rpmi_sysmsi_get_msi_state_req),
	.max_tx_len = माप(काष्ठा rpmi_sysmsi_get_msi_state_req),
	.min_rx_len = माप(काष्ठा rpmi_sysmsi_get_msi_state_resp),
	.max_rx_len = माप(काष्ठा rpmi_sysmsi_get_msi_state_resp),
},
{
	.id = RPMI_SYSMSI_SRV_SET_MSI_TARGET,
	.min_tx_len = माप(काष्ठा rpmi_sysmsi_set_msi_target_req),
	.max_tx_len = माप(काष्ठा rpmi_sysmsi_set_msi_target_req),
	.min_rx_len = माप(काष्ठा rpmi_sysmsi_set_msi_target_resp),
	.max_rx_len = माप(काष्ठा rpmi_sysmsi_set_msi_target_resp),
},
{
	.id = RPMI_SYSMSI_SRV_GET_MSI_TARGET,
	.min_tx_len = माप(काष्ठा rpmi_sysmsi_get_msi_target_req),
	.max_tx_len = माप(काष्ठा rpmi_sysmsi_get_msi_target_req),
	.min_rx_len = माप(काष्ठा rpmi_sysmsi_get_msi_target_resp),
	.max_rx_len = माप(काष्ठा rpmi_sysmsi_get_msi_target_resp),
},
};

अटल स्थिर काष्ठा mpxy_rpmi_mbox_तथ्य sysmsi_तथ्य = {
	.servicegrp_id = RPMI_SRVGRP_SYSTEM_MSI,
	.अंक_services = RPMI_SYSMSI_SRV_ID_MAX_गिनती,
	.service_तथ्य = sysmsi_services,
	.xfer_group = mpxy_rpmi_sysmis_xfer,
	.setup_group = mpxy_rpmi_sysmsi_setup,
	.cleanup_group = mpxy_rpmi_sysmsi_cleanup,
};

अटल स्थिर काष्ठा fdt_match sysmsi_match[] = {
	{ .compatible = "riscv,rpmi-mpxy-system-msi", .तथ्य = &sysmsi_तथ्य },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_mpxy_rpmi_sysmsi = {
	.match_table = sysmsi_match,
	.init = mpxy_rpmi_mbox_init,
};
