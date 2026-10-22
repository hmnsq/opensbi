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

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi/sbi_timer.h>
#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/riscv_locks.h>
#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/riscv_barrier.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_helper.h>
#अन्तर्भाव <sbi_utils/mailbox/mailbox.h>
#अन्तर्भाव <sbi_utils/mailbox/fdt_mailbox.h>
#अन्तर्भाव <sbi_utils/mailbox/rpmi_mailbox.h>

/** Minimum Base group version required */
#परिभाषय RPMI_आधारः_संस्करणम्_MIN		RPMI_संस्करणम्(1, 0)

/**************** RPMI Transport Structures and Macros ***********/

#परिभाषय GET_SERVICEGROUP_ID(सन्देशः)		\
({						\
	संरचना rpmi_सन्देशः *mbuf = सन्देशः;	\
	le16_गन्तव्यम्_cpu(mbuf->शीर्षकम्.servicegroup_id);\
})

#परिभाषय GET_SERVICE_ID(सन्देशः)			\
({						\
	संरचना rpmi_सन्देशः *mbuf = सन्देशः;	\
	mbuf->शीर्षकम्.service_id;		\
})

#परिभाषय GET_FLAGS(सन्देशः)				\
({						\
	संरचना rpmi_सन्देशः *mbuf = सन्देशः;	\
	mbuf->शीर्षकम्.flags;			\
})

#परिभाषय GET_सन्देशः_ID(सन्देशः)			\
({						\
	संरचना rpmi_सन्देशः *mbuf = सन्देशः;	\
	((u32)mbuf->शीर्षकम्.flags << (RPMI_सन्देशः_FLAGS_विस्थापनम् * 8)) | \
	((u32)mbuf->शीर्षकम्.service_id << (RPMI_सन्देशः_SERVICE_ID_विस्थापनम् * 8)) | \
	((u32)le16_गन्तव्यम्_cpu(mbuf->शीर्षकम्.servicegroup_id)); \
})

#परिभाषय MAKE_सन्देशः_ID(__group_id, __service_id, __flags)	\
({						\
	u32 __ret = 0;				\
	__ret |= (u32)(__group_id) << (RPMI_सन्देशः_SERVICEGROUP_ID_विस्थापनम् * 8); \
	__ret |= (u32)(__service_id) << (RPMI_सन्देशः_SERVICE_ID_विस्थापनम् * 8); \
	__ret |= (u32)(__flags) << (RPMI_सन्देशः_FLAGS_विस्थापनम् * 8); \
	__ret;					\
})

#परिभाषय GET_DLEN(सन्देशः)				\
({						\
	संरचना rpmi_सन्देशः *mbuf = सन्देशः;	\
	le16_गन्तव्यम्_cpu(mbuf->शीर्षकम्.datalen);	\
})

#परिभाषय GET_TOKEN(सन्देशः)				\
({						\
	संरचना rpmi_सन्देशः *mbuf = सन्देशः;	\
	le16_गन्तव्यम्_cpu(mbuf->शीर्षकम्.token);	\
})

#परिभाषय GET_सन्देशः_TYPE(सन्देशः)						\
({									\
	uint8_t flags = *((uint8_t *)सन्देशः + RPMI_सन्देशः_FLAGS_विस्थापनम्);	\
	((flags & RPMI_सन्देशः_FLAGS_TYPE) >> RPMI_सन्देशः_FLAGS_TYPE_POS));	\
})

गणना rpmi_queue_type {
	RPMI_QUEUE_TYPE_REQ = 0,
	RPMI_QUEUE_TYPE_ACK = 1,
};

गणना rpmi_queue_idx {
	RPMI_QUEUE_IDX_A2P_REQ = 0,
	RPMI_QUEUE_IDX_P2A_ACK = 1,
	RPMI_QUEUE_IDX_P2A_REQ = 2,
	RPMI_QUEUE_IDX_A2P_ACK = 3,
	RPMI_QUEUE_IDX_MAX_गणनम्,
};

गणना rpmi_reg_idx {
	RPMI_REG_IDX_DB_REG = 0, /* Doorbell register */
	RPMI_REG_IDX_MAX_गणनम्,
};

/** Mailbox registers */
संरचना rpmi_mb_regs {
	/* doorbell from AP -> PuC*/
	चञ्चल le32_t db_reg;
} __packed;

/** Single Queue Context Structure */
संरचना smq_queue_ctx {
	u32 queue_id;
	u32 अङ्कः_slots;
	spinlock_t queue_तालकम्;
	/* Type of queue - REQ or ACK */
	गणना rpmi_queue_type queue_type;
	/* Pointers to the queue shared memory */
	चञ्चल le32_t *headptr PT_GUARDED_BY(&queue_तालकम्);
	चञ्चल le32_t *tailptr PT_GUARDED_BY(&queue_तालकम्);
	चञ्चल uint8_t *बफर PT_GUARDED_BY(&queue_तालकम्);
	/* Name of the queue */
	वर्ण name[RPMI_NAME_CHARS_MAX];
};

संरचना rpmi_srvgrp_chan {
	u32 servicegroup_id;
	u32 servicegroup_संस्करणम्;
	संरचना mbox_chan chan;
};

#परिभाषय गन्तव्यम्_srvgrp_chan(mbox_chan)	\
		container_सम्बन्धि(mbox_chan, संरचना rpmi_srvgrp_chan, chan);

संरचना rpmi_shmem_mbox_controller {
	/* Driver specific members */
	u32 slot_आकार;
	u32 queue_गणनम्;
	u32 p2a_doorbell_sysmsi_सूचकांक;
	u32 a2p_doorbell_मानम्;
	संरचना rpmi_mb_regs *mb_regs;
	संरचना smq_queue_ctx queue_ctx_tbl[RPMI_QUEUE_IDX_MAX_गणनम्];
	/* Mailbox framework related members */
	संरचना mbox_controller controller;
	संरचना mbox_chan *आधारः_chan;
	u32 impl_संस्करणम्;
	u32 impl_id;
	u32 spec_संस्करणम्;
	u32 plat_info_len;
	वर्ण *plat_info;
	संरचना {
		u8 f0_priv_level;
		bool f0_ev_notif_en;
	} आधारः_flags;
};

/**************** Shared Memory Queues Helpers **************/

स्थिर bool __smq_queue_full(संरचना smq_queue_ctx *qctx)
	MUST_HOLD(&qctx->queue_तालकम्)
{
	प्रत्यागम ((le32_गन्तव्यम्_cpu(*qctx->tailptr) + 1) % qctx->अङ्कः_slots ==
			le32_गन्तव्यम्_cpu(*qctx->headptr)) ? true : false;
}

स्थिर bool __smq_queue_empty(संरचना smq_queue_ctx *qctx)
	MUST_HOLD(&qctx->queue_तालकम्)
{
	प्रत्यागम (le32_गन्तव्यम्_cpu(*qctx->headptr) ==
		le32_गन्तव्यम्_cpu(*qctx->tailptr)) ? true : false;
}

स्थिर पूर्णाङ्क __smq_rx(संरचना smq_queue_ctx *qctx, u32 slot_आकार,
		    u32 service_गणः_id, संरचना mbox_xfer *xfer)
	MUST_HOLD(&qctx->queue_तालकम्)
{
	शून्य *dst, *src;
	संरचना rpmi_सन्देशः *सन्देशः;
	u32 i, tmp, pos, dlen, msgidn, headidx, tailidx;
	संरचना rpmi_सन्देशः_तर्काः *तर्काः = xfer->तर्काः;
	bool no_rx_token = (तर्काः->flags & RPMI_सन्देशः_FLAGS_NO_RX_TOKEN) ?
			   true : false;

	/* Rx sanity checks */
	यदि ((परिमाण(u32) * तर्काः->rx_endian_शब्दाः) >
	    (slot_आकार - परिमाण(संरचना rpmi_सन्देशः_शीर्षकम्)))
		प्रत्यागम SBI_EINVAL;
	यदि ((परिमाण(u32) * तर्काः->rx_endian_शब्दाः) > xfer->rx_len)
		प्रत्यागम SBI_EINVAL;

	/* There should be some message in the queue */
	यदि (__smq_queue_empty(qctx))
		प्रत्यागम SBI_ENOENT;

	/* Get the head/read index and tail/write index */
	headidx = le32_गन्तव्यम्_cpu(*qctx->headptr);
	tailidx = le32_गन्तव्यम्_cpu(*qctx->tailptr);

	/*
	 * Compute msgidn expected in the incoming message
	 * NOTE: DOORBELL bit is not expected to be set.
	 */
	msgidn = MAKE_सन्देशः_ID(service_गणः_id, तर्काः->service_id, तर्काः->type);

	/* Find the Rx message with matching token */
	pos = headidx;
	यावत् (pos != tailidx) {
		src = (शून्य *)qctx->बफर + (pos * slot_आकार);
		यदि ((no_rx_token && GET_सन्देशः_ID(src) == msgidn) ||
		    (GET_TOKEN(src) == (xfer->seq & RPMI_सन्देशः_TOKEN_आवरणम्)))
			भङ्ग;
		pos = (pos + 1) % qctx->अङ्कः_slots;
	}
	यदि (pos == tailidx)
		प्रत्यागम SBI_ENOENT;

	/* If Rx message is not first message then make it first message */
	यदि (pos != headidx) {
		src = (शून्य *)qctx->बफर + (pos * slot_आकार);
		dst = (शून्य *)qctx->बफर + (headidx * slot_आकार);
		प्रति (i = 0; i < slot_आकार / परिमाण(u32); i++) {
			tmp = ((u32 *)dst)[i];
			((u32 *)dst)[i] = ((u32 *)src)[i];
			((u32 *)src)[i] = tmp;
		}
	}

	/* Update rx_token if not available */
	सन्देशः = (शून्य *)qctx->बफर + (headidx * slot_आकार);
	यदि (no_rx_token)
		तर्काः->rx_token = GET_TOKEN(सन्देशः);

	/* Extract data from the first message */
	यदि (xfer->rx) {
		तर्काः->rx_दत्त_len = dlen = GET_DLEN(सन्देशः);
		यदि (dlen > xfer->rx_len)
			dlen = xfer->rx_len;
		src = (शून्य *)सन्देशः + परिमाण(संरचना rpmi_सन्देशः_शीर्षकम्);
		dst = xfer->rx;
		प्रति (i = 0; i < तर्काः->rx_endian_शब्दाः; i++)
			((u32 *)dst)[i] = le32_गन्तव्यम्_cpu(((u32 *)src)[i]);
		dst += परिमाण(u32) * तर्काः->rx_endian_शब्दाः;
		src += परिमाण(u32) * तर्काः->rx_endian_शब्दाः;
		sbi_memcpy(dst, src,
			xfer->rx_len - (परिमाण(u32) * तर्काः->rx_endian_शब्दाः));
	}

	/* Update the head/read index */
	*qctx->headptr = cpu_गन्तव्यम्_le32(headidx + 1) % qctx->अङ्कः_slots;

	/* Make sure updates to head are immediately visible to PuC */
	smp_wmb();

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क __smq_tx(संरचना smq_queue_ctx *qctx, संरचना rpmi_mb_regs *mb_regs,
		    u32 a2p_doorbell_मानम्, u32 slot_आकार, u32 service_गणः_id,
		    संरचना mbox_xfer *xfer) MUST_HOLD(&qctx->queue_तालकम्)
{
	u32 i, tailidx;
	शून्य *dst, *src;
	संरचना rpmi_सन्देशः_शीर्षकम् शीर्षकम् = { 0 };
	संरचना rpmi_सन्देशः_तर्काः *तर्काः = xfer->तर्काः;

	/* Tx sanity checks */
	यदि ((परिमाण(u32) * तर्काः->tx_endian_शब्दाः) >
	    (slot_आकार - परिमाण(संरचना rpmi_सन्देशः_शीर्षकम्)))
		प्रत्यागम SBI_EINVAL;
	यदि ((परिमाण(u32) * तर्काः->tx_endian_शब्दाः) > xfer->tx_len)
		प्रत्यागम SBI_EINVAL;

	/* There should be some room in the queue */
	यदि (__smq_queue_full(qctx))
		प्रत्यागम SBI_ENOMEM;

	/* Get the tail/write index */
	tailidx = le32_गन्तव्यम्_cpu(*qctx->tailptr);

	/* Prepare the header to be written into the slot */
	शीर्षकम्.servicegroup_id = cpu_गन्तव्यम्_le16(service_गणः_id);
	शीर्षकम्.service_id = तर्काः->service_id;
	शीर्षकम्.flags = तर्काः->type;
	शीर्षकम्.datalen = cpu_गन्तव्यम्_le16((u16)xfer->tx_len);
	शीर्षकम्.token = cpu_गन्तव्यम्_le16((u16)xfer->seq);

	/* Write header into the slot */
	dst = (वर्ण *)qctx->बफर + (tailidx * slot_आकार);
	sbi_memcpy(dst, &शीर्षकम्, परिमाण(शीर्षकम्));
	dst += परिमाण(शीर्षकम्);

	/* Write data into the slot */
	यदि (xfer->tx) {
		src = xfer->tx;
		प्रति (i = 0; i < तर्काः->tx_endian_शब्दाः; i++)
			((u32 *)dst)[i] = cpu_गन्तव्यम्_le32(((u32 *)src)[i]);
		dst += परिमाण(u32) * तर्काः->tx_endian_शब्दाः;
		src += परिमाण(u32) * तर्काः->tx_endian_शब्दाः;
		sbi_memcpy(dst, src,
			xfer->tx_len - (परिमाण(u32) * तर्काः->tx_endian_शब्दाः));
	}

	/* Make sure queue chanages are visible to PuC before updating tail */
	smp_wmb();

	/* Update the tail/write index */
	*qctx->tailptr = cpu_गन्तव्यम्_le32(tailidx + 1) % qctx->अङ्कः_slots;

	/* Ring the RPMI doorbell if present */
	यदि (mb_regs)
		writel(a2p_doorbell_मानम्, &mb_regs->db_reg);

	प्रत्यागम SBI_OK;
}

स्थिर पूर्णाङ्क smq_rx(संरचना rpmi_shmem_mbox_controller *mctl,
		  u32 queue_id, u32 service_गणः_id, संरचना mbox_xfer *xfer)
{
	पूर्णाङ्क ret, rxretry = 0;
	संरचना smq_queue_ctx *qctx;

	यदि (mctl->queue_गणनम् < queue_id) {
		sbi_printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		प्रत्यागम SBI_EINVAL;
	}
	qctx = &mctl->queue_ctx_tbl[queue_id];

	/*
	 * Once the timeout happens and call this function is returned
	 * to the client then there is no way to deliver the response
	 * message after that if it comes later.
	 *
	 * REVISIT: In complete timeout duration how much duration
	 * it should wait(delay) before recv retry. udelay or mdelay
	 */
	कुरु {
		spin_तालकम्(&qctx->queue_तालकम्);
		ret = __smq_rx(qctx, mctl->slot_आकार, service_गणः_id, xfer);
		spin_unlock(&qctx->queue_तालकम्);
		यदि (!ret)
			प्रत्यागम 0;

		sbi_timer_mdelay(1);
		rxretry += 1;
	} यावत् (rxretry < xfer->rx_timeout);

	प्रत्यागम SBI_ETIMEDOUT;
}

स्थिर पूर्णाङ्क smq_tx(संरचना rpmi_shmem_mbox_controller *mctl,
		  u32 queue_id, u32 service_गणः_id, संरचना mbox_xfer *xfer)
{
	पूर्णाङ्क ret, txretry = 0;
	संरचना smq_queue_ctx *qctx;

	यदि (mctl->queue_गणनम् < queue_id) {
		sbi_printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		प्रत्यागम SBI_EINVAL;
	}
	qctx = &mctl->queue_ctx_tbl[queue_id];

	/*
	 * Ignoring the tx timeout since in RPMI has no mechanism
	 * with which other side can let know about the reception of
	 * message which marks as tx complete. For RPMI tx complete is
	 * marked as done when message in successfully copied in queue.
	 *
	 * REVISIT: In complete timeout duration how much duration
	 * it should wait(delay) before send retry. udelay or mdelay
	 */
	कुरु {
		spin_तालकम्(&qctx->queue_तालकम्);
		ret = __smq_tx(qctx, mctl->mb_regs, mctl->a2p_doorbell_मानम्,
				mctl->slot_आकार, service_गणः_id, xfer);
		spin_unlock(&qctx->queue_तालकम्);
		यदि (!ret)
			प्रत्यागम 0;

		sbi_timer_mdelay(1);
		txretry += 1;
	} यावत् (txretry < xfer->tx_timeout);

	प्रत्यागम SBI_ETIMEDOUT;
}

स्थिर पूर्णाङ्क rpmi_get_platform_info(संरचना rpmi_shmem_mbox_controller *mctl)
{
	पूर्णाङ्क ret = SBI_OK;

	/**
	 * platform string can occupy max possible size
	 * max possible space in the message data as
	 * per the format
	 */
	संरचना rpmi_आधारः_get_platform_info_resp *resp =
			sbi_zalloc(RPMI_सन्देशः_दत्त_आकार(mctl->slot_आकार));
	यदि (!resp)
		प्रत्यागम SBI_ENOMEM;

	ret = rpmi_प्रसामान्यम्_request_with_स्थितिः(mctl->आधारः_chan,
				 RPMI_आधारः_SRV_GET_PLATFORM_INFO,
				 रिक्त, 0, 0,
				 resp,
				 RPMI_सन्देशः_दत्त_आकार(mctl->slot_आकार)/4,
				 RPMI_सन्देशः_दत्त_आकार(mctl->slot_आकार)/4);
	यदि (ret)
		गच्छ fail_free_resp;

	mctl->plat_info_len = resp->plat_info_len;
	mctl->plat_info = sbi_zalloc(mctl->plat_info_len);
	यदि (!mctl->plat_info) {
		ret = SBI_ENOMEM;
		गच्छ fail_free_resp;
	}

	sbi_strncpy(mctl->plat_info, resp->plat_info, mctl->plat_info_len);

fail_free_resp:
	sbi_free(resp);
	प्रत्यागम ret;
}

स्थिर पूर्णाङ्क smq_आधारः_get_two_u32(संरचना rpmi_shmem_mbox_controller *mctl,
				u32 service_id, u32 *inarg, u32 *outvals)
{
	प्रत्यागम rpmi_प्रसामान्यम्_request_with_स्थितिः(
			mctl->आधारः_chan, service_id,
			inarg, (inarg) ? 1 : 0, (inarg) ? 1 : 0,
			outvals, 2, 2);
}

/**************** Mailbox Controller Functions **************/

स्थिर पूर्णाङ्क rpmi_shmem_mbox_xfer(संरचना mbox_chan *chan, संरचना mbox_xfer *xfer)
{
	पूर्णाङ्क ret;
	u32 tx_qid = 0, rx_qid = 0;
	संरचना rpmi_shmem_mbox_controller *mctl =
			container_सम्बन्धि(chan->mbox,
				     संरचना rpmi_shmem_mbox_controller,
				     controller);
	संरचना rpmi_srvgrp_chan *srvgrp_chan = गन्तव्यम्_srvgrp_chan(chan);

	संरचना rpmi_सन्देशः_तर्काः *तर्काः = xfer->तर्काः;
	bool do_tx = (तर्काः->flags & RPMI_सन्देशः_FLAGS_NO_TX) ? false : true;
	bool do_rx = (तर्काः->flags & RPMI_सन्देशः_FLAGS_NO_RX) ? false : true;

	यदि (!do_tx && !do_rx)
		प्रत्यागम SBI_EINVAL;

	विकल्प (तर्काः->type) {
	पक्ष RPMI_सन्देशः_प्रसामान्यम्_REQUEST:
		यदि (do_tx && do_rx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
			rx_qid = RPMI_QUEUE_IDX_P2A_ACK;
		} अन्यथा यदि (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
		} अन्यथा यदि (do_rx) {
			rx_qid = RPMI_QUEUE_IDX_P2A_REQ;
		}
		भङ्ग;
	पक्ष RPMI_सन्देशः_POSTED_REQUEST:
		यदि (do_tx && do_rx)
			प्रत्यागम SBI_EINVAL;
		यदि (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
		} अन्यथा {
			rx_qid = RPMI_QUEUE_IDX_P2A_REQ;
		}
		भङ्ग;
	पक्ष RPMI_सन्देशः_ACKNOWLDGEMENT:
		यदि (do_tx && do_rx)
			प्रत्यागम SBI_EINVAL;
		यदि (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_ACK;
		} अन्यथा {
			rx_qid = RPMI_QUEUE_IDX_P2A_ACK;
		}
		भङ्ग;
	स्वभाव:
		प्रत्यागम SBI_ENOTSUPP;
	}

	यदि (do_tx) {
		ret = smq_tx(mctl, tx_qid, srvgrp_chan->servicegroup_id, xfer);
		यदि (ret)
			प्रत्यागम ret;
	}

	यदि (do_rx) {
		ret = smq_rx(mctl, rx_qid, srvgrp_chan->servicegroup_id, xfer);
		यदि (ret)
			प्रत्यागम ret;
	}

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क rpmi_shmem_mbox_get_attribute(संरचना mbox_chan *chan,
					 पूर्णाङ्क attr_id, शून्य *बहिर्गमनम्_मानम्)
{
	संरचना rpmi_shmem_mbox_controller *mctl =
			container_सम्बन्धि(chan->mbox,
				     संरचना rpmi_shmem_mbox_controller,
				     controller);
	संरचना rpmi_srvgrp_chan *srvgrp_chan = गन्तव्यम्_srvgrp_chan(chan);

	विकल्प (attr_id) {
	पक्ष RPMI_CHANNEL_ATTR_PROTOCOL_संस्करणम्:
		*((u32 *)बहिर्गमनम्_मानम्) = mctl->spec_संस्करणम्;
		भङ्ग;
	पक्ष RPMI_CHANNEL_ATTR_MAX_दत्त_LEN:
		*((u32 *)बहिर्गमनम्_मानम्) = RPMI_सन्देशः_दत्त_आकार(mctl->slot_आकार);
		भङ्ग;
	पक्ष RPMI_CHANNEL_ATTR_P2_A_DOORBELL_SYSMSI_सूचकांक:
		*((u32 *)बहिर्गमनम्_मानम्) = mctl->p2a_doorbell_sysmsi_सूचकांक;
		भङ्ग;
	पक्ष RPMI_CHANNEL_ATTR_TX_TIMEOUT:
		*((u32 *)बहिर्गमनम्_मानम्) = RPMI_DEF_TX_TIMEOUT;
		भङ्ग;
	पक्ष RPMI_CHANNEL_ATTR_RX_TIMEOUT:
		*((u32 *)बहिर्गमनम्_मानम्) = RPMI_DEF_RX_TIMEOUT;
		भङ्ग;
	पक्ष RPMI_CHANNEL_ATTR_SERVICEGROUP_ID:
		*((u32 *)बहिर्गमनम्_मानम्) = srvgrp_chan->servicegroup_id;
		भङ्ग;
	पक्ष RPMI_CHANNEL_ATTR_SERVICEGROUP_संस्करणम्:
		*((u32 *)बहिर्गमनम्_मानम्) = srvgrp_chan->servicegroup_संस्करणम्;
		भङ्ग;
	पक्ष RPMI_CHANNEL_ATTR_IMPL_ID:
		*((u32 *)बहिर्गमनम्_मानम्) = mctl->impl_id;
		भङ्ग;
	पक्ष RPMI_CHANNEL_ATTR_IMPL_संस्करणम्:
		*((u32 *)बहिर्गमनम्_मानम्) = mctl->impl_संस्करणम्;
		भङ्ग;
	स्वभाव:
		प्रत्यागम SBI_ENOTSUPP;
	}

	प्रत्यागम 0;
}

स्थिर संरचना mbox_chan *rpmi_shmem_mbox_request_chan(
						संरचना mbox_controller *mbox,
						u32 *chan_तर्काः)
{
	पूर्णाङ्क ret;
	u32 tval[2] = { 0 };
	संरचना rpmi_srvgrp_chan *srvgrp_chan;
	संरचना rpmi_shmem_mbox_controller *mctl =
			container_सम्बन्धि(mbox,
				     संरचना rpmi_shmem_mbox_controller,
				     controller);

	/* Service group id not defined or in reserved range is invalid */
	यदि (chan_तर्काः[0] >= RPMI_SRVGRP_ID_MAX_गणनम् &&
		chan_तर्काः[0] <= RPMI_SRVGRP_RESERVE_अन्तः)
		प्रत्यागम रिक्त;

	/* Base serivce group is always present so probe other groups */
	यदि (chan_तर्काः[0] != RPMI_SRVGRP_आधारः) {
		/* Probe service group */
		ret = smq_आधारः_get_two_u32(mctl,
					   RPMI_आधारः_SRV_PROBE_SERVICE_गणः,
					   chan_तर्काः, tval);
		यदि (ret || !tval[1])
			प्रत्यागम रिक्त;
	}

	srvgrp_chan = sbi_zalloc(परिमाण(*srvgrp_chan));
	यदि (!srvgrp_chan)
		प्रत्यागम रिक्त;

	srvgrp_chan->servicegroup_id = chan_तर्काः[0];
	srvgrp_chan->servicegroup_संस्करणम् = tval[1];

	प्रत्यागम &srvgrp_chan->chan;
}

स्थिर शून्य rpmi_shmem_mbox_free_chan(संरचना mbox_controller *mbox,
				      संरचना mbox_chan *chan)
{
	संरचना rpmi_srvgrp_chan *srvgrp_chan = गन्तव्यम्_srvgrp_chan(chan);
	sbi_free(srvgrp_chan);
}

बाह्य संरचना fdt_mailbox fdt_mailbox_rpmi_shmem;

स्थिर पूर्णाङ्क rpmi_shmem_transport_init(संरचना rpmi_shmem_mbox_controller *mctl,
				     ध्रुव शून्य *fdt,
				     पूर्णाङ्क nodeoff) NO_THREAD_SAFETY_ANALYSIS
{
	ध्रुव वर्ण *name;
	ध्रुव fdt32_t *prop;
	पूर्णाङ्क गणनम्, len, ret, qid;
	uint64_t reg_addr, reg_आकार;
	संरचना smq_queue_ctx *qctx;

	ret = fdt_ग्रन्थिः_check_compatible(fdt, nodeoff,
					"riscv,rpmi-shmem-mbox");
	यदि (ret)
		प्रत्यागम ret;

	/* get queue slot size in bytes */
	prop = fdt_getprop(fdt, nodeoff, "riscv,slot-size", &len);
	यदि (!prop)
		प्रत्यागम SBI_ENOENT;

	mctl->slot_आकार = fdt32_गन्तव्यम्_cpu(*prop);
	यदि (mctl->slot_आकार < RPMI_SLOT_आकार_MIN) {
		sbi_printf("%s: slot_size < mimnum required message size\n",
			   __func__);
		mctl->slot_आकार = RPMI_SLOT_आकार_MIN;
	}

	/* get p2a doorbell system MSI index */
	prop = fdt_getprop(fdt, nodeoff, "riscv,p2a-doorbell-sysmsi-index", &len);
	mctl->p2a_doorbell_sysmsi_सूचकांक = prop ? fdt32_गन्तव्यम्_cpu(*prop) : -1U;

	/* get a2p doorbell value */
	prop = fdt_getprop(fdt, nodeoff, "riscv,a2p-doorbell-value", &len);
	mctl->a2p_doorbell_मानम् = prop ? fdt32_गन्तव्यम्_cpu(*prop) : 1;

	/*
	 * queue names count is taken as the number of queues
	 * supported which make it mandatory to provide the
	 * name of the queue.
	 */
	गणनम् = fdt_stringlist_गणनम्(fdt, nodeoff, "reg-names");
	यदि (गणनम् < 0 ||
	    गणनम् > (RPMI_QUEUE_IDX_MAX_गणनम् + RPMI_REG_IDX_MAX_गणनम्))
		प्रत्यागम SBI_EINVAL;

	mctl->queue_गणनम् = गणनम् - RPMI_REG_IDX_MAX_गणनम्;

	/* parse all queues and populate queues context structure */
	प्रति (qid = 0; qid < mctl->queue_गणनम्; qid++) {
		qctx = &mctl->queue_ctx_tbl[qid];

		/* get each queue share-memory base address and size*/
		ret = fdt_get_ग्रन्थिः_addr_आकार(fdt, nodeoff, qid,
					     &reg_addr, &reg_आकार);
		यदि (ret < 0 || !reg_addr || !reg_आकार)
			प्रत्यागम SBI_ENOENT;

		ret = sbi_domain_मूलम्_सङ्कलय_memrange(reg_addr, reg_आकार, reg_आकार,
						   (SBI_DOMAIN_MEMREGION_MMIO |
						    SBI_DOMAIN_MEMREGION_M_READABLE |
						    SBI_DOMAIN_MEMREGION_M_WRITABLE));
		यदि (ret)
			प्रत्यागम ret;

		/* calculate number of slots in each queue */
		qctx->अङ्कः_slots =
			(reg_आकार - (mctl->slot_आकार * RPMI_QUEUE_शीर्षकम्_SLOTS)) / mctl->slot_आकार;

		/* setup queue pointers */
		qctx->headptr = ((शून्य *)(अचिह्नित दीर्घ)reg_addr) +
				RPMI_QUEUE_शीर्ष_SLOT * mctl->slot_आकार;
		qctx->tailptr = ((शून्य *)(अचिह्नित दीर्घ)reg_addr) +
				RPMI_QUEUE_पुच्छ_SLOT * mctl->slot_आकार;
		qctx->बफर = ((शून्य *)(अचिह्नित दीर्घ)reg_addr) +
				RPMI_QUEUE_शीर्षकम्_SLOTS * mctl->slot_आकार;

		/* get the queue name */
		name = fdt_stringlist_get(fdt, nodeoff, "reg-names",
					  qid, &len);
		यदि (!name || (name && len < 0))
			प्रत्यागम len;

		sbi_memcpy(qctx->name, name, len);

		/* store the index as queue_id */
		qctx->queue_id = qid;

		SPIN_तालकम्_INIT(qctx->queue_तालकम्);
	}

	/* get the a2p-doorbell property name */
	name = fdt_stringlist_get(fdt, nodeoff, "reg-names", qid, &len);
	यदि (!name || (name && len < 0))
		प्रत्यागम len;

	/* fetch doorbell register address*/
	ret = fdt_get_ग्रन्थिः_addr_आकार(fdt, nodeoff, qid, &reg_addr,
				       &reg_आकार);
	यदि (!ret && !(वाक्ये_तुलय(name, "a2p-doorbell", वाक्यदैर्घ्यम्("a2p-doorbell")))) {
		mctl->mb_regs = (शून्य *)(अचिह्नित दीर्घ)reg_addr;
		ret = sbi_domain_मूलम्_सङ्कलय_memrange(reg_addr, reg_आकार, reg_आकार,
						   (SBI_DOMAIN_MEMREGION_MMIO |
						    SBI_DOMAIN_MEMREGION_M_READABLE |
						    SBI_DOMAIN_MEMREGION_M_WRITABLE));
		यदि (ret)
			प्रत्यागम ret;
	}

	प्रत्यागम SBI_SUCCESS;
}

स्थिर पूर्णाङ्क rpmi_shmem_mbox_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
				ध्रुव संरचना fdt_मेलनम् *मेलनम्)
{
	संरचना rpmi_आधारः_get_गुणाः_resp resp;
	संरचना rpmi_shmem_mbox_controller *mctl;
	संरचना rpmi_srvgrp_chan *आधारः_srvgrp;
	u32 tval[2], तर्काः[1];
	पूर्णाङ्क ret = 0;

	mctl = sbi_zalloc(परिमाण(*mctl));
	यदि (!mctl)
		प्रत्यागम SBI_ENOMEM;

	/* Initialization transport from device tree */
	ret = rpmi_shmem_transport_init(mctl, fdt, nodeoff);
	यदि (ret)
		गच्छ fail_free_controller;

	/* Register mailbox controller */
	mctl->controller.id = nodeoff;
	mctl->controller.max_xfer_len =
			mctl->slot_आकार - परिमाण(संरचना rpmi_सन्देशः_शीर्षकम्);
	mctl->controller.driver = &fdt_mailbox_rpmi_shmem;
	mctl->controller.request_chan = rpmi_shmem_mbox_request_chan;
	mctl->controller.free_chan = rpmi_shmem_mbox_free_chan;
	mctl->controller.xfer = rpmi_shmem_mbox_xfer;
	mctl->controller.get_attribute = rpmi_shmem_mbox_get_attribute;
	ret = mbox_controller_सङ्कलय(&mctl->controller);
	यदि (ret)
		गच्छ fail_free_controller;

	/* Request base service group channel */
	tval[0] = RPMI_SRVGRP_आधारः;
	mctl->आधारः_chan = mbox_controller_request_chan(&mctl->controller,
							tval);
	यदि (!mctl->आधारः_chan) {
		ret = SBI_ENOENT;
		गच्छ fail_remove_controller;
	}

	/* Update base service group version */
	आधारः_srvgrp = गन्तव्यम्_srvgrp_chan(mctl->आधारः_chan);
	तर्काः[0] = RPMI_SRVGRP_आधारः;
	ret = smq_आधारः_get_two_u32(mctl, RPMI_आधारः_SRV_PROBE_SERVICE_गणः,
				   &तर्काः[0], tval);
	यदि (ret)
		गच्छ fail_free_chan;
	आधारः_srvgrp->servicegroup_संस्करणम् = tval[1];
	यदि (आधारः_srvgrp->servicegroup_संस्करणम् < RPMI_आधारः_संस्करणम्_MIN) {
		ret = SBI_EINVAL;
		गच्छ fail_free_chan;
	}

	/* Get implementation id */
	ret = smq_आधारः_get_two_u32(mctl,
				   RPMI_आधारः_SRV_GET_IMPLEMENTATION_संस्करणम्,
				   रिक्त, tval);
	यदि (ret)
		गच्छ fail_free_chan;
	mctl->impl_संस्करणम् = tval[1];

	/* Get implementation version */
	ret = smq_आधारः_get_two_u32(mctl, RPMI_आधारः_SRV_GET_IMPLEMENTATION_IDN,
				   रिक्त, tval);
	यदि (ret)
		गच्छ fail_free_chan;
	mctl->impl_id = tval[1];

	/* Get specification version */
	ret = smq_आधारः_get_two_u32(mctl, RPMI_आधारः_SRV_GET_SPEC_संस्करणम्,
				   रिक्त, tval);
	यदि (ret)
		गच्छ fail_free_chan;
	mctl->spec_संस्करणम् = tval[1];
	यदि (mctl->spec_संस्करणम् < RPMI_आधारः_संस्करणम्_MIN ||
	    mctl->spec_संस्करणम् != आधारः_srvgrp->servicegroup_संस्करणम्) {
		ret = SBI_EINVAL;
		गच्छ fail_free_chan;
	}

	/* Get optional features implementation flags */
	ret = rpmi_प्रसामान्यम्_request_with_स्थितिः(
			mctl->आधारः_chan, RPMI_आधारः_SRV_GET_गुणाः,
			रिक्त, 0, 0,
			&resp, rpmi_u32_गणनम्(resp), rpmi_u32_गणनम्(resp));
	यदि (ret)
		गच्छ fail_free_chan;

	/* 1: M-mode, 0: S-mode */
	mctl->आधारः_flags.f0_priv_level =
			resp.f0 & RPMI_आधारः_FLAGS_F0_PRIVILEGE ? 1 : 0;
	/* 1: Supported, 0: Not Supported */
	mctl->आधारः_flags.f0_ev_notif_en =
			resp.f0 & RPMI_आधारः_FLAGS_F0_EV_NOTIFY ? 1 : 0;

	/* We only use M-mode RPMI context in OpenSBI */
	यदि (!mctl->आधारः_flags.f0_priv_level) {
		ret = SBI_ENODEV;
		गच्छ fail_free_chan;
	}

	/*
	 * Continue without platform information string if not
	 * available or if an error is encountered while fetching
	 */
	rpmi_get_platform_info(mctl);

	प्रत्यागम 0;

fail_free_chan:
	mbox_controller_free_chan(mctl->आधारः_chan);
fail_remove_controller:
	mbox_controller_remove(&mctl->controller);
fail_free_controller:
	sbi_free(mctl);
	प्रत्यागम ret;
}

स्थिर ध्रुव संरचना fdt_मेलनम् rpmi_shmem_mbox_मेलनम्[] = {
	{ .compatible = "riscv,rpmi-shmem-mbox" },
	{ },
};

संरचना fdt_mailbox fdt_mailbox_rpmi_shmem = {
	.driver = {
		.मेलनम्_सारणी = rpmi_shmem_mbox_मेलनम्,
		.init = rpmi_shmem_mbox_init,
	},
	.xlate = fdt_mailbox_simple_xlate,
};
