/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Rahul Pathak <rpathak@ventanamicro.com>
 *   Subrahmanya Lingappa <slingappa@ventanamicro.com>
 *   Anup Patel <apatel@ventanamicro.com>
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi/sbi_timer.h>
#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/riscv_locks.h>
#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/riscv_barrier.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_helper.h>
#అన్తర్భావ <sbi_utils/mailbox/mailbox.h>
#అన్తర్భావ <sbi_utils/mailbox/fdt_mailbox.h>
#అన్తర్భావ <sbi_utils/mailbox/rpmi_mailbox.h>

/** Minimum Base group version required */
#నిర్వచించు RPMI_పునాది_సంస్కరణ_MIN		RPMI_సంస్కరణ(1, 0)

/**************** RPMI Transport Structures and Macros ***********/

#నిర్వచించు GET_SERVICEGROUP_ID(సందేశం)		\
({						\
	నిర్మాణం rpmi_సందేశం *mbuf = సందేశం;	\
	le16_గమ్యం_cpu(mbuf->శీర్షిక.servicegroup_id);\
})

#నిర్వచించు GET_SERVICE_ID(సందేశం)			\
({						\
	నిర్మాణం rpmi_సందేశం *mbuf = సందేశం;	\
	mbuf->శీర్షిక.service_id;		\
})

#నిర్వచించు GET_FLAGS(సందేశం)				\
({						\
	నిర్మాణం rpmi_సందేశం *mbuf = సందేశం;	\
	mbuf->శీర్షిక.flags;			\
})

#నిర్వచించు GET_సందేశం_ID(సందేశం)			\
({						\
	నిర్మాణం rpmi_సందేశం *mbuf = సందేశం;	\
	((u32)mbuf->శీర్షిక.flags << (RPMI_సందేశం_FLAGS_స్థానభ్రంశం * 8)) | \
	((u32)mbuf->శీర్షిక.service_id << (RPMI_సందేశం_SERVICE_ID_స్థానభ్రంశం * 8)) | \
	((u32)le16_గమ్యం_cpu(mbuf->శీర్షిక.servicegroup_id)); \
})

#నిర్వచించు MAKE_సందేశం_ID(__group_id, __service_id, __flags)	\
({						\
	u32 __ret = 0;				\
	__ret |= (u32)(__group_id) << (RPMI_సందేశం_SERVICEGROUP_ID_స్థానభ్రంశం * 8); \
	__ret |= (u32)(__service_id) << (RPMI_సందేశం_SERVICE_ID_స్థానభ్రంశం * 8); \
	__ret |= (u32)(__flags) << (RPMI_సందేశం_FLAGS_స్థానభ్రంశం * 8); \
	__ret;					\
})

#నిర్వచించు GET_DLEN(సందేశం)				\
({						\
	నిర్మాణం rpmi_సందేశం *mbuf = సందేశం;	\
	le16_గమ్యం_cpu(mbuf->శీర్షిక.datalen);	\
})

#నిర్వచించు GET_TOKEN(సందేశం)				\
({						\
	నిర్మాణం rpmi_సందేశం *mbuf = సందేశం;	\
	le16_గమ్యం_cpu(mbuf->శీర్షిక.token);	\
})

#నిర్వచించు GET_సందేశం_TYPE(సందేశం)						\
({									\
	uint8_t flags = *((uint8_t *)సందేశం + RPMI_సందేశం_FLAGS_స్థానభ్రంశం);	\
	((flags & RPMI_సందేశం_FLAGS_TYPE) >> RPMI_సందేశం_FLAGS_TYPE_POS));	\
})

గణన rpmi_వరుస_type {
	RPMI_వరుస_TYPE_REQ = 0,
	RPMI_వరుస_TYPE_ACK = 1,
};

గణన rpmi_వరుస_idx {
	RPMI_వరుస_IDX_A2_P_REQ = 0,
	RPMI_వరుస_IDX_P2_A_ACK = 1,
	RPMI_వరుస_IDX_P2_A_REQ = 2,
	RPMI_వరుస_IDX_A2_P_ACK = 3,
	RPMI_వరుస_IDX_MAX_లెక్క,
};

గణన rpmi_reg_idx {
	RPMI_REG_IDX_DB_REG = 0, /* Doorbell register */
	RPMI_REG_IDX_MAX_లెక్క,
};

/** Mailbox registers */
నిర్మాణం rpmi_mb_regs {
	/* doorbell from AP -> PuC*/
	అస్థిర le32_t db_reg;
} __packed;

/** Single Queue Context Structure */
నిర్మాణం smq_వరుస_ctx {
	u32 వరుస_id;
	u32 అంకె_slots;
	spinlock_t వరుస_తాళం;
	/* Type of queue - REQ or ACK */
	గణన rpmi_వరుస_type వరుస_type;
	/* Pointers to the queue shared memory */
	అస్థిర le32_t *headptr PT_GUARDED_BY(&వరుస_తాళం);
	అస్థిర le32_t *tailptr PT_GUARDED_BY(&వరుస_తాళం);
	అస్థిర uint8_t *బఫరు PT_GUARDED_BY(&వరుస_తాళం);
	/* Name of the queue */
	అక్షరం name[RPMI_NAME_CHARS_MAX];
};

నిర్మాణం rpmi_srvgrp_chan {
	u32 servicegroup_id;
	u32 servicegroup_సంస్కరణ;
	నిర్మాణం mbox_chan chan;
};

#నిర్వచించు గమ్యం_srvgrp_chan(mbox_chan)	\
		container_యొక్క(mbox_chan, నిర్మాణం rpmi_srvgrp_chan, chan);

నిర్మాణం rpmi_shmem_mbox_controller {
	/* Driver specific members */
	u32 స్థానం_కొలత;
	u32 వరుస_లెక్క;
	u32 p2a_doorbell_sysmsi_సూచీ;
	u32 a2p_doorbell_విలువ;
	నిర్మాణం rpmi_mb_regs *mb_regs;
	నిర్మాణం smq_వరుస_ctx వరుస_ctx_tbl[RPMI_వరుస_IDX_MAX_లెక్క];
	/* Mailbox framework related members */
	నిర్మాణం mbox_controller controller;
	నిర్మాణం mbox_chan *పునాది_chan;
	u32 impl_సంస్కరణ;
	u32 impl_id;
	u32 spec_సంస్కరణ;
	u32 plat_info_len;
	అక్షరం *plat_info;
	నిర్మాణం {
		u8 f0_priv_level;
		bool f0_ev_notif_en;
	} పునాది_flags;
};

/**************** Shared Memory Queues Helpers **************/

స్థిర bool __smq_queue_full(నిర్మాణం smq_వరుస_ctx *qctx)
	MUST_HOLD(&qctx->వరుస_తాళం)
{
	తిరిగి ((le32_గమ్యం_cpu(*qctx->tailptr) + 1) % qctx->అంకె_slots ==
			le32_గమ్యం_cpu(*qctx->headptr)) ? true : false;
}

స్థిర bool __smq_queue_empty(నిర్మాణం smq_వరుస_ctx *qctx)
	MUST_HOLD(&qctx->వరుస_తాళం)
{
	తిరిగి (le32_గమ్యం_cpu(*qctx->headptr) ==
		le32_గమ్యం_cpu(*qctx->tailptr)) ? true : false;
}

స్థిర పూర్ణాంకం __smq_rx(నిర్మాణం smq_వరుస_ctx *qctx, u32 స్థానం_కొలత,
		    u32 service_సమూహం_id, నిర్మాణం mbox_xfer *xfer)
	MUST_HOLD(&qctx->వరుస_తాళం)
{
	శూన్య *dst, *src;
	నిర్మాణం rpmi_సందేశం *సందేశం;
	u32 i, tmp, pos, dlen, msgidn, headidx, tailidx;
	నిర్మాణం rpmi_సందేశం_వాదనలు *వాదనలు = xfer->వాదనలు;
	bool no_rx_token = (వాదనలు->flags & RPMI_సందేశం_FLAGS_NO_RX_TOKEN) ?
			   true : false;

	/* Rx sanity checks */
	ఒకవేళ ((పరిమాణం(u32) * వాదనలు->rx_endian_పదాలు) >
	    (స్థానం_కొలత - పరిమాణం(నిర్మాణం rpmi_సందేశం_శీర్షిక)))
		తిరిగి SBI_EINVAL;
	ఒకవేళ ((పరిమాణం(u32) * వాదనలు->rx_endian_పదాలు) > xfer->rx_len)
		తిరిగి SBI_EINVAL;

	/* There should be some message in the queue */
	ఒకవేళ (__smq_queue_empty(qctx))
		తిరిగి SBI_ENOENT;

	/* Get the head/read index and tail/write index */
	headidx = le32_గమ్యం_cpu(*qctx->headptr);
	tailidx = le32_గమ్యం_cpu(*qctx->tailptr);

	/*
	 * Compute msgidn expected in the incoming message
	 * NOTE: DOORBELL bit is not expected to be set.
	 */
	msgidn = MAKE_సందేశం_ID(service_సమూహం_id, వాదనలు->service_id, వాదనలు->type);

	/* Find the Rx message with matching token */
	pos = headidx;
	అయితే (pos != tailidx) {
		src = (శూన్య *)qctx->బఫరు + (pos * స్థానం_కొలత);
		ఒకవేళ ((no_rx_token && GET_సందేశం_ID(src) == msgidn) ||
		    (GET_TOKEN(src) == (xfer->seq & RPMI_సందేశం_TOKEN_ముసుగు)))
			ఆపు;
		pos = (pos + 1) % qctx->అంకె_slots;
	}
	ఒకవేళ (pos == tailidx)
		తిరిగి SBI_ENOENT;

	/* If Rx message is not first message then make it first message */
	ఒకవేళ (pos != headidx) {
		src = (శూన్య *)qctx->బఫరు + (pos * స్థానం_కొలత);
		dst = (శూన్య *)qctx->బఫరు + (headidx * స్థానం_కొలత);
		కొరకు (i = 0; i < స్థానం_కొలత / పరిమాణం(u32); i++) {
			tmp = ((u32 *)dst)[i];
			((u32 *)dst)[i] = ((u32 *)src)[i];
			((u32 *)src)[i] = tmp;
		}
	}

	/* Update rx_token if not available */
	సందేశం = (శూన్య *)qctx->బఫరు + (headidx * స్థానం_కొలత);
	ఒకవేళ (no_rx_token)
		వాదనలు->rx_token = GET_TOKEN(సందేశం);

	/* Extract data from the first message */
	ఒకవేళ (xfer->rx) {
		వాదనలు->rx_దత్తాంశం_len = dlen = GET_DLEN(సందేశం);
		ఒకవేళ (dlen > xfer->rx_len)
			dlen = xfer->rx_len;
		src = (శూన్య *)సందేశం + పరిమాణం(నిర్మాణం rpmi_సందేశం_శీర్షిక);
		dst = xfer->rx;
		కొరకు (i = 0; i < వాదనలు->rx_endian_పదాలు; i++)
			((u32 *)dst)[i] = le32_గమ్యం_cpu(((u32 *)src)[i]);
		dst += పరిమాణం(u32) * వాదనలు->rx_endian_పదాలు;
		src += పరిమాణం(u32) * వాదనలు->rx_endian_పదాలు;
		sbi_memcpy(dst, src,
			xfer->rx_len - (పరిమాణం(u32) * వాదనలు->rx_endian_పదాలు));
	}

	/* Update the head/read index */
	*qctx->headptr = cpu_గమ్యం_le32(headidx + 1) % qctx->అంకె_slots;

	/* Make sure updates to head are immediately visible to PuC */
	smp_wmb();

	తిరిగి SBI_OK;
}

స్థిర పూర్ణాంకం __smq_tx(నిర్మాణం smq_వరుస_ctx *qctx, నిర్మాణం rpmi_mb_regs *mb_regs,
		    u32 a2p_doorbell_విలువ, u32 స్థానం_కొలత, u32 service_సమూహం_id,
		    నిర్మాణం mbox_xfer *xfer) MUST_HOLD(&qctx->వరుస_తాళం)
{
	u32 i, tailidx;
	శూన్య *dst, *src;
	నిర్మాణం rpmi_సందేశం_శీర్షిక శీర్షిక = { 0 };
	నిర్మాణం rpmi_సందేశం_వాదనలు *వాదనలు = xfer->వాదనలు;

	/* Tx sanity checks */
	ఒకవేళ ((పరిమాణం(u32) * వాదనలు->tx_endian_పదాలు) >
	    (స్థానం_కొలత - పరిమాణం(నిర్మాణం rpmi_సందేశం_శీర్షిక)))
		తిరిగి SBI_EINVAL;
	ఒకవేళ ((పరిమాణం(u32) * వాదనలు->tx_endian_పదాలు) > xfer->tx_len)
		తిరిగి SBI_EINVAL;

	/* There should be some room in the queue */
	ఒకవేళ (__smq_queue_full(qctx))
		తిరిగి SBI_ENOMEM;

	/* Get the tail/write index */
	tailidx = le32_గమ్యం_cpu(*qctx->tailptr);

	/* Prepare the header to be written into the slot */
	శీర్షిక.servicegroup_id = cpu_గమ్యం_le16(service_సమూహం_id);
	శీర్షిక.service_id = వాదనలు->service_id;
	శీర్షిక.flags = వాదనలు->type;
	శీర్షిక.datalen = cpu_గమ్యం_le16((u16)xfer->tx_len);
	శీర్షిక.token = cpu_గమ్యం_le16((u16)xfer->seq);

	/* Write header into the slot */
	dst = (అక్షరం *)qctx->బఫరు + (tailidx * స్థానం_కొలత);
	sbi_memcpy(dst, &శీర్షిక, పరిమాణం(శీర్షిక));
	dst += పరిమాణం(శీర్షిక);

	/* Write data into the slot */
	ఒకవేళ (xfer->tx) {
		src = xfer->tx;
		కొరకు (i = 0; i < వాదనలు->tx_endian_పదాలు; i++)
			((u32 *)dst)[i] = cpu_గమ్యం_le32(((u32 *)src)[i]);
		dst += పరిమాణం(u32) * వాదనలు->tx_endian_పదాలు;
		src += పరిమాణం(u32) * వాదనలు->tx_endian_పదాలు;
		sbi_memcpy(dst, src,
			xfer->tx_len - (పరిమాణం(u32) * వాదనలు->tx_endian_పదాలు));
	}

	/* Make sure queue chanages are visible to PuC before updating tail */
	smp_wmb();

	/* Update the tail/write index */
	*qctx->tailptr = cpu_గమ్యం_le32(tailidx + 1) % qctx->అంకె_slots;

	/* Ring the RPMI doorbell if present */
	ఒకవేళ (mb_regs)
		writel(a2p_doorbell_విలువ, &mb_regs->db_reg);

	తిరిగి SBI_OK;
}

స్థిర పూర్ణాంకం smq_rx(నిర్మాణం rpmi_shmem_mbox_controller *mctl,
		  u32 వరుస_id, u32 service_సమూహం_id, నిర్మాణం mbox_xfer *xfer)
{
	పూర్ణాంకం ret, rxretry = 0;
	నిర్మాణం smq_వరుస_ctx *qctx;

	ఒకవేళ (mctl->వరుస_లెక్క < వరుస_id) {
		sbi_printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		తిరిగి SBI_EINVAL;
	}
	qctx = &mctl->వరుస_ctx_tbl[వరుస_id];

	/*
	 * Once the timeout happens and call this function is returned
	 * to the client then there is no way to deliver the response
	 * message after that if it comes later.
	 *
	 * REVISIT: In complete timeout duration how much duration
	 * it should wait(delay) before recv retry. udelay or mdelay
	 */
	చేయి {
		spin_తాళం(&qctx->వరుస_తాళం);
		ret = __smq_rx(qctx, mctl->స్థానం_కొలత, service_సమూహం_id, xfer);
		spin_unlock(&qctx->వరుస_తాళం);
		ఒకవేళ (!ret)
			తిరిగి 0;

		sbi_timer_mdelay(1);
		rxretry += 1;
	} అయితే (rxretry < xfer->rx_timeout);

	తిరిగి SBI_ETIMEDOUT;
}

స్థిర పూర్ణాంకం smq_tx(నిర్మాణం rpmi_shmem_mbox_controller *mctl,
		  u32 వరుస_id, u32 service_సమూహం_id, నిర్మాణం mbox_xfer *xfer)
{
	పూర్ణాంకం ret, txretry = 0;
	నిర్మాణం smq_వరుస_ctx *qctx;

	ఒకవేళ (mctl->వరుస_లెక్క < వరుస_id) {
		sbi_printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		తిరిగి SBI_EINVAL;
	}
	qctx = &mctl->వరుస_ctx_tbl[వరుస_id];

	/*
	 * Ignoring the tx timeout since in RPMI has no mechanism
	 * with which other side can let know about the reception of
	 * message which marks as tx complete. For RPMI tx complete is
	 * marked as done when message in successfully copied in queue.
	 *
	 * REVISIT: In complete timeout duration how much duration
	 * it should wait(delay) before send retry. udelay or mdelay
	 */
	చేయి {
		spin_తాళం(&qctx->వరుస_తాళం);
		ret = __smq_tx(qctx, mctl->mb_regs, mctl->a2p_doorbell_విలువ,
				mctl->స్థానం_కొలత, service_సమూహం_id, xfer);
		spin_unlock(&qctx->వరుస_తాళం);
		ఒకవేళ (!ret)
			తిరిగి 0;

		sbi_timer_mdelay(1);
		txretry += 1;
	} అయితే (txretry < xfer->tx_timeout);

	తిరిగి SBI_ETIMEDOUT;
}

స్థిర పూర్ణాంకం rpmi_get_platform_info(నిర్మాణం rpmi_shmem_mbox_controller *mctl)
{
	పూర్ణాంకం ret = SBI_OK;

	/**
	 * platform string can occupy max possible size
	 * max possible space in the message data as
	 * per the format
	 */
	నిర్మాణం rpmi_పునాది_get_platform_info_resp *resp =
			sbi_zalloc(RPMI_సందేశం_దత్తాంశం_కొలత(mctl->స్థానం_కొలత));
	ఒకవేళ (!resp)
		తిరిగి SBI_ENOMEM;

	ret = rpmi_సాధారణ_request_with_స్థితి(mctl->పునాది_chan,
				 RPMI_పునాది_SRV_GET_PLATFORM_INFO,
				 ఖాళీ, 0, 0,
				 resp,
				 RPMI_సందేశం_దత్తాంశం_కొలత(mctl->స్థానం_కొలత)/4,
				 RPMI_సందేశం_దత్తాంశం_కొలత(mctl->స్థానం_కొలత)/4);
	ఒకవేళ (ret)
		వెళ్ళు fail_free_resp;

	mctl->plat_info_len = resp->plat_info_len;
	mctl->plat_info = sbi_zalloc(mctl->plat_info_len);
	ఒకవేళ (!mctl->plat_info) {
		ret = SBI_ENOMEM;
		వెళ్ళు fail_free_resp;
	}

	sbi_strncpy(mctl->plat_info, resp->plat_info, mctl->plat_info_len);

fail_free_resp:
	sbi_free(resp);
	తిరిగి ret;
}

స్థిర పూర్ణాంకం smq_పునాది_get_two_u32(నిర్మాణం rpmi_shmem_mbox_controller *mctl,
				u32 service_id, u32 *inarg, u32 *outvals)
{
	తిరిగి rpmi_సాధారణ_request_with_స్థితి(
			mctl->పునాది_chan, service_id,
			inarg, (inarg) ? 1 : 0, (inarg) ? 1 : 0,
			outvals, 2, 2);
}

/**************** Mailbox Controller Functions **************/

స్థిర పూర్ణాంకం rpmi_shmem_mbox_xfer(నిర్మాణం mbox_chan *chan, నిర్మాణం mbox_xfer *xfer)
{
	పూర్ణాంకం ret;
	u32 tx_qid = 0, rx_qid = 0;
	నిర్మాణం rpmi_shmem_mbox_controller *mctl =
			container_యొక్క(chan->mbox,
				     నిర్మాణం rpmi_shmem_mbox_controller,
				     controller);
	నిర్మాణం rpmi_srvgrp_chan *srvgrp_chan = గమ్యం_srvgrp_chan(chan);

	నిర్మాణం rpmi_సందేశం_వాదనలు *వాదనలు = xfer->వాదనలు;
	bool do_tx = (వాదనలు->flags & RPMI_సందేశం_FLAGS_NO_TX) ? false : true;
	bool do_rx = (వాదనలు->flags & RPMI_సందేశం_FLAGS_NO_RX) ? false : true;

	ఒకవేళ (!do_tx && !do_rx)
		తిరిగి SBI_EINVAL;

	ఎంపిక (వాదనలు->type) {
	స్థితి RPMI_సందేశం_సాధారణ_REQUEST:
		ఒకవేళ (do_tx && do_rx) {
			tx_qid = RPMI_వరుస_IDX_A2_P_REQ;
			rx_qid = RPMI_వరుస_IDX_P2_A_ACK;
		} లేకపోతే ఒకవేళ (do_tx) {
			tx_qid = RPMI_వరుస_IDX_A2_P_REQ;
		} లేకపోతే ఒకవేళ (do_rx) {
			rx_qid = RPMI_వరుస_IDX_P2_A_REQ;
		}
		ఆపు;
	స్థితి RPMI_సందేశం_POSTED_REQUEST:
		ఒకవేళ (do_tx && do_rx)
			తిరిగి SBI_EINVAL;
		ఒకవేళ (do_tx) {
			tx_qid = RPMI_వరుస_IDX_A2_P_REQ;
		} లేకపోతే {
			rx_qid = RPMI_వరుస_IDX_P2_A_REQ;
		}
		ఆపు;
	స్థితి RPMI_సందేశం_ACKNOWLDGEMENT:
		ఒకవేళ (do_tx && do_rx)
			తిరిగి SBI_EINVAL;
		ఒకవేళ (do_tx) {
			tx_qid = RPMI_వరుస_IDX_A2_P_ACK;
		} లేకపోతే {
			rx_qid = RPMI_వరుస_IDX_P2_A_ACK;
		}
		ఆపు;
	అప్రమేయ:
		తిరిగి SBI_ENOTSUPP;
	}

	ఒకవేళ (do_tx) {
		ret = smq_tx(mctl, tx_qid, srvgrp_chan->servicegroup_id, xfer);
		ఒకవేళ (ret)
			తిరిగి ret;
	}

	ఒకవేళ (do_rx) {
		ret = smq_rx(mctl, rx_qid, srvgrp_chan->servicegroup_id, xfer);
		ఒకవేళ (ret)
			తిరిగి ret;
	}

	తిరిగి 0;
}

స్థిర పూర్ణాంకం rpmi_shmem_mbox_get_attribute(నిర్మాణం mbox_chan *chan,
					 పూర్ణాంకం attr_id, శూన్య *బహిర్గతం_విలువ)
{
	నిర్మాణం rpmi_shmem_mbox_controller *mctl =
			container_యొక్క(chan->mbox,
				     నిర్మాణం rpmi_shmem_mbox_controller,
				     controller);
	నిర్మాణం rpmi_srvgrp_chan *srvgrp_chan = గమ్యం_srvgrp_chan(chan);

	ఎంపిక (attr_id) {
	స్థితి RPMI_CHANNEL_ATTR_PROTOCOL_సంస్కరణ:
		*((u32 *)బహిర్గతం_విలువ) = mctl->spec_సంస్కరణ;
		ఆపు;
	స్థితి RPMI_CHANNEL_ATTR_MAX_దత్తాంశం_LEN:
		*((u32 *)బహిర్గతం_విలువ) = RPMI_సందేశం_దత్తాంశం_కొలత(mctl->స్థానం_కొలత);
		ఆపు;
	స్థితి RPMI_CHANNEL_ATTR_P2_A_DOORBELL_SYSMSI_సూచీ:
		*((u32 *)బహిర్గతం_విలువ) = mctl->p2a_doorbell_sysmsi_సూచీ;
		ఆపు;
	స్థితి RPMI_CHANNEL_ATTR_TX_TIMEOUT:
		*((u32 *)బహిర్గతం_విలువ) = RPMI_DEF_TX_TIMEOUT;
		ఆపు;
	స్థితి RPMI_CHANNEL_ATTR_RX_TIMEOUT:
		*((u32 *)బహిర్గతం_విలువ) = RPMI_DEF_RX_TIMEOUT;
		ఆపు;
	స్థితి RPMI_CHANNEL_ATTR_SERVICEGROUP_ID:
		*((u32 *)బహిర్గతం_విలువ) = srvgrp_chan->servicegroup_id;
		ఆపు;
	స్థితి RPMI_CHANNEL_ATTR_SERVICEGROUP_సంస్కరణ:
		*((u32 *)బహిర్గతం_విలువ) = srvgrp_chan->servicegroup_సంస్కరణ;
		ఆపు;
	స్థితి RPMI_CHANNEL_ATTR_IMPL_ID:
		*((u32 *)బహిర్గతం_విలువ) = mctl->impl_id;
		ఆపు;
	స్థితి RPMI_CHANNEL_ATTR_IMPL_సంస్కరణ:
		*((u32 *)బహిర్గతం_విలువ) = mctl->impl_సంస్కరణ;
		ఆపు;
	అప్రమేయ:
		తిరిగి SBI_ENOTSUPP;
	}

	తిరిగి 0;
}

స్థిర నిర్మాణం mbox_chan *rpmi_shmem_mbox_request_chan(
						నిర్మాణం mbox_controller *mbox,
						u32 *chan_వాదనలు)
{
	పూర్ణాంకం ret;
	u32 tval[2] = { 0 };
	నిర్మాణం rpmi_srvgrp_chan *srvgrp_chan;
	నిర్మాణం rpmi_shmem_mbox_controller *mctl =
			container_యొక్క(mbox,
				     నిర్మాణం rpmi_shmem_mbox_controller,
				     controller);

	/* Service group id not defined or in reserved range is invalid */
	ఒకవేళ (chan_వాదనలు[0] >= RPMI_SRVGRP_ID_MAX_లెక్క &&
		chan_వాదనలు[0] <= RPMI_SRVGRP_RESERVE_చివర)
		తిరిగి ఖాళీ;

	/* Base serivce group is always present so probe other groups */
	ఒకవేళ (chan_వాదనలు[0] != RPMI_SRVGRP_పునాది) {
		/* Probe service group */
		ret = smq_పునాది_get_two_u32(mctl,
					   RPMI_పునాది_SRV_PROBE_SERVICE_సమూహం,
					   chan_వాదనలు, tval);
		ఒకవేళ (ret || !tval[1])
			తిరిగి ఖాళీ;
	}

	srvgrp_chan = sbi_zalloc(పరిమాణం(*srvgrp_chan));
	ఒకవేళ (!srvgrp_chan)
		తిరిగి ఖాళీ;

	srvgrp_chan->servicegroup_id = chan_వాదనలు[0];
	srvgrp_chan->servicegroup_సంస్కరణ = tval[1];

	తిరిగి &srvgrp_chan->chan;
}

స్థిర శూన్య rpmi_shmem_mbox_free_chan(నిర్మాణం mbox_controller *mbox,
				      నిర్మాణం mbox_chan *chan)
{
	నిర్మాణం rpmi_srvgrp_chan *srvgrp_chan = గమ్యం_srvgrp_chan(chan);
	sbi_free(srvgrp_chan);
}

బాహ్య నిర్మాణం fdt_mailbox fdt_mailbox_rpmi_shmem;

స్థిర పూర్ణాంకం rpmi_shmem_transport_init(నిర్మాణం rpmi_shmem_mbox_controller *mctl,
				     స్థిరరాశి శూన్య *fdt,
				     పూర్ణాంకం nodeoff) NO_THREAD_SAFETY_ANALYSIS
{
	స్థిరరాశి అక్షరం *name;
	స్థిరరాశి fdt32_t *prop;
	పూర్ణాంకం లెక్క, len, ret, qid;
	uint64_t reg_addr, reg_కొలత;
	నిర్మాణం smq_వరుస_ctx *qctx;

	ret = fdt_కణుపు_check_compatible(fdt, nodeoff,
					"riscv,rpmi-shmem-mbox");
	ఒకవేళ (ret)
		తిరిగి ret;

	/* get queue slot size in bytes */
	prop = fdt_getprop(fdt, nodeoff, "riscv,slot-size", &len);
	ఒకవేళ (!prop)
		తిరిగి SBI_ENOENT;

	mctl->స్థానం_కొలత = fdt32_గమ్యం_cpu(*prop);
	ఒకవేళ (mctl->స్థానం_కొలత < RPMI_స్థానం_కొలత_MIN) {
		sbi_printf("%s: slot_size < mimnum required message size\n",
			   __func__);
		mctl->స్థానం_కొలత = RPMI_స్థానం_కొలత_MIN;
	}

	/* get p2a doorbell system MSI index */
	prop = fdt_getprop(fdt, nodeoff, "riscv,p2a-doorbell-sysmsi-index", &len);
	mctl->p2a_doorbell_sysmsi_సూచీ = prop ? fdt32_గమ్యం_cpu(*prop) : -1U;

	/* get a2p doorbell value */
	prop = fdt_getprop(fdt, nodeoff, "riscv,a2p-doorbell-value", &len);
	mctl->a2p_doorbell_విలువ = prop ? fdt32_గమ్యం_cpu(*prop) : 1;

	/*
	 * queue names count is taken as the number of queues
	 * supported which make it mandatory to provide the
	 * name of the queue.
	 */
	లెక్క = fdt_stringlist_లెక్క(fdt, nodeoff, "reg-names");
	ఒకవేళ (లెక్క < 0 ||
	    లెక్క > (RPMI_వరుస_IDX_MAX_లెక్క + RPMI_REG_IDX_MAX_లెక్క))
		తిరిగి SBI_EINVAL;

	mctl->వరుస_లెక్క = లెక్క - RPMI_REG_IDX_MAX_లెక్క;

	/* parse all queues and populate queues context structure */
	కొరకు (qid = 0; qid < mctl->వరుస_లెక్క; qid++) {
		qctx = &mctl->వరుస_ctx_tbl[qid];

		/* get each queue share-memory base address and size*/
		ret = fdt_get_కణుపు_addr_కొలత(fdt, nodeoff, qid,
					     &reg_addr, &reg_కొలత);
		ఒకవేళ (ret < 0 || !reg_addr || !reg_కొలత)
			తిరిగి SBI_ENOENT;

		ret = sbi_domain_మూలం_కూడు_memrange(reg_addr, reg_కొలత, reg_కొలత,
						   (SBI_DOMAIN_MEMREGION_MMIO |
						    SBI_DOMAIN_MEMREGION_M_READABLE |
						    SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ఒకవేళ (ret)
			తిరిగి ret;

		/* calculate number of slots in each queue */
		qctx->అంకె_slots =
			(reg_కొలత - (mctl->స్థానం_కొలత * RPMI_వరుస_శీర్షిక_SLOTS)) / mctl->స్థానం_కొలత;

		/* setup queue pointers */
		qctx->headptr = ((శూన్య *)(గుర్తులేని దీర్ఘ)reg_addr) +
				RPMI_వరుస_తల_స్థానం * mctl->స్థానం_కొలత;
		qctx->tailptr = ((శూన్య *)(గుర్తులేని దీర్ఘ)reg_addr) +
				RPMI_వరుస_తోక_స్థానం * mctl->స్థానం_కొలత;
		qctx->బఫరు = ((శూన్య *)(గుర్తులేని దీర్ఘ)reg_addr) +
				RPMI_వరుస_శీర్షిక_SLOTS * mctl->స్థానం_కొలత;

		/* get the queue name */
		name = fdt_stringlist_get(fdt, nodeoff, "reg-names",
					  qid, &len);
		ఒకవేళ (!name || (name && len < 0))
			తిరిగి len;

		sbi_memcpy(qctx->name, name, len);

		/* store the index as queue_id */
		qctx->వరుస_id = qid;

		SPIN_తాళం_INIT(qctx->వరుస_తాళం);
	}

	/* get the a2p-doorbell property name */
	name = fdt_stringlist_get(fdt, nodeoff, "reg-names", qid, &len);
	ఒకవేళ (!name || (name && len < 0))
		తిరిగి len;

	/* fetch doorbell register address*/
	ret = fdt_get_కణుపు_addr_కొలత(fdt, nodeoff, qid, &reg_addr,
				       &reg_కొలత);
	ఒకవేళ (!ret && !(అక్షరశ్రేణి_పోలిక(name, "a2p-doorbell", అక్షరశ్రేణి_పొడవు("a2p-doorbell")))) {
		mctl->mb_regs = (శూన్య *)(గుర్తులేని దీర్ఘ)reg_addr;
		ret = sbi_domain_మూలం_కూడు_memrange(reg_addr, reg_కొలత, reg_కొలత,
						   (SBI_DOMAIN_MEMREGION_MMIO |
						    SBI_DOMAIN_MEMREGION_M_READABLE |
						    SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ఒకవేళ (ret)
			తిరిగి ret;
	}

	తిరిగి SBI_SUCCESS;
}

స్థిర పూర్ణాంకం rpmi_shmem_mbox_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff,
				స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	నిర్మాణం rpmi_పునాది_get_లక్షణాలు_resp resp;
	నిర్మాణం rpmi_shmem_mbox_controller *mctl;
	నిర్మాణం rpmi_srvgrp_chan *పునాది_srvgrp;
	u32 tval[2], వాదనలు[1];
	పూర్ణాంకం ret = 0;

	mctl = sbi_zalloc(పరిమాణం(*mctl));
	ఒకవేళ (!mctl)
		తిరిగి SBI_ENOMEM;

	/* Initialization transport from device tree */
	ret = rpmi_shmem_transport_init(mctl, fdt, nodeoff);
	ఒకవేళ (ret)
		వెళ్ళు fail_free_controller;

	/* Register mailbox controller */
	mctl->controller.id = nodeoff;
	mctl->controller.max_xfer_len =
			mctl->స్థానం_కొలత - పరిమాణం(నిర్మాణం rpmi_సందేశం_శీర్షిక);
	mctl->controller.driver = &fdt_mailbox_rpmi_shmem;
	mctl->controller.request_chan = rpmi_shmem_mbox_request_chan;
	mctl->controller.free_chan = rpmi_shmem_mbox_free_chan;
	mctl->controller.xfer = rpmi_shmem_mbox_xfer;
	mctl->controller.get_attribute = rpmi_shmem_mbox_get_attribute;
	ret = mbox_controller_కూడు(&mctl->controller);
	ఒకవేళ (ret)
		వెళ్ళు fail_free_controller;

	/* Request base service group channel */
	tval[0] = RPMI_SRVGRP_పునాది;
	mctl->పునాది_chan = mbox_controller_request_chan(&mctl->controller,
							tval);
	ఒకవేళ (!mctl->పునాది_chan) {
		ret = SBI_ENOENT;
		వెళ్ళు fail_remove_controller;
	}

	/* Update base service group version */
	పునాది_srvgrp = గమ్యం_srvgrp_chan(mctl->పునాది_chan);
	వాదనలు[0] = RPMI_SRVGRP_పునాది;
	ret = smq_పునాది_get_two_u32(mctl, RPMI_పునాది_SRV_PROBE_SERVICE_సమూహం,
				   &వాదనలు[0], tval);
	ఒకవేళ (ret)
		వెళ్ళు fail_free_chan;
	పునాది_srvgrp->servicegroup_సంస్కరణ = tval[1];
	ఒకవేళ (పునాది_srvgrp->servicegroup_సంస్కరణ < RPMI_పునాది_సంస్కరణ_MIN) {
		ret = SBI_EINVAL;
		వెళ్ళు fail_free_chan;
	}

	/* Get implementation id */
	ret = smq_పునాది_get_two_u32(mctl,
				   RPMI_పునాది_SRV_GET_IMPLEMENTATION_సంస్కరణ,
				   ఖాళీ, tval);
	ఒకవేళ (ret)
		వెళ్ళు fail_free_chan;
	mctl->impl_సంస్కరణ = tval[1];

	/* Get implementation version */
	ret = smq_పునాది_get_two_u32(mctl, RPMI_పునాది_SRV_GET_IMPLEMENTATION_IDN,
				   ఖాళీ, tval);
	ఒకవేళ (ret)
		వెళ్ళు fail_free_chan;
	mctl->impl_id = tval[1];

	/* Get specification version */
	ret = smq_పునాది_get_two_u32(mctl, RPMI_పునాది_SRV_GET_SPEC_సంస్కరణ,
				   ఖాళీ, tval);
	ఒకవేళ (ret)
		వెళ్ళు fail_free_chan;
	mctl->spec_సంస్కరణ = tval[1];
	ఒకవేళ (mctl->spec_సంస్కరణ < RPMI_పునాది_సంస్కరణ_MIN ||
	    mctl->spec_సంస్కరణ != పునాది_srvgrp->servicegroup_సంస్కరణ) {
		ret = SBI_EINVAL;
		వెళ్ళు fail_free_chan;
	}

	/* Get optional features implementation flags */
	ret = rpmi_సాధారణ_request_with_స్థితి(
			mctl->పునాది_chan, RPMI_పునాది_SRV_GET_లక్షణాలు,
			ఖాళీ, 0, 0,
			&resp, rpmi_u32_లెక్క(resp), rpmi_u32_లెక్క(resp));
	ఒకవేళ (ret)
		వెళ్ళు fail_free_chan;

	/* 1: M-mode, 0: S-mode */
	mctl->పునాది_flags.f0_priv_level =
			resp.f0 & RPMI_పునాది_FLAGS_F0_PRIVILEGE ? 1 : 0;
	/* 1: Supported, 0: Not Supported */
	mctl->పునాది_flags.f0_ev_notif_en =
			resp.f0 & RPMI_పునాది_FLAGS_F0_EV_NOTIFY ? 1 : 0;

	/* We only use M-mode RPMI context in OpenSBI */
	ఒకవేళ (!mctl->పునాది_flags.f0_priv_level) {
		ret = SBI_ENODEV;
		వెళ్ళు fail_free_chan;
	}

	/*
	 * Continue without platform information string if not
	 * available or if an error is encountered while fetching
	 */
	rpmi_get_platform_info(mctl);

	తిరిగి 0;

fail_free_chan:
	mbox_controller_free_chan(mctl->పునాది_chan);
fail_remove_controller:
	mbox_controller_remove(&mctl->controller);
fail_free_controller:
	sbi_free(mctl);
	తిరిగి ret;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక rpmi_shmem_mbox_సరిపోలిక[] = {
	{ .compatible = "riscv,rpmi-shmem-mbox" },
	{ },
};

నిర్మాణం fdt_mailbox fdt_mailbox_rpmi_shmem = {
	.driver = {
		.సరిపోలిక_పట్టిక = rpmi_shmem_mbox_సరిపోలిక,
		.init = rpmi_shmem_mbox_init,
	},
	.xlate = fdt_mailbox_simple_xlate,
};
