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

#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi/sbi_timer.h>
#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/riscv_locks.h>
#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/riscv_barrier.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_helper.h>
#அந்தர்பாவ <sbi_utils/mailbox/mailbox.h>
#அந்தர்பாவ <sbi_utils/mailbox/fdt_mailbox.h>
#அந்தர்பாவ <sbi_utils/mailbox/rpmi_mailbox.h>

/** Minimum Base group version required */
#define RPMI_BASE_VERSION_MIN		RPMI_VERSION(1, 0)

/**************** RPMI Transport Structures and Macros ***********/

#define GET_SERVICEGROUP_ID(செய்தி)		\
({						\
	கட்டமைப்பு rpmi_message *mbuf = செய்தி;	\
	le16_to_cpu(mbuf->header.servicegroup_id);\
})

#define GET_SERVICE_ID(செய்தி)			\
({						\
	கட்டமைப்பு rpmi_message *mbuf = செய்தி;	\
	mbuf->header.service_id;		\
})

#define GET_FLAGS(செய்தி)				\
({						\
	கட்டமைப்பு rpmi_message *mbuf = செய்தி;	\
	mbuf->header.flags;			\
})

#define GET_MESSAGE_ID(செய்தி)			\
({						\
	கட்டமைப்பு rpmi_message *mbuf = செய்தி;	\
	((u32)mbuf->header.flags << (RPMI_செய்தி_FLAGS_OFFSET * 8)) | \
	((u32)mbuf->header.service_id << (RPMI_செய்தி_SERVICE_ID_OFFSET * 8)) | \
	((u32)le16_to_cpu(mbuf->header.servicegroup_id)); \
})

#define MAKE_MESSAGE_ID(__group_id, __service_id, __flags)	\
({						\
	u32 __ret = 0;				\
	__ret |= (u32)(__group_id) << (RPMI_செய்தி_SERVICEGROUP_ID_OFFSET * 8); \
	__ret |= (u32)(__service_id) << (RPMI_செய்தி_SERVICE_ID_OFFSET * 8); \
	__ret |= (u32)(__flags) << (RPMI_செய்தி_FLAGS_OFFSET * 8); \
	__ret;					\
})

#define GET_DLEN(செய்தி)				\
({						\
	கட்டமைப்பு rpmi_message *mbuf = செய்தி;	\
	le16_to_cpu(mbuf->header.datalen);	\
})

#define GET_TOKEN(செய்தி)				\
({						\
	கட்டமைப்பு rpmi_message *mbuf = செய்தி;	\
	le16_to_cpu(mbuf->header.token);	\
})

#define GET_MESSAGE_TYPE(செய்தி)						\
({									\
	uint8_t flags = *((uint8_t *)செய்தி + RPMI_செய்தி_FLAGS_OFFSET);	\
	((flags & RPMI_செய்தி_FLAGS_TYPE) >> RPMI_செய்தி_FLAGS_TYPE_POS));	\
})

எண்ணிக்கை rpmi_queue_type {
	RPMI_QUEUE_TYPE_REQ = 0,
	RPMI_QUEUE_TYPE_ACK = 1,
};

எண்ணிக்கை rpmi_queue_idx {
	RPMI_QUEUE_IDX_A2P_REQ = 0,
	RPMI_QUEUE_IDX_P2A_ACK = 1,
	RPMI_QUEUE_IDX_P2A_REQ = 2,
	RPMI_QUEUE_IDX_A2P_ACK = 3,
	RPMI_QUEUE_IDX_MAX_மொத்தம்,
};

எண்ணிக்கை rpmi_reg_idx {
	RPMI_REG_IDX_DB_REG = 0, /* Doorbell register */
	RPMI_REG_IDX_MAX_மொத்தம்,
};

/** Mailbox registers */
கட்டமைப்பு rpmi_mb_regs {
	/* doorbell from AP -> PuC*/
	நிலையற்ற le32_t db_reg;
} __packed;

/** Single Queue Context Structure */
கட்டமைப்பு smq_queue_ctx {
	u32 queue_id;
	u32 இலக்கம்_slots;
	spinlock_t queue_lock;
	/* Type of queue - REQ or ACK */
	எண்ணிக்கை rpmi_queue_type queue_type;
	/* Pointers to the queue shared memory */
	நிலையற்ற le32_t *headptr PT_GUARDED_BY(&queue_lock);
	நிலையற்ற le32_t *tailptr PT_GUARDED_BY(&queue_lock);
	நிலையற்ற uint8_t *buffer PT_GUARDED_BY(&queue_lock);
	/* Name of the queue */
	எழுத்து name[RPMI_NAME_CHARS_MAX];
};

கட்டமைப்பு rpmi_srvgrp_chan {
	u32 servicegroup_id;
	u32 servicegroup_version;
	கட்டமைப்பு mbox_chan chan;
};

#define to_srvgrp_chan(mbox_chan)	\
		container_of(mbox_chan, கட்டமைப்பு rpmi_srvgrp_chan, chan);

கட்டமைப்பு rpmi_shmem_mbox_controller {
	/* Driver specific members */
	u32 slot_size;
	u32 queue_மொத்தம்;
	u32 p2a_doorbell_sysmsi_index;
	u32 a2p_doorbell_மதிப்பு;
	கட்டமைப்பு rpmi_mb_regs *mb_regs;
	கட்டமைப்பு smq_queue_ctx queue_ctx_tbl[RPMI_QUEUE_IDX_MAX_மொத்தம்];
	/* Mailbox framework related members */
	கட்டமைப்பு mbox_controller controller;
	கட்டமைப்பு mbox_chan *base_chan;
	u32 impl_version;
	u32 impl_id;
	u32 spec_version;
	u32 plat_info_len;
	எழுத்து *plat_info;
	கட்டமைப்பு {
		u8 f0_priv_level;
		bool f0_ev_notif_en;
	} base_flags;
};

/**************** Shared Memory Queues Helpers **************/

நிலையான bool __smq_queue_full(கட்டமைப்பு smq_queue_ctx *qctx)
	MUST_HOLD(&qctx->queue_lock)
{
	திரும்பு ((le32_to_cpu(*qctx->tailptr) + 1) % qctx->இலக்கம்_slots ==
			le32_to_cpu(*qctx->headptr)) ? true : false;
}

நிலையான bool __smq_queue_empty(கட்டமைப்பு smq_queue_ctx *qctx)
	MUST_HOLD(&qctx->queue_lock)
{
	திரும்பு (le32_to_cpu(*qctx->headptr) ==
		le32_to_cpu(*qctx->tailptr)) ? true : false;
}

நிலையான முழு __smq_rx(கட்டமைப்பு smq_queue_ctx *qctx, u32 slot_size,
		    u32 service_group_id, கட்டமைப்பு mbox_xfer *xfer)
	MUST_HOLD(&qctx->queue_lock)
{
	வெறுமை *dst, *src;
	கட்டமைப்பு rpmi_message *செய்தி;
	u32 i, tmp, pos, dlen, msgidn, headidx, tailidx;
	கட்டமைப்பு rpmi_message_args *args = xfer->args;
	bool no_rx_token = (args->flags & RPMI_செய்தி_FLAGS_NO_RX_TOKEN) ?
			   true : false;

	/* Rx sanity checks */
	எனில் ((அளவு(u32) * args->rx_endian_சொற்கள்) >
	    (slot_size - அளவு(கட்டமைப்பு rpmi_message_header)))
		திரும்பு SBI_EINVAL;
	எனில் ((அளவு(u32) * args->rx_endian_சொற்கள்) > xfer->rx_len)
		திரும்பு SBI_EINVAL;

	/* There should be some message in the queue */
	எனில் (__smq_queue_empty(qctx))
		திரும்பு SBI_ENOENT;

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
	போது (pos != tailidx) {
		src = (வெறுமை *)qctx->buffer + (pos * slot_size);
		எனில் ((no_rx_token && GET_MESSAGE_ID(src) == msgidn) ||
		    (GET_TOKEN(src) == (xfer->seq & RPMI_செய்தி_TOKEN_MASK)))
			நிறுத்து;
		pos = (pos + 1) % qctx->இலக்கம்_slots;
	}
	எனில் (pos == tailidx)
		திரும்பு SBI_ENOENT;

	/* If Rx message is not first message then make it first message */
	எனில் (pos != headidx) {
		src = (வெறுமை *)qctx->buffer + (pos * slot_size);
		dst = (வெறுமை *)qctx->buffer + (headidx * slot_size);
		க்கு (i = 0; i < slot_size / அளவு(u32); i++) {
			tmp = ((u32 *)dst)[i];
			((u32 *)dst)[i] = ((u32 *)src)[i];
			((u32 *)src)[i] = tmp;
		}
	}

	/* Update rx_token if not available */
	செய்தி = (வெறுமை *)qctx->buffer + (headidx * slot_size);
	எனில் (no_rx_token)
		args->rx_token = GET_TOKEN(செய்தி);

	/* Extract data from the first message */
	எனில் (xfer->rx) {
		args->rx_data_len = dlen = GET_DLEN(செய்தி);
		எனில் (dlen > xfer->rx_len)
			dlen = xfer->rx_len;
		src = (வெறுமை *)செய்தி + அளவு(கட்டமைப்பு rpmi_message_header);
		dst = xfer->rx;
		க்கு (i = 0; i < args->rx_endian_சொற்கள்; i++)
			((u32 *)dst)[i] = le32_to_cpu(((u32 *)src)[i]);
		dst += அளவு(u32) * args->rx_endian_சொற்கள்;
		src += அளவு(u32) * args->rx_endian_சொற்கள்;
		sbi_memcpy(dst, src,
			xfer->rx_len - (அளவு(u32) * args->rx_endian_சொற்கள்));
	}

	/* Update the head/read index */
	*qctx->headptr = cpu_to_le32(headidx + 1) % qctx->இலக்கம்_slots;

	/* Make sure updates to head are immediately visible to PuC */
	smp_wmb();

	திரும்பு SBI_OK;
}

நிலையான முழு __smq_tx(கட்டமைப்பு smq_queue_ctx *qctx, கட்டமைப்பு rpmi_mb_regs *mb_regs,
		    u32 a2p_doorbell_மதிப்பு, u32 slot_size, u32 service_group_id,
		    கட்டமைப்பு mbox_xfer *xfer) MUST_HOLD(&qctx->queue_lock)
{
	u32 i, tailidx;
	வெறுமை *dst, *src;
	கட்டமைப்பு rpmi_message_header header = { 0 };
	கட்டமைப்பு rpmi_message_args *args = xfer->args;

	/* Tx sanity checks */
	எனில் ((அளவு(u32) * args->tx_endian_சொற்கள்) >
	    (slot_size - அளவு(கட்டமைப்பு rpmi_message_header)))
		திரும்பு SBI_EINVAL;
	எனில் ((அளவு(u32) * args->tx_endian_சொற்கள்) > xfer->tx_len)
		திரும்பு SBI_EINVAL;

	/* There should be some room in the queue */
	எனில் (__smq_queue_full(qctx))
		திரும்பு SBI_ENOMEM;

	/* Get the tail/write index */
	tailidx = le32_to_cpu(*qctx->tailptr);

	/* Prepare the header to be written into the slot */
	header.servicegroup_id = cpu_to_le16(service_group_id);
	header.service_id = args->service_id;
	header.flags = args->type;
	header.datalen = cpu_to_le16((u16)xfer->tx_len);
	header.token = cpu_to_le16((u16)xfer->seq);

	/* Write header into the slot */
	dst = (எழுத்து *)qctx->buffer + (tailidx * slot_size);
	sbi_memcpy(dst, &header, அளவு(header));
	dst += அளவு(header);

	/* Write data into the slot */
	எனில் (xfer->tx) {
		src = xfer->tx;
		க்கு (i = 0; i < args->tx_endian_சொற்கள்; i++)
			((u32 *)dst)[i] = cpu_to_le32(((u32 *)src)[i]);
		dst += அளவு(u32) * args->tx_endian_சொற்கள்;
		src += அளவு(u32) * args->tx_endian_சொற்கள்;
		sbi_memcpy(dst, src,
			xfer->tx_len - (அளவு(u32) * args->tx_endian_சொற்கள்));
	}

	/* Make sure queue chanages are visible to PuC before updating tail */
	smp_wmb();

	/* Update the tail/write index */
	*qctx->tailptr = cpu_to_le32(tailidx + 1) % qctx->இலக்கம்_slots;

	/* Ring the RPMI doorbell if present */
	எனில் (mb_regs)
		writel(a2p_doorbell_மதிப்பு, &mb_regs->db_reg);

	திரும்பு SBI_OK;
}

நிலையான முழு smq_rx(கட்டமைப்பு rpmi_shmem_mbox_controller *mctl,
		  u32 queue_id, u32 service_group_id, கட்டமைப்பு mbox_xfer *xfer)
{
	முழு ret, rxretry = 0;
	கட்டமைப்பு smq_queue_ctx *qctx;

	எனில் (mctl->queue_மொத்தம் < queue_id) {
		sbi_printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		திரும்பு SBI_EINVAL;
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
	செய் {
		spin_lock(&qctx->queue_lock);
		ret = __smq_rx(qctx, mctl->slot_size, service_group_id, xfer);
		spin_unlock(&qctx->queue_lock);
		எனில் (!ret)
			திரும்பு 0;

		sbi_timer_mdelay(1);
		rxretry += 1;
	} போது (rxretry < xfer->rx_timeout);

	திரும்பு SBI_ETIMEDOUT;
}

நிலையான முழு smq_tx(கட்டமைப்பு rpmi_shmem_mbox_controller *mctl,
		  u32 queue_id, u32 service_group_id, கட்டமைப்பு mbox_xfer *xfer)
{
	முழு ret, txretry = 0;
	கட்டமைப்பு smq_queue_ctx *qctx;

	எனில் (mctl->queue_மொத்தம் < queue_id) {
		sbi_printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		திரும்பு SBI_EINVAL;
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
	செய் {
		spin_lock(&qctx->queue_lock);
		ret = __smq_tx(qctx, mctl->mb_regs, mctl->a2p_doorbell_மதிப்பு,
				mctl->slot_size, service_group_id, xfer);
		spin_unlock(&qctx->queue_lock);
		எனில் (!ret)
			திரும்பு 0;

		sbi_timer_mdelay(1);
		txretry += 1;
	} போது (txretry < xfer->tx_timeout);

	திரும்பு SBI_ETIMEDOUT;
}

நிலையான முழு rpmi_get_platform_info(கட்டமைப்பு rpmi_shmem_mbox_controller *mctl)
{
	முழு ret = SBI_OK;

	/**
	 * platform string can occupy max possible size
	 * max possible space in the message data as
	 * per the format
	 */
	கட்டமைப்பு rpmi_base_get_platform_info_resp *resp =
			sbi_zalloc(RPMI_செய்தி_DATA_SIZE(mctl->slot_size));
	எனில் (!resp)
		திரும்பு SBI_ENOMEM;

	ret = rpmi_normal_request_with_status(mctl->base_chan,
				 RPMI_BASE_SRV_GET_PLATFORM_INFO,
				 வெற்று, 0, 0,
				 resp,
				 RPMI_செய்தி_DATA_SIZE(mctl->slot_size)/4,
				 RPMI_செய்தி_DATA_SIZE(mctl->slot_size)/4);
	எனில் (ret)
		செல் fail_free_resp;

	mctl->plat_info_len = resp->plat_info_len;
	mctl->plat_info = sbi_zalloc(mctl->plat_info_len);
	எனில் (!mctl->plat_info) {
		ret = SBI_ENOMEM;
		செல் fail_free_resp;
	}

	sbi_strncpy(mctl->plat_info, resp->plat_info, mctl->plat_info_len);

fail_free_resp:
	sbi_free(resp);
	திரும்பு ret;
}

நிலையான முழு smq_base_get_two_u32(கட்டமைப்பு rpmi_shmem_mbox_controller *mctl,
				u32 service_id, u32 *inarg, u32 *outvals)
{
	திரும்பு rpmi_normal_request_with_status(
			mctl->base_chan, service_id,
			inarg, (inarg) ? 1 : 0, (inarg) ? 1 : 0,
			outvals, 2, 2);
}

/**************** Mailbox Controller Functions **************/

நிலையான முழு rpmi_shmem_mbox_xfer(கட்டமைப்பு mbox_chan *chan, கட்டமைப்பு mbox_xfer *xfer)
{
	முழு ret;
	u32 tx_qid = 0, rx_qid = 0;
	கட்டமைப்பு rpmi_shmem_mbox_controller *mctl =
			container_of(chan->mbox,
				     கட்டமைப்பு rpmi_shmem_mbox_controller,
				     controller);
	கட்டமைப்பு rpmi_srvgrp_chan *srvgrp_chan = to_srvgrp_chan(chan);

	கட்டமைப்பு rpmi_message_args *args = xfer->args;
	bool do_tx = (args->flags & RPMI_செய்தி_FLAGS_NO_TX) ? false : true;
	bool do_rx = (args->flags & RPMI_செய்தி_FLAGS_NO_RX) ? false : true;

	எனில் (!do_tx && !do_rx)
		திரும்பு SBI_EINVAL;

	தேர்வு (args->type) {
	நிலை RPMI_செய்தி_NORMAL_REQUEST:
		எனில் (do_tx && do_rx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
			rx_qid = RPMI_QUEUE_IDX_P2A_ACK;
		} இல்லையெனில் எனில் (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
		} இல்லையெனில் எனில் (do_rx) {
			rx_qid = RPMI_QUEUE_IDX_P2A_REQ;
		}
		நிறுத்து;
	நிலை RPMI_செய்தி_POSTED_REQUEST:
		எனில் (do_tx && do_rx)
			திரும்பு SBI_EINVAL;
		எனில் (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
		} இல்லையெனில் {
			rx_qid = RPMI_QUEUE_IDX_P2A_REQ;
		}
		நிறுத்து;
	நிலை RPMI_செய்தி_ACKNOWLDGEMENT:
		எனில் (do_tx && do_rx)
			திரும்பு SBI_EINVAL;
		எனில் (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_ACK;
		} இல்லையெனில் {
			rx_qid = RPMI_QUEUE_IDX_P2A_ACK;
		}
		நிறுத்து;
	இயல்பு:
		திரும்பு SBI_ENOTSUPP;
	}

	எனில் (do_tx) {
		ret = smq_tx(mctl, tx_qid, srvgrp_chan->servicegroup_id, xfer);
		எனில் (ret)
			திரும்பு ret;
	}

	எனில் (do_rx) {
		ret = smq_rx(mctl, rx_qid, srvgrp_chan->servicegroup_id, xfer);
		எனில் (ret)
			திரும்பு ret;
	}

	திரும்பு 0;
}

நிலையான முழு rpmi_shmem_mbox_get_attribute(கட்டமைப்பு mbox_chan *chan,
					 முழு attr_id, வெறுமை *out_மதிப்பு)
{
	கட்டமைப்பு rpmi_shmem_mbox_controller *mctl =
			container_of(chan->mbox,
				     கட்டமைப்பு rpmi_shmem_mbox_controller,
				     controller);
	கட்டமைப்பு rpmi_srvgrp_chan *srvgrp_chan = to_srvgrp_chan(chan);

	தேர்வு (attr_id) {
	நிலை RPMI_CHANNEL_ATTR_PROTOCOL_VERSION:
		*((u32 *)out_மதிப்பு) = mctl->spec_version;
		நிறுத்து;
	நிலை RPMI_CHANNEL_ATTR_MAX_DATA_LEN:
		*((u32 *)out_மதிப்பு) = RPMI_செய்தி_DATA_SIZE(mctl->slot_size);
		நிறுத்து;
	நிலை RPMI_CHANNEL_ATTR_P2A_DOORBELL_SYSMSI_INDEX:
		*((u32 *)out_மதிப்பு) = mctl->p2a_doorbell_sysmsi_index;
		நிறுத்து;
	நிலை RPMI_CHANNEL_ATTR_TX_TIMEOUT:
		*((u32 *)out_மதிப்பு) = RPMI_DEF_TX_TIMEOUT;
		நிறுத்து;
	நிலை RPMI_CHANNEL_ATTR_RX_TIMEOUT:
		*((u32 *)out_மதிப்பு) = RPMI_DEF_RX_TIMEOUT;
		நிறுத்து;
	நிலை RPMI_CHANNEL_ATTR_SERVICEGROUP_ID:
		*((u32 *)out_மதிப்பு) = srvgrp_chan->servicegroup_id;
		நிறுத்து;
	நிலை RPMI_CHANNEL_ATTR_SERVICEGROUP_VERSION:
		*((u32 *)out_மதிப்பு) = srvgrp_chan->servicegroup_version;
		நிறுத்து;
	நிலை RPMI_CHANNEL_ATTR_IMPL_ID:
		*((u32 *)out_மதிப்பு) = mctl->impl_id;
		நிறுத்து;
	நிலை RPMI_CHANNEL_ATTR_IMPL_VERSION:
		*((u32 *)out_மதிப்பு) = mctl->impl_version;
		நிறுத்து;
	இயல்பு:
		திரும்பு SBI_ENOTSUPP;
	}

	திரும்பு 0;
}

நிலையான கட்டமைப்பு mbox_chan *rpmi_shmem_mbox_request_chan(
						கட்டமைப்பு mbox_controller *mbox,
						u32 *chan_args)
{
	முழு ret;
	u32 tval[2] = { 0 };
	கட்டமைப்பு rpmi_srvgrp_chan *srvgrp_chan;
	கட்டமைப்பு rpmi_shmem_mbox_controller *mctl =
			container_of(mbox,
				     கட்டமைப்பு rpmi_shmem_mbox_controller,
				     controller);

	/* Service group id not defined or in reserved range is invalid */
	எனில் (chan_args[0] >= RPMI_SRVGRP_ID_MAX_மொத்தம் &&
		chan_args[0] <= RPMI_SRVGRP_RESERVE_END)
		திரும்பு வெற்று;

	/* Base serivce group is always present so probe other groups */
	எனில் (chan_args[0] != RPMI_SRVGRP_BASE) {
		/* Probe service group */
		ret = smq_base_get_two_u32(mctl,
					   RPMI_BASE_SRV_PROBE_SERVICE_GROUP,
					   chan_args, tval);
		எனில் (ret || !tval[1])
			திரும்பு வெற்று;
	}

	srvgrp_chan = sbi_zalloc(அளவு(*srvgrp_chan));
	எனில் (!srvgrp_chan)
		திரும்பு வெற்று;

	srvgrp_chan->servicegroup_id = chan_args[0];
	srvgrp_chan->servicegroup_version = tval[1];

	திரும்பு &srvgrp_chan->chan;
}

நிலையான வெறுமை rpmi_shmem_mbox_free_chan(கட்டமைப்பு mbox_controller *mbox,
				      கட்டமைப்பு mbox_chan *chan)
{
	கட்டமைப்பு rpmi_srvgrp_chan *srvgrp_chan = to_srvgrp_chan(chan);
	sbi_free(srvgrp_chan);
}

வெளி கட்டமைப்பு fdt_mailbox fdt_mailbox_rpmi_shmem;

நிலையான முழு rpmi_shmem_transport_init(கட்டமைப்பு rpmi_shmem_mbox_controller *mctl,
				     மாறிலி வெறுமை *fdt,
				     முழு nodeoff) NO_THREAD_SAFETY_ANALYSIS
{
	மாறிலி எழுத்து *name;
	மாறிலி fdt32_t *prop;
	முழு மொத்தம், len, ret, qid;
	uint64_t reg_addr, reg_size;
	கட்டமைப்பு smq_queue_ctx *qctx;

	ret = fdt_முனை_check_compatible(fdt, nodeoff,
					"riscv,rpmi-shmem-mbox");
	எனில் (ret)
		திரும்பு ret;

	/* get queue slot size in bytes */
	prop = fdt_getprop(fdt, nodeoff, "riscv,slot-size", &len);
	எனில் (!prop)
		திரும்பு SBI_ENOENT;

	mctl->slot_size = fdt32_to_cpu(*prop);
	எனில் (mctl->slot_size < RPMI_SLOT_SIZE_MIN) {
		sbi_printf("%s: slot_size < mimnum required message size\n",
			   __func__);
		mctl->slot_size = RPMI_SLOT_SIZE_MIN;
	}

	/* get p2a doorbell system MSI index */
	prop = fdt_getprop(fdt, nodeoff, "riscv,p2a-doorbell-sysmsi-index", &len);
	mctl->p2a_doorbell_sysmsi_index = prop ? fdt32_to_cpu(*prop) : -1U;

	/* get a2p doorbell value */
	prop = fdt_getprop(fdt, nodeoff, "riscv,a2p-doorbell-value", &len);
	mctl->a2p_doorbell_மதிப்பு = prop ? fdt32_to_cpu(*prop) : 1;

	/*
	 * queue names count is taken as the number of queues
	 * supported which make it mandatory to provide the
	 * name of the queue.
	 */
	மொத்தம் = fdt_stringlist_மொத்தம்(fdt, nodeoff, "reg-names");
	எனில் (மொத்தம் < 0 ||
	    மொத்தம் > (RPMI_QUEUE_IDX_MAX_மொத்தம் + RPMI_REG_IDX_MAX_மொத்தம்))
		திரும்பு SBI_EINVAL;

	mctl->queue_மொத்தம் = மொத்தம் - RPMI_REG_IDX_MAX_மொத்தம்;

	/* parse all queues and populate queues context structure */
	க்கு (qid = 0; qid < mctl->queue_மொத்தம்; qid++) {
		qctx = &mctl->queue_ctx_tbl[qid];

		/* get each queue share-memory base address and size*/
		ret = fdt_get_முனை_addr_size(fdt, nodeoff, qid,
					     &reg_addr, &reg_size);
		எனில் (ret < 0 || !reg_addr || !reg_size)
			திரும்பு SBI_ENOENT;

		ret = sbi_domain_வேர்_add_memrange(reg_addr, reg_size, reg_size,
						   (SBI_DOMAIN_MEMREGION_MMIO |
						    SBI_DOMAIN_MEMREGION_M_READABLE |
						    SBI_DOMAIN_MEMREGION_M_WRITABLE));
		எனில் (ret)
			திரும்பு ret;

		/* calculate number of slots in each queue */
		qctx->இலக்கம்_slots =
			(reg_size - (mctl->slot_size * RPMI_QUEUE_HEADER_SLOTS)) / mctl->slot_size;

		/* setup queue pointers */
		qctx->headptr = ((வெறுமை *)(unsigned long)reg_addr) +
				RPMI_QUEUE_HEAD_SLOT * mctl->slot_size;
		qctx->tailptr = ((வெறுமை *)(unsigned long)reg_addr) +
				RPMI_QUEUE_TAIL_SLOT * mctl->slot_size;
		qctx->buffer = ((வெறுமை *)(unsigned long)reg_addr) +
				RPMI_QUEUE_HEADER_SLOTS * mctl->slot_size;

		/* get the queue name */
		name = fdt_stringlist_get(fdt, nodeoff, "reg-names",
					  qid, &len);
		எனில் (!name || (name && len < 0))
			திரும்பு len;

		sbi_memcpy(qctx->name, name, len);

		/* store the index as queue_id */
		qctx->queue_id = qid;

		SPIN_LOCK_INIT(qctx->queue_lock);
	}

	/* get the a2p-doorbell property name */
	name = fdt_stringlist_get(fdt, nodeoff, "reg-names", qid, &len);
	எனில் (!name || (name && len < 0))
		திரும்பு len;

	/* fetch doorbell register address*/
	ret = fdt_get_முனை_addr_size(fdt, nodeoff, qid, &reg_addr,
				       &reg_size);
	எனில் (!ret && !(strncmp(name, "a2p-doorbell", strlen("a2p-doorbell")))) {
		mctl->mb_regs = (வெறுமை *)(unsigned long)reg_addr;
		ret = sbi_domain_வேர்_add_memrange(reg_addr, reg_size, reg_size,
						   (SBI_DOMAIN_MEMREGION_MMIO |
						    SBI_DOMAIN_MEMREGION_M_READABLE |
						    SBI_DOMAIN_MEMREGION_M_WRITABLE));
		எனில் (ret)
			திரும்பு ret;
	}

	திரும்பு SBI_SUCCESS;
}

நிலையான முழு rpmi_shmem_mbox_init(மாறிலி வெறுமை *fdt, முழு nodeoff,
				மாறிலி கட்டமைப்பு fdt_match *match)
{
	கட்டமைப்பு rpmi_base_get_attributes_resp resp;
	கட்டமைப்பு rpmi_shmem_mbox_controller *mctl;
	கட்டமைப்பு rpmi_srvgrp_chan *base_srvgrp;
	u32 tval[2], args[1];
	முழு ret = 0;

	mctl = sbi_zalloc(அளவு(*mctl));
	எனில் (!mctl)
		திரும்பு SBI_ENOMEM;

	/* Initialization transport from device tree */
	ret = rpmi_shmem_transport_init(mctl, fdt, nodeoff);
	எனில் (ret)
		செல் fail_free_controller;

	/* Register mailbox controller */
	mctl->controller.id = nodeoff;
	mctl->controller.max_xfer_len =
			mctl->slot_size - அளவு(கட்டமைப்பு rpmi_message_header);
	mctl->controller.driver = &fdt_mailbox_rpmi_shmem;
	mctl->controller.request_chan = rpmi_shmem_mbox_request_chan;
	mctl->controller.free_chan = rpmi_shmem_mbox_free_chan;
	mctl->controller.xfer = rpmi_shmem_mbox_xfer;
	mctl->controller.get_attribute = rpmi_shmem_mbox_get_attribute;
	ret = mbox_controller_add(&mctl->controller);
	எனில் (ret)
		செல் fail_free_controller;

	/* Request base service group channel */
	tval[0] = RPMI_SRVGRP_BASE;
	mctl->base_chan = mbox_controller_request_chan(&mctl->controller,
							tval);
	எனில் (!mctl->base_chan) {
		ret = SBI_ENOENT;
		செல் fail_remove_controller;
	}

	/* Update base service group version */
	base_srvgrp = to_srvgrp_chan(mctl->base_chan);
	args[0] = RPMI_SRVGRP_BASE;
	ret = smq_base_get_two_u32(mctl, RPMI_BASE_SRV_PROBE_SERVICE_GROUP,
				   &args[0], tval);
	எனில் (ret)
		செல் fail_free_chan;
	base_srvgrp->servicegroup_version = tval[1];
	எனில் (base_srvgrp->servicegroup_version < RPMI_BASE_VERSION_MIN) {
		ret = SBI_EINVAL;
		செல் fail_free_chan;
	}

	/* Get implementation id */
	ret = smq_base_get_two_u32(mctl,
				   RPMI_BASE_SRV_GET_IMPLEMENTATION_VERSION,
				   வெற்று, tval);
	எனில் (ret)
		செல் fail_free_chan;
	mctl->impl_version = tval[1];

	/* Get implementation version */
	ret = smq_base_get_two_u32(mctl, RPMI_BASE_SRV_GET_IMPLEMENTATION_IDN,
				   வெற்று, tval);
	எனில் (ret)
		செல் fail_free_chan;
	mctl->impl_id = tval[1];

	/* Get specification version */
	ret = smq_base_get_two_u32(mctl, RPMI_BASE_SRV_GET_SPEC_VERSION,
				   வெற்று, tval);
	எனில் (ret)
		செல் fail_free_chan;
	mctl->spec_version = tval[1];
	எனில் (mctl->spec_version < RPMI_BASE_VERSION_MIN ||
	    mctl->spec_version != base_srvgrp->servicegroup_version) {
		ret = SBI_EINVAL;
		செல் fail_free_chan;
	}

	/* Get optional features implementation flags */
	ret = rpmi_normal_request_with_status(
			mctl->base_chan, RPMI_BASE_SRV_GET_ATTRIBUTES,
			வெற்று, 0, 0,
			&resp, rpmi_u32_மொத்தம்(resp), rpmi_u32_மொத்தம்(resp));
	எனில் (ret)
		செல் fail_free_chan;

	/* 1: M-mode, 0: S-mode */
	mctl->base_flags.f0_priv_level =
			resp.f0 & RPMI_BASE_FLAGS_F0_PRIVILEGE ? 1 : 0;
	/* 1: Supported, 0: Not Supported */
	mctl->base_flags.f0_ev_notif_en =
			resp.f0 & RPMI_BASE_FLAGS_F0_EV_NOTIFY ? 1 : 0;

	/* We only use M-mode RPMI context in OpenSBI */
	எனில் (!mctl->base_flags.f0_priv_level) {
		ret = SBI_ENODEV;
		செல் fail_free_chan;
	}

	/*
	 * Continue without platform information string if not
	 * available or if an error is encountered while fetching
	 */
	rpmi_get_platform_info(mctl);

	திரும்பு 0;

fail_free_chan:
	mbox_controller_free_chan(mctl->base_chan);
fail_remove_controller:
	mbox_controller_remove(&mctl->controller);
fail_free_controller:
	sbi_free(mctl);
	திரும்பு ret;
}

நிலையான மாறிலி கட்டமைப்பு fdt_match rpmi_shmem_mbox_match[] = {
	{ .compatible = "riscv,rpmi-shmem-mbox" },
	{ },
};

கட்டமைப்பு fdt_mailbox fdt_mailbox_rpmi_shmem = {
	.driver = {
		.match_table = rpmi_shmem_mbox_match,
		.init = rpmi_shmem_mbox_init,
	},
	.xlate = fdt_mailbox_simple_xlate,
};
