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

#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_timer.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_locks.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_barrier.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_helper.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/mailbox/mailbox.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/mailbox/fdt_mailbox.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/mailbox/rpmi_mailbox.h>

/** Minimum Base group version required */
#define RPMI_BASE_VERSION_MIN		RPMI_VERSION(1, 0)

/**************** RPMI Transport Structures and Macros ***********/

#define GET_SERVICEGROUP_ID(ସନ୍ଦେଶ)		\
({						\
	ସଂରଚନା rpmi_message *mbuf = ସନ୍ଦେଶ;	\
	le16_to_cpu(mbuf->header.servicegroup_id);\
})

#define GET_SERVICE_ID(ସନ୍ଦେଶ)			\
({						\
	ସଂରଚନା rpmi_message *mbuf = ସନ୍ଦେଶ;	\
	mbuf->header.service_id;		\
})

#define GET_FLAGS(ସନ୍ଦେଶ)				\
({						\
	ସଂରଚନା rpmi_message *mbuf = ସନ୍ଦେଶ;	\
	mbuf->header.flags;			\
})

#define GET_MESSAGE_ID(ସନ୍ଦେଶ)			\
({						\
	ସଂରଚନା rpmi_message *mbuf = ସନ୍ଦେଶ;	\
	((u32)mbuf->header.flags << (RPMI_ସନ୍ଦେଶ_FLAGS_OFFSET * 8)) | \
	((u32)mbuf->header.service_id << (RPMI_ସନ୍ଦେଶ_SERVICE_ID_OFFSET * 8)) | \
	((u32)le16_to_cpu(mbuf->header.servicegroup_id)); \
})

#define MAKE_MESSAGE_ID(__group_id, __service_id, __flags)	\
({						\
	u32 __ret = 0;				\
	__ret |= (u32)(__group_id) << (RPMI_ସନ୍ଦେଶ_SERVICEGROUP_ID_OFFSET * 8); \
	__ret |= (u32)(__service_id) << (RPMI_ସନ୍ଦେଶ_SERVICE_ID_OFFSET * 8); \
	__ret |= (u32)(__flags) << (RPMI_ସନ୍ଦେଶ_FLAGS_OFFSET * 8); \
	__ret;					\
})

#define GET_DLEN(ସନ୍ଦେଶ)				\
({						\
	ସଂରଚନା rpmi_message *mbuf = ସନ୍ଦେଶ;	\
	le16_to_cpu(mbuf->header.datalen);	\
})

#define GET_TOKEN(ସନ୍ଦେଶ)				\
({						\
	ସଂରଚନା rpmi_message *mbuf = ସନ୍ଦେଶ;	\
	le16_to_cpu(mbuf->header.token);	\
})

#define GET_MESSAGE_TYPE(ସନ୍ଦେଶ)						\
({									\
	uint8_t flags = *((uint8_t *)ସନ୍ଦେଶ + RPMI_ସନ୍ଦେଶ_FLAGS_OFFSET);	\
	((flags & RPMI_ସନ୍ଦେଶ_FLAGS_TYPE) >> RPMI_ସନ୍ଦେଶ_FLAGS_TYPE_POS));	\
})

ଗଣନା rpmi_queue_type {
	RPMI_QUEUE_TYPE_REQ = 0,
	RPMI_QUEUE_TYPE_ACK = 1,
};

ଗଣନା rpmi_queue_idx {
	RPMI_QUEUE_IDX_A2P_REQ = 0,
	RPMI_QUEUE_IDX_P2A_ACK = 1,
	RPMI_QUEUE_IDX_P2A_REQ = 2,
	RPMI_QUEUE_IDX_A2P_ACK = 3,
	RPMI_QUEUE_IDX_MAX_ଗଣନା,
};

ଗଣନା rpmi_reg_idx {
	RPMI_REG_IDX_DB_REG = 0, /* Doorbell register */
	RPMI_REG_IDX_MAX_ଗଣନା,
};

/** Mailbox registers */
ସଂରଚନା rpmi_mb_regs {
	/* doorbell from AP -> PuC*/
	ଅସ୍ଥିର le32_t db_reg;
} __packed;

/** Single Queue Context Structure */
ସଂରଚନା smq_queue_ctx {
	u32 queue_id;
	u32 ଅଙ୍କ_slots;
	spinlock_t queue_lock;
	/* Type of queue - REQ or ACK */
	ଗଣନା rpmi_queue_type queue_type;
	/* Pointers to the queue shared memory */
	ଅସ୍ଥିର le32_t *headptr PT_GUARDED_BY(&queue_lock);
	ଅସ୍ଥିର le32_t *tailptr PT_GUARDED_BY(&queue_lock);
	ଅସ୍ଥିର uint8_t *buffer PT_GUARDED_BY(&queue_lock);
	/* Name of the queue */
	ଅକ୍ଷର name[RPMI_NAME_CHARS_MAX];
};

ସଂରଚନା rpmi_srvgrp_chan {
	u32 servicegroup_id;
	u32 servicegroup_version;
	ସଂରଚନା mbox_chan chan;
};

#define to_srvgrp_chan(mbox_chan)	\
		container_of(mbox_chan, ସଂରଚନା rpmi_srvgrp_chan, chan);

ସଂରଚନା rpmi_shmem_mbox_controller {
	/* Driver specific members */
	u32 slot_size;
	u32 queue_ଗଣନା;
	u32 p2a_doorbell_sysmsi_index;
	u32 a2p_doorbell_ମୂଲ୍ୟ;
	ସଂରଚନା rpmi_mb_regs *mb_regs;
	ସଂରଚନା smq_queue_ctx queue_ctx_tbl[RPMI_QUEUE_IDX_MAX_ଗଣନା];
	/* Mailbox framework related members */
	ସଂରଚନା mbox_controller controller;
	ସଂରଚନା mbox_chan *base_chan;
	u32 impl_version;
	u32 impl_id;
	u32 spec_version;
	u32 plat_info_len;
	ଅକ୍ଷର *plat_info;
	ସଂରଚନା {
		u8 f0_priv_level;
		bool f0_ev_notif_en;
	} base_flags;
};

/**************** Shared Memory Queues Helpers **************/

ସ୍ଥିର bool __smq_queue_full(ସଂରଚନା smq_queue_ctx *qctx)
	MUST_HOLD(&qctx->queue_lock)
{
	ଫେରନ୍ତୁ ((le32_to_cpu(*qctx->tailptr) + 1) % qctx->ଅଙ୍କ_slots ==
			le32_to_cpu(*qctx->headptr)) ? true : false;
}

ସ୍ଥିର bool __smq_queue_empty(ସଂରଚନା smq_queue_ctx *qctx)
	MUST_HOLD(&qctx->queue_lock)
{
	ଫେରନ୍ତୁ (le32_to_cpu(*qctx->headptr) ==
		le32_to_cpu(*qctx->tailptr)) ? true : false;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା __smq_rx(ସଂରଚନା smq_queue_ctx *qctx, u32 slot_size,
		    u32 service_group_id, ସଂରଚନା mbox_xfer *xfer)
	MUST_HOLD(&qctx->queue_lock)
{
	ଶୂନ୍ୟ *dst, *src;
	ସଂରଚନା rpmi_message *ସନ୍ଦେଶ;
	u32 i, tmp, pos, dlen, msgidn, headidx, tailidx;
	ସଂରଚନା rpmi_message_args *args = xfer->args;
	bool no_rx_token = (args->flags & RPMI_ସନ୍ଦେଶ_FLAGS_NO_RX_TOKEN) ?
			   true : false;

	/* Rx sanity checks */
	ଯଦି ((ଆକାର(u32) * args->rx_endian_ଶବ୍ଦଗୁଡ଼ିକ) >
	    (slot_size - ଆକାର(ସଂରଚନା rpmi_message_header)))
		ଫେରନ୍ତୁ SBI_EINVAL;
	ଯଦି ((ଆକାର(u32) * args->rx_endian_ଶବ୍ଦଗୁଡ଼ିକ) > xfer->rx_len)
		ଫେରନ୍ତୁ SBI_EINVAL;

	/* There should be some message in the queue */
	ଯଦି (__smq_queue_empty(qctx))
		ଫେରନ୍ତୁ SBI_ENOENT;

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
	ଯେତେବେଳେ (pos != tailidx) {
		src = (ଶୂନ୍ୟ *)qctx->buffer + (pos * slot_size);
		ଯଦି ((no_rx_token && GET_MESSAGE_ID(src) == msgidn) ||
		    (GET_TOKEN(src) == (xfer->seq & RPMI_ସନ୍ଦେଶ_TOKEN_MASK)))
			ଭାଙ୍ଗ;
		pos = (pos + 1) % qctx->ଅଙ୍କ_slots;
	}
	ଯଦି (pos == tailidx)
		ଫେରନ୍ତୁ SBI_ENOENT;

	/* If Rx message is not first message then make it first message */
	ଯଦି (pos != headidx) {
		src = (ଶୂନ୍ୟ *)qctx->buffer + (pos * slot_size);
		dst = (ଶୂନ୍ୟ *)qctx->buffer + (headidx * slot_size);
		ପାଇଁ (i = 0; i < slot_size / ଆକାର(u32); i++) {
			tmp = ((u32 *)dst)[i];
			((u32 *)dst)[i] = ((u32 *)src)[i];
			((u32 *)src)[i] = tmp;
		}
	}

	/* Update rx_token if not available */
	ସନ୍ଦେଶ = (ଶୂନ୍ୟ *)qctx->buffer + (headidx * slot_size);
	ଯଦି (no_rx_token)
		args->rx_token = GET_TOKEN(ସନ୍ଦେଶ);

	/* Extract data from the first message */
	ଯଦି (xfer->rx) {
		args->rx_data_len = dlen = GET_DLEN(ସନ୍ଦେଶ);
		ଯଦି (dlen > xfer->rx_len)
			dlen = xfer->rx_len;
		src = (ଶୂନ୍ୟ *)ସନ୍ଦେଶ + ଆକାର(ସଂରଚନା rpmi_message_header);
		dst = xfer->rx;
		ପାଇଁ (i = 0; i < args->rx_endian_ଶବ୍ଦଗୁଡ଼ିକ; i++)
			((u32 *)dst)[i] = le32_to_cpu(((u32 *)src)[i]);
		dst += ଆକାର(u32) * args->rx_endian_ଶବ୍ଦଗୁଡ଼ିକ;
		src += ଆକାର(u32) * args->rx_endian_ଶବ୍ଦଗୁଡ଼ିକ;
		sbi_memcpy(dst, src,
			xfer->rx_len - (ଆକାର(u32) * args->rx_endian_ଶବ୍ଦଗୁଡ଼ିକ));
	}

	/* Update the head/read index */
	*qctx->headptr = cpu_to_le32(headidx + 1) % qctx->ଅଙ୍କ_slots;

	/* Make sure updates to head are immediately visible to PuC */
	smp_wmb();

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା __smq_tx(ସଂରଚନା smq_queue_ctx *qctx, ସଂରଚନା rpmi_mb_regs *mb_regs,
		    u32 a2p_doorbell_ମୂଲ୍ୟ, u32 slot_size, u32 service_group_id,
		    ସଂରଚନା mbox_xfer *xfer) MUST_HOLD(&qctx->queue_lock)
{
	u32 i, tailidx;
	ଶୂନ୍ୟ *dst, *src;
	ସଂରଚନା rpmi_message_header header = { 0 };
	ସଂରଚନା rpmi_message_args *args = xfer->args;

	/* Tx sanity checks */
	ଯଦି ((ଆକାର(u32) * args->tx_endian_ଶବ୍ଦଗୁଡ଼ିକ) >
	    (slot_size - ଆକାର(ସଂରଚନା rpmi_message_header)))
		ଫେରନ୍ତୁ SBI_EINVAL;
	ଯଦି ((ଆକାର(u32) * args->tx_endian_ଶବ୍ଦଗୁଡ଼ିକ) > xfer->tx_len)
		ଫେରନ୍ତୁ SBI_EINVAL;

	/* There should be some room in the queue */
	ଯଦି (__smq_queue_full(qctx))
		ଫେରନ୍ତୁ SBI_ENOMEM;

	/* Get the tail/write index */
	tailidx = le32_to_cpu(*qctx->tailptr);

	/* Prepare the header to be written into the slot */
	header.servicegroup_id = cpu_to_le16(service_group_id);
	header.service_id = args->service_id;
	header.flags = args->type;
	header.datalen = cpu_to_le16((u16)xfer->tx_len);
	header.token = cpu_to_le16((u16)xfer->seq);

	/* Write header into the slot */
	dst = (ଅକ୍ଷର *)qctx->buffer + (tailidx * slot_size);
	sbi_memcpy(dst, &header, ଆକାର(header));
	dst += ଆକାର(header);

	/* Write data into the slot */
	ଯଦି (xfer->tx) {
		src = xfer->tx;
		ପାଇଁ (i = 0; i < args->tx_endian_ଶବ୍ଦଗୁଡ଼ିକ; i++)
			((u32 *)dst)[i] = cpu_to_le32(((u32 *)src)[i]);
		dst += ଆକାର(u32) * args->tx_endian_ଶବ୍ଦଗୁଡ଼ିକ;
		src += ଆକାର(u32) * args->tx_endian_ଶବ୍ଦଗୁଡ଼ିକ;
		sbi_memcpy(dst, src,
			xfer->tx_len - (ଆକାର(u32) * args->tx_endian_ଶବ୍ଦଗୁଡ଼ିକ));
	}

	/* Make sure queue chanages are visible to PuC before updating tail */
	smp_wmb();

	/* Update the tail/write index */
	*qctx->tailptr = cpu_to_le32(tailidx + 1) % qctx->ଅଙ୍କ_slots;

	/* Ring the RPMI doorbell if present */
	ଯଦି (mb_regs)
		writel(a2p_doorbell_ମୂଲ୍ୟ, &mb_regs->db_reg);

	ଫେରନ୍ତୁ SBI_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା smq_rx(ସଂରଚନା rpmi_shmem_mbox_controller *mctl,
		  u32 queue_id, u32 service_group_id, ସଂରଚନା mbox_xfer *xfer)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret, rxretry = 0;
	ସଂରଚନା smq_queue_ctx *qctx;

	ଯଦି (mctl->queue_ଗଣନା < queue_id) {
		sbi_printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		ଫେରନ୍ତୁ SBI_EINVAL;
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
	କର {
		spin_lock(&qctx->queue_lock);
		ret = __smq_rx(qctx, mctl->slot_size, service_group_id, xfer);
		spin_unlock(&qctx->queue_lock);
		ଯଦି (!ret)
			ଫେରନ୍ତୁ 0;

		sbi_timer_mdelay(1);
		rxretry += 1;
	} ଯେତେବେଳେ (rxretry < xfer->rx_timeout);

	ଫେରନ୍ତୁ SBI_ETIMEDOUT;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା smq_tx(ସଂରଚନା rpmi_shmem_mbox_controller *mctl,
		  u32 queue_id, u32 service_group_id, ସଂରଚନା mbox_xfer *xfer)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret, txretry = 0;
	ସଂରଚନା smq_queue_ctx *qctx;

	ଯଦି (mctl->queue_ଗଣନା < queue_id) {
		sbi_printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		ଫେରନ୍ତୁ SBI_EINVAL;
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
	କର {
		spin_lock(&qctx->queue_lock);
		ret = __smq_tx(qctx, mctl->mb_regs, mctl->a2p_doorbell_ମୂଲ୍ୟ,
				mctl->slot_size, service_group_id, xfer);
		spin_unlock(&qctx->queue_lock);
		ଯଦି (!ret)
			ଫେରନ୍ତୁ 0;

		sbi_timer_mdelay(1);
		txretry += 1;
	} ଯେତେବେଳେ (txretry < xfer->tx_timeout);

	ଫେରନ୍ତୁ SBI_ETIMEDOUT;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rpmi_get_platform_info(ସଂରଚନା rpmi_shmem_mbox_controller *mctl)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = SBI_OK;

	/**
	 * platform string can occupy max possible size
	 * max possible space in the message data as
	 * per the format
	 */
	ସଂରଚନା rpmi_base_get_platform_info_resp *resp =
			sbi_zalloc(RPMI_ସନ୍ଦେଶ_DATA_SIZE(mctl->slot_size));
	ଯଦି (!resp)
		ଫେରନ୍ତୁ SBI_ENOMEM;

	ret = rpmi_normal_request_with_status(mctl->base_chan,
				 RPMI_BASE_SRV_GET_PLATFORM_INFO,
				 ଖାଲି, 0, 0,
				 resp,
				 RPMI_ସନ୍ଦେଶ_DATA_SIZE(mctl->slot_size)/4,
				 RPMI_ସନ୍ଦେଶ_DATA_SIZE(mctl->slot_size)/4);
	ଯଦି (ret)
		ଯାଅ fail_free_resp;

	mctl->plat_info_len = resp->plat_info_len;
	mctl->plat_info = sbi_zalloc(mctl->plat_info_len);
	ଯଦି (!mctl->plat_info) {
		ret = SBI_ENOMEM;
		ଯାଅ fail_free_resp;
	}

	sbi_strncpy(mctl->plat_info, resp->plat_info, mctl->plat_info_len);

fail_free_resp:
	sbi_free(resp);
	ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା smq_base_get_two_u32(ସଂରଚନା rpmi_shmem_mbox_controller *mctl,
				u32 service_id, u32 *inarg, u32 *outvals)
{
	ଫେରନ୍ତୁ rpmi_normal_request_with_status(
			mctl->base_chan, service_id,
			inarg, (inarg) ? 1 : 0, (inarg) ? 1 : 0,
			outvals, 2, 2);
}

/**************** Mailbox Controller Functions **************/

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rpmi_shmem_mbox_xfer(ସଂରଚନା mbox_chan *chan, ସଂରଚନା mbox_xfer *xfer)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
	u32 tx_qid = 0, rx_qid = 0;
	ସଂରଚନା rpmi_shmem_mbox_controller *mctl =
			container_of(chan->mbox,
				     ସଂରଚନା rpmi_shmem_mbox_controller,
				     controller);
	ସଂରଚନା rpmi_srvgrp_chan *srvgrp_chan = to_srvgrp_chan(chan);

	ସଂରଚନା rpmi_message_args *args = xfer->args;
	bool do_tx = (args->flags & RPMI_ସନ୍ଦେଶ_FLAGS_NO_TX) ? false : true;
	bool do_rx = (args->flags & RPMI_ସନ୍ଦେଶ_FLAGS_NO_RX) ? false : true;

	ଯଦି (!do_tx && !do_rx)
		ଫେରନ୍ତୁ SBI_EINVAL;

	ବାଛ (args->type) {
	ସ୍ଥିତି RPMI_ସନ୍ଦେଶ_NORMAL_REQUEST:
		ଯଦି (do_tx && do_rx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
			rx_qid = RPMI_QUEUE_IDX_P2A_ACK;
		} ଅନ୍ୟଥା ଯଦି (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
		} ଅନ୍ୟଥା ଯଦି (do_rx) {
			rx_qid = RPMI_QUEUE_IDX_P2A_REQ;
		}
		ଭାଙ୍ଗ;
	ସ୍ଥିତି RPMI_ସନ୍ଦେଶ_POSTED_REQUEST:
		ଯଦି (do_tx && do_rx)
			ଫେରନ୍ତୁ SBI_EINVAL;
		ଯଦି (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
		} ଅନ୍ୟଥା {
			rx_qid = RPMI_QUEUE_IDX_P2A_REQ;
		}
		ଭାଙ୍ଗ;
	ସ୍ଥିତି RPMI_ସନ୍ଦେଶ_ACKNOWLDGEMENT:
		ଯଦି (do_tx && do_rx)
			ଫେରନ୍ତୁ SBI_EINVAL;
		ଯଦି (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_ACK;
		} ଅନ୍ୟଥା {
			rx_qid = RPMI_QUEUE_IDX_P2A_ACK;
		}
		ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଫେରନ୍ତୁ SBI_ENOTSUPP;
	}

	ଯଦି (do_tx) {
		ret = smq_tx(mctl, tx_qid, srvgrp_chan->servicegroup_id, xfer);
		ଯଦି (ret)
			ଫେରନ୍ତୁ ret;
	}

	ଯଦି (do_rx) {
		ret = smq_rx(mctl, rx_qid, srvgrp_chan->servicegroup_id, xfer);
		ଯଦି (ret)
			ଫେରନ୍ତୁ ret;
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rpmi_shmem_mbox_get_attribute(ସଂରଚନା mbox_chan *chan,
					 ପୂର୍ଣ୍ଣସଂଖ୍ୟା attr_id, ଶୂନ୍ୟ *out_ମୂଲ୍ୟ)
{
	ସଂରଚନା rpmi_shmem_mbox_controller *mctl =
			container_of(chan->mbox,
				     ସଂରଚନା rpmi_shmem_mbox_controller,
				     controller);
	ସଂରଚନା rpmi_srvgrp_chan *srvgrp_chan = to_srvgrp_chan(chan);

	ବାଛ (attr_id) {
	ସ୍ଥିତି RPMI_CHANNEL_ATTR_PROTOCOL_VERSION:
		*((u32 *)out_ମୂଲ୍ୟ) = mctl->spec_version;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି RPMI_CHANNEL_ATTR_MAX_DATA_LEN:
		*((u32 *)out_ମୂଲ୍ୟ) = RPMI_ସନ୍ଦେଶ_DATA_SIZE(mctl->slot_size);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି RPMI_CHANNEL_ATTR_P2A_DOORBELL_SYSMSI_INDEX:
		*((u32 *)out_ମୂଲ୍ୟ) = mctl->p2a_doorbell_sysmsi_index;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି RPMI_CHANNEL_ATTR_TX_TIMEOUT:
		*((u32 *)out_ମୂଲ୍ୟ) = RPMI_DEF_TX_TIMEOUT;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି RPMI_CHANNEL_ATTR_RX_TIMEOUT:
		*((u32 *)out_ମୂଲ୍ୟ) = RPMI_DEF_RX_TIMEOUT;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି RPMI_CHANNEL_ATTR_SERVICEGROUP_ID:
		*((u32 *)out_ମୂଲ୍ୟ) = srvgrp_chan->servicegroup_id;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି RPMI_CHANNEL_ATTR_SERVICEGROUP_VERSION:
		*((u32 *)out_ମୂଲ୍ୟ) = srvgrp_chan->servicegroup_version;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି RPMI_CHANNEL_ATTR_IMPL_ID:
		*((u32 *)out_ମୂଲ୍ୟ) = mctl->impl_id;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି RPMI_CHANNEL_ATTR_IMPL_VERSION:
		*((u32 *)out_ମୂଲ୍ୟ) = mctl->impl_version;
		ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଫେରନ୍ତୁ SBI_ENOTSUPP;
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ସଂରଚନା mbox_chan *rpmi_shmem_mbox_request_chan(
						ସଂରଚନା mbox_controller *mbox,
						u32 *chan_args)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
	u32 tval[2] = { 0 };
	ସଂରଚନା rpmi_srvgrp_chan *srvgrp_chan;
	ସଂରଚନା rpmi_shmem_mbox_controller *mctl =
			container_of(mbox,
				     ସଂରଚନା rpmi_shmem_mbox_controller,
				     controller);

	/* Service group id not defined or in reserved range is invalid */
	ଯଦି (chan_args[0] >= RPMI_SRVGRP_ID_MAX_ଗଣନା &&
		chan_args[0] <= RPMI_SRVGRP_RESERVE_END)
		ଫେରନ୍ତୁ ଖାଲି;

	/* Base serivce group is always present so probe other groups */
	ଯଦି (chan_args[0] != RPMI_SRVGRP_BASE) {
		/* Probe service group */
		ret = smq_base_get_two_u32(mctl,
					   RPMI_BASE_SRV_PROBE_SERVICE_GROUP,
					   chan_args, tval);
		ଯଦି (ret || !tval[1])
			ଫେରନ୍ତୁ ଖାଲି;
	}

	srvgrp_chan = sbi_zalloc(ଆକାର(*srvgrp_chan));
	ଯଦି (!srvgrp_chan)
		ଫେରନ୍ତୁ ଖାଲି;

	srvgrp_chan->servicegroup_id = chan_args[0];
	srvgrp_chan->servicegroup_version = tval[1];

	ଫେରନ୍ତୁ &srvgrp_chan->chan;
}

ସ୍ଥିର ଶୂନ୍ୟ rpmi_shmem_mbox_free_chan(ସଂରଚନା mbox_controller *mbox,
				      ସଂରଚନା mbox_chan *chan)
{
	ସଂରଚନା rpmi_srvgrp_chan *srvgrp_chan = to_srvgrp_chan(chan);
	sbi_free(srvgrp_chan);
}

ବାହ୍ୟ ସଂରଚନା fdt_mailbox fdt_mailbox_rpmi_shmem;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rpmi_shmem_transport_init(ସଂରଚନା rpmi_shmem_mbox_controller *mctl,
				     ଅଚଳ ଶୂନ୍ୟ *fdt,
				     ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff) NO_THREAD_SAFETY_ANALYSIS
{
	ଅଚଳ ଅକ୍ଷର *name;
	ଅଚଳ fdt32_t *prop;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2, len, ret, qid;
	uint64_t reg_addr, reg_size;
	ସଂରଚନା smq_queue_ctx *qctx;

	ret = fdt_ଗଣ୍ଠି_check_compatible(fdt, nodeoff,
					"riscv,rpmi-shmem-mbox");
	ଯଦି (ret)
		ଫେରନ୍ତୁ ret;

	/* get queue slot size in bytes */
	prop = fdt_getprop(fdt, nodeoff, "riscv,slot-size", &len);
	ଯଦି (!prop)
		ଫେରନ୍ତୁ SBI_ENOENT;

	mctl->slot_size = fdt32_to_cpu(*prop);
	ଯଦି (mctl->slot_size < RPMI_SLOT_SIZE_MIN) {
		sbi_printf("%s: slot_size < mimnum required message size\n",
			   __func__);
		mctl->slot_size = RPMI_SLOT_SIZE_MIN;
	}

	/* get p2a doorbell system MSI index */
	prop = fdt_getprop(fdt, nodeoff, "riscv,p2a-doorbell-sysmsi-index", &len);
	mctl->p2a_doorbell_sysmsi_index = prop ? fdt32_to_cpu(*prop) : -1U;

	/* get a2p doorbell value */
	prop = fdt_getprop(fdt, nodeoff, "riscv,a2p-doorbell-value", &len);
	mctl->a2p_doorbell_ମୂଲ୍ୟ = prop ? fdt32_to_cpu(*prop) : 1;

	/*
	 * queue names count is taken as the number of queues
	 * supported which make it mandatory to provide the
	 * name of the queue.
	 */
	ଗଣନା_2 = fdt_stringlist_ଗଣନା(fdt, nodeoff, "reg-names");
	ଯଦି (ଗଣନା_2 < 0 ||
	    ଗଣନା_2 > (RPMI_QUEUE_IDX_MAX_ଗଣନା + RPMI_REG_IDX_MAX_ଗଣନା))
		ଫେରନ୍ତୁ SBI_EINVAL;

	mctl->queue_ଗଣନା = ଗଣନା_2 - RPMI_REG_IDX_MAX_ଗଣନା;

	/* parse all queues and populate queues context structure */
	ପାଇଁ (qid = 0; qid < mctl->queue_ଗଣନା; qid++) {
		qctx = &mctl->queue_ctx_tbl[qid];

		/* get each queue share-memory base address and size*/
		ret = fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoff, qid,
					     &reg_addr, &reg_size);
		ଯଦି (ret < 0 || !reg_addr || !reg_size)
			ଫେରନ୍ତୁ SBI_ENOENT;

		ret = sbi_domain_ମୂଳ_add_memrange(reg_addr, reg_size, reg_size,
						   (SBI_DOMAIN_MEMREGION_MMIO |
						    SBI_DOMAIN_MEMREGION_M_READABLE |
						    SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ଯଦି (ret)
			ଫେରନ୍ତୁ ret;

		/* calculate number of slots in each queue */
		qctx->ଅଙ୍କ_slots =
			(reg_size - (mctl->slot_size * RPMI_QUEUE_HEADER_SLOTS)) / mctl->slot_size;

		/* setup queue pointers */
		qctx->headptr = ((ଶୂନ୍ୟ *)(unsigned long)reg_addr) +
				RPMI_QUEUE_HEAD_SLOT * mctl->slot_size;
		qctx->tailptr = ((ଶୂନ୍ୟ *)(unsigned long)reg_addr) +
				RPMI_QUEUE_TAIL_SLOT * mctl->slot_size;
		qctx->buffer = ((ଶୂନ୍ୟ *)(unsigned long)reg_addr) +
				RPMI_QUEUE_HEADER_SLOTS * mctl->slot_size;

		/* get the queue name */
		name = fdt_stringlist_get(fdt, nodeoff, "reg-names",
					  qid, &len);
		ଯଦି (!name || (name && len < 0))
			ଫେରନ୍ତୁ len;

		sbi_memcpy(qctx->name, name, len);

		/* store the index as queue_id */
		qctx->queue_id = qid;

		SPIN_LOCK_INIT(qctx->queue_lock);
	}

	/* get the a2p-doorbell property name */
	name = fdt_stringlist_get(fdt, nodeoff, "reg-names", qid, &len);
	ଯଦି (!name || (name && len < 0))
		ଫେରନ୍ତୁ len;

	/* fetch doorbell register address*/
	ret = fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoff, qid, &reg_addr,
				       &reg_size);
	ଯଦି (!ret && !(strncmp(name, "a2p-doorbell", strlen("a2p-doorbell")))) {
		mctl->mb_regs = (ଶୂନ୍ୟ *)(unsigned long)reg_addr;
		ret = sbi_domain_ମୂଳ_add_memrange(reg_addr, reg_size, reg_size,
						   (SBI_DOMAIN_MEMREGION_MMIO |
						    SBI_DOMAIN_MEMREGION_M_READABLE |
						    SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ଯଦି (ret)
			ଫେରନ୍ତୁ ret;
	}

	ଫେରନ୍ତୁ SBI_SUCCESS;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rpmi_shmem_mbox_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff,
				ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ସଂରଚନା rpmi_base_get_attributes_resp resp;
	ସଂରଚନା rpmi_shmem_mbox_controller *mctl;
	ସଂରଚନା rpmi_srvgrp_chan *base_srvgrp;
	u32 tval[2], args[1];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = 0;

	mctl = sbi_zalloc(ଆକାର(*mctl));
	ଯଦି (!mctl)
		ଫେରନ୍ତୁ SBI_ENOMEM;

	/* Initialization transport from device tree */
	ret = rpmi_shmem_transport_init(mctl, fdt, nodeoff);
	ଯଦି (ret)
		ଯାଅ fail_free_controller;

	/* Register mailbox controller */
	mctl->controller.id = nodeoff;
	mctl->controller.max_xfer_len =
			mctl->slot_size - ଆକାର(ସଂରଚନା rpmi_message_header);
	mctl->controller.driver = &fdt_mailbox_rpmi_shmem;
	mctl->controller.request_chan = rpmi_shmem_mbox_request_chan;
	mctl->controller.free_chan = rpmi_shmem_mbox_free_chan;
	mctl->controller.xfer = rpmi_shmem_mbox_xfer;
	mctl->controller.get_attribute = rpmi_shmem_mbox_get_attribute;
	ret = mbox_controller_add(&mctl->controller);
	ଯଦି (ret)
		ଯାଅ fail_free_controller;

	/* Request base service group channel */
	tval[0] = RPMI_SRVGRP_BASE;
	mctl->base_chan = mbox_controller_request_chan(&mctl->controller,
							tval);
	ଯଦି (!mctl->base_chan) {
		ret = SBI_ENOENT;
		ଯାଅ fail_remove_controller;
	}

	/* Update base service group version */
	base_srvgrp = to_srvgrp_chan(mctl->base_chan);
	args[0] = RPMI_SRVGRP_BASE;
	ret = smq_base_get_two_u32(mctl, RPMI_BASE_SRV_PROBE_SERVICE_GROUP,
				   &args[0], tval);
	ଯଦି (ret)
		ଯାଅ fail_free_chan;
	base_srvgrp->servicegroup_version = tval[1];
	ଯଦି (base_srvgrp->servicegroup_version < RPMI_BASE_VERSION_MIN) {
		ret = SBI_EINVAL;
		ଯାଅ fail_free_chan;
	}

	/* Get implementation id */
	ret = smq_base_get_two_u32(mctl,
				   RPMI_BASE_SRV_GET_IMPLEMENTATION_VERSION,
				   ଖାଲି, tval);
	ଯଦି (ret)
		ଯାଅ fail_free_chan;
	mctl->impl_version = tval[1];

	/* Get implementation version */
	ret = smq_base_get_two_u32(mctl, RPMI_BASE_SRV_GET_IMPLEMENTATION_IDN,
				   ଖାଲି, tval);
	ଯଦି (ret)
		ଯାଅ fail_free_chan;
	mctl->impl_id = tval[1];

	/* Get specification version */
	ret = smq_base_get_two_u32(mctl, RPMI_BASE_SRV_GET_SPEC_VERSION,
				   ଖାଲି, tval);
	ଯଦି (ret)
		ଯାଅ fail_free_chan;
	mctl->spec_version = tval[1];
	ଯଦି (mctl->spec_version < RPMI_BASE_VERSION_MIN ||
	    mctl->spec_version != base_srvgrp->servicegroup_version) {
		ret = SBI_EINVAL;
		ଯାଅ fail_free_chan;
	}

	/* Get optional features implementation flags */
	ret = rpmi_normal_request_with_status(
			mctl->base_chan, RPMI_BASE_SRV_GET_ATTRIBUTES,
			ଖାଲି, 0, 0,
			&resp, rpmi_u32_ଗଣନା(resp), rpmi_u32_ଗଣନା(resp));
	ଯଦି (ret)
		ଯାଅ fail_free_chan;

	/* 1: M-mode, 0: S-mode */
	mctl->base_flags.f0_priv_level =
			resp.f0 & RPMI_BASE_FLAGS_F0_PRIVILEGE ? 1 : 0;
	/* 1: Supported, 0: Not Supported */
	mctl->base_flags.f0_ev_notif_en =
			resp.f0 & RPMI_BASE_FLAGS_F0_EV_NOTIFY ? 1 : 0;

	/* We only use M-mode RPMI context in OpenSBI */
	ଯଦି (!mctl->base_flags.f0_priv_level) {
		ret = SBI_ENODEV;
		ଯାଅ fail_free_chan;
	}

	/*
	 * Continue without platform information string if not
	 * available or if an error is encountered while fetching
	 */
	rpmi_get_platform_info(mctl);

	ଫେରନ୍ତୁ 0;

fail_free_chan:
	mbox_controller_free_chan(mctl->base_chan);
fail_remove_controller:
	mbox_controller_remove(&mctl->controller);
fail_free_controller:
	sbi_free(mctl);
	ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match rpmi_shmem_mbox_match[] = {
	{ .compatible = "riscv,rpmi-shmem-mbox" },
	{ },
};

ସଂରଚନା fdt_mailbox fdt_mailbox_rpmi_shmem = {
	.driver = {
		.match_table = rpmi_shmem_mbox_match,
		.init = rpmi_shmem_mbox_init,
	},
	.xlate = fdt_mailbox_simple_xlate,
};
