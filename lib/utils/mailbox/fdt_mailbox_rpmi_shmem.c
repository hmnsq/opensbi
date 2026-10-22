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

#অন্তর্ভাৱ <libfdt.h>
#অন্তর্ভাৱ <sbi/sbi_console.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_heap.h>
#অন্তর্ভাৱ <sbi/sbi_timer.h>
#অন্তর্ভাৱ <sbi/riscv_io.h>
#অন্তর্ভাৱ <sbi/riscv_locks.h>
#অন্তর্ভাৱ <sbi/riscv_asm.h>
#অন্তর্ভাৱ <sbi/riscv_barrier.h>
#অন্তর্ভাৱ <sbi_utils/fdt/fdt_helper.h>
#অন্তর্ভাৱ <sbi_utils/mailbox/mailbox.h>
#অন্তর্ভাৱ <sbi_utils/mailbox/fdt_mailbox.h>
#অন্তর্ভাৱ <sbi_utils/mailbox/rpmi_mailbox.h>

/** Minimum Base group version required */
#define RPMI_BASE_VERSION_MIN		RPMI_VERSION(1, 0)

/**************** RPMI Transport Structures and Macros ***********/

#define GET_SERVICEGROUP_ID(বাৰ্তা)		\
({						\
	সংরচনা rpmi_message *mbuf = বাৰ্তা;	\
	le16_to_cpu(mbuf->header.servicegroup_id);\
})

#define GET_SERVICE_ID(বাৰ্তা)			\
({						\
	সংরচনা rpmi_message *mbuf = বাৰ্তা;	\
	mbuf->header.service_id;		\
})

#define GET_FLAGS(বাৰ্তা)				\
({						\
	সংরচনা rpmi_message *mbuf = বাৰ্তা;	\
	mbuf->header.flags;			\
})

#define GET_MESSAGE_ID(বাৰ্তা)			\
({						\
	সংরচনা rpmi_message *mbuf = বাৰ্তা;	\
	((u32)mbuf->header.flags << (RPMI_বাৰ্তা_FLAGS_OFFSET * 8)) | \
	((u32)mbuf->header.service_id << (RPMI_বাৰ্তা_SERVICE_ID_OFFSET * 8)) | \
	((u32)le16_to_cpu(mbuf->header.servicegroup_id)); \
})

#define MAKE_MESSAGE_ID(__group_id, __service_id, __flags)	\
({						\
	u32 __ret = 0;				\
	__ret |= (u32)(__group_id) << (RPMI_বাৰ্তা_SERVICEGROUP_ID_OFFSET * 8); \
	__ret |= (u32)(__service_id) << (RPMI_বাৰ্তা_SERVICE_ID_OFFSET * 8); \
	__ret |= (u32)(__flags) << (RPMI_বাৰ্তা_FLAGS_OFFSET * 8); \
	__ret;					\
})

#define GET_DLEN(বাৰ্তা)				\
({						\
	সংরচনা rpmi_message *mbuf = বাৰ্তা;	\
	le16_to_cpu(mbuf->header.datalen);	\
})

#define GET_TOKEN(বাৰ্তা)				\
({						\
	সংরচনা rpmi_message *mbuf = বাৰ্তা;	\
	le16_to_cpu(mbuf->header.token);	\
})

#define GET_MESSAGE_TYPE(বাৰ্তা)						\
({									\
	uint8_t flags = *((uint8_t *)বাৰ্তা + RPMI_বাৰ্তা_FLAGS_OFFSET);	\
	((flags & RPMI_বাৰ্তা_FLAGS_TYPE) >> RPMI_বাৰ্তা_FLAGS_TYPE_POS));	\
})

গণনা rpmi_queue_type {
	RPMI_QUEUE_TYPE_REQ = 0,
	RPMI_QUEUE_TYPE_ACK = 1,
};

গণনা rpmi_queue_idx {
	RPMI_QUEUE_IDX_A2P_REQ = 0,
	RPMI_QUEUE_IDX_P2A_ACK = 1,
	RPMI_QUEUE_IDX_P2A_REQ = 2,
	RPMI_QUEUE_IDX_A2P_ACK = 3,
	RPMI_QUEUE_IDX_MAX_গুনতি,
};

গণনা rpmi_reg_idx {
	RPMI_REG_IDX_DB_REG = 0, /* Doorbell register */
	RPMI_REG_IDX_MAX_গুনতি,
};

/** Mailbox registers */
সংরচনা rpmi_mb_regs {
	/* doorbell from AP -> PuC*/
	চঞ্চল le32_t db_reg;
} __packed;

/** Single Queue Context Structure */
সংরচনা smq_queue_ctx {
	u32 queue_id;
	u32 অঙ্ক_slots;
	spinlock_t queue_lock;
	/* Type of queue - REQ or ACK */
	গণনা rpmi_queue_type queue_type;
	/* Pointers to the queue shared memory */
	চঞ্চল le32_t *headptr PT_GUARDED_BY(&queue_lock);
	চঞ্চল le32_t *tailptr PT_GUARDED_BY(&queue_lock);
	চঞ্চল uint8_t *buffer PT_GUARDED_BY(&queue_lock);
	/* Name of the queue */
	ৱর্ণ name[RPMI_NAME_CHARS_MAX];
};

সংরচনা rpmi_srvgrp_chan {
	u32 servicegroup_id;
	u32 servicegroup_version;
	সংরচনা mbox_chan chan;
};

#define to_srvgrp_chan(mbox_chan)	\
		container_of(mbox_chan, সংরচনা rpmi_srvgrp_chan, chan);

সংরচনা rpmi_shmem_mbox_controller {
	/* Driver specific members */
	u32 slot_size;
	u32 queue_গুনতি;
	u32 p2a_doorbell_sysmsi_index;
	u32 a2p_doorbell_মান;
	সংরচনা rpmi_mb_regs *mb_regs;
	সংরচনা smq_queue_ctx queue_ctx_tbl[RPMI_QUEUE_IDX_MAX_গুনতি];
	/* Mailbox framework related members */
	সংরচনা mbox_controller controller;
	সংরচনা mbox_chan *base_chan;
	u32 impl_version;
	u32 impl_id;
	u32 spec_version;
	u32 plat_info_len;
	ৱর্ণ *plat_info;
	সংরচনা {
		u8 f0_priv_level;
		bool f0_ev_notif_en;
	} base_flags;
};

/**************** Shared Memory Queues Helpers **************/

স্থির bool __smq_queue_full(সংরচনা smq_queue_ctx *qctx)
	MUST_HOLD(&qctx->queue_lock)
{
	ঘূৰাই ((le32_to_cpu(*qctx->tailptr) + 1) % qctx->অঙ্ক_slots ==
			le32_to_cpu(*qctx->headptr)) ? true : false;
}

স্থির bool __smq_queue_empty(সংরচনা smq_queue_ctx *qctx)
	MUST_HOLD(&qctx->queue_lock)
{
	ঘূৰাই (le32_to_cpu(*qctx->headptr) ==
		le32_to_cpu(*qctx->tailptr)) ? true : false;
}

স্থির পূর্ণাঙ্ক __smq_rx(সংরচনা smq_queue_ctx *qctx, u32 slot_size,
		    u32 service_group_id, সংরচনা mbox_xfer *xfer)
	MUST_HOLD(&qctx->queue_lock)
{
	শূন্য *dst, *src;
	সংরচনা rpmi_message *বাৰ্তা;
	u32 i, tmp, pos, dlen, msgidn, headidx, tailidx;
	সংরচনা rpmi_message_args *args = xfer->args;
	bool no_rx_token = (args->flags & RPMI_বাৰ্তা_FLAGS_NO_RX_TOKEN) ?
			   true : false;

	/* Rx sanity checks */
	যদি ((পরিমাণ(u32) * args->rx_endian_শব্দগুলি) >
	    (slot_size - পরিমাণ(সংরচনা rpmi_message_header)))
		ঘূৰাই SBI_EINVAL;
	যদি ((পরিমাণ(u32) * args->rx_endian_শব্দগুলি) > xfer->rx_len)
		ঘূৰাই SBI_EINVAL;

	/* There should be some message in the queue */
	যদি (__smq_queue_empty(qctx))
		ঘূৰাই SBI_ENOENT;

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
	যেতিয়ালৈ (pos != tailidx) {
		src = (শূন্য *)qctx->buffer + (pos * slot_size);
		যদি ((no_rx_token && GET_MESSAGE_ID(src) == msgidn) ||
		    (GET_TOKEN(src) == (xfer->seq & RPMI_বাৰ্তা_TOKEN_MASK)))
			ভঙ্গ;
		pos = (pos + 1) % qctx->অঙ্ক_slots;
	}
	যদি (pos == tailidx)
		ঘূৰাই SBI_ENOENT;

	/* If Rx message is not first message then make it first message */
	যদি (pos != headidx) {
		src = (শূন্য *)qctx->buffer + (pos * slot_size);
		dst = (শূন্য *)qctx->buffer + (headidx * slot_size);
		বাবে (i = 0; i < slot_size / পরিমাণ(u32); i++) {
			tmp = ((u32 *)dst)[i];
			((u32 *)dst)[i] = ((u32 *)src)[i];
			((u32 *)src)[i] = tmp;
		}
	}

	/* Update rx_token if not available */
	বাৰ্তা = (শূন্য *)qctx->buffer + (headidx * slot_size);
	যদি (no_rx_token)
		args->rx_token = GET_TOKEN(বাৰ্তা);

	/* Extract data from the first message */
	যদি (xfer->rx) {
		args->rx_data_len = dlen = GET_DLEN(বাৰ্তা);
		যদি (dlen > xfer->rx_len)
			dlen = xfer->rx_len;
		src = (শূন্য *)বাৰ্তা + পরিমাণ(সংরচনা rpmi_message_header);
		dst = xfer->rx;
		বাবে (i = 0; i < args->rx_endian_শব্দগুলি; i++)
			((u32 *)dst)[i] = le32_to_cpu(((u32 *)src)[i]);
		dst += পরিমাণ(u32) * args->rx_endian_শব্দগুলি;
		src += পরিমাণ(u32) * args->rx_endian_শব্দগুলি;
		sbi_memcpy(dst, src,
			xfer->rx_len - (পরিমাণ(u32) * args->rx_endian_শব্দগুলি));
	}

	/* Update the head/read index */
	*qctx->headptr = cpu_to_le32(headidx + 1) % qctx->অঙ্ক_slots;

	/* Make sure updates to head are immediately visible to PuC */
	smp_wmb();

	ঘূৰাই SBI_OK;
}

স্থির পূর্ণাঙ্ক __smq_tx(সংরচনা smq_queue_ctx *qctx, সংরচনা rpmi_mb_regs *mb_regs,
		    u32 a2p_doorbell_মান, u32 slot_size, u32 service_group_id,
		    সংরচনা mbox_xfer *xfer) MUST_HOLD(&qctx->queue_lock)
{
	u32 i, tailidx;
	শূন্য *dst, *src;
	সংরচনা rpmi_message_header header = { 0 };
	সংরচনা rpmi_message_args *args = xfer->args;

	/* Tx sanity checks */
	যদি ((পরিমাণ(u32) * args->tx_endian_শব্দগুলি) >
	    (slot_size - পরিমাণ(সংরচনা rpmi_message_header)))
		ঘূৰাই SBI_EINVAL;
	যদি ((পরিমাণ(u32) * args->tx_endian_শব্দগুলি) > xfer->tx_len)
		ঘূৰাই SBI_EINVAL;

	/* There should be some room in the queue */
	যদি (__smq_queue_full(qctx))
		ঘূৰাই SBI_ENOMEM;

	/* Get the tail/write index */
	tailidx = le32_to_cpu(*qctx->tailptr);

	/* Prepare the header to be written into the slot */
	header.servicegroup_id = cpu_to_le16(service_group_id);
	header.service_id = args->service_id;
	header.flags = args->type;
	header.datalen = cpu_to_le16((u16)xfer->tx_len);
	header.token = cpu_to_le16((u16)xfer->seq);

	/* Write header into the slot */
	dst = (ৱর্ণ *)qctx->buffer + (tailidx * slot_size);
	sbi_memcpy(dst, &header, পরিমাণ(header));
	dst += পরিমাণ(header);

	/* Write data into the slot */
	যদি (xfer->tx) {
		src = xfer->tx;
		বাবে (i = 0; i < args->tx_endian_শব্দগুলি; i++)
			((u32 *)dst)[i] = cpu_to_le32(((u32 *)src)[i]);
		dst += পরিমাণ(u32) * args->tx_endian_শব্দগুলি;
		src += পরিমাণ(u32) * args->tx_endian_শব্দগুলি;
		sbi_memcpy(dst, src,
			xfer->tx_len - (পরিমাণ(u32) * args->tx_endian_শব্দগুলি));
	}

	/* Make sure queue chanages are visible to PuC before updating tail */
	smp_wmb();

	/* Update the tail/write index */
	*qctx->tailptr = cpu_to_le32(tailidx + 1) % qctx->অঙ্ক_slots;

	/* Ring the RPMI doorbell if present */
	যদি (mb_regs)
		writel(a2p_doorbell_মান, &mb_regs->db_reg);

	ঘূৰাই SBI_OK;
}

স্থির পূর্ণাঙ্ক smq_rx(সংরচনা rpmi_shmem_mbox_controller *mctl,
		  u32 queue_id, u32 service_group_id, সংরচনা mbox_xfer *xfer)
{
	পূর্ণাঙ্ক ret, rxretry = 0;
	সংরচনা smq_queue_ctx *qctx;

	যদি (mctl->queue_গুনতি < queue_id) {
		sbi_printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		ঘূৰাই SBI_EINVAL;
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
	কৰা {
		spin_lock(&qctx->queue_lock);
		ret = __smq_rx(qctx, mctl->slot_size, service_group_id, xfer);
		spin_unlock(&qctx->queue_lock);
		যদি (!ret)
			ঘূৰাই 0;

		sbi_timer_mdelay(1);
		rxretry += 1;
	} যেতিয়ালৈ (rxretry < xfer->rx_timeout);

	ঘূৰাই SBI_ETIMEDOUT;
}

স্থির পূর্ণাঙ্ক smq_tx(সংরচনা rpmi_shmem_mbox_controller *mctl,
		  u32 queue_id, u32 service_group_id, সংরচনা mbox_xfer *xfer)
{
	পূর্ণাঙ্ক ret, txretry = 0;
	সংরচনা smq_queue_ctx *qctx;

	যদি (mctl->queue_গুনতি < queue_id) {
		sbi_printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		ঘূৰাই SBI_EINVAL;
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
	কৰা {
		spin_lock(&qctx->queue_lock);
		ret = __smq_tx(qctx, mctl->mb_regs, mctl->a2p_doorbell_মান,
				mctl->slot_size, service_group_id, xfer);
		spin_unlock(&qctx->queue_lock);
		যদি (!ret)
			ঘূৰাই 0;

		sbi_timer_mdelay(1);
		txretry += 1;
	} যেতিয়ালৈ (txretry < xfer->tx_timeout);

	ঘূৰাই SBI_ETIMEDOUT;
}

স্থির পূর্ণাঙ্ক rpmi_get_platform_info(সংরচনা rpmi_shmem_mbox_controller *mctl)
{
	পূর্ণাঙ্ক ret = SBI_OK;

	/**
	 * platform string can occupy max possible size
	 * max possible space in the message data as
	 * per the format
	 */
	সংরচনা rpmi_base_get_platform_info_resp *resp =
			sbi_zalloc(RPMI_বাৰ্তা_DATA_SIZE(mctl->slot_size));
	যদি (!resp)
		ঘূৰাই SBI_ENOMEM;

	ret = rpmi_normal_request_with_status(mctl->base_chan,
				 RPMI_BASE_SRV_GET_PLATFORM_INFO,
				 রিক্ত, 0, 0,
				 resp,
				 RPMI_বাৰ্তা_DATA_SIZE(mctl->slot_size)/4,
				 RPMI_বাৰ্তা_DATA_SIZE(mctl->slot_size)/4);
	যদি (ret)
		গচ্ছ fail_free_resp;

	mctl->plat_info_len = resp->plat_info_len;
	mctl->plat_info = sbi_zalloc(mctl->plat_info_len);
	যদি (!mctl->plat_info) {
		ret = SBI_ENOMEM;
		গচ্ছ fail_free_resp;
	}

	sbi_strncpy(mctl->plat_info, resp->plat_info, mctl->plat_info_len);

fail_free_resp:
	sbi_free(resp);
	ঘূৰাই ret;
}

স্থির পূর্ণাঙ্ক smq_base_get_two_u32(সংরচনা rpmi_shmem_mbox_controller *mctl,
				u32 service_id, u32 *inarg, u32 *outvals)
{
	ঘূৰাই rpmi_normal_request_with_status(
			mctl->base_chan, service_id,
			inarg, (inarg) ? 1 : 0, (inarg) ? 1 : 0,
			outvals, 2, 2);
}

/**************** Mailbox Controller Functions **************/

স্থির পূর্ণাঙ্ক rpmi_shmem_mbox_xfer(সংরচনা mbox_chan *chan, সংরচনা mbox_xfer *xfer)
{
	পূর্ণাঙ্ক ret;
	u32 tx_qid = 0, rx_qid = 0;
	সংরচনা rpmi_shmem_mbox_controller *mctl =
			container_of(chan->mbox,
				     সংরচনা rpmi_shmem_mbox_controller,
				     controller);
	সংরচনা rpmi_srvgrp_chan *srvgrp_chan = to_srvgrp_chan(chan);

	সংরচনা rpmi_message_args *args = xfer->args;
	bool do_tx = (args->flags & RPMI_বাৰ্তা_FLAGS_NO_TX) ? false : true;
	bool do_rx = (args->flags & RPMI_বাৰ্তা_FLAGS_NO_RX) ? false : true;

	যদি (!do_tx && !do_rx)
		ঘূৰাই SBI_EINVAL;

	ৱিকল্প (args->type) {
	পক্ষ RPMI_বাৰ্তা_NORMAL_REQUEST:
		যদি (do_tx && do_rx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
			rx_qid = RPMI_QUEUE_IDX_P2A_ACK;
		} অন্যথা যদি (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
		} অন্যথা যদি (do_rx) {
			rx_qid = RPMI_QUEUE_IDX_P2A_REQ;
		}
		ভঙ্গ;
	পক্ষ RPMI_বাৰ্তা_POSTED_REQUEST:
		যদি (do_tx && do_rx)
			ঘূৰাই SBI_EINVAL;
		যদি (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
		} অন্যথা {
			rx_qid = RPMI_QUEUE_IDX_P2A_REQ;
		}
		ভঙ্গ;
	পক্ষ RPMI_বাৰ্তা_ACKNOWLDGEMENT:
		যদি (do_tx && do_rx)
			ঘূৰাই SBI_EINVAL;
		যদি (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_ACK;
		} অন্যথা {
			rx_qid = RPMI_QUEUE_IDX_P2A_ACK;
		}
		ভঙ্গ;
	স্ৱভাৱ:
		ঘূৰাই SBI_ENOTSUPP;
	}

	যদি (do_tx) {
		ret = smq_tx(mctl, tx_qid, srvgrp_chan->servicegroup_id, xfer);
		যদি (ret)
			ঘূৰাই ret;
	}

	যদি (do_rx) {
		ret = smq_rx(mctl, rx_qid, srvgrp_chan->servicegroup_id, xfer);
		যদি (ret)
			ঘূৰাই ret;
	}

	ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক rpmi_shmem_mbox_get_attribute(সংরচনা mbox_chan *chan,
					 পূর্ণাঙ্ক attr_id, শূন্য *out_মান)
{
	সংরচনা rpmi_shmem_mbox_controller *mctl =
			container_of(chan->mbox,
				     সংরচনা rpmi_shmem_mbox_controller,
				     controller);
	সংরচনা rpmi_srvgrp_chan *srvgrp_chan = to_srvgrp_chan(chan);

	ৱিকল্প (attr_id) {
	পক্ষ RPMI_CHANNEL_ATTR_PROTOCOL_VERSION:
		*((u32 *)out_মান) = mctl->spec_version;
		ভঙ্গ;
	পক্ষ RPMI_CHANNEL_ATTR_MAX_DATA_LEN:
		*((u32 *)out_মান) = RPMI_বাৰ্তা_DATA_SIZE(mctl->slot_size);
		ভঙ্গ;
	পক্ষ RPMI_CHANNEL_ATTR_P2A_DOORBELL_SYSMSI_INDEX:
		*((u32 *)out_মান) = mctl->p2a_doorbell_sysmsi_index;
		ভঙ্গ;
	পক্ষ RPMI_CHANNEL_ATTR_TX_TIMEOUT:
		*((u32 *)out_মান) = RPMI_DEF_TX_TIMEOUT;
		ভঙ্গ;
	পক্ষ RPMI_CHANNEL_ATTR_RX_TIMEOUT:
		*((u32 *)out_মান) = RPMI_DEF_RX_TIMEOUT;
		ভঙ্গ;
	পক্ষ RPMI_CHANNEL_ATTR_SERVICEGROUP_ID:
		*((u32 *)out_মান) = srvgrp_chan->servicegroup_id;
		ভঙ্গ;
	পক্ষ RPMI_CHANNEL_ATTR_SERVICEGROUP_VERSION:
		*((u32 *)out_মান) = srvgrp_chan->servicegroup_version;
		ভঙ্গ;
	পক্ষ RPMI_CHANNEL_ATTR_IMPL_ID:
		*((u32 *)out_মান) = mctl->impl_id;
		ভঙ্গ;
	পক্ষ RPMI_CHANNEL_ATTR_IMPL_VERSION:
		*((u32 *)out_মান) = mctl->impl_version;
		ভঙ্গ;
	স্ৱভাৱ:
		ঘূৰাই SBI_ENOTSUPP;
	}

	ঘূৰাই 0;
}

স্থির সংরচনা mbox_chan *rpmi_shmem_mbox_request_chan(
						সংরচনা mbox_controller *mbox,
						u32 *chan_args)
{
	পূর্ণাঙ্ক ret;
	u32 tval[2] = { 0 };
	সংরচনা rpmi_srvgrp_chan *srvgrp_chan;
	সংরচনা rpmi_shmem_mbox_controller *mctl =
			container_of(mbox,
				     সংরচনা rpmi_shmem_mbox_controller,
				     controller);

	/* Service group id not defined or in reserved range is invalid */
	যদি (chan_args[0] >= RPMI_SRVGRP_ID_MAX_গুনতি &&
		chan_args[0] <= RPMI_SRVGRP_RESERVE_END)
		ঘূৰাই রিক্ত;

	/* Base serivce group is always present so probe other groups */
	যদি (chan_args[0] != RPMI_SRVGRP_BASE) {
		/* Probe service group */
		ret = smq_base_get_two_u32(mctl,
					   RPMI_BASE_SRV_PROBE_SERVICE_GROUP,
					   chan_args, tval);
		যদি (ret || !tval[1])
			ঘূৰাই রিক্ত;
	}

	srvgrp_chan = sbi_zalloc(পরিমাণ(*srvgrp_chan));
	যদি (!srvgrp_chan)
		ঘূৰাই রিক্ত;

	srvgrp_chan->servicegroup_id = chan_args[0];
	srvgrp_chan->servicegroup_version = tval[1];

	ঘূৰাই &srvgrp_chan->chan;
}

স্থির শূন্য rpmi_shmem_mbox_free_chan(সংরচনা mbox_controller *mbox,
				      সংরচনা mbox_chan *chan)
{
	সংরচনা rpmi_srvgrp_chan *srvgrp_chan = to_srvgrp_chan(chan);
	sbi_free(srvgrp_chan);
}

বাহ্য সংরচনা fdt_mailbox fdt_mailbox_rpmi_shmem;

স্থির পূর্ণাঙ্ক rpmi_shmem_transport_init(সংরচনা rpmi_shmem_mbox_controller *mctl,
				     ধ্রুৱ শূন্য *fdt,
				     পূর্ণাঙ্ক nodeoff) NO_THREAD_SAFETY_ANALYSIS
{
	ধ্রুৱ ৱর্ণ *name;
	ধ্রুৱ fdt32_t *prop;
	পূর্ণাঙ্ক গুনতি, len, ret, qid;
	uint64_t reg_addr, reg_size;
	সংরচনা smq_queue_ctx *qctx;

	ret = fdt_গ্ৰন্থি_check_compatible(fdt, nodeoff,
					"riscv,rpmi-shmem-mbox");
	যদি (ret)
		ঘূৰাই ret;

	/* get queue slot size in bytes */
	prop = fdt_getprop(fdt, nodeoff, "riscv,slot-size", &len);
	যদি (!prop)
		ঘূৰাই SBI_ENOENT;

	mctl->slot_size = fdt32_to_cpu(*prop);
	যদি (mctl->slot_size < RPMI_SLOT_SIZE_MIN) {
		sbi_printf("%s: slot_size < mimnum required message size\n",
			   __func__);
		mctl->slot_size = RPMI_SLOT_SIZE_MIN;
	}

	/* get p2a doorbell system MSI index */
	prop = fdt_getprop(fdt, nodeoff, "riscv,p2a-doorbell-sysmsi-index", &len);
	mctl->p2a_doorbell_sysmsi_index = prop ? fdt32_to_cpu(*prop) : -1U;

	/* get a2p doorbell value */
	prop = fdt_getprop(fdt, nodeoff, "riscv,a2p-doorbell-value", &len);
	mctl->a2p_doorbell_মান = prop ? fdt32_to_cpu(*prop) : 1;

	/*
	 * queue names count is taken as the number of queues
	 * supported which make it mandatory to provide the
	 * name of the queue.
	 */
	গুনতি = fdt_stringlist_গুনতি(fdt, nodeoff, "reg-names");
	যদি (গুনতি < 0 ||
	    গুনতি > (RPMI_QUEUE_IDX_MAX_গুনতি + RPMI_REG_IDX_MAX_গুনতি))
		ঘূৰাই SBI_EINVAL;

	mctl->queue_গুনতি = গুনতি - RPMI_REG_IDX_MAX_গুনতি;

	/* parse all queues and populate queues context structure */
	বাবে (qid = 0; qid < mctl->queue_গুনতি; qid++) {
		qctx = &mctl->queue_ctx_tbl[qid];

		/* get each queue share-memory base address and size*/
		ret = fdt_get_গ্ৰন্থি_addr_size(fdt, nodeoff, qid,
					     &reg_addr, &reg_size);
		যদি (ret < 0 || !reg_addr || !reg_size)
			ঘূৰাই SBI_ENOENT;

		ret = sbi_domain_মূল_add_memrange(reg_addr, reg_size, reg_size,
						   (SBI_DOMAIN_MEMREGION_MMIO |
						    SBI_DOMAIN_MEMREGION_M_READABLE |
						    SBI_DOMAIN_MEMREGION_M_WRITABLE));
		যদি (ret)
			ঘূৰাই ret;

		/* calculate number of slots in each queue */
		qctx->অঙ্ক_slots =
			(reg_size - (mctl->slot_size * RPMI_QUEUE_HEADER_SLOTS)) / mctl->slot_size;

		/* setup queue pointers */
		qctx->headptr = ((শূন্য *)(unsigned long)reg_addr) +
				RPMI_QUEUE_HEAD_SLOT * mctl->slot_size;
		qctx->tailptr = ((শূন্য *)(unsigned long)reg_addr) +
				RPMI_QUEUE_TAIL_SLOT * mctl->slot_size;
		qctx->buffer = ((শূন্য *)(unsigned long)reg_addr) +
				RPMI_QUEUE_HEADER_SLOTS * mctl->slot_size;

		/* get the queue name */
		name = fdt_stringlist_get(fdt, nodeoff, "reg-names",
					  qid, &len);
		যদি (!name || (name && len < 0))
			ঘূৰাই len;

		sbi_memcpy(qctx->name, name, len);

		/* store the index as queue_id */
		qctx->queue_id = qid;

		SPIN_LOCK_INIT(qctx->queue_lock);
	}

	/* get the a2p-doorbell property name */
	name = fdt_stringlist_get(fdt, nodeoff, "reg-names", qid, &len);
	যদি (!name || (name && len < 0))
		ঘূৰাই len;

	/* fetch doorbell register address*/
	ret = fdt_get_গ্ৰন্থি_addr_size(fdt, nodeoff, qid, &reg_addr,
				       &reg_size);
	যদি (!ret && !(strncmp(name, "a2p-doorbell", strlen("a2p-doorbell")))) {
		mctl->mb_regs = (শূন্য *)(unsigned long)reg_addr;
		ret = sbi_domain_মূল_add_memrange(reg_addr, reg_size, reg_size,
						   (SBI_DOMAIN_MEMREGION_MMIO |
						    SBI_DOMAIN_MEMREGION_M_READABLE |
						    SBI_DOMAIN_MEMREGION_M_WRITABLE));
		যদি (ret)
			ঘূৰাই ret;
	}

	ঘূৰাই SBI_SUCCESS;
}

স্থির পূর্ণাঙ্ক rpmi_shmem_mbox_init(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff,
				ধ্রুৱ সংরচনা fdt_match *match)
{
	সংরচনা rpmi_base_get_attributes_resp resp;
	সংরচনা rpmi_shmem_mbox_controller *mctl;
	সংরচনা rpmi_srvgrp_chan *base_srvgrp;
	u32 tval[2], args[1];
	পূর্ণাঙ্ক ret = 0;

	mctl = sbi_zalloc(পরিমাণ(*mctl));
	যদি (!mctl)
		ঘূৰাই SBI_ENOMEM;

	/* Initialization transport from device tree */
	ret = rpmi_shmem_transport_init(mctl, fdt, nodeoff);
	যদি (ret)
		গচ্ছ fail_free_controller;

	/* Register mailbox controller */
	mctl->controller.id = nodeoff;
	mctl->controller.max_xfer_len =
			mctl->slot_size - পরিমাণ(সংরচনা rpmi_message_header);
	mctl->controller.driver = &fdt_mailbox_rpmi_shmem;
	mctl->controller.request_chan = rpmi_shmem_mbox_request_chan;
	mctl->controller.free_chan = rpmi_shmem_mbox_free_chan;
	mctl->controller.xfer = rpmi_shmem_mbox_xfer;
	mctl->controller.get_attribute = rpmi_shmem_mbox_get_attribute;
	ret = mbox_controller_add(&mctl->controller);
	যদি (ret)
		গচ্ছ fail_free_controller;

	/* Request base service group channel */
	tval[0] = RPMI_SRVGRP_BASE;
	mctl->base_chan = mbox_controller_request_chan(&mctl->controller,
							tval);
	যদি (!mctl->base_chan) {
		ret = SBI_ENOENT;
		গচ্ছ fail_remove_controller;
	}

	/* Update base service group version */
	base_srvgrp = to_srvgrp_chan(mctl->base_chan);
	args[0] = RPMI_SRVGRP_BASE;
	ret = smq_base_get_two_u32(mctl, RPMI_BASE_SRV_PROBE_SERVICE_GROUP,
				   &args[0], tval);
	যদি (ret)
		গচ্ছ fail_free_chan;
	base_srvgrp->servicegroup_version = tval[1];
	যদি (base_srvgrp->servicegroup_version < RPMI_BASE_VERSION_MIN) {
		ret = SBI_EINVAL;
		গচ্ছ fail_free_chan;
	}

	/* Get implementation id */
	ret = smq_base_get_two_u32(mctl,
				   RPMI_BASE_SRV_GET_IMPLEMENTATION_VERSION,
				   রিক্ত, tval);
	যদি (ret)
		গচ্ছ fail_free_chan;
	mctl->impl_version = tval[1];

	/* Get implementation version */
	ret = smq_base_get_two_u32(mctl, RPMI_BASE_SRV_GET_IMPLEMENTATION_IDN,
				   রিক্ত, tval);
	যদি (ret)
		গচ্ছ fail_free_chan;
	mctl->impl_id = tval[1];

	/* Get specification version */
	ret = smq_base_get_two_u32(mctl, RPMI_BASE_SRV_GET_SPEC_VERSION,
				   রিক্ত, tval);
	যদি (ret)
		গচ্ছ fail_free_chan;
	mctl->spec_version = tval[1];
	যদি (mctl->spec_version < RPMI_BASE_VERSION_MIN ||
	    mctl->spec_version != base_srvgrp->servicegroup_version) {
		ret = SBI_EINVAL;
		গচ্ছ fail_free_chan;
	}

	/* Get optional features implementation flags */
	ret = rpmi_normal_request_with_status(
			mctl->base_chan, RPMI_BASE_SRV_GET_ATTRIBUTES,
			রিক্ত, 0, 0,
			&resp, rpmi_u32_গুনতি(resp), rpmi_u32_গুনতি(resp));
	যদি (ret)
		গচ্ছ fail_free_chan;

	/* 1: M-mode, 0: S-mode */
	mctl->base_flags.f0_priv_level =
			resp.f0 & RPMI_BASE_FLAGS_F0_PRIVILEGE ? 1 : 0;
	/* 1: Supported, 0: Not Supported */
	mctl->base_flags.f0_ev_notif_en =
			resp.f0 & RPMI_BASE_FLAGS_F0_EV_NOTIFY ? 1 : 0;

	/* We only use M-mode RPMI context in OpenSBI */
	যদি (!mctl->base_flags.f0_priv_level) {
		ret = SBI_ENODEV;
		গচ্ছ fail_free_chan;
	}

	/*
	 * Continue without platform information string if not
	 * available or if an error is encountered while fetching
	 */
	rpmi_get_platform_info(mctl);

	ঘূৰাই 0;

fail_free_chan:
	mbox_controller_free_chan(mctl->base_chan);
fail_remove_controller:
	mbox_controller_remove(&mctl->controller);
fail_free_controller:
	sbi_free(mctl);
	ঘূৰাই ret;
}

স্থির ধ্রুৱ সংরচনা fdt_match rpmi_shmem_mbox_match[] = {
	{ .compatible = "riscv,rpmi-shmem-mbox" },
	{ },
};

সংরচনা fdt_mailbox fdt_mailbox_rpmi_shmem = {
	.driver = {
		.match_table = rpmi_shmem_mbox_match,
		.init = rpmi_shmem_mbox_init,
	},
	.xlate = fdt_mailbox_simple_xlate,
};
