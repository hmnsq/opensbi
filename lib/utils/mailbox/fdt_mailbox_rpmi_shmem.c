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
#ವ್ಯಾಖ್ಯಾನಿಸು RPMI_ತಳ_ಆವೃತ್ತಿ_MIN		RPMI_ಆವೃತ್ತಿ(1, 0)

/**************** RPMI Transport Structures and Macros ***********/

#ವ್ಯಾಖ್ಯಾನಿಸು GET_SERVICEGROUP_ID(ಸಂದೇಶ)		\
({						\
	ರಚನೆ rpmi_ಸಂದೇಶ *mbuf = ಸಂದೇಶ;	\
	le16_ಗಮ್ಯ_cpu(mbuf->ಶೀರ್ಷಿಕೆ.servicegroup_id);\
})

#ವ್ಯಾಖ್ಯಾನಿಸು GET_SERVICE_ID(ಸಂದೇಶ)			\
({						\
	ರಚನೆ rpmi_ಸಂದೇಶ *mbuf = ಸಂದೇಶ;	\
	mbuf->ಶೀರ್ಷಿಕೆ.service_id;		\
})

#ವ್ಯಾಖ್ಯಾನಿಸು GET_FLAGS(ಸಂದೇಶ)				\
({						\
	ರಚನೆ rpmi_ಸಂದೇಶ *mbuf = ಸಂದೇಶ;	\
	mbuf->ಶೀರ್ಷಿಕೆ.flags;			\
})

#ವ್ಯಾಖ್ಯಾನಿಸು GET_ಸಂದೇಶ_ID(ಸಂದೇಶ)			\
({						\
	ರಚನೆ rpmi_ಸಂದೇಶ *mbuf = ಸಂದೇಶ;	\
	((u32)mbuf->ಶೀರ್ಷಿಕೆ.flags << (RPMI_ಸಂದೇಶ_FLAGS_ಸ್ಥಾನಾಂತರ * 8)) | \
	((u32)mbuf->ಶೀರ್ಷಿಕೆ.service_id << (RPMI_ಸಂದೇಶ_SERVICE_ID_ಸ್ಥಾನಾಂತರ * 8)) | \
	((u32)le16_ಗಮ್ಯ_cpu(mbuf->ಶೀರ್ಷಿಕೆ.servicegroup_id)); \
})

#ವ್ಯಾಖ್ಯಾನಿಸು MAKE_ಸಂದೇಶ_ID(__group_id, __service_id, __flags)	\
({						\
	u32 __ret = 0;				\
	__ret |= (u32)(__group_id) << (RPMI_ಸಂದೇಶ_SERVICEGROUP_ID_ಸ್ಥಾನಾಂತರ * 8); \
	__ret |= (u32)(__service_id) << (RPMI_ಸಂದೇಶ_SERVICE_ID_ಸ್ಥಾನಾಂತರ * 8); \
	__ret |= (u32)(__flags) << (RPMI_ಸಂದೇಶ_FLAGS_ಸ್ಥಾನಾಂತರ * 8); \
	__ret;					\
})

#ವ್ಯಾಖ್ಯಾನಿಸು GET_DLEN(ಸಂದೇಶ)				\
({						\
	ರಚನೆ rpmi_ಸಂದೇಶ *mbuf = ಸಂದೇಶ;	\
	le16_ಗಮ್ಯ_cpu(mbuf->ಶೀರ್ಷಿಕೆ.datalen);	\
})

#ವ್ಯಾಖ್ಯಾನಿಸು GET_TOKEN(ಸಂದೇಶ)				\
({						\
	ರಚನೆ rpmi_ಸಂದೇಶ *mbuf = ಸಂದೇಶ;	\
	le16_ಗಮ್ಯ_cpu(mbuf->ಶೀರ್ಷಿಕೆ.token);	\
})

#ವ್ಯಾಖ್ಯಾನಿಸು GET_ಸಂದೇಶ_TYPE(ಸಂದೇಶ)						\
({									\
	uint8_t flags = *((uint8_t *)ಸಂದೇಶ + RPMI_ಸಂದೇಶ_FLAGS_ಸ್ಥಾನಾಂತರ);	\
	((flags & RPMI_ಸಂದೇಶ_FLAGS_TYPE) >> RPMI_ಸಂದೇಶ_FLAGS_TYPE_POS));	\
})

ಎಣಿಕೆ rpmi_ಸರತಿ_type {
	RPMI_ಸರತಿ_TYPE_REQ = 0,
	RPMI_ಸರತಿ_TYPE_ACK = 1,
};

ಎಣಿಕೆ rpmi_ಸರತಿ_idx {
	RPMI_ಸರತಿ_IDX_A2_P_REQ = 0,
	RPMI_ಸರತಿ_IDX_P2_A_ACK = 1,
	RPMI_ಸರತಿ_IDX_P2_A_REQ = 2,
	RPMI_ಸರತಿ_IDX_A2_P_ACK = 3,
	RPMI_ಸರತಿ_IDX_MAX_ಎಣಿಕೆ,
};

ಎಣಿಕೆ rpmi_reg_idx {
	RPMI_REG_IDX_DB_REG = 0, /* Doorbell register */
	RPMI_REG_IDX_MAX_ಎಣಿಕೆ,
};

/** Mailbox registers */
ರಚನೆ rpmi_mb_regs {
	/* doorbell from AP -> PuC*/
	ಅಸ್ಥಿರ le32_t db_reg;
} __packed;

/** Single Queue Context Structure */
ರಚನೆ smq_ಸರತಿ_ctx {
	u32 ಸರತಿ_id;
	u32 ಅಂಕೆ_slots;
	spinlock_t ಸರತಿ_ಬೀಗ;
	/* Type of queue - REQ or ACK */
	ಎಣಿಕೆ rpmi_ಸರತಿ_type ಸರತಿ_type;
	/* Pointers to the queue shared memory */
	ಅಸ್ಥಿರ le32_t *headptr PT_GUARDED_BY(&ಸರತಿ_ಬೀಗ);
	ಅಸ್ಥಿರ le32_t *tailptr PT_GUARDED_BY(&ಸರತಿ_ಬೀಗ);
	ಅಸ್ಥಿರ uint8_t *ಬಫರ್ PT_GUARDED_BY(&ಸರತಿ_ಬೀಗ);
	/* Name of the queue */
	ಅಕ್ಷರ name[RPMI_NAME_CHARS_MAX];
};

ರಚನೆ rpmi_srvgrp_chan {
	u32 servicegroup_id;
	u32 servicegroup_ಆವೃತ್ತಿ;
	ರಚನೆ mbox_chan chan;
};

#ವ್ಯಾಖ್ಯಾನಿಸು ಗಮ್ಯ_srvgrp_chan(mbox_chan)	\
		container_ದ(mbox_chan, ರಚನೆ rpmi_srvgrp_chan, chan);

ರಚನೆ rpmi_shmem_mbox_controller {
	/* Driver specific members */
	u32 ಸ್ಥಾನ_ಗಾತ್ರ;
	u32 ಸರತಿ_ಎಣಿಕೆ;
	u32 p2a_doorbell_sysmsi_ಸೂಚ್ಯಂಕ;
	u32 a2p_doorbell_ಮೌಲ್ಯ;
	ರಚನೆ rpmi_mb_regs *mb_regs;
	ರಚನೆ smq_ಸರತಿ_ctx ಸರತಿ_ctx_tbl[RPMI_ಸರತಿ_IDX_MAX_ಎಣಿಕೆ];
	/* Mailbox framework related members */
	ರಚನೆ mbox_controller controller;
	ರಚನೆ mbox_chan *ತಳ_chan;
	u32 impl_ಆವೃತ್ತಿ;
	u32 impl_id;
	u32 spec_ಆವೃತ್ತಿ;
	u32 plat_info_len;
	ಅಕ್ಷರ *plat_info;
	ರಚನೆ {
		u8 f0_priv_level;
		bool f0_ev_notif_en;
	} ತಳ_flags;
};

/**************** Shared Memory Queues Helpers **************/

ಸ್ಥಿರ bool __smq_queue_full(ರಚನೆ smq_ಸರತಿ_ctx *qctx)
	MUST_HOLD(&qctx->ಸರತಿ_ಬೀಗ)
{
	ಹಿಂತಿರುಗಿ ((le32_ಗಮ್ಯ_cpu(*qctx->tailptr) + 1) % qctx->ಅಂಕೆ_slots ==
			le32_ಗಮ್ಯ_cpu(*qctx->headptr)) ? true : false;
}

ಸ್ಥಿರ bool __smq_queue_empty(ರಚನೆ smq_ಸರತಿ_ctx *qctx)
	MUST_HOLD(&qctx->ಸರತಿ_ಬೀಗ)
{
	ಹಿಂತಿರುಗಿ (le32_ಗಮ್ಯ_cpu(*qctx->headptr) ==
		le32_ಗಮ್ಯ_cpu(*qctx->tailptr)) ? true : false;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ __smq_rx(ರಚನೆ smq_ಸರತಿ_ctx *qctx, u32 ಸ್ಥಾನ_ಗಾತ್ರ,
		    u32 service_ಗುಂಪು_id, ರಚನೆ mbox_xfer *xfer)
	MUST_HOLD(&qctx->ಸರತಿ_ಬೀಗ)
{
	ಶೂನ್ಯ *dst, *src;
	ರಚನೆ rpmi_ಸಂದೇಶ *ಸಂದೇಶ;
	u32 i, tmp, pos, dlen, msgidn, headidx, tailidx;
	ರಚನೆ rpmi_ಸಂದೇಶ_ವಾದಗಳು *ವಾದಗಳು = xfer->ವಾದಗಳು;
	bool no_rx_token = (ವಾದಗಳು->flags & RPMI_ಸಂದೇಶ_FLAGS_NO_RX_TOKEN) ?
			   true : false;

	/* Rx sanity checks */
	ಆದರೆ ((ಗಾತ್ರ(u32) * ವಾದಗಳು->rx_endian_ಪದಗಳು) >
	    (ಸ್ಥಾನ_ಗಾತ್ರ - ಗಾತ್ರ(ರಚನೆ rpmi_ಸಂದೇಶ_ಶೀರ್ಷಿಕೆ)))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ಆದರೆ ((ಗಾತ್ರ(u32) * ವಾದಗಳು->rx_endian_ಪದಗಳು) > xfer->rx_len)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	/* There should be some message in the queue */
	ಆದರೆ (__smq_queue_empty(qctx))
		ಹಿಂತಿರುಗಿ SBI_ENOENT;

	/* Get the head/read index and tail/write index */
	headidx = le32_ಗಮ್ಯ_cpu(*qctx->headptr);
	tailidx = le32_ಗಮ್ಯ_cpu(*qctx->tailptr);

	/*
	 * Compute msgidn expected in the incoming message
	 * NOTE: DOORBELL bit is not expected to be set.
	 */
	msgidn = MAKE_ಸಂದೇಶ_ID(service_ಗುಂಪು_id, ವಾದಗಳು->service_id, ವಾದಗಳು->type);

	/* Find the Rx message with matching token */
	pos = headidx;
	ಆಗ (pos != tailidx) {
		src = (ಶೂನ್ಯ *)qctx->ಬಫರ್ + (pos * ಸ್ಥಾನ_ಗಾತ್ರ);
		ಆದರೆ ((no_rx_token && GET_ಸಂದೇಶ_ID(src) == msgidn) ||
		    (GET_TOKEN(src) == (xfer->seq & RPMI_ಸಂದೇಶ_TOKEN_ಮುಖವಾಡ)))
			ನಿಲ್ಲಿಸು;
		pos = (pos + 1) % qctx->ಅಂಕೆ_slots;
	}
	ಆದರೆ (pos == tailidx)
		ಹಿಂತಿರುಗಿ SBI_ENOENT;

	/* If Rx message is not first message then make it first message */
	ಆದರೆ (pos != headidx) {
		src = (ಶೂನ್ಯ *)qctx->ಬಫರ್ + (pos * ಸ್ಥಾನ_ಗಾತ್ರ);
		dst = (ಶೂನ್ಯ *)qctx->ಬಫರ್ + (headidx * ಸ್ಥಾನ_ಗಾತ್ರ);
		ಗಾಗಿ (i = 0; i < ಸ್ಥಾನ_ಗಾತ್ರ / ಗಾತ್ರ(u32); i++) {
			tmp = ((u32 *)dst)[i];
			((u32 *)dst)[i] = ((u32 *)src)[i];
			((u32 *)src)[i] = tmp;
		}
	}

	/* Update rx_token if not available */
	ಸಂದೇಶ = (ಶೂನ್ಯ *)qctx->ಬಫರ್ + (headidx * ಸ್ಥಾನ_ಗಾತ್ರ);
	ಆದರೆ (no_rx_token)
		ವಾದಗಳು->rx_token = GET_TOKEN(ಸಂದೇಶ);

	/* Extract data from the first message */
	ಆದರೆ (xfer->rx) {
		ವಾದಗಳು->rx_ದತ್ತಾಂಶ_len = dlen = GET_DLEN(ಸಂದೇಶ);
		ಆದರೆ (dlen > xfer->rx_len)
			dlen = xfer->rx_len;
		src = (ಶೂನ್ಯ *)ಸಂದೇಶ + ಗಾತ್ರ(ರಚನೆ rpmi_ಸಂದೇಶ_ಶೀರ್ಷಿಕೆ);
		dst = xfer->rx;
		ಗಾಗಿ (i = 0; i < ವಾದಗಳು->rx_endian_ಪದಗಳು; i++)
			((u32 *)dst)[i] = le32_ಗಮ್ಯ_cpu(((u32 *)src)[i]);
		dst += ಗಾತ್ರ(u32) * ವಾದಗಳು->rx_endian_ಪದಗಳು;
		src += ಗಾತ್ರ(u32) * ವಾದಗಳು->rx_endian_ಪದಗಳು;
		sbi_memcpy(dst, src,
			xfer->rx_len - (ಗಾತ್ರ(u32) * ವಾದಗಳು->rx_endian_ಪದಗಳು));
	}

	/* Update the head/read index */
	*qctx->headptr = cpu_ಗಮ್ಯ_le32(headidx + 1) % qctx->ಅಂಕೆ_slots;

	/* Make sure updates to head are immediately visible to PuC */
	smp_wmb();

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ __smq_tx(ರಚನೆ smq_ಸರತಿ_ctx *qctx, ರಚನೆ rpmi_mb_regs *mb_regs,
		    u32 a2p_doorbell_ಮೌಲ್ಯ, u32 ಸ್ಥಾನ_ಗಾತ್ರ, u32 service_ಗುಂಪು_id,
		    ರಚನೆ mbox_xfer *xfer) MUST_HOLD(&qctx->ಸರತಿ_ಬೀಗ)
{
	u32 i, tailidx;
	ಶೂನ್ಯ *dst, *src;
	ರಚನೆ rpmi_ಸಂದೇಶ_ಶೀರ್ಷಿಕೆ ಶೀರ್ಷಿಕೆ = { 0 };
	ರಚನೆ rpmi_ಸಂದೇಶ_ವಾದಗಳು *ವಾದಗಳು = xfer->ವಾದಗಳು;

	/* Tx sanity checks */
	ಆದರೆ ((ಗಾತ್ರ(u32) * ವಾದಗಳು->tx_endian_ಪದಗಳು) >
	    (ಸ್ಥಾನ_ಗಾತ್ರ - ಗಾತ್ರ(ರಚನೆ rpmi_ಸಂದೇಶ_ಶೀರ್ಷಿಕೆ)))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ಆದರೆ ((ಗಾತ್ರ(u32) * ವಾದಗಳು->tx_endian_ಪದಗಳು) > xfer->tx_len)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	/* There should be some room in the queue */
	ಆದರೆ (__smq_queue_full(qctx))
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;

	/* Get the tail/write index */
	tailidx = le32_ಗಮ್ಯ_cpu(*qctx->tailptr);

	/* Prepare the header to be written into the slot */
	ಶೀರ್ಷಿಕೆ.servicegroup_id = cpu_ಗಮ್ಯ_le16(service_ಗುಂಪು_id);
	ಶೀರ್ಷಿಕೆ.service_id = ವಾದಗಳು->service_id;
	ಶೀರ್ಷಿಕೆ.flags = ವಾದಗಳು->type;
	ಶೀರ್ಷಿಕೆ.datalen = cpu_ಗಮ್ಯ_le16((u16)xfer->tx_len);
	ಶೀರ್ಷಿಕೆ.token = cpu_ಗಮ್ಯ_le16((u16)xfer->seq);

	/* Write header into the slot */
	dst = (ಅಕ್ಷರ *)qctx->ಬಫರ್ + (tailidx * ಸ್ಥಾನ_ಗಾತ್ರ);
	sbi_memcpy(dst, &ಶೀರ್ಷಿಕೆ, ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ));
	dst += ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ);

	/* Write data into the slot */
	ಆದರೆ (xfer->tx) {
		src = xfer->tx;
		ಗಾಗಿ (i = 0; i < ವಾದಗಳು->tx_endian_ಪದಗಳು; i++)
			((u32 *)dst)[i] = cpu_ಗಮ್ಯ_le32(((u32 *)src)[i]);
		dst += ಗಾತ್ರ(u32) * ವಾದಗಳು->tx_endian_ಪದಗಳು;
		src += ಗಾತ್ರ(u32) * ವಾದಗಳು->tx_endian_ಪದಗಳು;
		sbi_memcpy(dst, src,
			xfer->tx_len - (ಗಾತ್ರ(u32) * ವಾದಗಳು->tx_endian_ಪದಗಳು));
	}

	/* Make sure queue chanages are visible to PuC before updating tail */
	smp_wmb();

	/* Update the tail/write index */
	*qctx->tailptr = cpu_ಗಮ್ಯ_le32(tailidx + 1) % qctx->ಅಂಕೆ_slots;

	/* Ring the RPMI doorbell if present */
	ಆದರೆ (mb_regs)
		writel(a2p_doorbell_ಮೌಲ್ಯ, &mb_regs->db_reg);

	ಹಿಂತಿರುಗಿ SBI_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ smq_rx(ರಚನೆ rpmi_shmem_mbox_controller *mctl,
		  u32 ಸರತಿ_id, u32 service_ಗುಂಪು_id, ರಚನೆ mbox_xfer *xfer)
{
	ಪೂರ್ಣಾಂಕ ret, rxretry = 0;
	ರಚನೆ smq_ಸರತಿ_ctx *qctx;

	ಆದರೆ (mctl->ಸರತಿ_ಎಣಿಕೆ < ಸರತಿ_id) {
		sbi_printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	}
	qctx = &mctl->ಸರತಿ_ctx_tbl[ಸರತಿ_id];

	/*
	 * Once the timeout happens and call this function is returned
	 * to the client then there is no way to deliver the response
	 * message after that if it comes later.
	 *
	 * REVISIT: In complete timeout duration how much duration
	 * it should wait(delay) before recv retry. udelay or mdelay
	 */
	ಮಾಡು {
		spin_ಬೀಗ(&qctx->ಸರತಿ_ಬೀಗ);
		ret = __smq_rx(qctx, mctl->ಸ್ಥಾನ_ಗಾತ್ರ, service_ಗುಂಪು_id, xfer);
		spin_unlock(&qctx->ಸರತಿ_ಬೀಗ);
		ಆದರೆ (!ret)
			ಹಿಂತಿರುಗಿ 0;

		sbi_timer_mdelay(1);
		rxretry += 1;
	} ಆಗ (rxretry < xfer->rx_timeout);

	ಹಿಂತಿರುಗಿ SBI_ETIMEDOUT;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ smq_tx(ರಚನೆ rpmi_shmem_mbox_controller *mctl,
		  u32 ಸರತಿ_id, u32 service_ಗುಂಪು_id, ರಚನೆ mbox_xfer *xfer)
{
	ಪೂರ್ಣಾಂಕ ret, txretry = 0;
	ರಚನೆ smq_ಸರತಿ_ctx *qctx;

	ಆದರೆ (mctl->ಸರತಿ_ಎಣಿಕೆ < ಸರತಿ_id) {
		sbi_printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	}
	qctx = &mctl->ಸರತಿ_ctx_tbl[ಸರತಿ_id];

	/*
	 * Ignoring the tx timeout since in RPMI has no mechanism
	 * with which other side can let know about the reception of
	 * message which marks as tx complete. For RPMI tx complete is
	 * marked as done when message in successfully copied in queue.
	 *
	 * REVISIT: In complete timeout duration how much duration
	 * it should wait(delay) before send retry. udelay or mdelay
	 */
	ಮಾಡು {
		spin_ಬೀಗ(&qctx->ಸರತಿ_ಬೀಗ);
		ret = __smq_tx(qctx, mctl->mb_regs, mctl->a2p_doorbell_ಮೌಲ್ಯ,
				mctl->ಸ್ಥಾನ_ಗಾತ್ರ, service_ಗುಂಪು_id, xfer);
		spin_unlock(&qctx->ಸರತಿ_ಬೀಗ);
		ಆದರೆ (!ret)
			ಹಿಂತಿರುಗಿ 0;

		sbi_timer_mdelay(1);
		txretry += 1;
	} ಆಗ (txretry < xfer->tx_timeout);

	ಹಿಂತಿರುಗಿ SBI_ETIMEDOUT;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rpmi_get_platform_info(ರಚನೆ rpmi_shmem_mbox_controller *mctl)
{
	ಪೂರ್ಣಾಂಕ ret = SBI_OK;

	/**
	 * platform string can occupy max possible size
	 * max possible space in the message data as
	 * per the format
	 */
	ರಚನೆ rpmi_ತಳ_get_platform_info_resp *resp =
			sbi_zalloc(RPMI_ಸಂದೇಶ_ದತ್ತಾಂಶ_ಗಾತ್ರ(mctl->ಸ್ಥಾನ_ಗಾತ್ರ));
	ಆದರೆ (!resp)
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;

	ret = rpmi_ಸಾಮಾನ್ಯ_request_with_ಸ್ಥಿತಿ(mctl->ತಳ_chan,
				 RPMI_ತಳ_SRV_GET_PLATFORM_INFO,
				 ಖಾಲಿ, 0, 0,
				 resp,
				 RPMI_ಸಂದೇಶ_ದತ್ತಾಂಶ_ಗಾತ್ರ(mctl->ಸ್ಥಾನ_ಗಾತ್ರ)/4,
				 RPMI_ಸಂದೇಶ_ದತ್ತಾಂಶ_ಗಾತ್ರ(mctl->ಸ್ಥಾನ_ಗಾತ್ರ)/4);
	ಆದರೆ (ret)
		ಹೋಗು fail_free_resp;

	mctl->plat_info_len = resp->plat_info_len;
	mctl->plat_info = sbi_zalloc(mctl->plat_info_len);
	ಆದರೆ (!mctl->plat_info) {
		ret = SBI_ENOMEM;
		ಹೋಗು fail_free_resp;
	}

	sbi_strncpy(mctl->plat_info, resp->plat_info, mctl->plat_info_len);

fail_free_resp:
	sbi_free(resp);
	ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ smq_ತಳ_get_two_u32(ರಚನೆ rpmi_shmem_mbox_controller *mctl,
				u32 service_id, u32 *inarg, u32 *outvals)
{
	ಹಿಂತಿರುಗಿ rpmi_ಸಾಮಾನ್ಯ_request_with_ಸ್ಥಿತಿ(
			mctl->ತಳ_chan, service_id,
			inarg, (inarg) ? 1 : 0, (inarg) ? 1 : 0,
			outvals, 2, 2);
}

/**************** Mailbox Controller Functions **************/

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rpmi_shmem_mbox_xfer(ರಚನೆ mbox_chan *chan, ರಚನೆ mbox_xfer *xfer)
{
	ಪೂರ್ಣಾಂಕ ret;
	u32 tx_qid = 0, rx_qid = 0;
	ರಚನೆ rpmi_shmem_mbox_controller *mctl =
			container_ದ(chan->mbox,
				     ರಚನೆ rpmi_shmem_mbox_controller,
				     controller);
	ರಚನೆ rpmi_srvgrp_chan *srvgrp_chan = ಗಮ್ಯ_srvgrp_chan(chan);

	ರಚನೆ rpmi_ಸಂದೇಶ_ವಾದಗಳು *ವಾದಗಳು = xfer->ವಾದಗಳು;
	bool do_tx = (ವಾದಗಳು->flags & RPMI_ಸಂದೇಶ_FLAGS_NO_TX) ? false : true;
	bool do_rx = (ವಾದಗಳು->flags & RPMI_ಸಂದೇಶ_FLAGS_NO_RX) ? false : true;

	ಆದರೆ (!do_tx && !do_rx)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	ಆಯ್ಕೆ (ವಾದಗಳು->type) {
	ಸ್ಥಿತಿ RPMI_ಸಂದೇಶ_ಸಾಮಾನ್ಯ_REQUEST:
		ಆದರೆ (do_tx && do_rx) {
			tx_qid = RPMI_ಸರತಿ_IDX_A2_P_REQ;
			rx_qid = RPMI_ಸರತಿ_IDX_P2_A_ACK;
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (do_tx) {
			tx_qid = RPMI_ಸರತಿ_IDX_A2_P_REQ;
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (do_rx) {
			rx_qid = RPMI_ಸರತಿ_IDX_P2_A_REQ;
		}
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ RPMI_ಸಂದೇಶ_POSTED_REQUEST:
		ಆದರೆ (do_tx && do_rx)
			ಹಿಂತಿರುಗಿ SBI_EINVAL;
		ಆದರೆ (do_tx) {
			tx_qid = RPMI_ಸರತಿ_IDX_A2_P_REQ;
		} ಇಲ್ಲದಿದ್ದರೆ {
			rx_qid = RPMI_ಸರತಿ_IDX_P2_A_REQ;
		}
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ RPMI_ಸಂದೇಶ_ACKNOWLDGEMENT:
		ಆದರೆ (do_tx && do_rx)
			ಹಿಂತಿರುಗಿ SBI_EINVAL;
		ಆದರೆ (do_tx) {
			tx_qid = RPMI_ಸರತಿ_IDX_A2_P_ACK;
		} ಇಲ್ಲದಿದ್ದರೆ {
			rx_qid = RPMI_ಸರತಿ_IDX_P2_A_ACK;
		}
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;
	}

	ಆದರೆ (do_tx) {
		ret = smq_tx(mctl, tx_qid, srvgrp_chan->servicegroup_id, xfer);
		ಆದರೆ (ret)
			ಹಿಂತಿರುಗಿ ret;
	}

	ಆದರೆ (do_rx) {
		ret = smq_rx(mctl, rx_qid, srvgrp_chan->servicegroup_id, xfer);
		ಆದರೆ (ret)
			ಹಿಂತಿರುಗಿ ret;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rpmi_shmem_mbox_get_attribute(ರಚನೆ mbox_chan *chan,
					 ಪೂರ್ಣಾಂಕ attr_id, ಶೂನ್ಯ *ಹೊರಹೋಗು_ಮೌಲ್ಯ)
{
	ರಚನೆ rpmi_shmem_mbox_controller *mctl =
			container_ದ(chan->mbox,
				     ರಚನೆ rpmi_shmem_mbox_controller,
				     controller);
	ರಚನೆ rpmi_srvgrp_chan *srvgrp_chan = ಗಮ್ಯ_srvgrp_chan(chan);

	ಆಯ್ಕೆ (attr_id) {
	ಸ್ಥಿತಿ RPMI_CHANNEL_ATTR_PROTOCOL_ಆವೃತ್ತಿ:
		*((u32 *)ಹೊರಹೋಗು_ಮೌಲ್ಯ) = mctl->spec_ಆವೃತ್ತಿ;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ RPMI_CHANNEL_ATTR_MAX_ದತ್ತಾಂಶ_LEN:
		*((u32 *)ಹೊರಹೋಗು_ಮೌಲ್ಯ) = RPMI_ಸಂದೇಶ_ದತ್ತಾಂಶ_ಗಾತ್ರ(mctl->ಸ್ಥಾನ_ಗಾತ್ರ);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ RPMI_CHANNEL_ATTR_P2_A_DOORBELL_SYSMSI_ಸೂಚ್ಯಂಕ:
		*((u32 *)ಹೊರಹೋಗು_ಮೌಲ್ಯ) = mctl->p2a_doorbell_sysmsi_ಸೂಚ್ಯಂಕ;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ RPMI_CHANNEL_ATTR_TX_TIMEOUT:
		*((u32 *)ಹೊರಹೋಗು_ಮೌಲ್ಯ) = RPMI_DEF_TX_TIMEOUT;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ RPMI_CHANNEL_ATTR_RX_TIMEOUT:
		*((u32 *)ಹೊರಹೋಗು_ಮೌಲ್ಯ) = RPMI_DEF_RX_TIMEOUT;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ RPMI_CHANNEL_ATTR_SERVICEGROUP_ID:
		*((u32 *)ಹೊರಹೋಗು_ಮೌಲ್ಯ) = srvgrp_chan->servicegroup_id;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ RPMI_CHANNEL_ATTR_SERVICEGROUP_ಆವೃತ್ತಿ:
		*((u32 *)ಹೊರಹೋಗು_ಮೌಲ್ಯ) = srvgrp_chan->servicegroup_ಆವೃತ್ತಿ;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ RPMI_CHANNEL_ATTR_IMPL_ID:
		*((u32 *)ಹೊರಹೋಗು_ಮೌಲ್ಯ) = mctl->impl_id;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ RPMI_CHANNEL_ATTR_IMPL_ಆವೃತ್ತಿ:
		*((u32 *)ಹೊರಹೋಗು_ಮೌಲ್ಯ) = mctl->impl_ಆವೃತ್ತಿ;
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ರಚನೆ mbox_chan *rpmi_shmem_mbox_request_chan(
						ರಚನೆ mbox_controller *mbox,
						u32 *chan_ವಾದಗಳು)
{
	ಪೂರ್ಣಾಂಕ ret;
	u32 tval[2] = { 0 };
	ರಚನೆ rpmi_srvgrp_chan *srvgrp_chan;
	ರಚನೆ rpmi_shmem_mbox_controller *mctl =
			container_ದ(mbox,
				     ರಚನೆ rpmi_shmem_mbox_controller,
				     controller);

	/* Service group id not defined or in reserved range is invalid */
	ಆದರೆ (chan_ವಾದಗಳು[0] >= RPMI_SRVGRP_ID_MAX_ಎಣಿಕೆ &&
		chan_ವಾದಗಳು[0] <= RPMI_SRVGRP_RESERVE_ಕೊನೆ)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	/* Base serivce group is always present so probe other groups */
	ಆದರೆ (chan_ವಾದಗಳು[0] != RPMI_SRVGRP_ತಳ) {
		/* Probe service group */
		ret = smq_ತಳ_get_two_u32(mctl,
					   RPMI_ತಳ_SRV_PROBE_SERVICE_ಗುಂಪು,
					   chan_ವಾದಗಳು, tval);
		ಆದರೆ (ret || !tval[1])
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	srvgrp_chan = sbi_zalloc(ಗಾತ್ರ(*srvgrp_chan));
	ಆದರೆ (!srvgrp_chan)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	srvgrp_chan->servicegroup_id = chan_ವಾದಗಳು[0];
	srvgrp_chan->servicegroup_ಆವೃತ್ತಿ = tval[1];

	ಹಿಂತಿರುಗಿ &srvgrp_chan->chan;
}

ಸ್ಥಿರ ಶೂನ್ಯ rpmi_shmem_mbox_free_chan(ರಚನೆ mbox_controller *mbox,
				      ರಚನೆ mbox_chan *chan)
{
	ರಚನೆ rpmi_srvgrp_chan *srvgrp_chan = ಗಮ್ಯ_srvgrp_chan(chan);
	sbi_free(srvgrp_chan);
}

ಬಾಹ್ಯ ರಚನೆ fdt_mailbox fdt_mailbox_rpmi_shmem;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rpmi_shmem_transport_init(ರಚನೆ rpmi_shmem_mbox_controller *mctl,
				     ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt,
				     ಪೂರ್ಣಾಂಕ nodeoff) NO_THREAD_SAFETY_ANALYSIS
{
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
	ಸ್ಥಿರಾಂಕ fdt32_t *prop;
	ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2, len, ret, qid;
	uint64_t reg_addr, reg_ಗಾತ್ರ;
	ರಚನೆ smq_ಸರತಿ_ctx *qctx;

	ret = fdt_ಗಂಟು_check_compatible(fdt, nodeoff,
					"riscv,rpmi-shmem-mbox");
	ಆದರೆ (ret)
		ಹಿಂತಿರುಗಿ ret;

	/* get queue slot size in bytes */
	prop = fdt_getprop(fdt, nodeoff, "riscv,slot-size", &len);
	ಆದರೆ (!prop)
		ಹಿಂತಿರುಗಿ SBI_ENOENT;

	mctl->ಸ್ಥಾನ_ಗಾತ್ರ = fdt32_ಗಮ್ಯ_cpu(*prop);
	ಆದರೆ (mctl->ಸ್ಥಾನ_ಗಾತ್ರ < RPMI_ಸ್ಥಾನ_ಗಾತ್ರ_MIN) {
		sbi_printf("%s: slot_size < mimnum required message size\n",
			   __func__);
		mctl->ಸ್ಥಾನ_ಗಾತ್ರ = RPMI_ಸ್ಥಾನ_ಗಾತ್ರ_MIN;
	}

	/* get p2a doorbell system MSI index */
	prop = fdt_getprop(fdt, nodeoff, "riscv,p2a-doorbell-sysmsi-index", &len);
	mctl->p2a_doorbell_sysmsi_ಸೂಚ್ಯಂಕ = prop ? fdt32_ಗಮ್ಯ_cpu(*prop) : -1U;

	/* get a2p doorbell value */
	prop = fdt_getprop(fdt, nodeoff, "riscv,a2p-doorbell-value", &len);
	mctl->a2p_doorbell_ಮೌಲ್ಯ = prop ? fdt32_ಗಮ್ಯ_cpu(*prop) : 1;

	/*
	 * queue names count is taken as the number of queues
	 * supported which make it mandatory to provide the
	 * name of the queue.
	 */
	ಎಣಿಕೆ_2 = fdt_stringlist_ಎಣಿಕೆ(fdt, nodeoff, "reg-names");
	ಆದರೆ (ಎಣಿಕೆ_2 < 0 ||
	    ಎಣಿಕೆ_2 > (RPMI_ಸರತಿ_IDX_MAX_ಎಣಿಕೆ + RPMI_REG_IDX_MAX_ಎಣಿಕೆ))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	mctl->ಸರತಿ_ಎಣಿಕೆ = ಎಣಿಕೆ_2 - RPMI_REG_IDX_MAX_ಎಣಿಕೆ;

	/* parse all queues and populate queues context structure */
	ಗಾಗಿ (qid = 0; qid < mctl->ಸರತಿ_ಎಣಿಕೆ; qid++) {
		qctx = &mctl->ಸರತಿ_ctx_tbl[qid];

		/* get each queue share-memory base address and size*/
		ret = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoff, qid,
					     &reg_addr, &reg_ಗಾತ್ರ);
		ಆದರೆ (ret < 0 || !reg_addr || !reg_ಗಾತ್ರ)
			ಹಿಂತಿರುಗಿ SBI_ENOENT;

		ret = sbi_domain_ಬೇರು_ಕೂಡಿಸು_memrange(reg_addr, reg_ಗಾತ್ರ, reg_ಗಾತ್ರ,
						   (SBI_DOMAIN_MEMREGION_MMIO |
						    SBI_DOMAIN_MEMREGION_M_READABLE |
						    SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ಆದರೆ (ret)
			ಹಿಂತಿರುಗಿ ret;

		/* calculate number of slots in each queue */
		qctx->ಅಂಕೆ_slots =
			(reg_ಗಾತ್ರ - (mctl->ಸ್ಥಾನ_ಗಾತ್ರ * RPMI_ಸರತಿ_ಶೀರ್ಷಿಕೆ_SLOTS)) / mctl->ಸ್ಥಾನ_ಗಾತ್ರ;

		/* setup queue pointers */
		qctx->headptr = ((ಶೂನ್ಯ *)(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)reg_addr) +
				RPMI_ಸರತಿ_ತಲೆ_ಸ್ಥಾನ * mctl->ಸ್ಥಾನ_ಗಾತ್ರ;
		qctx->tailptr = ((ಶೂನ್ಯ *)(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)reg_addr) +
				RPMI_ಸರತಿ_ಬಾಲ_ಸ್ಥಾನ * mctl->ಸ್ಥಾನ_ಗಾತ್ರ;
		qctx->ಬಫರ್ = ((ಶೂನ್ಯ *)(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)reg_addr) +
				RPMI_ಸರತಿ_ಶೀರ್ಷಿಕೆ_SLOTS * mctl->ಸ್ಥಾನ_ಗಾತ್ರ;

		/* get the queue name */
		name = fdt_stringlist_get(fdt, nodeoff, "reg-names",
					  qid, &len);
		ಆದರೆ (!name || (name && len < 0))
			ಹಿಂತಿರುಗಿ len;

		sbi_memcpy(qctx->name, name, len);

		/* store the index as queue_id */
		qctx->ಸರತಿ_id = qid;

		SPIN_ಬೀಗ_INIT(qctx->ಸರತಿ_ಬೀಗ);
	}

	/* get the a2p-doorbell property name */
	name = fdt_stringlist_get(fdt, nodeoff, "reg-names", qid, &len);
	ಆದರೆ (!name || (name && len < 0))
		ಹಿಂತಿರುಗಿ len;

	/* fetch doorbell register address*/
	ret = fdt_get_ಗಂಟು_addr_ಗಾತ್ರ(fdt, nodeoff, qid, &reg_addr,
				       &reg_ಗಾತ್ರ);
	ಆದರೆ (!ret && !(ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(name, "a2p-doorbell", ಅಕ್ಷರಸರಣಿ_ಉದ್ದ("a2p-doorbell")))) {
		mctl->mb_regs = (ಶೂನ್ಯ *)(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)reg_addr;
		ret = sbi_domain_ಬೇರು_ಕೂಡಿಸು_memrange(reg_addr, reg_ಗಾತ್ರ, reg_ಗಾತ್ರ,
						   (SBI_DOMAIN_MEMREGION_MMIO |
						    SBI_DOMAIN_MEMREGION_M_READABLE |
						    SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ಆದರೆ (ret)
			ಹಿಂತಿರುಗಿ ret;
	}

	ಹಿಂತಿರುಗಿ SBI_SUCCESS;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rpmi_shmem_mbox_init(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff,
				ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	ರಚನೆ rpmi_ತಳ_get_ಗುಣಲಕ್ಷಣಗಳು_resp resp;
	ರಚನೆ rpmi_shmem_mbox_controller *mctl;
	ರಚನೆ rpmi_srvgrp_chan *ತಳ_srvgrp;
	u32 tval[2], ವಾದಗಳು[1];
	ಪೂರ್ಣಾಂಕ ret = 0;

	mctl = sbi_zalloc(ಗಾತ್ರ(*mctl));
	ಆದರೆ (!mctl)
		ಹಿಂತಿರುಗಿ SBI_ENOMEM;

	/* Initialization transport from device tree */
	ret = rpmi_shmem_transport_init(mctl, fdt, nodeoff);
	ಆದರೆ (ret)
		ಹೋಗು fail_free_controller;

	/* Register mailbox controller */
	mctl->controller.id = nodeoff;
	mctl->controller.max_xfer_len =
			mctl->ಸ್ಥಾನ_ಗಾತ್ರ - ಗಾತ್ರ(ರಚನೆ rpmi_ಸಂದೇಶ_ಶೀರ್ಷಿಕೆ);
	mctl->controller.driver = &fdt_mailbox_rpmi_shmem;
	mctl->controller.request_chan = rpmi_shmem_mbox_request_chan;
	mctl->controller.free_chan = rpmi_shmem_mbox_free_chan;
	mctl->controller.xfer = rpmi_shmem_mbox_xfer;
	mctl->controller.get_attribute = rpmi_shmem_mbox_get_attribute;
	ret = mbox_controller_ಕೂಡಿಸು(&mctl->controller);
	ಆದರೆ (ret)
		ಹೋಗು fail_free_controller;

	/* Request base service group channel */
	tval[0] = RPMI_SRVGRP_ತಳ;
	mctl->ತಳ_chan = mbox_controller_request_chan(&mctl->controller,
							tval);
	ಆದರೆ (!mctl->ತಳ_chan) {
		ret = SBI_ENOENT;
		ಹೋಗು fail_remove_controller;
	}

	/* Update base service group version */
	ತಳ_srvgrp = ಗಮ್ಯ_srvgrp_chan(mctl->ತಳ_chan);
	ವಾದಗಳು[0] = RPMI_SRVGRP_ತಳ;
	ret = smq_ತಳ_get_two_u32(mctl, RPMI_ತಳ_SRV_PROBE_SERVICE_ಗುಂಪು,
				   &ವಾದಗಳು[0], tval);
	ಆದರೆ (ret)
		ಹೋಗು fail_free_chan;
	ತಳ_srvgrp->servicegroup_ಆವೃತ್ತಿ = tval[1];
	ಆದರೆ (ತಳ_srvgrp->servicegroup_ಆವೃತ್ತಿ < RPMI_ತಳ_ಆವೃತ್ತಿ_MIN) {
		ret = SBI_EINVAL;
		ಹೋಗು fail_free_chan;
	}

	/* Get implementation id */
	ret = smq_ತಳ_get_two_u32(mctl,
				   RPMI_ತಳ_SRV_GET_IMPLEMENTATION_ಆವೃತ್ತಿ,
				   ಖಾಲಿ, tval);
	ಆದರೆ (ret)
		ಹೋಗು fail_free_chan;
	mctl->impl_ಆವೃತ್ತಿ = tval[1];

	/* Get implementation version */
	ret = smq_ತಳ_get_two_u32(mctl, RPMI_ತಳ_SRV_GET_IMPLEMENTATION_IDN,
				   ಖಾಲಿ, tval);
	ಆದರೆ (ret)
		ಹೋಗು fail_free_chan;
	mctl->impl_id = tval[1];

	/* Get specification version */
	ret = smq_ತಳ_get_two_u32(mctl, RPMI_ತಳ_SRV_GET_SPEC_ಆವೃತ್ತಿ,
				   ಖಾಲಿ, tval);
	ಆದರೆ (ret)
		ಹೋಗು fail_free_chan;
	mctl->spec_ಆವೃತ್ತಿ = tval[1];
	ಆದರೆ (mctl->spec_ಆವೃತ್ತಿ < RPMI_ತಳ_ಆವೃತ್ತಿ_MIN ||
	    mctl->spec_ಆವೃತ್ತಿ != ತಳ_srvgrp->servicegroup_ಆವೃತ್ತಿ) {
		ret = SBI_EINVAL;
		ಹೋಗು fail_free_chan;
	}

	/* Get optional features implementation flags */
	ret = rpmi_ಸಾಮಾನ್ಯ_request_with_ಸ್ಥಿತಿ(
			mctl->ತಳ_chan, RPMI_ತಳ_SRV_GET_ಗುಣಲಕ್ಷಣಗಳು,
			ಖಾಲಿ, 0, 0,
			&resp, rpmi_u32_ಎಣಿಕೆ(resp), rpmi_u32_ಎಣಿಕೆ(resp));
	ಆದರೆ (ret)
		ಹೋಗು fail_free_chan;

	/* 1: M-mode, 0: S-mode */
	mctl->ತಳ_flags.f0_priv_level =
			resp.f0 & RPMI_ತಳ_FLAGS_F0_PRIVILEGE ? 1 : 0;
	/* 1: Supported, 0: Not Supported */
	mctl->ತಳ_flags.f0_ev_notif_en =
			resp.f0 & RPMI_ತಳ_FLAGS_F0_EV_NOTIFY ? 1 : 0;

	/* We only use M-mode RPMI context in OpenSBI */
	ಆದರೆ (!mctl->ತಳ_flags.f0_priv_level) {
		ret = SBI_ENODEV;
		ಹೋಗು fail_free_chan;
	}

	/*
	 * Continue without platform information string if not
	 * available or if an error is encountered while fetching
	 */
	rpmi_get_platform_info(mctl);

	ಹಿಂತಿರುಗಿ 0;

fail_free_chan:
	mbox_controller_free_chan(mctl->ತಳ_chan);
fail_remove_controller:
	mbox_controller_remove(&mctl->controller);
fail_free_controller:
	sbi_free(mctl);
	ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ rpmi_shmem_mbox_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "riscv,rpmi-shmem-mbox" },
	{ },
};

ರಚನೆ fdt_mailbox fdt_mailbox_rpmi_shmem = {
	.driver = {
		.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = rpmi_shmem_mbox_ಹೊಂದಿಕೆ,
		.init = rpmi_shmem_mbox_init,
	},
	.xlate = fdt_mailbox_simple_xlate,
};
