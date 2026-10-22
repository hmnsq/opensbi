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

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_heap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_timer.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_locks.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_barrier.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/fdt/fdt_helper.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/mailbox/mailbox.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/mailbox/fdt_mailbox.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/mailbox/rpmi_mailbox.h>

/** Minimum Base group version required */
#define RPMI_BASE_VERSION_MIN		RPMI_VERSION(1, 0)

/**************** RPMI Transport Structures and Macros ***********/

#define GET_SERVICEGROUP_ID(ಸಂದೇಶ)		\
({						\
	ಸಂರಚನಾ rpmi_message *mbuf = ಸಂದೇಶ;	\
	le16_to_cpu(mbuf->header.servicegroup_id);\
})

#define GET_SERVICE_ID(ಸಂದೇಶ)			\
({						\
	ಸಂರಚನಾ rpmi_message *mbuf = ಸಂದೇಶ;	\
	mbuf->header.service_id;		\
})

#define GET_FLAGS(ಸಂದೇಶ)				\
({						\
	ಸಂರಚನಾ rpmi_message *mbuf = ಸಂದೇಶ;	\
	mbuf->header.flags;			\
})

#define GET_MESSAGE_ID(ಸಂದೇಶ)			\
({						\
	ಸಂರಚನಾ rpmi_message *mbuf = ಸಂದೇಶ;	\
	((u32)mbuf->header.flags << (RPMI_ಸಂದೇಶ_FLAGS_OFFSET * 8)) | \
	((u32)mbuf->header.service_id << (RPMI_ಸಂದೇಶ_SERVICE_ID_OFFSET * 8)) | \
	((u32)le16_to_cpu(mbuf->header.servicegroup_id)); \
})

#define MAKE_MESSAGE_ID(__group_id, __service_id, __flags)	\
({						\
	u32 __ret = 0;				\
	__ret |= (u32)(__group_id) << (RPMI_ಸಂದೇಶ_SERVICEGROUP_ID_OFFSET * 8); \
	__ret |= (u32)(__service_id) << (RPMI_ಸಂದೇಶ_SERVICE_ID_OFFSET * 8); \
	__ret |= (u32)(__flags) << (RPMI_ಸಂದೇಶ_FLAGS_OFFSET * 8); \
	__ret;					\
})

#define GET_DLEN(ಸಂದೇಶ)				\
({						\
	ಸಂರಚನಾ rpmi_message *mbuf = ಸಂದೇಶ;	\
	le16_to_cpu(mbuf->header.datalen);	\
})

#define GET_TOKEN(ಸಂದೇಶ)				\
({						\
	ಸಂರಚನಾ rpmi_message *mbuf = ಸಂದೇಶ;	\
	le16_to_cpu(mbuf->header.token);	\
})

#define GET_MESSAGE_TYPE(ಸಂದೇಶ)						\
({									\
	uint8_t flags = *((uint8_t *)ಸಂದೇಶ + RPMI_ಸಂದೇಶ_FLAGS_OFFSET);	\
	((flags & RPMI_ಸಂದೇಶ_FLAGS_TYPE) >> RPMI_ಸಂದೇಶ_FLAGS_TYPE_POS));	\
})

ಗಣನಾ rpmi_queue_type {
	RPMI_QUEUE_TYPE_REQ = 0,
	RPMI_QUEUE_TYPE_ACK = 1,
};

ಗಣನಾ rpmi_queue_idx {
	RPMI_QUEUE_IDX_A2P_REQ = 0,
	RPMI_QUEUE_IDX_P2A_ACK = 1,
	RPMI_QUEUE_IDX_P2A_REQ = 2,
	RPMI_QUEUE_IDX_A2P_ACK = 3,
	RPMI_QUEUE_IDX_MAX_ಎಣಿಕೆ,
};

ಗಣನಾ rpmi_reg_idx {
	RPMI_REG_IDX_DB_REG = 0, /* Doorbell register */
	RPMI_REG_IDX_MAX_ಎಣಿಕೆ,
};

/** Mailbox registers */
ಸಂರಚನಾ rpmi_mb_regs {
	/* doorbell from AP -> PuC*/
	ಚಞ್ಚಲ le32_t db_reg;
} __packed;

/** Single Queue Context Structure */
ಸಂರಚನಾ smq_queue_ctx {
	u32 queue_id;
	u32 ಅಂಕೆ_slots;
	spinlock_t queue_lock;
	/* Type of queue - REQ or ACK */
	ಗಣನಾ rpmi_queue_type queue_type;
	/* Pointers to the queue shared memory */
	ಚಞ್ಚಲ le32_t *headptr PT_GUARDED_BY(&queue_lock);
	ಚಞ್ಚಲ le32_t *tailptr PT_GUARDED_BY(&queue_lock);
	ಚಞ್ಚಲ uint8_t *buffer PT_GUARDED_BY(&queue_lock);
	/* Name of the queue */
	ವರ್ಣ name[RPMI_NAME_CHARS_MAX];
};

ಸಂರಚನಾ rpmi_srvgrp_chan {
	u32 servicegroup_id;
	u32 servicegroup_version;
	ಸಂರಚನಾ mbox_chan chan;
};

#define to_srvgrp_chan(mbox_chan)	\
		container_of(mbox_chan, ಸಂರಚನಾ rpmi_srvgrp_chan, chan);

ಸಂರಚನಾ rpmi_shmem_mbox_controller {
	/* Driver specific members */
	u32 slot_size;
	u32 queue_ಎಣಿಕೆ;
	u32 p2a_doorbell_sysmsi_index;
	u32 a2p_doorbell_ಮೌಲ್ಯ;
	ಸಂರಚನಾ rpmi_mb_regs *mb_regs;
	ಸಂರಚನಾ smq_queue_ctx queue_ctx_tbl[RPMI_QUEUE_IDX_MAX_ಎಣಿಕೆ];
	/* Mailbox framework related members */
	ಸಂರಚನಾ mbox_controller controller;
	ಸಂರಚನಾ mbox_chan *base_chan;
	u32 impl_version;
	u32 impl_id;
	u32 spec_version;
	u32 plat_info_len;
	ವರ್ಣ *plat_info;
	ಸಂರಚನಾ {
		u8 f0_priv_level;
		bool f0_ev_notif_en;
	} base_flags;
};

/**************** Shared Memory Queues Helpers **************/

ಸ್ಥಿರ bool __smq_queue_full(ಸಂರಚನಾ smq_queue_ctx *qctx)
	MUST_HOLD(&qctx->queue_lock)
{
	ಪ್ರತ್ಯಾಗಮ ((le32_to_cpu(*qctx->tailptr) + 1) % qctx->ಅಂಕೆ_slots ==
			le32_to_cpu(*qctx->headptr)) ? true : false;
}

ಸ್ಥಿರ bool __smq_queue_empty(ಸಂರಚನಾ smq_queue_ctx *qctx)
	MUST_HOLD(&qctx->queue_lock)
{
	ಪ್ರತ್ಯಾಗಮ (le32_to_cpu(*qctx->headptr) ==
		le32_to_cpu(*qctx->tailptr)) ? true : false;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ __smq_rx(ಸಂರಚನಾ smq_queue_ctx *qctx, u32 slot_size,
		    u32 service_group_id, ಸಂರಚನಾ mbox_xfer *xfer)
	MUST_HOLD(&qctx->queue_lock)
{
	ಶೂನ್ಯ *dst, *src;
	ಸಂರಚನಾ rpmi_message *ಸಂದೇಶ;
	u32 i, tmp, pos, dlen, msgidn, headidx, tailidx;
	ಸಂರಚನಾ rpmi_message_args *args = xfer->args;
	bool no_rx_token = (args->flags & RPMI_ಸಂದೇಶ_FLAGS_NO_RX_TOKEN) ?
			   true : false;

	/* Rx sanity checks */
	ಯದಿ ((ಪರಿಮಾಣ(u32) * args->rx_endian_ಪದಗಳು) >
	    (slot_size - ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_message_header)))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ಯದಿ ((ಪರಿಮಾಣ(u32) * args->rx_endian_ಪದಗಳು) > xfer->rx_len)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	/* There should be some message in the queue */
	ಯದಿ (__smq_queue_empty(qctx))
		ಪ್ರತ್ಯಾಗಮ SBI_ENOENT;

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
	ಯಾವತ್ (pos != tailidx) {
		src = (ಶೂನ್ಯ *)qctx->buffer + (pos * slot_size);
		ಯದಿ ((no_rx_token && GET_MESSAGE_ID(src) == msgidn) ||
		    (GET_TOKEN(src) == (xfer->seq & RPMI_ಸಂದೇಶ_TOKEN_MASK)))
			ಭಙ್ಗ;
		pos = (pos + 1) % qctx->ಅಂಕೆ_slots;
	}
	ಯದಿ (pos == tailidx)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOENT;

	/* If Rx message is not first message then make it first message */
	ಯದಿ (pos != headidx) {
		src = (ಶೂನ್ಯ *)qctx->buffer + (pos * slot_size);
		dst = (ಶೂನ್ಯ *)qctx->buffer + (headidx * slot_size);
		ಪ್ರತಿ (i = 0; i < slot_size / ಪರಿಮಾಣ(u32); i++) {
			tmp = ((u32 *)dst)[i];
			((u32 *)dst)[i] = ((u32 *)src)[i];
			((u32 *)src)[i] = tmp;
		}
	}

	/* Update rx_token if not available */
	ಸಂದೇಶ = (ಶೂನ್ಯ *)qctx->buffer + (headidx * slot_size);
	ಯದಿ (no_rx_token)
		args->rx_token = GET_TOKEN(ಸಂದೇಶ);

	/* Extract data from the first message */
	ಯದಿ (xfer->rx) {
		args->rx_data_len = dlen = GET_DLEN(ಸಂದೇಶ);
		ಯದಿ (dlen > xfer->rx_len)
			dlen = xfer->rx_len;
		src = (ಶೂನ್ಯ *)ಸಂದೇಶ + ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_message_header);
		dst = xfer->rx;
		ಪ್ರತಿ (i = 0; i < args->rx_endian_ಪದಗಳು; i++)
			((u32 *)dst)[i] = le32_to_cpu(((u32 *)src)[i]);
		dst += ಪರಿಮಾಣ(u32) * args->rx_endian_ಪದಗಳು;
		src += ಪರಿಮಾಣ(u32) * args->rx_endian_ಪದಗಳು;
		sbi_memcpy(dst, src,
			xfer->rx_len - (ಪರಿಮಾಣ(u32) * args->rx_endian_ಪದಗಳು));
	}

	/* Update the head/read index */
	*qctx->headptr = cpu_to_le32(headidx + 1) % qctx->ಅಂಕೆ_slots;

	/* Make sure updates to head are immediately visible to PuC */
	smp_wmb();

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ __smq_tx(ಸಂರಚನಾ smq_queue_ctx *qctx, ಸಂರಚನಾ rpmi_mb_regs *mb_regs,
		    u32 a2p_doorbell_ಮೌಲ್ಯ, u32 slot_size, u32 service_group_id,
		    ಸಂರಚನಾ mbox_xfer *xfer) MUST_HOLD(&qctx->queue_lock)
{
	u32 i, tailidx;
	ಶೂನ್ಯ *dst, *src;
	ಸಂರಚನಾ rpmi_message_header header = { 0 };
	ಸಂರಚನಾ rpmi_message_args *args = xfer->args;

	/* Tx sanity checks */
	ಯದಿ ((ಪರಿಮಾಣ(u32) * args->tx_endian_ಪದಗಳು) >
	    (slot_size - ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_message_header)))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ಯದಿ ((ಪರಿಮಾಣ(u32) * args->tx_endian_ಪದಗಳು) > xfer->tx_len)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	/* There should be some room in the queue */
	ಯದಿ (__smq_queue_full(qctx))
		ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

	/* Get the tail/write index */
	tailidx = le32_to_cpu(*qctx->tailptr);

	/* Prepare the header to be written into the slot */
	header.servicegroup_id = cpu_to_le16(service_group_id);
	header.service_id = args->service_id;
	header.flags = args->type;
	header.datalen = cpu_to_le16((u16)xfer->tx_len);
	header.token = cpu_to_le16((u16)xfer->seq);

	/* Write header into the slot */
	dst = (ವರ್ಣ *)qctx->buffer + (tailidx * slot_size);
	sbi_memcpy(dst, &header, ಪರಿಮಾಣ(header));
	dst += ಪರಿಮಾಣ(header);

	/* Write data into the slot */
	ಯದಿ (xfer->tx) {
		src = xfer->tx;
		ಪ್ರತಿ (i = 0; i < args->tx_endian_ಪದಗಳು; i++)
			((u32 *)dst)[i] = cpu_to_le32(((u32 *)src)[i]);
		dst += ಪರಿಮಾಣ(u32) * args->tx_endian_ಪದಗಳು;
		src += ಪರಿಮಾಣ(u32) * args->tx_endian_ಪದಗಳು;
		sbi_memcpy(dst, src,
			xfer->tx_len - (ಪರಿಮಾಣ(u32) * args->tx_endian_ಪದಗಳು));
	}

	/* Make sure queue chanages are visible to PuC before updating tail */
	smp_wmb();

	/* Update the tail/write index */
	*qctx->tailptr = cpu_to_le32(tailidx + 1) % qctx->ಅಂಕೆ_slots;

	/* Ring the RPMI doorbell if present */
	ಯದಿ (mb_regs)
		writel(a2p_doorbell_ಮೌಲ್ಯ, &mb_regs->db_reg);

	ಪ್ರತ್ಯಾಗಮ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ smq_rx(ಸಂರಚನಾ rpmi_shmem_mbox_controller *mctl,
		  u32 queue_id, u32 service_group_id, ಸಂರಚನಾ mbox_xfer *xfer)
{
	ಪೂರ್ಣಾಙ್ಕ ret, rxretry = 0;
	ಸಂರಚನಾ smq_queue_ctx *qctx;

	ಯದಿ (mctl->queue_ಎಣಿಕೆ < queue_id) {
		sbi_printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
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
	ಕುರು {
		spin_lock(&qctx->queue_lock);
		ret = __smq_rx(qctx, mctl->slot_size, service_group_id, xfer);
		spin_unlock(&qctx->queue_lock);
		ಯದಿ (!ret)
			ಪ್ರತ್ಯಾಗಮ 0;

		sbi_timer_mdelay(1);
		rxretry += 1;
	} ಯಾವತ್ (rxretry < xfer->rx_timeout);

	ಪ್ರತ್ಯಾಗಮ SBI_ETIMEDOUT;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ smq_tx(ಸಂರಚನಾ rpmi_shmem_mbox_controller *mctl,
		  u32 queue_id, u32 service_group_id, ಸಂರಚನಾ mbox_xfer *xfer)
{
	ಪೂರ್ಣಾಙ್ಕ ret, txretry = 0;
	ಸಂರಚನಾ smq_queue_ctx *qctx;

	ಯದಿ (mctl->queue_ಎಣಿಕೆ < queue_id) {
		sbi_printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
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
	ಕುರು {
		spin_lock(&qctx->queue_lock);
		ret = __smq_tx(qctx, mctl->mb_regs, mctl->a2p_doorbell_ಮೌಲ್ಯ,
				mctl->slot_size, service_group_id, xfer);
		spin_unlock(&qctx->queue_lock);
		ಯದಿ (!ret)
			ಪ್ರತ್ಯಾಗಮ 0;

		sbi_timer_mdelay(1);
		txretry += 1;
	} ಯಾವತ್ (txretry < xfer->tx_timeout);

	ಪ್ರತ್ಯಾಗಮ SBI_ETIMEDOUT;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rpmi_get_platform_info(ಸಂರಚನಾ rpmi_shmem_mbox_controller *mctl)
{
	ಪೂರ್ಣಾಙ್ಕ ret = SBI_OK;

	/**
	 * platform string can occupy max possible size
	 * max possible space in the message data as
	 * per the format
	 */
	ಸಂರಚನಾ rpmi_base_get_platform_info_resp *resp =
			sbi_zalloc(RPMI_ಸಂದೇಶ_DATA_SIZE(mctl->slot_size));
	ಯದಿ (!resp)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

	ret = rpmi_normal_request_with_status(mctl->base_chan,
				 RPMI_BASE_SRV_GET_PLATFORM_INFO,
				 ರಿಕ್ತ, 0, 0,
				 resp,
				 RPMI_ಸಂದೇಶ_DATA_SIZE(mctl->slot_size)/4,
				 RPMI_ಸಂದೇಶ_DATA_SIZE(mctl->slot_size)/4);
	ಯದಿ (ret)
		ಗಚ್ಛ fail_free_resp;

	mctl->plat_info_len = resp->plat_info_len;
	mctl->plat_info = sbi_zalloc(mctl->plat_info_len);
	ಯದಿ (!mctl->plat_info) {
		ret = SBI_ENOMEM;
		ಗಚ್ಛ fail_free_resp;
	}

	sbi_strncpy(mctl->plat_info, resp->plat_info, mctl->plat_info_len);

fail_free_resp:
	sbi_free(resp);
	ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ smq_base_get_two_u32(ಸಂರಚನಾ rpmi_shmem_mbox_controller *mctl,
				u32 service_id, u32 *inarg, u32 *outvals)
{
	ಪ್ರತ್ಯಾಗಮ rpmi_normal_request_with_status(
			mctl->base_chan, service_id,
			inarg, (inarg) ? 1 : 0, (inarg) ? 1 : 0,
			outvals, 2, 2);
}

/**************** Mailbox Controller Functions **************/

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rpmi_shmem_mbox_xfer(ಸಂರಚನಾ mbox_chan *chan, ಸಂರಚನಾ mbox_xfer *xfer)
{
	ಪೂರ್ಣಾಙ್ಕ ret;
	u32 tx_qid = 0, rx_qid = 0;
	ಸಂರಚನಾ rpmi_shmem_mbox_controller *mctl =
			container_of(chan->mbox,
				     ಸಂರಚನಾ rpmi_shmem_mbox_controller,
				     controller);
	ಸಂರಚನಾ rpmi_srvgrp_chan *srvgrp_chan = to_srvgrp_chan(chan);

	ಸಂರಚನಾ rpmi_message_args *args = xfer->args;
	bool do_tx = (args->flags & RPMI_ಸಂದೇಶ_FLAGS_NO_TX) ? false : true;
	bool do_rx = (args->flags & RPMI_ಸಂದೇಶ_FLAGS_NO_RX) ? false : true;

	ಯದಿ (!do_tx && !do_rx)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	ವಿಕಲ್ಪ (args->type) {
	ಪಕ್ಷ RPMI_ಸಂದೇಶ_NORMAL_REQUEST:
		ಯದಿ (do_tx && do_rx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
			rx_qid = RPMI_QUEUE_IDX_P2A_ACK;
		} ಅನ್ಯಥಾ ಯದಿ (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
		} ಅನ್ಯಥಾ ಯದಿ (do_rx) {
			rx_qid = RPMI_QUEUE_IDX_P2A_REQ;
		}
		ಭಙ್ಗ;
	ಪಕ್ಷ RPMI_ಸಂದೇಶ_POSTED_REQUEST:
		ಯದಿ (do_tx && do_rx)
			ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
		ಯದಿ (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
		} ಅನ್ಯಥಾ {
			rx_qid = RPMI_QUEUE_IDX_P2A_REQ;
		}
		ಭಙ್ಗ;
	ಪಕ್ಷ RPMI_ಸಂದೇಶ_ACKNOWLDGEMENT:
		ಯದಿ (do_tx && do_rx)
			ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
		ಯದಿ (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_ACK;
		} ಅನ್ಯಥಾ {
			rx_qid = RPMI_QUEUE_IDX_P2A_ACK;
		}
		ಭಙ್ಗ;
	ಸ್ವಭಾವ:
		ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;
	}

	ಯದಿ (do_tx) {
		ret = smq_tx(mctl, tx_qid, srvgrp_chan->servicegroup_id, xfer);
		ಯದಿ (ret)
			ಪ್ರತ್ಯಾಗಮ ret;
	}

	ಯದಿ (do_rx) {
		ret = smq_rx(mctl, rx_qid, srvgrp_chan->servicegroup_id, xfer);
		ಯದಿ (ret)
			ಪ್ರತ್ಯಾಗಮ ret;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rpmi_shmem_mbox_get_attribute(ಸಂರಚನಾ mbox_chan *chan,
					 ಪೂರ್ಣಾಙ್ಕ attr_id, ಶೂನ್ಯ *out_ಮೌಲ್ಯ)
{
	ಸಂರಚನಾ rpmi_shmem_mbox_controller *mctl =
			container_of(chan->mbox,
				     ಸಂರಚನಾ rpmi_shmem_mbox_controller,
				     controller);
	ಸಂರಚನಾ rpmi_srvgrp_chan *srvgrp_chan = to_srvgrp_chan(chan);

	ವಿಕಲ್ಪ (attr_id) {
	ಪಕ್ಷ RPMI_CHANNEL_ATTR_PROTOCOL_VERSION:
		*((u32 *)out_ಮೌಲ್ಯ) = mctl->spec_version;
		ಭಙ್ಗ;
	ಪಕ್ಷ RPMI_CHANNEL_ATTR_MAX_DATA_LEN:
		*((u32 *)out_ಮೌಲ್ಯ) = RPMI_ಸಂದೇಶ_DATA_SIZE(mctl->slot_size);
		ಭಙ್ಗ;
	ಪಕ್ಷ RPMI_CHANNEL_ATTR_P2A_DOORBELL_SYSMSI_INDEX:
		*((u32 *)out_ಮೌಲ್ಯ) = mctl->p2a_doorbell_sysmsi_index;
		ಭಙ್ಗ;
	ಪಕ್ಷ RPMI_CHANNEL_ATTR_TX_TIMEOUT:
		*((u32 *)out_ಮೌಲ್ಯ) = RPMI_DEF_TX_TIMEOUT;
		ಭಙ್ಗ;
	ಪಕ್ಷ RPMI_CHANNEL_ATTR_RX_TIMEOUT:
		*((u32 *)out_ಮೌಲ್ಯ) = RPMI_DEF_RX_TIMEOUT;
		ಭಙ್ಗ;
	ಪಕ್ಷ RPMI_CHANNEL_ATTR_SERVICEGROUP_ID:
		*((u32 *)out_ಮೌಲ್ಯ) = srvgrp_chan->servicegroup_id;
		ಭಙ್ಗ;
	ಪಕ್ಷ RPMI_CHANNEL_ATTR_SERVICEGROUP_VERSION:
		*((u32 *)out_ಮೌಲ್ಯ) = srvgrp_chan->servicegroup_version;
		ಭಙ್ಗ;
	ಪಕ್ಷ RPMI_CHANNEL_ATTR_IMPL_ID:
		*((u32 *)out_ಮೌಲ್ಯ) = mctl->impl_id;
		ಭಙ್ಗ;
	ಪಕ್ಷ RPMI_CHANNEL_ATTR_IMPL_VERSION:
		*((u32 *)out_ಮೌಲ್ಯ) = mctl->impl_version;
		ಭಙ್ಗ;
	ಸ್ವಭಾವ:
		ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಸಂರಚನಾ mbox_chan *rpmi_shmem_mbox_request_chan(
						ಸಂರಚನಾ mbox_controller *mbox,
						u32 *chan_args)
{
	ಪೂರ್ಣಾಙ್ಕ ret;
	u32 tval[2] = { 0 };
	ಸಂರಚನಾ rpmi_srvgrp_chan *srvgrp_chan;
	ಸಂರಚನಾ rpmi_shmem_mbox_controller *mctl =
			container_of(mbox,
				     ಸಂರಚನಾ rpmi_shmem_mbox_controller,
				     controller);

	/* Service group id not defined or in reserved range is invalid */
	ಯದಿ (chan_args[0] >= RPMI_SRVGRP_ID_MAX_ಎಣಿಕೆ &&
		chan_args[0] <= RPMI_SRVGRP_RESERVE_END)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	/* Base serivce group is always present so probe other groups */
	ಯದಿ (chan_args[0] != RPMI_SRVGRP_BASE) {
		/* Probe service group */
		ret = smq_base_get_two_u32(mctl,
					   RPMI_BASE_SRV_PROBE_SERVICE_GROUP,
					   chan_args, tval);
		ಯದಿ (ret || !tval[1])
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	srvgrp_chan = sbi_zalloc(ಪರಿಮಾಣ(*srvgrp_chan));
	ಯದಿ (!srvgrp_chan)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	srvgrp_chan->servicegroup_id = chan_args[0];
	srvgrp_chan->servicegroup_version = tval[1];

	ಪ್ರತ್ಯಾಗಮ &srvgrp_chan->chan;
}

ಸ್ಥಿರ ಶೂನ್ಯ rpmi_shmem_mbox_free_chan(ಸಂರಚನಾ mbox_controller *mbox,
				      ಸಂರಚನಾ mbox_chan *chan)
{
	ಸಂರಚನಾ rpmi_srvgrp_chan *srvgrp_chan = to_srvgrp_chan(chan);
	sbi_free(srvgrp_chan);
}

ಬಾಹ್ಯ ಸಂರಚನಾ fdt_mailbox fdt_mailbox_rpmi_shmem;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rpmi_shmem_transport_init(ಸಂರಚನಾ rpmi_shmem_mbox_controller *mctl,
				     ಧ್ರುವ ಶೂನ್ಯ *fdt,
				     ಪೂರ್ಣಾಙ್ಕ nodeoff) NO_THREAD_SAFETY_ANALYSIS
{
	ಧ್ರುವ ವರ್ಣ *name;
	ಧ್ರುವ fdt32_t *prop;
	ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ, len, ret, qid;
	uint64_t reg_addr, reg_size;
	ಸಂರಚನಾ smq_queue_ctx *qctx;

	ret = fdt_ಗಂಟು_check_compatible(fdt, nodeoff,
					"riscv,rpmi-shmem-mbox");
	ಯದಿ (ret)
		ಪ್ರತ್ಯಾಗಮ ret;

	/* get queue slot size in bytes */
	prop = fdt_getprop(fdt, nodeoff, "riscv,slot-size", &len);
	ಯದಿ (!prop)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOENT;

	mctl->slot_size = fdt32_to_cpu(*prop);
	ಯದಿ (mctl->slot_size < RPMI_SLOT_SIZE_MIN) {
		sbi_printf("%s: slot_size < mimnum required message size\n",
			   __func__);
		mctl->slot_size = RPMI_SLOT_SIZE_MIN;
	}

	/* get p2a doorbell system MSI index */
	prop = fdt_getprop(fdt, nodeoff, "riscv,p2a-doorbell-sysmsi-index", &len);
	mctl->p2a_doorbell_sysmsi_index = prop ? fdt32_to_cpu(*prop) : -1U;

	/* get a2p doorbell value */
	prop = fdt_getprop(fdt, nodeoff, "riscv,a2p-doorbell-value", &len);
	mctl->a2p_doorbell_ಮೌಲ್ಯ = prop ? fdt32_to_cpu(*prop) : 1;

	/*
	 * queue names count is taken as the number of queues
	 * supported which make it mandatory to provide the
	 * name of the queue.
	 */
	ಎಣಿಕೆ = fdt_stringlist_ಎಣಿಕೆ(fdt, nodeoff, "reg-names");
	ಯದಿ (ಎಣಿಕೆ < 0 ||
	    ಎಣಿಕೆ > (RPMI_QUEUE_IDX_MAX_ಎಣಿಕೆ + RPMI_REG_IDX_MAX_ಎಣಿಕೆ))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	mctl->queue_ಎಣಿಕೆ = ಎಣಿಕೆ - RPMI_REG_IDX_MAX_ಎಣಿಕೆ;

	/* parse all queues and populate queues context structure */
	ಪ್ರತಿ (qid = 0; qid < mctl->queue_ಎಣಿಕೆ; qid++) {
		qctx = &mctl->queue_ctx_tbl[qid];

		/* get each queue share-memory base address and size*/
		ret = fdt_get_ಗಂಟು_addr_size(fdt, nodeoff, qid,
					     &reg_addr, &reg_size);
		ಯದಿ (ret < 0 || !reg_addr || !reg_size)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOENT;

		ret = sbi_domain_ಬೇರು_add_memrange(reg_addr, reg_size, reg_size,
						   (SBI_DOMAIN_MEMREGION_MMIO |
						    SBI_DOMAIN_MEMREGION_M_READABLE |
						    SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ಯದಿ (ret)
			ಪ್ರತ್ಯಾಗಮ ret;

		/* calculate number of slots in each queue */
		qctx->ಅಂಕೆ_slots =
			(reg_size - (mctl->slot_size * RPMI_QUEUE_HEADER_SLOTS)) / mctl->slot_size;

		/* setup queue pointers */
		qctx->headptr = ((ಶೂನ್ಯ *)(unsigned long)reg_addr) +
				RPMI_QUEUE_HEAD_SLOT * mctl->slot_size;
		qctx->tailptr = ((ಶೂನ್ಯ *)(unsigned long)reg_addr) +
				RPMI_QUEUE_TAIL_SLOT * mctl->slot_size;
		qctx->buffer = ((ಶೂನ್ಯ *)(unsigned long)reg_addr) +
				RPMI_QUEUE_HEADER_SLOTS * mctl->slot_size;

		/* get the queue name */
		name = fdt_stringlist_get(fdt, nodeoff, "reg-names",
					  qid, &len);
		ಯದಿ (!name || (name && len < 0))
			ಪ್ರತ್ಯಾಗಮ len;

		sbi_memcpy(qctx->name, name, len);

		/* store the index as queue_id */
		qctx->queue_id = qid;

		SPIN_LOCK_INIT(qctx->queue_lock);
	}

	/* get the a2p-doorbell property name */
	name = fdt_stringlist_get(fdt, nodeoff, "reg-names", qid, &len);
	ಯದಿ (!name || (name && len < 0))
		ಪ್ರತ್ಯಾಗಮ len;

	/* fetch doorbell register address*/
	ret = fdt_get_ಗಂಟು_addr_size(fdt, nodeoff, qid, &reg_addr,
				       &reg_size);
	ಯದಿ (!ret && !(strncmp(name, "a2p-doorbell", strlen("a2p-doorbell")))) {
		mctl->mb_regs = (ಶೂನ್ಯ *)(unsigned long)reg_addr;
		ret = sbi_domain_ಬೇರು_add_memrange(reg_addr, reg_size, reg_size,
						   (SBI_DOMAIN_MEMREGION_MMIO |
						    SBI_DOMAIN_MEMREGION_M_READABLE |
						    SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ಯದಿ (ret)
			ಪ್ರತ್ಯಾಗಮ ret;
	}

	ಪ್ರತ್ಯಾಗಮ SBI_SUCCESS;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rpmi_shmem_mbox_init(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff,
				ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	ಸಂರಚನಾ rpmi_base_get_attributes_resp resp;
	ಸಂರಚನಾ rpmi_shmem_mbox_controller *mctl;
	ಸಂರಚನಾ rpmi_srvgrp_chan *base_srvgrp;
	u32 tval[2], args[1];
	ಪೂರ್ಣಾಙ್ಕ ret = 0;

	mctl = sbi_zalloc(ಪರಿಮಾಣ(*mctl));
	ಯದಿ (!mctl)
		ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;

	/* Initialization transport from device tree */
	ret = rpmi_shmem_transport_init(mctl, fdt, nodeoff);
	ಯದಿ (ret)
		ಗಚ್ಛ fail_free_controller;

	/* Register mailbox controller */
	mctl->controller.id = nodeoff;
	mctl->controller.max_xfer_len =
			mctl->slot_size - ಪರಿಮಾಣ(ಸಂರಚನಾ rpmi_message_header);
	mctl->controller.driver = &fdt_mailbox_rpmi_shmem;
	mctl->controller.request_chan = rpmi_shmem_mbox_request_chan;
	mctl->controller.free_chan = rpmi_shmem_mbox_free_chan;
	mctl->controller.xfer = rpmi_shmem_mbox_xfer;
	mctl->controller.get_attribute = rpmi_shmem_mbox_get_attribute;
	ret = mbox_controller_add(&mctl->controller);
	ಯದಿ (ret)
		ಗಚ್ಛ fail_free_controller;

	/* Request base service group channel */
	tval[0] = RPMI_SRVGRP_BASE;
	mctl->base_chan = mbox_controller_request_chan(&mctl->controller,
							tval);
	ಯದಿ (!mctl->base_chan) {
		ret = SBI_ENOENT;
		ಗಚ್ಛ fail_remove_controller;
	}

	/* Update base service group version */
	base_srvgrp = to_srvgrp_chan(mctl->base_chan);
	args[0] = RPMI_SRVGRP_BASE;
	ret = smq_base_get_two_u32(mctl, RPMI_BASE_SRV_PROBE_SERVICE_GROUP,
				   &args[0], tval);
	ಯದಿ (ret)
		ಗಚ್ಛ fail_free_chan;
	base_srvgrp->servicegroup_version = tval[1];
	ಯದಿ (base_srvgrp->servicegroup_version < RPMI_BASE_VERSION_MIN) {
		ret = SBI_EINVAL;
		ಗಚ್ಛ fail_free_chan;
	}

	/* Get implementation id */
	ret = smq_base_get_two_u32(mctl,
				   RPMI_BASE_SRV_GET_IMPLEMENTATION_VERSION,
				   ರಿಕ್ತ, tval);
	ಯದಿ (ret)
		ಗಚ್ಛ fail_free_chan;
	mctl->impl_version = tval[1];

	/* Get implementation version */
	ret = smq_base_get_two_u32(mctl, RPMI_BASE_SRV_GET_IMPLEMENTATION_IDN,
				   ರಿಕ್ತ, tval);
	ಯದಿ (ret)
		ಗಚ್ಛ fail_free_chan;
	mctl->impl_id = tval[1];

	/* Get specification version */
	ret = smq_base_get_two_u32(mctl, RPMI_BASE_SRV_GET_SPEC_VERSION,
				   ರಿಕ್ತ, tval);
	ಯದಿ (ret)
		ಗಚ್ಛ fail_free_chan;
	mctl->spec_version = tval[1];
	ಯದಿ (mctl->spec_version < RPMI_BASE_VERSION_MIN ||
	    mctl->spec_version != base_srvgrp->servicegroup_version) {
		ret = SBI_EINVAL;
		ಗಚ್ಛ fail_free_chan;
	}

	/* Get optional features implementation flags */
	ret = rpmi_normal_request_with_status(
			mctl->base_chan, RPMI_BASE_SRV_GET_ATTRIBUTES,
			ರಿಕ್ತ, 0, 0,
			&resp, rpmi_u32_ಎಣಿಕೆ(resp), rpmi_u32_ಎಣಿಕೆ(resp));
	ಯದಿ (ret)
		ಗಚ್ಛ fail_free_chan;

	/* 1: M-mode, 0: S-mode */
	mctl->base_flags.f0_priv_level =
			resp.f0 & RPMI_BASE_FLAGS_F0_PRIVILEGE ? 1 : 0;
	/* 1: Supported, 0: Not Supported */
	mctl->base_flags.f0_ev_notif_en =
			resp.f0 & RPMI_BASE_FLAGS_F0_EV_NOTIFY ? 1 : 0;

	/* We only use M-mode RPMI context in OpenSBI */
	ಯದಿ (!mctl->base_flags.f0_priv_level) {
		ret = SBI_ENODEV;
		ಗಚ್ಛ fail_free_chan;
	}

	/*
	 * Continue without platform information string if not
	 * available or if an error is encountered while fetching
	 */
	rpmi_get_platform_info(mctl);

	ಪ್ರತ್ಯಾಗಮ 0;

fail_free_chan:
	mbox_controller_free_chan(mctl->base_chan);
fail_remove_controller:
	mbox_controller_remove(&mctl->controller);
fail_free_controller:
	sbi_free(mctl);
	ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match rpmi_shmem_mbox_match[] = {
	{ .compatible = "riscv,rpmi-shmem-mbox" },
	{ },
};

ಸಂರಚನಾ fdt_mailbox fdt_mailbox_rpmi_shmem = {
	.driver = {
		.match_table = rpmi_shmem_mbox_match,
		.init = rpmi_shmem_mbox_init,
	},
	.xlate = fdt_mailbox_simple_xlate,
};
