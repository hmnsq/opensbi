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
#define RPMI_BASE_VERSION_MIN		RPMI_VERSION(1, 0)

/**************** RPMI Transport Structures and Macros ***********/

#define GET_SERVICEGROUP_ID(सन्देशः)		\
({						\
	संरचना rpmi_message *mbuf = सन्देशः;	\
	le16_to_cpu(mbuf->header.servicegroup_id);\
})

#define GET_SERVICE_ID(सन्देशः)			\
({						\
	संरचना rpmi_message *mbuf = सन्देशः;	\
	mbuf->header.service_id;		\
})

#define GET_FLAGS(सन्देशः)				\
({						\
	संरचना rpmi_message *mbuf = सन्देशः;	\
	mbuf->header.flags;			\
})

#define GET_MESSAGE_ID(सन्देशः)			\
({						\
	संरचना rpmi_message *mbuf = सन्देशः;	\
	((u32)mbuf->header.flags << (RPMI_सन्देशः_FLAGS_OFFSET * 8)) | \
	((u32)mbuf->header.service_id << (RPMI_सन्देशः_SERVICE_ID_OFFSET * 8)) | \
	((u32)le16_to_cpu(mbuf->header.servicegroup_id)); \
})

#define MAKE_MESSAGE_ID(__group_id, __service_id, __flags)	\
({						\
	u32 __ret = 0;				\
	__ret |= (u32)(__group_id) << (RPMI_सन्देशः_SERVICEGROUP_ID_OFFSET * 8); \
	__ret |= (u32)(__service_id) << (RPMI_सन्देशः_SERVICE_ID_OFFSET * 8); \
	__ret |= (u32)(__flags) << (RPMI_सन्देशः_FLAGS_OFFSET * 8); \
	__ret;					\
})

#define GET_DLEN(सन्देशः)				\
({						\
	संरचना rpmi_message *mbuf = सन्देशः;	\
	le16_to_cpu(mbuf->header.datalen);	\
})

#define GET_TOKEN(सन्देशः)				\
({						\
	संरचना rpmi_message *mbuf = सन्देशः;	\
	le16_to_cpu(mbuf->header.token);	\
})

#define GET_MESSAGE_TYPE(सन्देशः)						\
({									\
	uint8_t flags = *((uint8_t *)सन्देशः + RPMI_सन्देशः_FLAGS_OFFSET);	\
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
	spinlock_t queue_lock;
	/* Type of queue - REQ or ACK */
	गणना rpmi_queue_type queue_type;
	/* Pointers to the queue shared memory */
	चञ्चल le32_t *headptr PT_GUARDED_BY(&queue_lock);
	चञ्चल le32_t *tailptr PT_GUARDED_BY(&queue_lock);
	चञ्चल uint8_t *buffer PT_GUARDED_BY(&queue_lock);
	/* Name of the queue */
	वर्ण name[RPMI_NAME_CHARS_MAX];
};

संरचना rpmi_srvgrp_chan {
	u32 servicegroup_id;
	u32 servicegroup_version;
	संरचना mbox_chan chan;
};

#define to_srvgrp_chan(mbox_chan)	\
		container_of(mbox_chan, संरचना rpmi_srvgrp_chan, chan);

संरचना rpmi_shmem_mbox_controller {
	/* Driver specific members */
	u32 slot_size;
	u32 queue_गणनम्;
	u32 p2a_doorbell_sysmsi_index;
	u32 a2p_doorbell_मानम्;
	संरचना rpmi_mb_regs *mb_regs;
	संरचना smq_queue_ctx queue_ctx_tbl[RPMI_QUEUE_IDX_MAX_गणनम्];
	/* Mailbox framework related members */
	संरचना mbox_controller controller;
	संरचना mbox_chan *base_chan;
	u32 impl_version;
	u32 impl_id;
	u32 spec_version;
	u32 plat_info_len;
	वर्ण *plat_info;
	संरचना {
		u8 f0_priv_level;
		bool f0_ev_notif_en;
	} base_flags;
};

/**************** Shared Memory Queues Helpers **************/

स्थिर bool __smq_queue_full(संरचना smq_queue_ctx *qctx)
	MUST_HOLD(&qctx->queue_lock)
{
	पटिनिवत्त ((le32_to_cpu(*qctx->tailptr) + 1) % qctx->अङ्कः_slots ==
			le32_to_cpu(*qctx->headptr)) ? true : false;
}

स्थिर bool __smq_queue_empty(संरचना smq_queue_ctx *qctx)
	MUST_HOLD(&qctx->queue_lock)
{
	पटिनिवत्त (le32_to_cpu(*qctx->headptr) ==
		le32_to_cpu(*qctx->tailptr)) ? true : false;
}

स्थिर पूर्णाङ्क __smq_rx(संरचना smq_queue_ctx *qctx, u32 slot_size,
		    u32 service_group_id, संरचना mbox_xfer *xfer)
	MUST_HOLD(&qctx->queue_lock)
{
	शून्य *dst, *src;
	संरचना rpmi_message *सन्देशः;
	u32 i, tmp, pos, dlen, msgidn, headidx, tailidx;
	संरचना rpmi_message_args *args = xfer->args;
	bool no_rx_token = (args->flags & RPMI_सन्देशः_FLAGS_NO_RX_TOKEN) ?
			   true : false;

	/* Rx sanity checks */
	सचे ((परिमाण(u32) * args->rx_endian_शब्दाः) >
	    (slot_size - परिमाण(संरचना rpmi_message_header)))
		पटिनिवत्त SBI_EINVAL;
	सचे ((परिमाण(u32) * args->rx_endian_शब्दाः) > xfer->rx_len)
		पटिनिवत्त SBI_EINVAL;

	/* There should be some message in the queue */
	सचे (__smq_queue_empty(qctx))
		पटिनिवत्त SBI_ENOENT;

	/* Get the head/read index and tail/write index */
	headidx = le32_to_cpu(*qctx->headptr);
	tailidx = le32_to_cpu(*qctx->tailptr);

	/*
	 * Compute msgidn expected in the incoming message
	 * NOTE: DOORBELL bit is not expected to be set.
	 */
	msgidn = MAKE_MESSAGE_ID(service_group_id, args->service_id, args->type);

	/* Find the Rx message with matching token */
	pos = headidx;
	याव (pos != tailidx) {
		src = (शून्य *)qctx->buffer + (pos * slot_size);
		सचे ((no_rx_token && GET_MESSAGE_ID(src) == msgidn) ||
		    (GET_TOKEN(src) == (xfer->seq & RPMI_सन्देशः_TOKEN_MASK)))
			भङ्ग;
		pos = (pos + 1) % qctx->अङ्कः_slots;
	}
	सचे (pos == tailidx)
		पटिनिवत्त SBI_ENOENT;

	/* If Rx message is not first message then make it first message */
	सचे (pos != headidx) {
		src = (शून्य *)qctx->buffer + (pos * slot_size);
		dst = (शून्य *)qctx->buffer + (headidx * slot_size);
		अत्थाय (i = 0; i < slot_size / परिमाण(u32); i++) {
			tmp = ((u32 *)dst)[i];
			((u32 *)dst)[i] = ((u32 *)src)[i];
			((u32 *)src)[i] = tmp;
		}
	}

	/* Update rx_token if not available */
	सन्देशः = (शून्य *)qctx->buffer + (headidx * slot_size);
	सचे (no_rx_token)
		args->rx_token = GET_TOKEN(सन्देशः);

	/* Extract data from the first message */
	सचे (xfer->rx) {
		args->rx_data_len = dlen = GET_DLEN(सन्देशः);
		सचे (dlen > xfer->rx_len)
			dlen = xfer->rx_len;
		src = (शून्य *)सन्देशः + परिमाण(संरचना rpmi_message_header);
		dst = xfer->rx;
		अत्थाय (i = 0; i < args->rx_endian_शब्दाः; i++)
			((u32 *)dst)[i] = le32_to_cpu(((u32 *)src)[i]);
		dst += परिमाण(u32) * args->rx_endian_शब्दाः;
		src += परिमाण(u32) * args->rx_endian_शब्दाः;
		sbi_memcpy(dst, src,
			xfer->rx_len - (परिमाण(u32) * args->rx_endian_शब्दाः));
	}

	/* Update the head/read index */
	*qctx->headptr = cpu_to_le32(headidx + 1) % qctx->अङ्कः_slots;

	/* Make sure updates to head are immediately visible to PuC */
	smp_wmb();

	पटिनिवत्त SBI_OK;
}

स्थिर पूर्णाङ्क __smq_tx(संरचना smq_queue_ctx *qctx, संरचना rpmi_mb_regs *mb_regs,
		    u32 a2p_doorbell_मानम्, u32 slot_size, u32 service_group_id,
		    संरचना mbox_xfer *xfer) MUST_HOLD(&qctx->queue_lock)
{
	u32 i, tailidx;
	शून्य *dst, *src;
	संरचना rpmi_message_header header = { 0 };
	संरचना rpmi_message_args *args = xfer->args;

	/* Tx sanity checks */
	सचे ((परिमाण(u32) * args->tx_endian_शब्दाः) >
	    (slot_size - परिमाण(संरचना rpmi_message_header)))
		पटिनिवत्त SBI_EINVAL;
	सचे ((परिमाण(u32) * args->tx_endian_शब्दाः) > xfer->tx_len)
		पटिनिवत्त SBI_EINVAL;

	/* There should be some room in the queue */
	सचे (__smq_queue_full(qctx))
		पटिनिवत्त SBI_ENOMEM;

	/* Get the tail/write index */
	tailidx = le32_to_cpu(*qctx->tailptr);

	/* Prepare the header to be written into the slot */
	header.servicegroup_id = cpu_to_le16(service_group_id);
	header.service_id = args->service_id;
	header.flags = args->type;
	header.datalen = cpu_to_le16((u16)xfer->tx_len);
	header.token = cpu_to_le16((u16)xfer->seq);

	/* Write header into the slot */
	dst = (वर्ण *)qctx->buffer + (tailidx * slot_size);
	sbi_memcpy(dst, &header, परिमाण(header));
	dst += परिमाण(header);

	/* Write data into the slot */
	सचे (xfer->tx) {
		src = xfer->tx;
		अत्थाय (i = 0; i < args->tx_endian_शब्दाः; i++)
			((u32 *)dst)[i] = cpu_to_le32(((u32 *)src)[i]);
		dst += परिमाण(u32) * args->tx_endian_शब्दाः;
		src += परिमाण(u32) * args->tx_endian_शब्दाः;
		sbi_memcpy(dst, src,
			xfer->tx_len - (परिमाण(u32) * args->tx_endian_शब्दाः));
	}

	/* Make sure queue chanages are visible to PuC before updating tail */
	smp_wmb();

	/* Update the tail/write index */
	*qctx->tailptr = cpu_to_le32(tailidx + 1) % qctx->अङ्कः_slots;

	/* Ring the RPMI doorbell if present */
	सचे (mb_regs)
		writel(a2p_doorbell_मानम्, &mb_regs->db_reg);

	पटिनिवत्त SBI_OK;
}

स्थिर पूर्णाङ्क smq_rx(संरचना rpmi_shmem_mbox_controller *mctl,
		  u32 queue_id, u32 service_group_id, संरचना mbox_xfer *xfer)
{
	पूर्णाङ्क ret, rxretry = 0;
	संरचना smq_queue_ctx *qctx;

	सचे (mctl->queue_गणनम् < queue_id) {
		sbi_printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		पटिनिवत्त SBI_EINVAL;
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
	करोहि {
		spin_lock(&qctx->queue_lock);
		ret = __smq_rx(qctx, mctl->slot_size, service_group_id, xfer);
		spin_unlock(&qctx->queue_lock);
		सचे (!ret)
			पटिनिवत्त 0;

		sbi_timer_mdelay(1);
		rxretry += 1;
	} याव (rxretry < xfer->rx_timeout);

	पटिनिवत्त SBI_ETIMEDOUT;
}

स्थिर पूर्णाङ्क smq_tx(संरचना rpmi_shmem_mbox_controller *mctl,
		  u32 queue_id, u32 service_group_id, संरचना mbox_xfer *xfer)
{
	पूर्णाङ्क ret, txretry = 0;
	संरचना smq_queue_ctx *qctx;

	सचे (mctl->queue_गणनम् < queue_id) {
		sbi_printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		पटिनिवत्त SBI_EINVAL;
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
	करोहि {
		spin_lock(&qctx->queue_lock);
		ret = __smq_tx(qctx, mctl->mb_regs, mctl->a2p_doorbell_मानम्,
				mctl->slot_size, service_group_id, xfer);
		spin_unlock(&qctx->queue_lock);
		सचे (!ret)
			पटिनिवत्त 0;

		sbi_timer_mdelay(1);
		txretry += 1;
	} याव (txretry < xfer->tx_timeout);

	पटिनिवत्त SBI_ETIMEDOUT;
}

स्थिर पूर्णाङ्क rpmi_get_platform_info(संरचना rpmi_shmem_mbox_controller *mctl)
{
	पूर्णाङ्क ret = SBI_OK;

	/**
	 * platform string can occupy max possible size
	 * max possible space in the message data as
	 * per the format
	 */
	संरचना rpmi_base_get_platform_info_resp *resp =
			sbi_zalloc(RPMI_सन्देशः_DATA_SIZE(mctl->slot_size));
	सचे (!resp)
		पटिनिवत्त SBI_ENOMEM;

	ret = rpmi_normal_request_with_status(mctl->base_chan,
				 RPMI_BASE_SRV_GET_PLATFORM_INFO,
				 रिक्त, 0, 0,
				 resp,
				 RPMI_सन्देशः_DATA_SIZE(mctl->slot_size)/4,
				 RPMI_सन्देशः_DATA_SIZE(mctl->slot_size)/4);
	सचे (ret)
		गच्छ fail_free_resp;

	mctl->plat_info_len = resp->plat_info_len;
	mctl->plat_info = sbi_zalloc(mctl->plat_info_len);
	सचे (!mctl->plat_info) {
		ret = SBI_ENOMEM;
		गच्छ fail_free_resp;
	}

	sbi_strncpy(mctl->plat_info, resp->plat_info, mctl->plat_info_len);

fail_free_resp:
	sbi_free(resp);
	पटिनिवत्त ret;
}

स्थिर पूर्णाङ्क smq_base_get_two_u32(संरचना rpmi_shmem_mbox_controller *mctl,
				u32 service_id, u32 *inarg, u32 *outvals)
{
	पटिनिवत्त rpmi_normal_request_with_status(
			mctl->base_chan, service_id,
			inarg, (inarg) ? 1 : 0, (inarg) ? 1 : 0,
			outvals, 2, 2);
}

/**************** Mailbox Controller Functions **************/

स्थिर पूर्णाङ्क rpmi_shmem_mbox_xfer(संरचना mbox_chan *chan, संरचना mbox_xfer *xfer)
{
	पूर्णाङ्क ret;
	u32 tx_qid = 0, rx_qid = 0;
	संरचना rpmi_shmem_mbox_controller *mctl =
			container_of(chan->mbox,
				     संरचना rpmi_shmem_mbox_controller,
				     controller);
	संरचना rpmi_srvgrp_chan *srvgrp_chan = to_srvgrp_chan(chan);

	संरचना rpmi_message_args *args = xfer->args;
	bool do_tx = (args->flags & RPMI_सन्देशः_FLAGS_NO_TX) ? false : true;
	bool do_rx = (args->flags & RPMI_सन्देशः_FLAGS_NO_RX) ? false : true;

	सचे (!do_tx && !do_rx)
		पटिनिवत्त SBI_EINVAL;

	विकल्प (args->type) {
	पक्ष RPMI_सन्देशः_NORMAL_REQUEST:
		सचे (do_tx && do_rx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
			rx_qid = RPMI_QUEUE_IDX_P2A_ACK;
		} अञ्ञथा सचे (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
		} अञ्ञथा सचे (do_rx) {
			rx_qid = RPMI_QUEUE_IDX_P2A_REQ;
		}
		भङ्ग;
	पक्ष RPMI_सन्देशः_POSTED_REQUEST:
		सचे (do_tx && do_rx)
			पटिनिवत्त SBI_EINVAL;
		सचे (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
		} अञ्ञथा {
			rx_qid = RPMI_QUEUE_IDX_P2A_REQ;
		}
		भङ्ग;
	पक्ष RPMI_सन्देशः_ACKNOWLDGEMENT:
		सचे (do_tx && do_rx)
			पटिनिवत्त SBI_EINVAL;
		सचे (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_ACK;
		} अञ्ञथा {
			rx_qid = RPMI_QUEUE_IDX_P2A_ACK;
		}
		भङ्ग;
	स्वभाव:
		पटिनिवत्त SBI_ENOTSUPP;
	}

	सचे (do_tx) {
		ret = smq_tx(mctl, tx_qid, srvgrp_chan->servicegroup_id, xfer);
		सचे (ret)
			पटिनिवत्त ret;
	}

	सचे (do_rx) {
		ret = smq_rx(mctl, rx_qid, srvgrp_chan->servicegroup_id, xfer);
		सचे (ret)
			पटिनिवत्त ret;
	}

	पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क rpmi_shmem_mbox_get_attribute(संरचना mbox_chan *chan,
					 पूर्णाङ्क attr_id, शून्य *out_मानम्)
{
	संरचना rpmi_shmem_mbox_controller *mctl =
			container_of(chan->mbox,
				     संरचना rpmi_shmem_mbox_controller,
				     controller);
	संरचना rpmi_srvgrp_chan *srvgrp_chan = to_srvgrp_chan(chan);

	विकल्प (attr_id) {
	पक्ष RPMI_CHANNEL_ATTR_PROTOCOL_VERSION:
		*((u32 *)out_मानम्) = mctl->spec_version;
		भङ्ग;
	पक्ष RPMI_CHANNEL_ATTR_MAX_DATA_LEN:
		*((u32 *)out_मानम्) = RPMI_सन्देशः_DATA_SIZE(mctl->slot_size);
		भङ्ग;
	पक्ष RPMI_CHANNEL_ATTR_P2A_DOORBELL_SYSMSI_INDEX:
		*((u32 *)out_मानम्) = mctl->p2a_doorbell_sysmsi_index;
		भङ्ग;
	पक्ष RPMI_CHANNEL_ATTR_TX_TIMEOUT:
		*((u32 *)out_मानम्) = RPMI_DEF_TX_TIMEOUT;
		भङ्ग;
	पक्ष RPMI_CHANNEL_ATTR_RX_TIMEOUT:
		*((u32 *)out_मानम्) = RPMI_DEF_RX_TIMEOUT;
		भङ्ग;
	पक्ष RPMI_CHANNEL_ATTR_SERVICEGROUP_ID:
		*((u32 *)out_मानम्) = srvgrp_chan->servicegroup_id;
		भङ्ग;
	पक्ष RPMI_CHANNEL_ATTR_SERVICEGROUP_VERSION:
		*((u32 *)out_मानम्) = srvgrp_chan->servicegroup_version;
		भङ्ग;
	पक्ष RPMI_CHANNEL_ATTR_IMPL_ID:
		*((u32 *)out_मानम्) = mctl->impl_id;
		भङ्ग;
	पक्ष RPMI_CHANNEL_ATTR_IMPL_VERSION:
		*((u32 *)out_मानम्) = mctl->impl_version;
		भङ्ग;
	स्वभाव:
		पटिनिवत्त SBI_ENOTSUPP;
	}

	पटिनिवत्त 0;
}

स्थिर संरचना mbox_chan *rpmi_shmem_mbox_request_chan(
						संरचना mbox_controller *mbox,
						u32 *chan_args)
{
	पूर्णाङ्क ret;
	u32 tval[2] = { 0 };
	संरचना rpmi_srvgrp_chan *srvgrp_chan;
	संरचना rpmi_shmem_mbox_controller *mctl =
			container_of(mbox,
				     संरचना rpmi_shmem_mbox_controller,
				     controller);

	/* Service group id not defined or in reserved range is invalid */
	सचे (chan_args[0] >= RPMI_SRVGRP_ID_MAX_गणनम् &&
		chan_args[0] <= RPMI_SRVGRP_RESERVE_END)
		पटिनिवत्त रिक्त;

	/* Base serivce group is always present so probe other groups */
	सचे (chan_args[0] != RPMI_SRVGRP_BASE) {
		/* Probe service group */
		ret = smq_base_get_two_u32(mctl,
					   RPMI_BASE_SRV_PROBE_SERVICE_GROUP,
					   chan_args, tval);
		सचे (ret || !tval[1])
			पटिनिवत्त रिक्त;
	}

	srvgrp_chan = sbi_zalloc(परिमाण(*srvgrp_chan));
	सचे (!srvgrp_chan)
		पटिनिवत्त रिक्त;

	srvgrp_chan->servicegroup_id = chan_args[0];
	srvgrp_chan->servicegroup_version = tval[1];

	पटिनिवत्त &srvgrp_chan->chan;
}

स्थिर शून्य rpmi_shmem_mbox_free_chan(संरचना mbox_controller *mbox,
				      संरचना mbox_chan *chan)
{
	संरचना rpmi_srvgrp_chan *srvgrp_chan = to_srvgrp_chan(chan);
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
	uint64_t reg_addr, reg_size;
	संरचना smq_queue_ctx *qctx;

	ret = fdt_ग्रन्थिः_check_compatible(fdt, nodeoff,
					"riscv,rpmi-shmem-mbox");
	सचे (ret)
		पटिनिवत्त ret;

	/* get queue slot size in bytes */
	prop = fdt_getprop(fdt, nodeoff, "riscv,slot-size", &len);
	सचे (!prop)
		पटिनिवत्त SBI_ENOENT;

	mctl->slot_size = fdt32_to_cpu(*prop);
	सचे (mctl->slot_size < RPMI_SLOT_SIZE_MIN) {
		sbi_printf("%s: slot_size < mimnum required message size\n",
			   __func__);
		mctl->slot_size = RPMI_SLOT_SIZE_MIN;
	}

	/* get p2a doorbell system MSI index */
	prop = fdt_getprop(fdt, nodeoff, "riscv,p2a-doorbell-sysmsi-index", &len);
	mctl->p2a_doorbell_sysmsi_index = prop ? fdt32_to_cpu(*prop) : -1U;

	/* get a2p doorbell value */
	prop = fdt_getprop(fdt, nodeoff, "riscv,a2p-doorbell-value", &len);
	mctl->a2p_doorbell_मानम् = prop ? fdt32_to_cpu(*prop) : 1;

	/*
	 * queue names count is taken as the number of queues
	 * supported which make it mandatory to provide the
	 * name of the queue.
	 */
	गणनम् = fdt_stringlist_गणनम्(fdt, nodeoff, "reg-names");
	सचे (गणनम् < 0 ||
	    गणनम् > (RPMI_QUEUE_IDX_MAX_गणनम् + RPMI_REG_IDX_MAX_गणनम्))
		पटिनिवत्त SBI_EINVAL;

	mctl->queue_गणनम् = गणनम् - RPMI_REG_IDX_MAX_गणनम्;

	/* parse all queues and populate queues context structure */
	अत्थाय (qid = 0; qid < mctl->queue_गणनम्; qid++) {
		qctx = &mctl->queue_ctx_tbl[qid];

		/* get each queue share-memory base address and size*/
		ret = fdt_get_ग्रन्थिः_addr_size(fdt, nodeoff, qid,
					     &reg_addr, &reg_size);
		सचे (ret < 0 || !reg_addr || !reg_size)
			पटिनिवत्त SBI_ENOENT;

		ret = sbi_domain_मूलम्_add_memrange(reg_addr, reg_size, reg_size,
						   (SBI_DOMAIN_MEMREGION_MMIO |
						    SBI_DOMAIN_MEMREGION_M_READABLE |
						    SBI_DOMAIN_MEMREGION_M_WRITABLE));
		सचे (ret)
			पटिनिवत्त ret;

		/* calculate number of slots in each queue */
		qctx->अङ्कः_slots =
			(reg_size - (mctl->slot_size * RPMI_QUEUE_HEADER_SLOTS)) / mctl->slot_size;

		/* setup queue pointers */
		qctx->headptr = ((शून्य *)(unsigned long)reg_addr) +
				RPMI_QUEUE_HEAD_SLOT * mctl->slot_size;
		qctx->tailptr = ((शून्य *)(unsigned long)reg_addr) +
				RPMI_QUEUE_TAIL_SLOT * mctl->slot_size;
		qctx->buffer = ((शून्य *)(unsigned long)reg_addr) +
				RPMI_QUEUE_HEADER_SLOTS * mctl->slot_size;

		/* get the queue name */
		name = fdt_stringlist_get(fdt, nodeoff, "reg-names",
					  qid, &len);
		सचे (!name || (name && len < 0))
			पटिनिवत्त len;

		sbi_memcpy(qctx->name, name, len);

		/* store the index as queue_id */
		qctx->queue_id = qid;

		SPIN_LOCK_INIT(qctx->queue_lock);
	}

	/* get the a2p-doorbell property name */
	name = fdt_stringlist_get(fdt, nodeoff, "reg-names", qid, &len);
	सचे (!name || (name && len < 0))
		पटिनिवत्त len;

	/* fetch doorbell register address*/
	ret = fdt_get_ग्रन्थिः_addr_size(fdt, nodeoff, qid, &reg_addr,
				       &reg_size);
	सचे (!ret && !(strncmp(name, "a2p-doorbell", strlen("a2p-doorbell")))) {
		mctl->mb_regs = (शून्य *)(unsigned long)reg_addr;
		ret = sbi_domain_मूलम्_add_memrange(reg_addr, reg_size, reg_size,
						   (SBI_DOMAIN_MEMREGION_MMIO |
						    SBI_DOMAIN_MEMREGION_M_READABLE |
						    SBI_DOMAIN_MEMREGION_M_WRITABLE));
		सचे (ret)
			पटिनिवत्त ret;
	}

	पटिनिवत्त SBI_SUCCESS;
}

स्थिर पूर्णाङ्क rpmi_shmem_mbox_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff,
				ध्रुव संरचना fdt_match *match)
{
	संरचना rpmi_base_get_attributes_resp resp;
	संरचना rpmi_shmem_mbox_controller *mctl;
	संरचना rpmi_srvgrp_chan *base_srvgrp;
	u32 tval[2], args[1];
	पूर्णाङ्क ret = 0;

	mctl = sbi_zalloc(परिमाण(*mctl));
	सचे (!mctl)
		पटिनिवत्त SBI_ENOMEM;

	/* Initialization transport from device tree */
	ret = rpmi_shmem_transport_init(mctl, fdt, nodeoff);
	सचे (ret)
		गच्छ fail_free_controller;

	/* Register mailbox controller */
	mctl->controller.id = nodeoff;
	mctl->controller.max_xfer_len =
			mctl->slot_size - परिमाण(संरचना rpmi_message_header);
	mctl->controller.driver = &fdt_mailbox_rpmi_shmem;
	mctl->controller.request_chan = rpmi_shmem_mbox_request_chan;
	mctl->controller.free_chan = rpmi_shmem_mbox_free_chan;
	mctl->controller.xfer = rpmi_shmem_mbox_xfer;
	mctl->controller.get_attribute = rpmi_shmem_mbox_get_attribute;
	ret = mbox_controller_add(&mctl->controller);
	सचे (ret)
		गच्छ fail_free_controller;

	/* Request base service group channel */
	tval[0] = RPMI_SRVGRP_BASE;
	mctl->base_chan = mbox_controller_request_chan(&mctl->controller,
							tval);
	सचे (!mctl->base_chan) {
		ret = SBI_ENOENT;
		गच्छ fail_remove_controller;
	}

	/* Update base service group version */
	base_srvgrp = to_srvgrp_chan(mctl->base_chan);
	args[0] = RPMI_SRVGRP_BASE;
	ret = smq_base_get_two_u32(mctl, RPMI_BASE_SRV_PROBE_SERVICE_GROUP,
				   &args[0], tval);
	सचे (ret)
		गच्छ fail_free_chan;
	base_srvgrp->servicegroup_version = tval[1];
	सचे (base_srvgrp->servicegroup_version < RPMI_BASE_VERSION_MIN) {
		ret = SBI_EINVAL;
		गच्छ fail_free_chan;
	}

	/* Get implementation id */
	ret = smq_base_get_two_u32(mctl,
				   RPMI_BASE_SRV_GET_IMPLEMENTATION_VERSION,
				   रिक्त, tval);
	सचे (ret)
		गच्छ fail_free_chan;
	mctl->impl_version = tval[1];

	/* Get implementation version */
	ret = smq_base_get_two_u32(mctl, RPMI_BASE_SRV_GET_IMPLEMENTATION_IDN,
				   रिक्त, tval);
	सचे (ret)
		गच्छ fail_free_chan;
	mctl->impl_id = tval[1];

	/* Get specification version */
	ret = smq_base_get_two_u32(mctl, RPMI_BASE_SRV_GET_SPEC_VERSION,
				   रिक्त, tval);
	सचे (ret)
		गच्छ fail_free_chan;
	mctl->spec_version = tval[1];
	सचे (mctl->spec_version < RPMI_BASE_VERSION_MIN ||
	    mctl->spec_version != base_srvgrp->servicegroup_version) {
		ret = SBI_EINVAL;
		गच्छ fail_free_chan;
	}

	/* Get optional features implementation flags */
	ret = rpmi_normal_request_with_status(
			mctl->base_chan, RPMI_BASE_SRV_GET_ATTRIBUTES,
			रिक्त, 0, 0,
			&resp, rpmi_u32_गणनम्(resp), rpmi_u32_गणनम्(resp));
	सचे (ret)
		गच्छ fail_free_chan;

	/* 1: M-mode, 0: S-mode */
	mctl->base_flags.f0_priv_level =
			resp.f0 & RPMI_BASE_FLAGS_F0_PRIVILEGE ? 1 : 0;
	/* 1: Supported, 0: Not Supported */
	mctl->base_flags.f0_ev_notif_en =
			resp.f0 & RPMI_BASE_FLAGS_F0_EV_NOTIFY ? 1 : 0;

	/* We only use M-mode RPMI context in OpenSBI */
	सचे (!mctl->base_flags.f0_priv_level) {
		ret = SBI_ENODEV;
		गच्छ fail_free_chan;
	}

	/*
	 * Continue without platform information string if not
	 * available or if an error is encountered while fetching
	 */
	rpmi_get_platform_info(mctl);

	पटिनिवत्त 0;

fail_free_chan:
	mbox_controller_free_chan(mctl->base_chan);
fail_remove_controller:
	mbox_controller_remove(&mctl->controller);
fail_free_controller:
	sbi_free(mctl);
	पटिनिवत्त ret;
}

स्थिर ध्रुव संरचना fdt_match rpmi_shmem_mbox_match[] = {
	{ .compatible = "riscv,rpmi-shmem-mbox" },
	{ },
};

संरचना fdt_mailbox fdt_mailbox_rpmi_shmem = {
	.driver = {
		.match_table = rpmi_shmem_mbox_match,
		.init = rpmi_shmem_mbox_init,
	},
	.xlate = fdt_mailbox_simple_xlate,
};
