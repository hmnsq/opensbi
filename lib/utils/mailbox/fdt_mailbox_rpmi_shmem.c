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

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi/sbi_timer.h>
#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/riscv_locks.h>
#અન્તર્ભાવ <sbi/riscv_asm.h>
#અન્તર્ભાવ <sbi/riscv_barrier.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_helper.h>
#અન્તર્ભાવ <sbi_utils/mailbox/mailbox.h>
#અન્તર્ભાવ <sbi_utils/mailbox/fdt_mailbox.h>
#અન્તર્ભાવ <sbi_utils/mailbox/rpmi_mailbox.h>

/** Minimum Base group version required */
#define RPMI_BASE_VERSION_MIN		RPMI_VERSION(1, 0)

/**************** RPMI Transport Structures and Macros ***********/

#define GET_SERVICEGROUP_ID(સંદેશ)		\
({						\
	સંરચના rpmi_message *mbuf = સંદેશ;	\
	le16_to_cpu(mbuf->header.servicegroup_id);\
})

#define GET_SERVICE_ID(સંદેશ)			\
({						\
	સંરચના rpmi_message *mbuf = સંદેશ;	\
	mbuf->header.service_id;		\
})

#define GET_FLAGS(સંદેશ)				\
({						\
	સંરચના rpmi_message *mbuf = સંદેશ;	\
	mbuf->header.flags;			\
})

#define GET_MESSAGE_ID(સંદેશ)			\
({						\
	સંરચના rpmi_message *mbuf = સંદેશ;	\
	((u32)mbuf->header.flags << (RPMI_સંદેશ_FLAGS_OFFSET * 8)) | \
	((u32)mbuf->header.service_id << (RPMI_સંદેશ_SERVICE_ID_OFFSET * 8)) | \
	((u32)le16_to_cpu(mbuf->header.servicegroup_id)); \
})

#define MAKE_MESSAGE_ID(__group_id, __service_id, __flags)	\
({						\
	u32 __ret = 0;				\
	__ret |= (u32)(__group_id) << (RPMI_સંદેશ_SERVICEGROUP_ID_OFFSET * 8); \
	__ret |= (u32)(__service_id) << (RPMI_સંદેશ_SERVICE_ID_OFFSET * 8); \
	__ret |= (u32)(__flags) << (RPMI_સંદેશ_FLAGS_OFFSET * 8); \
	__ret;					\
})

#define GET_DLEN(સંદેશ)				\
({						\
	સંરચના rpmi_message *mbuf = સંદેશ;	\
	le16_to_cpu(mbuf->header.datalen);	\
})

#define GET_TOKEN(સંદેશ)				\
({						\
	સંરચના rpmi_message *mbuf = સંદેશ;	\
	le16_to_cpu(mbuf->header.token);	\
})

#define GET_MESSAGE_TYPE(સંદેશ)						\
({									\
	uint8_t flags = *((uint8_t *)સંદેશ + RPMI_સંદેશ_FLAGS_OFFSET);	\
	((flags & RPMI_સંદેશ_FLAGS_TYPE) >> RPMI_સંદેશ_FLAGS_TYPE_POS));	\
})

ગણના rpmi_queue_type {
	RPMI_QUEUE_TYPE_REQ = 0,
	RPMI_QUEUE_TYPE_ACK = 1,
};

ગણના rpmi_queue_idx {
	RPMI_QUEUE_IDX_A2P_REQ = 0,
	RPMI_QUEUE_IDX_P2A_ACK = 1,
	RPMI_QUEUE_IDX_P2A_REQ = 2,
	RPMI_QUEUE_IDX_A2P_ACK = 3,
	RPMI_QUEUE_IDX_MAX_ગણતરી,
};

ગણના rpmi_reg_idx {
	RPMI_REG_IDX_DB_REG = 0, /* Doorbell register */
	RPMI_REG_IDX_MAX_ગણતરી,
};

/** Mailbox registers */
સંરચના rpmi_mb_regs {
	/* doorbell from AP -> PuC*/
	અસ્થિર le32_t db_reg;
} __packed;

/** Single Queue Context Structure */
સંરચના smq_queue_ctx {
	u32 queue_id;
	u32 અંક_slots;
	spinlock_t queue_lock;
	/* Type of queue - REQ or ACK */
	ગણના rpmi_queue_type queue_type;
	/* Pointers to the queue shared memory */
	અસ્થિર le32_t *headptr PT_GUARDED_BY(&queue_lock);
	અસ્થિર le32_t *tailptr PT_GUARDED_BY(&queue_lock);
	અસ્થિર uint8_t *buffer PT_GUARDED_BY(&queue_lock);
	/* Name of the queue */
	અક્ષર name[RPMI_NAME_CHARS_MAX];
};

સંરચના rpmi_srvgrp_chan {
	u32 servicegroup_id;
	u32 servicegroup_version;
	સંરચના mbox_chan chan;
};

#define to_srvgrp_chan(mbox_chan)	\
		container_of(mbox_chan, સંરચના rpmi_srvgrp_chan, chan);

સંરચના rpmi_shmem_mbox_controller {
	/* Driver specific members */
	u32 slot_size;
	u32 queue_ગણતરી;
	u32 p2a_doorbell_sysmsi_index;
	u32 a2p_doorbell_કિંમત;
	સંરચના rpmi_mb_regs *mb_regs;
	સંરચના smq_queue_ctx queue_ctx_tbl[RPMI_QUEUE_IDX_MAX_ગણતરી];
	/* Mailbox framework related members */
	સંરચના mbox_controller controller;
	સંરચના mbox_chan *base_chan;
	u32 impl_version;
	u32 impl_id;
	u32 spec_version;
	u32 plat_info_len;
	અક્ષર *plat_info;
	સંરચના {
		u8 f0_priv_level;
		bool f0_ev_notif_en;
	} base_flags;
};

/**************** Shared Memory Queues Helpers **************/

સ્થિર bool __smq_queue_full(સંરચના smq_queue_ctx *qctx)
	MUST_HOLD(&qctx->queue_lock)
{
	પાછા ((le32_to_cpu(*qctx->tailptr) + 1) % qctx->અંક_slots ==
			le32_to_cpu(*qctx->headptr)) ? true : false;
}

સ્થિર bool __smq_queue_empty(સંરચના smq_queue_ctx *qctx)
	MUST_HOLD(&qctx->queue_lock)
{
	પાછા (le32_to_cpu(*qctx->headptr) ==
		le32_to_cpu(*qctx->tailptr)) ? true : false;
}

સ્થિર પૂર્ણાંક __smq_rx(સંરચના smq_queue_ctx *qctx, u32 slot_size,
		    u32 service_group_id, સંરચના mbox_xfer *xfer)
	MUST_HOLD(&qctx->queue_lock)
{
	શૂન્ય *dst, *src;
	સંરચના rpmi_message *સંદેશ;
	u32 i, tmp, pos, dlen, msgidn, headidx, tailidx;
	સંરચના rpmi_message_args *args = xfer->args;
	bool no_rx_token = (args->flags & RPMI_સંદેશ_FLAGS_NO_RX_TOKEN) ?
			   true : false;

	/* Rx sanity checks */
	જો ((કદ(u32) * args->rx_endian_શબ્દો) >
	    (slot_size - કદ(સંરચના rpmi_message_header)))
		પાછા SBI_EINVAL;
	જો ((કદ(u32) * args->rx_endian_શબ્દો) > xfer->rx_len)
		પાછા SBI_EINVAL;

	/* There should be some message in the queue */
	જો (__smq_queue_empty(qctx))
		પાછા SBI_ENOENT;

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
	જ્યારે (pos != tailidx) {
		src = (શૂન્ય *)qctx->buffer + (pos * slot_size);
		જો ((no_rx_token && GET_MESSAGE_ID(src) == msgidn) ||
		    (GET_TOKEN(src) == (xfer->seq & RPMI_સંદેશ_TOKEN_MASK)))
			તોડો;
		pos = (pos + 1) % qctx->અંક_slots;
	}
	જો (pos == tailidx)
		પાછા SBI_ENOENT;

	/* If Rx message is not first message then make it first message */
	જો (pos != headidx) {
		src = (શૂન્ય *)qctx->buffer + (pos * slot_size);
		dst = (શૂન્ય *)qctx->buffer + (headidx * slot_size);
		માટે (i = 0; i < slot_size / કદ(u32); i++) {
			tmp = ((u32 *)dst)[i];
			((u32 *)dst)[i] = ((u32 *)src)[i];
			((u32 *)src)[i] = tmp;
		}
	}

	/* Update rx_token if not available */
	સંદેશ = (શૂન્ય *)qctx->buffer + (headidx * slot_size);
	જો (no_rx_token)
		args->rx_token = GET_TOKEN(સંદેશ);

	/* Extract data from the first message */
	જો (xfer->rx) {
		args->rx_data_len = dlen = GET_DLEN(સંદેશ);
		જો (dlen > xfer->rx_len)
			dlen = xfer->rx_len;
		src = (શૂન્ય *)સંદેશ + કદ(સંરચના rpmi_message_header);
		dst = xfer->rx;
		માટે (i = 0; i < args->rx_endian_શબ્દો; i++)
			((u32 *)dst)[i] = le32_to_cpu(((u32 *)src)[i]);
		dst += કદ(u32) * args->rx_endian_શબ્દો;
		src += કદ(u32) * args->rx_endian_શબ્દો;
		sbi_memcpy(dst, src,
			xfer->rx_len - (કદ(u32) * args->rx_endian_શબ્દો));
	}

	/* Update the head/read index */
	*qctx->headptr = cpu_to_le32(headidx + 1) % qctx->અંક_slots;

	/* Make sure updates to head are immediately visible to PuC */
	smp_wmb();

	પાછા SBI_OK;
}

સ્થિર પૂર્ણાંક __smq_tx(સંરચના smq_queue_ctx *qctx, સંરચના rpmi_mb_regs *mb_regs,
		    u32 a2p_doorbell_કિંમત, u32 slot_size, u32 service_group_id,
		    સંરચના mbox_xfer *xfer) MUST_HOLD(&qctx->queue_lock)
{
	u32 i, tailidx;
	શૂન્ય *dst, *src;
	સંરચના rpmi_message_header header = { 0 };
	સંરચના rpmi_message_args *args = xfer->args;

	/* Tx sanity checks */
	જો ((કદ(u32) * args->tx_endian_શબ્દો) >
	    (slot_size - કદ(સંરચના rpmi_message_header)))
		પાછા SBI_EINVAL;
	જો ((કદ(u32) * args->tx_endian_શબ્દો) > xfer->tx_len)
		પાછા SBI_EINVAL;

	/* There should be some room in the queue */
	જો (__smq_queue_full(qctx))
		પાછા SBI_ENOMEM;

	/* Get the tail/write index */
	tailidx = le32_to_cpu(*qctx->tailptr);

	/* Prepare the header to be written into the slot */
	header.servicegroup_id = cpu_to_le16(service_group_id);
	header.service_id = args->service_id;
	header.flags = args->type;
	header.datalen = cpu_to_le16((u16)xfer->tx_len);
	header.token = cpu_to_le16((u16)xfer->seq);

	/* Write header into the slot */
	dst = (અક્ષર *)qctx->buffer + (tailidx * slot_size);
	sbi_memcpy(dst, &header, કદ(header));
	dst += કદ(header);

	/* Write data into the slot */
	જો (xfer->tx) {
		src = xfer->tx;
		માટે (i = 0; i < args->tx_endian_શબ્દો; i++)
			((u32 *)dst)[i] = cpu_to_le32(((u32 *)src)[i]);
		dst += કદ(u32) * args->tx_endian_શબ્દો;
		src += કદ(u32) * args->tx_endian_શબ્દો;
		sbi_memcpy(dst, src,
			xfer->tx_len - (કદ(u32) * args->tx_endian_શબ્દો));
	}

	/* Make sure queue chanages are visible to PuC before updating tail */
	smp_wmb();

	/* Update the tail/write index */
	*qctx->tailptr = cpu_to_le32(tailidx + 1) % qctx->અંક_slots;

	/* Ring the RPMI doorbell if present */
	જો (mb_regs)
		writel(a2p_doorbell_કિંમત, &mb_regs->db_reg);

	પાછા SBI_OK;
}

સ્થિર પૂર્ણાંક smq_rx(સંરચના rpmi_shmem_mbox_controller *mctl,
		  u32 queue_id, u32 service_group_id, સંરચના mbox_xfer *xfer)
{
	પૂર્ણાંક ret, rxretry = 0;
	સંરચના smq_queue_ctx *qctx;

	જો (mctl->queue_ગણતરી < queue_id) {
		sbi_printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		પાછા SBI_EINVAL;
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
	કરો {
		spin_lock(&qctx->queue_lock);
		ret = __smq_rx(qctx, mctl->slot_size, service_group_id, xfer);
		spin_unlock(&qctx->queue_lock);
		જો (!ret)
			પાછા 0;

		sbi_timer_mdelay(1);
		rxretry += 1;
	} જ્યારે (rxretry < xfer->rx_timeout);

	પાછા SBI_ETIMEDOUT;
}

સ્થિર પૂર્ણાંક smq_tx(સંરચના rpmi_shmem_mbox_controller *mctl,
		  u32 queue_id, u32 service_group_id, સંરચના mbox_xfer *xfer)
{
	પૂર્ણાંક ret, txretry = 0;
	સંરચના smq_queue_ctx *qctx;

	જો (mctl->queue_ગણતરી < queue_id) {
		sbi_printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		પાછા SBI_EINVAL;
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
	કરો {
		spin_lock(&qctx->queue_lock);
		ret = __smq_tx(qctx, mctl->mb_regs, mctl->a2p_doorbell_કિંમત,
				mctl->slot_size, service_group_id, xfer);
		spin_unlock(&qctx->queue_lock);
		જો (!ret)
			પાછા 0;

		sbi_timer_mdelay(1);
		txretry += 1;
	} જ્યારે (txretry < xfer->tx_timeout);

	પાછા SBI_ETIMEDOUT;
}

સ્થિર પૂર્ણાંક rpmi_get_platform_info(સંરચના rpmi_shmem_mbox_controller *mctl)
{
	પૂર્ણાંક ret = SBI_OK;

	/**
	 * platform string can occupy max possible size
	 * max possible space in the message data as
	 * per the format
	 */
	સંરચના rpmi_base_get_platform_info_resp *resp =
			sbi_zalloc(RPMI_સંદેશ_DATA_SIZE(mctl->slot_size));
	જો (!resp)
		પાછા SBI_ENOMEM;

	ret = rpmi_normal_request_with_status(mctl->base_chan,
				 RPMI_BASE_SRV_GET_PLATFORM_INFO,
				 ખાલી, 0, 0,
				 resp,
				 RPMI_સંદેશ_DATA_SIZE(mctl->slot_size)/4,
				 RPMI_સંદેશ_DATA_SIZE(mctl->slot_size)/4);
	જો (ret)
		જાઓ fail_free_resp;

	mctl->plat_info_len = resp->plat_info_len;
	mctl->plat_info = sbi_zalloc(mctl->plat_info_len);
	જો (!mctl->plat_info) {
		ret = SBI_ENOMEM;
		જાઓ fail_free_resp;
	}

	sbi_strncpy(mctl->plat_info, resp->plat_info, mctl->plat_info_len);

fail_free_resp:
	sbi_free(resp);
	પાછા ret;
}

સ્થિર પૂર્ણાંક smq_base_get_two_u32(સંરચના rpmi_shmem_mbox_controller *mctl,
				u32 service_id, u32 *inarg, u32 *outvals)
{
	પાછા rpmi_normal_request_with_status(
			mctl->base_chan, service_id,
			inarg, (inarg) ? 1 : 0, (inarg) ? 1 : 0,
			outvals, 2, 2);
}

/**************** Mailbox Controller Functions **************/

સ્થિર પૂર્ણાંક rpmi_shmem_mbox_xfer(સંરચના mbox_chan *chan, સંરચના mbox_xfer *xfer)
{
	પૂર્ણાંક ret;
	u32 tx_qid = 0, rx_qid = 0;
	સંરચના rpmi_shmem_mbox_controller *mctl =
			container_of(chan->mbox,
				     સંરચના rpmi_shmem_mbox_controller,
				     controller);
	સંરચના rpmi_srvgrp_chan *srvgrp_chan = to_srvgrp_chan(chan);

	સંરચના rpmi_message_args *args = xfer->args;
	bool do_tx = (args->flags & RPMI_સંદેશ_FLAGS_NO_TX) ? false : true;
	bool do_rx = (args->flags & RPMI_સંદેશ_FLAGS_NO_RX) ? false : true;

	જો (!do_tx && !do_rx)
		પાછા SBI_EINVAL;

	પસંદગી (args->type) {
	સ્થિતિ RPMI_સંદેશ_NORMAL_REQUEST:
		જો (do_tx && do_rx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
			rx_qid = RPMI_QUEUE_IDX_P2A_ACK;
		} નહીંતર જો (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
		} નહીંતર જો (do_rx) {
			rx_qid = RPMI_QUEUE_IDX_P2A_REQ;
		}
		તોડો;
	સ્થિતિ RPMI_સંદેશ_POSTED_REQUEST:
		જો (do_tx && do_rx)
			પાછા SBI_EINVAL;
		જો (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
		} નહીંતર {
			rx_qid = RPMI_QUEUE_IDX_P2A_REQ;
		}
		તોડો;
	સ્થિતિ RPMI_સંદેશ_ACKNOWLDGEMENT:
		જો (do_tx && do_rx)
			પાછા SBI_EINVAL;
		જો (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_ACK;
		} નહીંતર {
			rx_qid = RPMI_QUEUE_IDX_P2A_ACK;
		}
		તોડો;
	મૂળભૂત:
		પાછા SBI_ENOTSUPP;
	}

	જો (do_tx) {
		ret = smq_tx(mctl, tx_qid, srvgrp_chan->servicegroup_id, xfer);
		જો (ret)
			પાછા ret;
	}

	જો (do_rx) {
		ret = smq_rx(mctl, rx_qid, srvgrp_chan->servicegroup_id, xfer);
		જો (ret)
			પાછા ret;
	}

	પાછા 0;
}

સ્થિર પૂર્ણાંક rpmi_shmem_mbox_get_attribute(સંરચના mbox_chan *chan,
					 પૂર્ણાંક attr_id, શૂન્ય *out_કિંમત)
{
	સંરચના rpmi_shmem_mbox_controller *mctl =
			container_of(chan->mbox,
				     સંરચના rpmi_shmem_mbox_controller,
				     controller);
	સંરચના rpmi_srvgrp_chan *srvgrp_chan = to_srvgrp_chan(chan);

	પસંદગી (attr_id) {
	સ્થિતિ RPMI_CHANNEL_ATTR_PROTOCOL_VERSION:
		*((u32 *)out_કિંમત) = mctl->spec_version;
		તોડો;
	સ્થિતિ RPMI_CHANNEL_ATTR_MAX_DATA_LEN:
		*((u32 *)out_કિંમત) = RPMI_સંદેશ_DATA_SIZE(mctl->slot_size);
		તોડો;
	સ્થિતિ RPMI_CHANNEL_ATTR_P2A_DOORBELL_SYSMSI_INDEX:
		*((u32 *)out_કિંમત) = mctl->p2a_doorbell_sysmsi_index;
		તોડો;
	સ્થિતિ RPMI_CHANNEL_ATTR_TX_TIMEOUT:
		*((u32 *)out_કિંમત) = RPMI_DEF_TX_TIMEOUT;
		તોડો;
	સ્થિતિ RPMI_CHANNEL_ATTR_RX_TIMEOUT:
		*((u32 *)out_કિંમત) = RPMI_DEF_RX_TIMEOUT;
		તોડો;
	સ્થિતિ RPMI_CHANNEL_ATTR_SERVICEGROUP_ID:
		*((u32 *)out_કિંમત) = srvgrp_chan->servicegroup_id;
		તોડો;
	સ્થિતિ RPMI_CHANNEL_ATTR_SERVICEGROUP_VERSION:
		*((u32 *)out_કિંમત) = srvgrp_chan->servicegroup_version;
		તોડો;
	સ્થિતિ RPMI_CHANNEL_ATTR_IMPL_ID:
		*((u32 *)out_કિંમત) = mctl->impl_id;
		તોડો;
	સ્થિતિ RPMI_CHANNEL_ATTR_IMPL_VERSION:
		*((u32 *)out_કિંમત) = mctl->impl_version;
		તોડો;
	મૂળભૂત:
		પાછા SBI_ENOTSUPP;
	}

	પાછા 0;
}

સ્થિર સંરચના mbox_chan *rpmi_shmem_mbox_request_chan(
						સંરચના mbox_controller *mbox,
						u32 *chan_args)
{
	પૂર્ણાંક ret;
	u32 tval[2] = { 0 };
	સંરચના rpmi_srvgrp_chan *srvgrp_chan;
	સંરચના rpmi_shmem_mbox_controller *mctl =
			container_of(mbox,
				     સંરચના rpmi_shmem_mbox_controller,
				     controller);

	/* Service group id not defined or in reserved range is invalid */
	જો (chan_args[0] >= RPMI_SRVGRP_ID_MAX_ગણતરી &&
		chan_args[0] <= RPMI_SRVGRP_RESERVE_END)
		પાછા ખાલી;

	/* Base serivce group is always present so probe other groups */
	જો (chan_args[0] != RPMI_SRVGRP_BASE) {
		/* Probe service group */
		ret = smq_base_get_two_u32(mctl,
					   RPMI_BASE_SRV_PROBE_SERVICE_GROUP,
					   chan_args, tval);
		જો (ret || !tval[1])
			પાછા ખાલી;
	}

	srvgrp_chan = sbi_zalloc(કદ(*srvgrp_chan));
	જો (!srvgrp_chan)
		પાછા ખાલી;

	srvgrp_chan->servicegroup_id = chan_args[0];
	srvgrp_chan->servicegroup_version = tval[1];

	પાછા &srvgrp_chan->chan;
}

સ્થિર શૂન્ય rpmi_shmem_mbox_free_chan(સંરચના mbox_controller *mbox,
				      સંરચના mbox_chan *chan)
{
	સંરચના rpmi_srvgrp_chan *srvgrp_chan = to_srvgrp_chan(chan);
	sbi_free(srvgrp_chan);
}

બાહ્ય સંરચના fdt_mailbox fdt_mailbox_rpmi_shmem;

સ્થિર પૂર્ણાંક rpmi_shmem_transport_init(સંરચના rpmi_shmem_mbox_controller *mctl,
				     અચલ શૂન્ય *fdt,
				     પૂર્ણાંક nodeoff) NO_THREAD_SAFETY_ANALYSIS
{
	અચલ અક્ષર *name;
	અચલ fdt32_t *prop;
	પૂર્ણાંક ગણતરી, len, ret, qid;
	uint64_t reg_addr, reg_size;
	સંરચના smq_queue_ctx *qctx;

	ret = fdt_ગાંઠ_check_compatible(fdt, nodeoff,
					"riscv,rpmi-shmem-mbox");
	જો (ret)
		પાછા ret;

	/* get queue slot size in bytes */
	prop = fdt_getprop(fdt, nodeoff, "riscv,slot-size", &len);
	જો (!prop)
		પાછા SBI_ENOENT;

	mctl->slot_size = fdt32_to_cpu(*prop);
	જો (mctl->slot_size < RPMI_SLOT_SIZE_MIN) {
		sbi_printf("%s: slot_size < mimnum required message size\n",
			   __func__);
		mctl->slot_size = RPMI_SLOT_SIZE_MIN;
	}

	/* get p2a doorbell system MSI index */
	prop = fdt_getprop(fdt, nodeoff, "riscv,p2a-doorbell-sysmsi-index", &len);
	mctl->p2a_doorbell_sysmsi_index = prop ? fdt32_to_cpu(*prop) : -1U;

	/* get a2p doorbell value */
	prop = fdt_getprop(fdt, nodeoff, "riscv,a2p-doorbell-value", &len);
	mctl->a2p_doorbell_કિંમત = prop ? fdt32_to_cpu(*prop) : 1;

	/*
	 * queue names count is taken as the number of queues
	 * supported which make it mandatory to provide the
	 * name of the queue.
	 */
	ગણતરી = fdt_stringlist_ગણતરી(fdt, nodeoff, "reg-names");
	જો (ગણતરી < 0 ||
	    ગણતરી > (RPMI_QUEUE_IDX_MAX_ગણતરી + RPMI_REG_IDX_MAX_ગણતરી))
		પાછા SBI_EINVAL;

	mctl->queue_ગણતરી = ગણતરી - RPMI_REG_IDX_MAX_ગણતરી;

	/* parse all queues and populate queues context structure */
	માટે (qid = 0; qid < mctl->queue_ગણતરી; qid++) {
		qctx = &mctl->queue_ctx_tbl[qid];

		/* get each queue share-memory base address and size*/
		ret = fdt_get_ગાંઠ_addr_size(fdt, nodeoff, qid,
					     &reg_addr, &reg_size);
		જો (ret < 0 || !reg_addr || !reg_size)
			પાછા SBI_ENOENT;

		ret = sbi_domain_મૂળ_add_memrange(reg_addr, reg_size, reg_size,
						   (SBI_DOMAIN_MEMREGION_MMIO |
						    SBI_DOMAIN_MEMREGION_M_READABLE |
						    SBI_DOMAIN_MEMREGION_M_WRITABLE));
		જો (ret)
			પાછા ret;

		/* calculate number of slots in each queue */
		qctx->અંક_slots =
			(reg_size - (mctl->slot_size * RPMI_QUEUE_HEADER_SLOTS)) / mctl->slot_size;

		/* setup queue pointers */
		qctx->headptr = ((શૂન્ય *)(unsigned long)reg_addr) +
				RPMI_QUEUE_HEAD_SLOT * mctl->slot_size;
		qctx->tailptr = ((શૂન્ય *)(unsigned long)reg_addr) +
				RPMI_QUEUE_TAIL_SLOT * mctl->slot_size;
		qctx->buffer = ((શૂન્ય *)(unsigned long)reg_addr) +
				RPMI_QUEUE_HEADER_SLOTS * mctl->slot_size;

		/* get the queue name */
		name = fdt_stringlist_get(fdt, nodeoff, "reg-names",
					  qid, &len);
		જો (!name || (name && len < 0))
			પાછા len;

		sbi_memcpy(qctx->name, name, len);

		/* store the index as queue_id */
		qctx->queue_id = qid;

		SPIN_LOCK_INIT(qctx->queue_lock);
	}

	/* get the a2p-doorbell property name */
	name = fdt_stringlist_get(fdt, nodeoff, "reg-names", qid, &len);
	જો (!name || (name && len < 0))
		પાછા len;

	/* fetch doorbell register address*/
	ret = fdt_get_ગાંઠ_addr_size(fdt, nodeoff, qid, &reg_addr,
				       &reg_size);
	જો (!ret && !(strncmp(name, "a2p-doorbell", strlen("a2p-doorbell")))) {
		mctl->mb_regs = (શૂન્ય *)(unsigned long)reg_addr;
		ret = sbi_domain_મૂળ_add_memrange(reg_addr, reg_size, reg_size,
						   (SBI_DOMAIN_MEMREGION_MMIO |
						    SBI_DOMAIN_MEMREGION_M_READABLE |
						    SBI_DOMAIN_MEMREGION_M_WRITABLE));
		જો (ret)
			પાછા ret;
	}

	પાછા SBI_SUCCESS;
}

સ્થિર પૂર્ણાંક rpmi_shmem_mbox_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff,
				અચલ સંરચના fdt_match *match)
{
	સંરચના rpmi_base_get_attributes_resp resp;
	સંરચના rpmi_shmem_mbox_controller *mctl;
	સંરચના rpmi_srvgrp_chan *base_srvgrp;
	u32 tval[2], args[1];
	પૂર્ણાંક ret = 0;

	mctl = sbi_zalloc(કદ(*mctl));
	જો (!mctl)
		પાછા SBI_ENOMEM;

	/* Initialization transport from device tree */
	ret = rpmi_shmem_transport_init(mctl, fdt, nodeoff);
	જો (ret)
		જાઓ fail_free_controller;

	/* Register mailbox controller */
	mctl->controller.id = nodeoff;
	mctl->controller.max_xfer_len =
			mctl->slot_size - કદ(સંરચના rpmi_message_header);
	mctl->controller.driver = &fdt_mailbox_rpmi_shmem;
	mctl->controller.request_chan = rpmi_shmem_mbox_request_chan;
	mctl->controller.free_chan = rpmi_shmem_mbox_free_chan;
	mctl->controller.xfer = rpmi_shmem_mbox_xfer;
	mctl->controller.get_attribute = rpmi_shmem_mbox_get_attribute;
	ret = mbox_controller_add(&mctl->controller);
	જો (ret)
		જાઓ fail_free_controller;

	/* Request base service group channel */
	tval[0] = RPMI_SRVGRP_BASE;
	mctl->base_chan = mbox_controller_request_chan(&mctl->controller,
							tval);
	જો (!mctl->base_chan) {
		ret = SBI_ENOENT;
		જાઓ fail_remove_controller;
	}

	/* Update base service group version */
	base_srvgrp = to_srvgrp_chan(mctl->base_chan);
	args[0] = RPMI_SRVGRP_BASE;
	ret = smq_base_get_two_u32(mctl, RPMI_BASE_SRV_PROBE_SERVICE_GROUP,
				   &args[0], tval);
	જો (ret)
		જાઓ fail_free_chan;
	base_srvgrp->servicegroup_version = tval[1];
	જો (base_srvgrp->servicegroup_version < RPMI_BASE_VERSION_MIN) {
		ret = SBI_EINVAL;
		જાઓ fail_free_chan;
	}

	/* Get implementation id */
	ret = smq_base_get_two_u32(mctl,
				   RPMI_BASE_SRV_GET_IMPLEMENTATION_VERSION,
				   ખાલી, tval);
	જો (ret)
		જાઓ fail_free_chan;
	mctl->impl_version = tval[1];

	/* Get implementation version */
	ret = smq_base_get_two_u32(mctl, RPMI_BASE_SRV_GET_IMPLEMENTATION_IDN,
				   ખાલી, tval);
	જો (ret)
		જાઓ fail_free_chan;
	mctl->impl_id = tval[1];

	/* Get specification version */
	ret = smq_base_get_two_u32(mctl, RPMI_BASE_SRV_GET_SPEC_VERSION,
				   ખાલી, tval);
	જો (ret)
		જાઓ fail_free_chan;
	mctl->spec_version = tval[1];
	જો (mctl->spec_version < RPMI_BASE_VERSION_MIN ||
	    mctl->spec_version != base_srvgrp->servicegroup_version) {
		ret = SBI_EINVAL;
		જાઓ fail_free_chan;
	}

	/* Get optional features implementation flags */
	ret = rpmi_normal_request_with_status(
			mctl->base_chan, RPMI_BASE_SRV_GET_ATTRIBUTES,
			ખાલી, 0, 0,
			&resp, rpmi_u32_ગણતરી(resp), rpmi_u32_ગણતરી(resp));
	જો (ret)
		જાઓ fail_free_chan;

	/* 1: M-mode, 0: S-mode */
	mctl->base_flags.f0_priv_level =
			resp.f0 & RPMI_BASE_FLAGS_F0_PRIVILEGE ? 1 : 0;
	/* 1: Supported, 0: Not Supported */
	mctl->base_flags.f0_ev_notif_en =
			resp.f0 & RPMI_BASE_FLAGS_F0_EV_NOTIFY ? 1 : 0;

	/* We only use M-mode RPMI context in OpenSBI */
	જો (!mctl->base_flags.f0_priv_level) {
		ret = SBI_ENODEV;
		જાઓ fail_free_chan;
	}

	/*
	 * Continue without platform information string if not
	 * available or if an error is encountered while fetching
	 */
	rpmi_get_platform_info(mctl);

	પાછા 0;

fail_free_chan:
	mbox_controller_free_chan(mctl->base_chan);
fail_remove_controller:
	mbox_controller_remove(&mctl->controller);
fail_free_controller:
	sbi_free(mctl);
	પાછા ret;
}

સ્થિર અચલ સંરચના fdt_match rpmi_shmem_mbox_match[] = {
	{ .compatible = "riscv,rpmi-shmem-mbox" },
	{ },
};

સંરચના fdt_mailbox fdt_mailbox_rpmi_shmem = {
	.driver = {
		.match_table = rpmi_shmem_mbox_match,
		.init = rpmi_shmem_mbox_init,
	},
	.xlate = fdt_mailbox_simple_xlate,
};
