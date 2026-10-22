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

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_heap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_timer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_locks.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_barrier.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/mailbox/mailbox.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/mailbox/fdt_mailbox.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/mailbox/rpmi_mailbox.h>

/** Minimum Base group version required */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RPMI_ਅਧਾਰ_ਸੰਸਕਰਣ_MIN		RPMI_ਸੰਸਕਰਣ(1, 0)

/**************** RPMI Transport Structures and Macros ***********/

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GET_SERVICEGROUP_ID(ਸੁਨੇਹਾ)		\
({						\
	ਬਣਤਰ rpmi_ਸੁਨੇਹਾ *mbuf = ਸੁਨੇਹਾ;	\
	le16_ਮੰਜ਼ਿਲ_cpu(mbuf->ਸਿਰਲੇਖ.servicegroup_id);\
})

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GET_SERVICE_ID(ਸੁਨੇਹਾ)			\
({						\
	ਬਣਤਰ rpmi_ਸੁਨੇਹਾ *mbuf = ਸੁਨੇਹਾ;	\
	mbuf->ਸਿਰਲੇਖ.service_id;		\
})

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GET_FLAGS(ਸੁਨੇਹਾ)				\
({						\
	ਬਣਤਰ rpmi_ਸੁਨੇਹਾ *mbuf = ਸੁਨੇਹਾ;	\
	mbuf->ਸਿਰਲੇਖ.flags;			\
})

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GET_ਸੁਨੇਹਾ_ID(ਸੁਨੇਹਾ)			\
({						\
	ਬਣਤਰ rpmi_ਸੁਨੇਹਾ *mbuf = ਸੁਨੇਹਾ;	\
	((u32)mbuf->ਸਿਰਲੇਖ.flags << (RPMI_ਸੁਨੇਹਾ_FLAGS_ਵਿੱਥ * 8)) | \
	((u32)mbuf->ਸਿਰਲੇਖ.service_id << (RPMI_ਸੁਨੇਹਾ_SERVICE_ID_ਵਿੱਥ * 8)) | \
	((u32)le16_ਮੰਜ਼ਿਲ_cpu(mbuf->ਸਿਰਲੇਖ.servicegroup_id)); \
})

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAKE_ਸੁਨੇਹਾ_ID(__group_id, __service_id, __flags)	\
({						\
	u32 __ret = 0;				\
	__ret |= (u32)(__group_id) << (RPMI_ਸੁਨੇਹਾ_SERVICEGROUP_ID_ਵਿੱਥ * 8); \
	__ret |= (u32)(__service_id) << (RPMI_ਸੁਨੇਹਾ_SERVICE_ID_ਵਿੱਥ * 8); \
	__ret |= (u32)(__flags) << (RPMI_ਸੁਨੇਹਾ_FLAGS_ਵਿੱਥ * 8); \
	__ret;					\
})

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GET_DLEN(ਸੁਨੇਹਾ)				\
({						\
	ਬਣਤਰ rpmi_ਸੁਨੇਹਾ *mbuf = ਸੁਨੇਹਾ;	\
	le16_ਮੰਜ਼ਿਲ_cpu(mbuf->ਸਿਰਲੇਖ.datalen);	\
})

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GET_TOKEN(ਸੁਨੇਹਾ)				\
({						\
	ਬਣਤਰ rpmi_ਸੁਨੇਹਾ *mbuf = ਸੁਨੇਹਾ;	\
	le16_ਮੰਜ਼ਿਲ_cpu(mbuf->ਸਿਰਲੇਖ.token);	\
})

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GET_ਸੁਨੇਹਾ_TYPE(ਸੁਨੇਹਾ)						\
({									\
	uint8_t flags = *((uint8_t *)ਸੁਨੇਹਾ + RPMI_ਸੁਨੇਹਾ_FLAGS_ਵਿੱਥ);	\
	((flags & RPMI_ਸੁਨੇਹਾ_FLAGS_TYPE) >> RPMI_ਸੁਨੇਹਾ_FLAGS_TYPE_POS));	\
})

ਗਿਣਤੀ rpmi_ਕਤਾਰ_type {
	RPMI_ਕਤਾਰ_TYPE_REQ = 0,
	RPMI_ਕਤਾਰ_TYPE_ACK = 1,
};

ਗਿਣਤੀ rpmi_ਕਤਾਰ_idx {
	RPMI_ਕਤਾਰ_IDX_A2_P_REQ = 0,
	RPMI_ਕਤਾਰ_IDX_P2_A_ACK = 1,
	RPMI_ਕਤਾਰ_IDX_P2_A_REQ = 2,
	RPMI_ਕਤਾਰ_IDX_A2_P_ACK = 3,
	RPMI_ਕਤਾਰ_IDX_MAX_ਗਿਣਤੀ,
};

ਗਿਣਤੀ rpmi_reg_idx {
	RPMI_REG_IDX_DB_REG = 0, /* Doorbell register */
	RPMI_REG_IDX_MAX_ਗਿਣਤੀ,
};

/** Mailbox registers */
ਬਣਤਰ rpmi_mb_regs {
	/* doorbell from AP -> PuC*/
	ਅਸਥਿਰ le32_t db_reg;
} __packed;

/** Single Queue Context Structure */
ਬਣਤਰ smq_ਕਤਾਰ_ctx {
	u32 ਕਤਾਰ_id;
	u32 ਅੰਕ_slots;
	spinlock_t ਕਤਾਰ_ਤਾਲਾ;
	/* Type of queue - REQ or ACK */
	ਗਿਣਤੀ rpmi_ਕਤਾਰ_type ਕਤਾਰ_type;
	/* Pointers to the queue shared memory */
	ਅਸਥਿਰ le32_t *headptr PT_GUARDED_BY(&ਕਤਾਰ_ਤਾਲਾ);
	ਅਸਥਿਰ le32_t *tailptr PT_GUARDED_BY(&ਕਤਾਰ_ਤਾਲਾ);
	ਅਸਥਿਰ uint8_t *ਬਫਰ PT_GUARDED_BY(&ਕਤਾਰ_ਤਾਲਾ);
	/* Name of the queue */
	ਅੱਖਰ name[RPMI_NAME_CHARS_MAX];
};

ਬਣਤਰ rpmi_srvgrp_chan {
	u32 servicegroup_id;
	u32 servicegroup_ਸੰਸਕਰਣ;
	ਬਣਤਰ mbox_chan chan;
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਮੰਜ਼ਿਲ_srvgrp_chan(mbox_chan)	\
		container_ਦਾ(mbox_chan, ਬਣਤਰ rpmi_srvgrp_chan, chan);

ਬਣਤਰ rpmi_shmem_mbox_controller {
	/* Driver specific members */
	u32 ਥਾਂ_ਮਾਪ;
	u32 ਕਤਾਰ_ਗਿਣਤੀ;
	u32 p2a_doorbell_sysmsi_ਸੂਚਕਾਂਕ;
	u32 a2p_doorbell_ਮੁੱਲ;
	ਬਣਤਰ rpmi_mb_regs *mb_regs;
	ਬਣਤਰ smq_ਕਤਾਰ_ctx ਕਤਾਰ_ctx_tbl[RPMI_ਕਤਾਰ_IDX_MAX_ਗਿਣਤੀ];
	/* Mailbox framework related members */
	ਬਣਤਰ mbox_controller controller;
	ਬਣਤਰ mbox_chan *ਅਧਾਰ_chan;
	u32 impl_ਸੰਸਕਰਣ;
	u32 impl_id;
	u32 spec_ਸੰਸਕਰਣ;
	u32 plat_info_len;
	ਅੱਖਰ *plat_info;
	ਬਣਤਰ {
		u8 f0_priv_level;
		bool f0_ev_notif_en;
	} ਅਧਾਰ_flags;
};

/**************** Shared Memory Queues Helpers **************/

ਸਥਿਰ bool __smq_queue_full(ਬਣਤਰ smq_ਕਤਾਰ_ctx *qctx)
	MUST_HOLD(&qctx->ਕਤਾਰ_ਤਾਲਾ)
{
	ਵਾਪਸ ((le32_ਮੰਜ਼ਿਲ_cpu(*qctx->tailptr) + 1) % qctx->ਅੰਕ_slots ==
			le32_ਮੰਜ਼ਿਲ_cpu(*qctx->headptr)) ? true : false;
}

ਸਥਿਰ bool __smq_queue_empty(ਬਣਤਰ smq_ਕਤਾਰ_ctx *qctx)
	MUST_HOLD(&qctx->ਕਤਾਰ_ਤਾਲਾ)
{
	ਵਾਪਸ (le32_ਮੰਜ਼ਿਲ_cpu(*qctx->headptr) ==
		le32_ਮੰਜ਼ਿਲ_cpu(*qctx->tailptr)) ? true : false;
}

ਸਥਿਰ ਪੂਰਨ __smq_rx(ਬਣਤਰ smq_ਕਤਾਰ_ctx *qctx, u32 ਥਾਂ_ਮਾਪ,
		    u32 service_ਸਮੂਹ_id, ਬਣਤਰ mbox_xfer *xfer)
	MUST_HOLD(&qctx->ਕਤਾਰ_ਤਾਲਾ)
{
	ਖਾਲੀ *dst, *src;
	ਬਣਤਰ rpmi_ਸੁਨੇਹਾ *ਸੁਨੇਹਾ;
	u32 i, tmp, pos, dlen, msgidn, headidx, tailidx;
	ਬਣਤਰ rpmi_ਸੁਨੇਹਾ_ਦਲੀਲਾਂ *ਦਲੀਲਾਂ = xfer->ਦਲੀਲਾਂ;
	bool no_rx_token = (ਦਲੀਲਾਂ->flags & RPMI_ਸੁਨੇਹਾ_FLAGS_NO_RX_TOKEN) ?
			   true : false;

	/* Rx sanity checks */
	ਜੇ ((ਆਕਾਰ(u32) * ਦਲੀਲਾਂ->rx_endian_ਸ਼ਬਦ) >
	    (ਥਾਂ_ਮਾਪ - ਆਕਾਰ(ਬਣਤਰ rpmi_ਸੁਨੇਹਾ_ਸਿਰਲੇਖ)))
		ਵਾਪਸ SBI_EINVAL;
	ਜੇ ((ਆਕਾਰ(u32) * ਦਲੀਲਾਂ->rx_endian_ਸ਼ਬਦ) > xfer->rx_len)
		ਵਾਪਸ SBI_EINVAL;

	/* There should be some message in the queue */
	ਜੇ (__smq_queue_empty(qctx))
		ਵਾਪਸ SBI_ENOENT;

	/* Get the head/read index and tail/write index */
	headidx = le32_ਮੰਜ਼ਿਲ_cpu(*qctx->headptr);
	tailidx = le32_ਮੰਜ਼ਿਲ_cpu(*qctx->tailptr);

	/*
	 * Compute msgidn expected in the incoming message
	 * NOTE: DOORBELL bit is not expected to be set.
	 */
	msgidn = MAKE_ਸੁਨੇਹਾ_ID(service_ਸਮੂਹ_id, ਦਲੀਲਾਂ->service_id, ਦਲੀਲਾਂ->type);

	/* Find the Rx message with matching token */
	pos = headidx;
	ਜਦੋਂ (pos != tailidx) {
		src = (ਖਾਲੀ *)qctx->ਬਫਰ + (pos * ਥਾਂ_ਮਾਪ);
		ਜੇ ((no_rx_token && GET_ਸੁਨੇਹਾ_ID(src) == msgidn) ||
		    (GET_TOKEN(src) == (xfer->seq & RPMI_ਸੁਨੇਹਾ_TOKEN_ਨਕਾਬ)))
			ਰੋਕੋ;
		pos = (pos + 1) % qctx->ਅੰਕ_slots;
	}
	ਜੇ (pos == tailidx)
		ਵਾਪਸ SBI_ENOENT;

	/* If Rx message is not first message then make it first message */
	ਜੇ (pos != headidx) {
		src = (ਖਾਲੀ *)qctx->ਬਫਰ + (pos * ਥਾਂ_ਮਾਪ);
		dst = (ਖਾਲੀ *)qctx->ਬਫਰ + (headidx * ਥਾਂ_ਮਾਪ);
		ਲਈ (i = 0; i < ਥਾਂ_ਮਾਪ / ਆਕਾਰ(u32); i++) {
			tmp = ((u32 *)dst)[i];
			((u32 *)dst)[i] = ((u32 *)src)[i];
			((u32 *)src)[i] = tmp;
		}
	}

	/* Update rx_token if not available */
	ਸੁਨੇਹਾ = (ਖਾਲੀ *)qctx->ਬਫਰ + (headidx * ਥਾਂ_ਮਾਪ);
	ਜੇ (no_rx_token)
		ਦਲੀਲਾਂ->rx_token = GET_TOKEN(ਸੁਨੇਹਾ);

	/* Extract data from the first message */
	ਜੇ (xfer->rx) {
		ਦਲੀਲਾਂ->rx_ਅੰਕੜੇ_len = dlen = GET_DLEN(ਸੁਨੇਹਾ);
		ਜੇ (dlen > xfer->rx_len)
			dlen = xfer->rx_len;
		src = (ਖਾਲੀ *)ਸੁਨੇਹਾ + ਆਕਾਰ(ਬਣਤਰ rpmi_ਸੁਨੇਹਾ_ਸਿਰਲੇਖ);
		dst = xfer->rx;
		ਲਈ (i = 0; i < ਦਲੀਲਾਂ->rx_endian_ਸ਼ਬਦ; i++)
			((u32 *)dst)[i] = le32_ਮੰਜ਼ਿਲ_cpu(((u32 *)src)[i]);
		dst += ਆਕਾਰ(u32) * ਦਲੀਲਾਂ->rx_endian_ਸ਼ਬਦ;
		src += ਆਕਾਰ(u32) * ਦਲੀਲਾਂ->rx_endian_ਸ਼ਬਦ;
		sbi_memcpy(dst, src,
			xfer->rx_len - (ਆਕਾਰ(u32) * ਦਲੀਲਾਂ->rx_endian_ਸ਼ਬਦ));
	}

	/* Update the head/read index */
	*qctx->headptr = cpu_ਮੰਜ਼ਿਲ_le32(headidx + 1) % qctx->ਅੰਕ_slots;

	/* Make sure updates to head are immediately visible to PuC */
	smp_wmb();

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਪੂਰਨ __smq_tx(ਬਣਤਰ smq_ਕਤਾਰ_ctx *qctx, ਬਣਤਰ rpmi_mb_regs *mb_regs,
		    u32 a2p_doorbell_ਮੁੱਲ, u32 ਥਾਂ_ਮਾਪ, u32 service_ਸਮੂਹ_id,
		    ਬਣਤਰ mbox_xfer *xfer) MUST_HOLD(&qctx->ਕਤਾਰ_ਤਾਲਾ)
{
	u32 i, tailidx;
	ਖਾਲੀ *dst, *src;
	ਬਣਤਰ rpmi_ਸੁਨੇਹਾ_ਸਿਰਲੇਖ ਸਿਰਲੇਖ = { 0 };
	ਬਣਤਰ rpmi_ਸੁਨੇਹਾ_ਦਲੀਲਾਂ *ਦਲੀਲਾਂ = xfer->ਦਲੀਲਾਂ;

	/* Tx sanity checks */
	ਜੇ ((ਆਕਾਰ(u32) * ਦਲੀਲਾਂ->tx_endian_ਸ਼ਬਦ) >
	    (ਥਾਂ_ਮਾਪ - ਆਕਾਰ(ਬਣਤਰ rpmi_ਸੁਨੇਹਾ_ਸਿਰਲੇਖ)))
		ਵਾਪਸ SBI_EINVAL;
	ਜੇ ((ਆਕਾਰ(u32) * ਦਲੀਲਾਂ->tx_endian_ਸ਼ਬਦ) > xfer->tx_len)
		ਵਾਪਸ SBI_EINVAL;

	/* There should be some room in the queue */
	ਜੇ (__smq_queue_full(qctx))
		ਵਾਪਸ SBI_ENOMEM;

	/* Get the tail/write index */
	tailidx = le32_ਮੰਜ਼ਿਲ_cpu(*qctx->tailptr);

	/* Prepare the header to be written into the slot */
	ਸਿਰਲੇਖ.servicegroup_id = cpu_ਮੰਜ਼ਿਲ_le16(service_ਸਮੂਹ_id);
	ਸਿਰਲੇਖ.service_id = ਦਲੀਲਾਂ->service_id;
	ਸਿਰਲੇਖ.flags = ਦਲੀਲਾਂ->type;
	ਸਿਰਲੇਖ.datalen = cpu_ਮੰਜ਼ਿਲ_le16((u16)xfer->tx_len);
	ਸਿਰਲੇਖ.token = cpu_ਮੰਜ਼ਿਲ_le16((u16)xfer->seq);

	/* Write header into the slot */
	dst = (ਅੱਖਰ *)qctx->ਬਫਰ + (tailidx * ਥਾਂ_ਮਾਪ);
	sbi_memcpy(dst, &ਸਿਰਲੇਖ, ਆਕਾਰ(ਸਿਰਲੇਖ));
	dst += ਆਕਾਰ(ਸਿਰਲੇਖ);

	/* Write data into the slot */
	ਜੇ (xfer->tx) {
		src = xfer->tx;
		ਲਈ (i = 0; i < ਦਲੀਲਾਂ->tx_endian_ਸ਼ਬਦ; i++)
			((u32 *)dst)[i] = cpu_ਮੰਜ਼ਿਲ_le32(((u32 *)src)[i]);
		dst += ਆਕਾਰ(u32) * ਦਲੀਲਾਂ->tx_endian_ਸ਼ਬਦ;
		src += ਆਕਾਰ(u32) * ਦਲੀਲਾਂ->tx_endian_ਸ਼ਬਦ;
		sbi_memcpy(dst, src,
			xfer->tx_len - (ਆਕਾਰ(u32) * ਦਲੀਲਾਂ->tx_endian_ਸ਼ਬਦ));
	}

	/* Make sure queue chanages are visible to PuC before updating tail */
	smp_wmb();

	/* Update the tail/write index */
	*qctx->tailptr = cpu_ਮੰਜ਼ਿਲ_le32(tailidx + 1) % qctx->ਅੰਕ_slots;

	/* Ring the RPMI doorbell if present */
	ਜੇ (mb_regs)
		writel(a2p_doorbell_ਮੁੱਲ, &mb_regs->db_reg);

	ਵਾਪਸ SBI_OK;
}

ਸਥਿਰ ਪੂਰਨ smq_rx(ਬਣਤਰ rpmi_shmem_mbox_controller *mctl,
		  u32 ਕਤਾਰ_id, u32 service_ਸਮੂਹ_id, ਬਣਤਰ mbox_xfer *xfer)
{
	ਪੂਰਨ ret, rxretry = 0;
	ਬਣਤਰ smq_ਕਤਾਰ_ctx *qctx;

	ਜੇ (mctl->ਕਤਾਰ_ਗਿਣਤੀ < ਕਤਾਰ_id) {
		sbi_printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		ਵਾਪਸ SBI_EINVAL;
	}
	qctx = &mctl->ਕਤਾਰ_ctx_tbl[ਕਤਾਰ_id];

	/*
	 * Once the timeout happens and call this function is returned
	 * to the client then there is no way to deliver the response
	 * message after that if it comes later.
	 *
	 * REVISIT: In complete timeout duration how much duration
	 * it should wait(delay) before recv retry. udelay or mdelay
	 */
	ਕਰੋ {
		spin_ਤਾਲਾ(&qctx->ਕਤਾਰ_ਤਾਲਾ);
		ret = __smq_rx(qctx, mctl->ਥਾਂ_ਮਾਪ, service_ਸਮੂਹ_id, xfer);
		spin_unlock(&qctx->ਕਤਾਰ_ਤਾਲਾ);
		ਜੇ (!ret)
			ਵਾਪਸ 0;

		sbi_timer_mdelay(1);
		rxretry += 1;
	} ਜਦੋਂ (rxretry < xfer->rx_timeout);

	ਵਾਪਸ SBI_ETIMEDOUT;
}

ਸਥਿਰ ਪੂਰਨ smq_tx(ਬਣਤਰ rpmi_shmem_mbox_controller *mctl,
		  u32 ਕਤਾਰ_id, u32 service_ਸਮੂਹ_id, ਬਣਤਰ mbox_xfer *xfer)
{
	ਪੂਰਨ ret, txretry = 0;
	ਬਣਤਰ smq_ਕਤਾਰ_ctx *qctx;

	ਜੇ (mctl->ਕਤਾਰ_ਗਿਣਤੀ < ਕਤਾਰ_id) {
		sbi_printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		ਵਾਪਸ SBI_EINVAL;
	}
	qctx = &mctl->ਕਤਾਰ_ctx_tbl[ਕਤਾਰ_id];

	/*
	 * Ignoring the tx timeout since in RPMI has no mechanism
	 * with which other side can let know about the reception of
	 * message which marks as tx complete. For RPMI tx complete is
	 * marked as done when message in successfully copied in queue.
	 *
	 * REVISIT: In complete timeout duration how much duration
	 * it should wait(delay) before send retry. udelay or mdelay
	 */
	ਕਰੋ {
		spin_ਤਾਲਾ(&qctx->ਕਤਾਰ_ਤਾਲਾ);
		ret = __smq_tx(qctx, mctl->mb_regs, mctl->a2p_doorbell_ਮੁੱਲ,
				mctl->ਥਾਂ_ਮਾਪ, service_ਸਮੂਹ_id, xfer);
		spin_unlock(&qctx->ਕਤਾਰ_ਤਾਲਾ);
		ਜੇ (!ret)
			ਵਾਪਸ 0;

		sbi_timer_mdelay(1);
		txretry += 1;
	} ਜਦੋਂ (txretry < xfer->tx_timeout);

	ਵਾਪਸ SBI_ETIMEDOUT;
}

ਸਥਿਰ ਪੂਰਨ rpmi_get_platform_info(ਬਣਤਰ rpmi_shmem_mbox_controller *mctl)
{
	ਪੂਰਨ ret = SBI_OK;

	/**
	 * platform string can occupy max possible size
	 * max possible space in the message data as
	 * per the format
	 */
	ਬਣਤਰ rpmi_ਅਧਾਰ_get_platform_info_resp *resp =
			sbi_zalloc(RPMI_ਸੁਨੇਹਾ_ਅੰਕੜੇ_ਮਾਪ(mctl->ਥਾਂ_ਮਾਪ));
	ਜੇ (!resp)
		ਵਾਪਸ SBI_ENOMEM;

	ret = rpmi_ਸਾਧਾਰਨ_request_with_ਸਥਿਤੀ(mctl->ਅਧਾਰ_chan,
				 RPMI_ਅਧਾਰ_SRV_GET_PLATFORM_INFO,
				 ਨਿਰਾ, 0, 0,
				 resp,
				 RPMI_ਸੁਨੇਹਾ_ਅੰਕੜੇ_ਮਾਪ(mctl->ਥਾਂ_ਮਾਪ)/4,
				 RPMI_ਸੁਨੇਹਾ_ਅੰਕੜੇ_ਮਾਪ(mctl->ਥਾਂ_ਮਾਪ)/4);
	ਜੇ (ret)
		ਜਾਓ fail_free_resp;

	mctl->plat_info_len = resp->plat_info_len;
	mctl->plat_info = sbi_zalloc(mctl->plat_info_len);
	ਜੇ (!mctl->plat_info) {
		ret = SBI_ENOMEM;
		ਜਾਓ fail_free_resp;
	}

	sbi_strncpy(mctl->plat_info, resp->plat_info, mctl->plat_info_len);

fail_free_resp:
	sbi_free(resp);
	ਵਾਪਸ ret;
}

ਸਥਿਰ ਪੂਰਨ smq_ਅਧਾਰ_get_two_u32(ਬਣਤਰ rpmi_shmem_mbox_controller *mctl,
				u32 service_id, u32 *inarg, u32 *outvals)
{
	ਵਾਪਸ rpmi_ਸਾਧਾਰਨ_request_with_ਸਥਿਤੀ(
			mctl->ਅਧਾਰ_chan, service_id,
			inarg, (inarg) ? 1 : 0, (inarg) ? 1 : 0,
			outvals, 2, 2);
}

/**************** Mailbox Controller Functions **************/

ਸਥਿਰ ਪੂਰਨ rpmi_shmem_mbox_xfer(ਬਣਤਰ mbox_chan *chan, ਬਣਤਰ mbox_xfer *xfer)
{
	ਪੂਰਨ ret;
	u32 tx_qid = 0, rx_qid = 0;
	ਬਣਤਰ rpmi_shmem_mbox_controller *mctl =
			container_ਦਾ(chan->mbox,
				     ਬਣਤਰ rpmi_shmem_mbox_controller,
				     controller);
	ਬਣਤਰ rpmi_srvgrp_chan *srvgrp_chan = ਮੰਜ਼ਿਲ_srvgrp_chan(chan);

	ਬਣਤਰ rpmi_ਸੁਨੇਹਾ_ਦਲੀਲਾਂ *ਦਲੀਲਾਂ = xfer->ਦਲੀਲਾਂ;
	bool do_tx = (ਦਲੀਲਾਂ->flags & RPMI_ਸੁਨੇਹਾ_FLAGS_NO_TX) ? false : true;
	bool do_rx = (ਦਲੀਲਾਂ->flags & RPMI_ਸੁਨੇਹਾ_FLAGS_NO_RX) ? false : true;

	ਜੇ (!do_tx && !do_rx)
		ਵਾਪਸ SBI_EINVAL;

	ਚੋਣ (ਦਲੀਲਾਂ->type) {
	ਹਾਲਤ RPMI_ਸੁਨੇਹਾ_ਸਾਧਾਰਨ_REQUEST:
		ਜੇ (do_tx && do_rx) {
			tx_qid = RPMI_ਕਤਾਰ_IDX_A2_P_REQ;
			rx_qid = RPMI_ਕਤਾਰ_IDX_P2_A_ACK;
		} ਨਹੀਂਤਾਂ ਜੇ (do_tx) {
			tx_qid = RPMI_ਕਤਾਰ_IDX_A2_P_REQ;
		} ਨਹੀਂਤਾਂ ਜੇ (do_rx) {
			rx_qid = RPMI_ਕਤਾਰ_IDX_P2_A_REQ;
		}
		ਰੋਕੋ;
	ਹਾਲਤ RPMI_ਸੁਨੇਹਾ_POSTED_REQUEST:
		ਜੇ (do_tx && do_rx)
			ਵਾਪਸ SBI_EINVAL;
		ਜੇ (do_tx) {
			tx_qid = RPMI_ਕਤਾਰ_IDX_A2_P_REQ;
		} ਨਹੀਂਤਾਂ {
			rx_qid = RPMI_ਕਤਾਰ_IDX_P2_A_REQ;
		}
		ਰੋਕੋ;
	ਹਾਲਤ RPMI_ਸੁਨੇਹਾ_ACKNOWLDGEMENT:
		ਜੇ (do_tx && do_rx)
			ਵਾਪਸ SBI_EINVAL;
		ਜੇ (do_tx) {
			tx_qid = RPMI_ਕਤਾਰ_IDX_A2_P_ACK;
		} ਨਹੀਂਤਾਂ {
			rx_qid = RPMI_ਕਤਾਰ_IDX_P2_A_ACK;
		}
		ਰੋਕੋ;
	ਮੂਲ:
		ਵਾਪਸ SBI_ENOTSUPP;
	}

	ਜੇ (do_tx) {
		ret = smq_tx(mctl, tx_qid, srvgrp_chan->servicegroup_id, xfer);
		ਜੇ (ret)
			ਵਾਪਸ ret;
	}

	ਜੇ (do_rx) {
		ret = smq_rx(mctl, rx_qid, srvgrp_chan->servicegroup_id, xfer);
		ਜੇ (ret)
			ਵਾਪਸ ret;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ rpmi_shmem_mbox_get_attribute(ਬਣਤਰ mbox_chan *chan,
					 ਪੂਰਨ attr_id, ਖਾਲੀ *ਬਾਹਰਲਾ_ਮੁੱਲ)
{
	ਬਣਤਰ rpmi_shmem_mbox_controller *mctl =
			container_ਦਾ(chan->mbox,
				     ਬਣਤਰ rpmi_shmem_mbox_controller,
				     controller);
	ਬਣਤਰ rpmi_srvgrp_chan *srvgrp_chan = ਮੰਜ਼ਿਲ_srvgrp_chan(chan);

	ਚੋਣ (attr_id) {
	ਹਾਲਤ RPMI_CHANNEL_ATTR_PROTOCOL_ਸੰਸਕਰਣ:
		*((u32 *)ਬਾਹਰਲਾ_ਮੁੱਲ) = mctl->spec_ਸੰਸਕਰਣ;
		ਰੋਕੋ;
	ਹਾਲਤ RPMI_CHANNEL_ATTR_MAX_ਅੰਕੜੇ_LEN:
		*((u32 *)ਬਾਹਰਲਾ_ਮੁੱਲ) = RPMI_ਸੁਨੇਹਾ_ਅੰਕੜੇ_ਮਾਪ(mctl->ਥਾਂ_ਮਾਪ);
		ਰੋਕੋ;
	ਹਾਲਤ RPMI_CHANNEL_ATTR_P2_A_DOORBELL_SYSMSI_ਸੂਚਕਾਂਕ:
		*((u32 *)ਬਾਹਰਲਾ_ਮੁੱਲ) = mctl->p2a_doorbell_sysmsi_ਸੂਚਕਾਂਕ;
		ਰੋਕੋ;
	ਹਾਲਤ RPMI_CHANNEL_ATTR_TX_TIMEOUT:
		*((u32 *)ਬਾਹਰਲਾ_ਮੁੱਲ) = RPMI_DEF_TX_TIMEOUT;
		ਰੋਕੋ;
	ਹਾਲਤ RPMI_CHANNEL_ATTR_RX_TIMEOUT:
		*((u32 *)ਬਾਹਰਲਾ_ਮੁੱਲ) = RPMI_DEF_RX_TIMEOUT;
		ਰੋਕੋ;
	ਹਾਲਤ RPMI_CHANNEL_ATTR_SERVICEGROUP_ID:
		*((u32 *)ਬਾਹਰਲਾ_ਮੁੱਲ) = srvgrp_chan->servicegroup_id;
		ਰੋਕੋ;
	ਹਾਲਤ RPMI_CHANNEL_ATTR_SERVICEGROUP_ਸੰਸਕਰਣ:
		*((u32 *)ਬਾਹਰਲਾ_ਮੁੱਲ) = srvgrp_chan->servicegroup_ਸੰਸਕਰਣ;
		ਰੋਕੋ;
	ਹਾਲਤ RPMI_CHANNEL_ATTR_IMPL_ID:
		*((u32 *)ਬਾਹਰਲਾ_ਮੁੱਲ) = mctl->impl_id;
		ਰੋਕੋ;
	ਹਾਲਤ RPMI_CHANNEL_ATTR_IMPL_ਸੰਸਕਰਣ:
		*((u32 *)ਬਾਹਰਲਾ_ਮੁੱਲ) = mctl->impl_ਸੰਸਕਰਣ;
		ਰੋਕੋ;
	ਮੂਲ:
		ਵਾਪਸ SBI_ENOTSUPP;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਬਣਤਰ mbox_chan *rpmi_shmem_mbox_request_chan(
						ਬਣਤਰ mbox_controller *mbox,
						u32 *chan_ਦਲੀਲਾਂ)
{
	ਪੂਰਨ ret;
	u32 tval[2] = { 0 };
	ਬਣਤਰ rpmi_srvgrp_chan *srvgrp_chan;
	ਬਣਤਰ rpmi_shmem_mbox_controller *mctl =
			container_ਦਾ(mbox,
				     ਬਣਤਰ rpmi_shmem_mbox_controller,
				     controller);

	/* Service group id not defined or in reserved range is invalid */
	ਜੇ (chan_ਦਲੀਲਾਂ[0] >= RPMI_SRVGRP_ID_MAX_ਗਿਣਤੀ &&
		chan_ਦਲੀਲਾਂ[0] <= RPMI_SRVGRP_RESERVE_ਸਿਰਾ)
		ਵਾਪਸ ਨਿਰਾ;

	/* Base serivce group is always present so probe other groups */
	ਜੇ (chan_ਦਲੀਲਾਂ[0] != RPMI_SRVGRP_ਅਧਾਰ) {
		/* Probe service group */
		ret = smq_ਅਧਾਰ_get_two_u32(mctl,
					   RPMI_ਅਧਾਰ_SRV_PROBE_SERVICE_ਸਮੂਹ,
					   chan_ਦਲੀਲਾਂ, tval);
		ਜੇ (ret || !tval[1])
			ਵਾਪਸ ਨਿਰਾ;
	}

	srvgrp_chan = sbi_zalloc(ਆਕਾਰ(*srvgrp_chan));
	ਜੇ (!srvgrp_chan)
		ਵਾਪਸ ਨਿਰਾ;

	srvgrp_chan->servicegroup_id = chan_ਦਲੀਲਾਂ[0];
	srvgrp_chan->servicegroup_ਸੰਸਕਰਣ = tval[1];

	ਵਾਪਸ &srvgrp_chan->chan;
}

ਸਥਿਰ ਖਾਲੀ rpmi_shmem_mbox_free_chan(ਬਣਤਰ mbox_controller *mbox,
				      ਬਣਤਰ mbox_chan *chan)
{
	ਬਣਤਰ rpmi_srvgrp_chan *srvgrp_chan = ਮੰਜ਼ਿਲ_srvgrp_chan(chan);
	sbi_free(srvgrp_chan);
}

ਬਾਹਰੀ ਬਣਤਰ fdt_mailbox fdt_mailbox_rpmi_shmem;

ਸਥਿਰ ਪੂਰਨ rpmi_shmem_transport_init(ਬਣਤਰ rpmi_shmem_mbox_controller *mctl,
				     ਅਚਲ ਖਾਲੀ *fdt,
				     ਪੂਰਨ nodeoff) NO_THREAD_SAFETY_ANALYSIS
{
	ਅਚਲ ਅੱਖਰ *name;
	ਅਚਲ fdt32_t *prop;
	ਪੂਰਨ ਗਿਣਤੀ_2, len, ret, qid;
	uint64_t reg_addr, reg_ਮਾਪ;
	ਬਣਤਰ smq_ਕਤਾਰ_ctx *qctx;

	ret = fdt_ਗੰਢ_check_compatible(fdt, nodeoff,
					"riscv,rpmi-shmem-mbox");
	ਜੇ (ret)
		ਵਾਪਸ ret;

	/* get queue slot size in bytes */
	prop = fdt_getprop(fdt, nodeoff, "riscv,slot-size", &len);
	ਜੇ (!prop)
		ਵਾਪਸ SBI_ENOENT;

	mctl->ਥਾਂ_ਮਾਪ = fdt32_ਮੰਜ਼ਿਲ_cpu(*prop);
	ਜੇ (mctl->ਥਾਂ_ਮਾਪ < RPMI_ਥਾਂ_ਮਾਪ_MIN) {
		sbi_printf("%s: slot_size < mimnum required message size\n",
			   __func__);
		mctl->ਥਾਂ_ਮਾਪ = RPMI_ਥਾਂ_ਮਾਪ_MIN;
	}

	/* get p2a doorbell system MSI index */
	prop = fdt_getprop(fdt, nodeoff, "riscv,p2a-doorbell-sysmsi-index", &len);
	mctl->p2a_doorbell_sysmsi_ਸੂਚਕਾਂਕ = prop ? fdt32_ਮੰਜ਼ਿਲ_cpu(*prop) : -1U;

	/* get a2p doorbell value */
	prop = fdt_getprop(fdt, nodeoff, "riscv,a2p-doorbell-value", &len);
	mctl->a2p_doorbell_ਮੁੱਲ = prop ? fdt32_ਮੰਜ਼ਿਲ_cpu(*prop) : 1;

	/*
	 * queue names count is taken as the number of queues
	 * supported which make it mandatory to provide the
	 * name of the queue.
	 */
	ਗਿਣਤੀ_2 = fdt_stringlist_ਗਿਣਤੀ(fdt, nodeoff, "reg-names");
	ਜੇ (ਗਿਣਤੀ_2 < 0 ||
	    ਗਿਣਤੀ_2 > (RPMI_ਕਤਾਰ_IDX_MAX_ਗਿਣਤੀ + RPMI_REG_IDX_MAX_ਗਿਣਤੀ))
		ਵਾਪਸ SBI_EINVAL;

	mctl->ਕਤਾਰ_ਗਿਣਤੀ = ਗਿਣਤੀ_2 - RPMI_REG_IDX_MAX_ਗਿਣਤੀ;

	/* parse all queues and populate queues context structure */
	ਲਈ (qid = 0; qid < mctl->ਕਤਾਰ_ਗਿਣਤੀ; qid++) {
		qctx = &mctl->ਕਤਾਰ_ctx_tbl[qid];

		/* get each queue share-memory base address and size*/
		ret = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoff, qid,
					     &reg_addr, &reg_ਮਾਪ);
		ਜੇ (ret < 0 || !reg_addr || !reg_ਮਾਪ)
			ਵਾਪਸ SBI_ENOENT;

		ret = sbi_domain_ਜੜ੍ਹ_ਜੋੜੋ_memrange(reg_addr, reg_ਮਾਪ, reg_ਮਾਪ,
						   (SBI_DOMAIN_MEMREGION_MMIO |
						    SBI_DOMAIN_MEMREGION_M_READABLE |
						    SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ਜੇ (ret)
			ਵਾਪਸ ret;

		/* calculate number of slots in each queue */
		qctx->ਅੰਕ_slots =
			(reg_ਮਾਪ - (mctl->ਥਾਂ_ਮਾਪ * RPMI_ਕਤਾਰ_ਸਿਰਲੇਖ_SLOTS)) / mctl->ਥਾਂ_ਮਾਪ;

		/* setup queue pointers */
		qctx->headptr = ((ਖਾਲੀ *)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)reg_addr) +
				RPMI_ਕਤਾਰ_ਸਿਰ_ਥਾਂ * mctl->ਥਾਂ_ਮਾਪ;
		qctx->tailptr = ((ਖਾਲੀ *)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)reg_addr) +
				RPMI_ਕਤਾਰ_ਪੂਛ_ਥਾਂ * mctl->ਥਾਂ_ਮਾਪ;
		qctx->ਬਫਰ = ((ਖਾਲੀ *)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)reg_addr) +
				RPMI_ਕਤਾਰ_ਸਿਰਲੇਖ_SLOTS * mctl->ਥਾਂ_ਮਾਪ;

		/* get the queue name */
		name = fdt_stringlist_get(fdt, nodeoff, "reg-names",
					  qid, &len);
		ਜੇ (!name || (name && len < 0))
			ਵਾਪਸ len;

		sbi_memcpy(qctx->name, name, len);

		/* store the index as queue_id */
		qctx->ਕਤਾਰ_id = qid;

		SPIN_ਤਾਲਾ_INIT(qctx->ਕਤਾਰ_ਤਾਲਾ);
	}

	/* get the a2p-doorbell property name */
	name = fdt_stringlist_get(fdt, nodeoff, "reg-names", qid, &len);
	ਜੇ (!name || (name && len < 0))
		ਵਾਪਸ len;

	/* fetch doorbell register address*/
	ret = fdt_get_ਗੰਢ_addr_ਮਾਪ(fdt, nodeoff, qid, &reg_addr,
				       &reg_ਮਾਪ);
	ਜੇ (!ret && !(ਲੜੀ_ਤੁਲਨਾ(name, "a2p-doorbell", ਲੜੀ_ਲੰਬਾਈ("a2p-doorbell")))) {
		mctl->mb_regs = (ਖਾਲੀ *)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)reg_addr;
		ret = sbi_domain_ਜੜ੍ਹ_ਜੋੜੋ_memrange(reg_addr, reg_ਮਾਪ, reg_ਮਾਪ,
						   (SBI_DOMAIN_MEMREGION_MMIO |
						    SBI_DOMAIN_MEMREGION_M_READABLE |
						    SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ਜੇ (ret)
			ਵਾਪਸ ret;
	}

	ਵਾਪਸ SBI_SUCCESS;
}

ਸਥਿਰ ਪੂਰਨ rpmi_shmem_mbox_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
				ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਬਣਤਰ rpmi_ਅਧਾਰ_get_ਗੁਣ_resp resp;
	ਬਣਤਰ rpmi_shmem_mbox_controller *mctl;
	ਬਣਤਰ rpmi_srvgrp_chan *ਅਧਾਰ_srvgrp;
	u32 tval[2], ਦਲੀਲਾਂ[1];
	ਪੂਰਨ ret = 0;

	mctl = sbi_zalloc(ਆਕਾਰ(*mctl));
	ਜੇ (!mctl)
		ਵਾਪਸ SBI_ENOMEM;

	/* Initialization transport from device tree */
	ret = rpmi_shmem_transport_init(mctl, fdt, nodeoff);
	ਜੇ (ret)
		ਜਾਓ fail_free_controller;

	/* Register mailbox controller */
	mctl->controller.id = nodeoff;
	mctl->controller.max_xfer_len =
			mctl->ਥਾਂ_ਮਾਪ - ਆਕਾਰ(ਬਣਤਰ rpmi_ਸੁਨੇਹਾ_ਸਿਰਲੇਖ);
	mctl->controller.driver = &fdt_mailbox_rpmi_shmem;
	mctl->controller.request_chan = rpmi_shmem_mbox_request_chan;
	mctl->controller.free_chan = rpmi_shmem_mbox_free_chan;
	mctl->controller.xfer = rpmi_shmem_mbox_xfer;
	mctl->controller.get_attribute = rpmi_shmem_mbox_get_attribute;
	ret = mbox_controller_ਜੋੜੋ(&mctl->controller);
	ਜੇ (ret)
		ਜਾਓ fail_free_controller;

	/* Request base service group channel */
	tval[0] = RPMI_SRVGRP_ਅਧਾਰ;
	mctl->ਅਧਾਰ_chan = mbox_controller_request_chan(&mctl->controller,
							tval);
	ਜੇ (!mctl->ਅਧਾਰ_chan) {
		ret = SBI_ENOENT;
		ਜਾਓ fail_remove_controller;
	}

	/* Update base service group version */
	ਅਧਾਰ_srvgrp = ਮੰਜ਼ਿਲ_srvgrp_chan(mctl->ਅਧਾਰ_chan);
	ਦਲੀਲਾਂ[0] = RPMI_SRVGRP_ਅਧਾਰ;
	ret = smq_ਅਧਾਰ_get_two_u32(mctl, RPMI_ਅਧਾਰ_SRV_PROBE_SERVICE_ਸਮੂਹ,
				   &ਦਲੀਲਾਂ[0], tval);
	ਜੇ (ret)
		ਜਾਓ fail_free_chan;
	ਅਧਾਰ_srvgrp->servicegroup_ਸੰਸਕਰਣ = tval[1];
	ਜੇ (ਅਧਾਰ_srvgrp->servicegroup_ਸੰਸਕਰਣ < RPMI_ਅਧਾਰ_ਸੰਸਕਰਣ_MIN) {
		ret = SBI_EINVAL;
		ਜਾਓ fail_free_chan;
	}

	/* Get implementation id */
	ret = smq_ਅਧਾਰ_get_two_u32(mctl,
				   RPMI_ਅਧਾਰ_SRV_GET_IMPLEMENTATION_ਸੰਸਕਰਣ,
				   ਨਿਰਾ, tval);
	ਜੇ (ret)
		ਜਾਓ fail_free_chan;
	mctl->impl_ਸੰਸਕਰਣ = tval[1];

	/* Get implementation version */
	ret = smq_ਅਧਾਰ_get_two_u32(mctl, RPMI_ਅਧਾਰ_SRV_GET_IMPLEMENTATION_IDN,
				   ਨਿਰਾ, tval);
	ਜੇ (ret)
		ਜਾਓ fail_free_chan;
	mctl->impl_id = tval[1];

	/* Get specification version */
	ret = smq_ਅਧਾਰ_get_two_u32(mctl, RPMI_ਅਧਾਰ_SRV_GET_SPEC_ਸੰਸਕਰਣ,
				   ਨਿਰਾ, tval);
	ਜੇ (ret)
		ਜਾਓ fail_free_chan;
	mctl->spec_ਸੰਸਕਰਣ = tval[1];
	ਜੇ (mctl->spec_ਸੰਸਕਰਣ < RPMI_ਅਧਾਰ_ਸੰਸਕਰਣ_MIN ||
	    mctl->spec_ਸੰਸਕਰਣ != ਅਧਾਰ_srvgrp->servicegroup_ਸੰਸਕਰਣ) {
		ret = SBI_EINVAL;
		ਜਾਓ fail_free_chan;
	}

	/* Get optional features implementation flags */
	ret = rpmi_ਸਾਧਾਰਨ_request_with_ਸਥਿਤੀ(
			mctl->ਅਧਾਰ_chan, RPMI_ਅਧਾਰ_SRV_GET_ਗੁਣ,
			ਨਿਰਾ, 0, 0,
			&resp, rpmi_u32_ਗਿਣਤੀ(resp), rpmi_u32_ਗਿਣਤੀ(resp));
	ਜੇ (ret)
		ਜਾਓ fail_free_chan;

	/* 1: M-mode, 0: S-mode */
	mctl->ਅਧਾਰ_flags.f0_priv_level =
			resp.f0 & RPMI_ਅਧਾਰ_FLAGS_F0_PRIVILEGE ? 1 : 0;
	/* 1: Supported, 0: Not Supported */
	mctl->ਅਧਾਰ_flags.f0_ev_notif_en =
			resp.f0 & RPMI_ਅਧਾਰ_FLAGS_F0_EV_NOTIFY ? 1 : 0;

	/* We only use M-mode RPMI context in OpenSBI */
	ਜੇ (!mctl->ਅਧਾਰ_flags.f0_priv_level) {
		ret = SBI_ENODEV;
		ਜਾਓ fail_free_chan;
	}

	/*
	 * Continue without platform information string if not
	 * available or if an error is encountered while fetching
	 */
	rpmi_get_platform_info(mctl);

	ਵਾਪਸ 0;

fail_free_chan:
	mbox_controller_free_chan(mctl->ਅਧਾਰ_chan);
fail_remove_controller:
	mbox_controller_remove(&mctl->controller);
fail_free_controller:
	sbi_free(mctl);
	ਵਾਪਸ ret;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ rpmi_shmem_mbox_ਮੇਲ[] = {
	{ .compatible = "riscv,rpmi-shmem-mbox" },
	{ },
};

ਬਣਤਰ fdt_mailbox fdt_mailbox_rpmi_shmem = {
	.driver = {
		.ਮੇਲ_ਸਾਰਣੀ = rpmi_shmem_mbox_ਮੇਲ,
		.init = rpmi_shmem_mbox_init,
	},
	.xlate = fdt_mailbox_simple_xlate,
};
