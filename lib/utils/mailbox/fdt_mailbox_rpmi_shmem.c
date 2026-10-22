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

#شامل <libfdt.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_timer.h>
#شامل <sbi/riscv_io.h>
#شامل <sbi/riscv_locks.h>
#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_barrier.h>
#شامل <sbi_utils/fdt/fdt_helper.h>
#شامل <sbi_utils/mailbox/mailbox.h>
#شامل <sbi_utils/mailbox/fdt_mailbox.h>
#شامل <sbi_utils/mailbox/rpmi_mailbox.h>

/** Minimum Base group version required */
#تعریف_کرو RPMI_بنیاد_نسخہ_MIN		RPMI_نسخہ(1, 0)

/**************** RPMI Transport Structures and Macros ***********/

#تعریف_کرو GET_SERVICEGROUP_ID(پیغام)		\
({						\
	ساخت rpmi_پیغام *mbuf = پیغام;	\
	le16_مقصد_cpu(mbuf->سرخی.servicegroup_id);\
})

#تعریف_کرو GET_SERVICE_ID(پیغام)			\
({						\
	ساخت rpmi_پیغام *mbuf = پیغام;	\
	mbuf->سرخی.service_id;		\
})

#تعریف_کرو GET_FLAGS(پیغام)				\
({						\
	ساخت rpmi_پیغام *mbuf = پیغام;	\
	mbuf->سرخی.flags;			\
})

#تعریف_کرو GET_پیغام_ID(پیغام)			\
({						\
	ساخت rpmi_پیغام *mbuf = پیغام;	\
	((u32)mbuf->سرخی.flags << (RPMI_پیغام_FLAGS_انحراف * 8)) | \
	((u32)mbuf->سرخی.service_id << (RPMI_پیغام_SERVICE_ID_انحراف * 8)) | \
	((u32)le16_مقصد_cpu(mbuf->سرخی.servicegroup_id)); \
})

#تعریف_کرو MAKE_پیغام_ID(__group_id, __service_id, __flags)	\
({						\
	u32 __ret = 0;				\
	__ret |= (u32)(__group_id) << (RPMI_پیغام_SERVICEGROUP_ID_انحراف * 8); \
	__ret |= (u32)(__service_id) << (RPMI_پیغام_SERVICE_ID_انحراف * 8); \
	__ret |= (u32)(__flags) << (RPMI_پیغام_FLAGS_انحراف * 8); \
	__ret;					\
})

#تعریف_کرو GET_DLEN(پیغام)				\
({						\
	ساخت rpmi_پیغام *mbuf = پیغام;	\
	le16_مقصد_cpu(mbuf->سرخی.datalen);	\
})

#تعریف_کرو GET_TOKEN(پیغام)				\
({						\
	ساخت rpmi_پیغام *mbuf = پیغام;	\
	le16_مقصد_cpu(mbuf->سرخی.token);	\
})

#تعریف_کرو GET_پیغام_TYPE(پیغام)						\
({									\
	uint8_t flags = *((uint8_t *)پیغام + RPMI_پیغام_FLAGS_انحراف);	\
	((flags & RPMI_پیغام_FLAGS_TYPE) >> RPMI_پیغام_FLAGS_TYPE_POS));	\
})

شمار rpmi_قطار_type {
	RPMI_قطار_TYPE_REQ = 0,
	RPMI_قطار_TYPE_ACK = 1,
};

شمار rpmi_قطار_idx {
	RPMI_قطار_IDX_A2_P_REQ = 0,
	RPMI_قطار_IDX_P2_A_ACK = 1,
	RPMI_قطار_IDX_P2_A_REQ = 2,
	RPMI_قطار_IDX_A2_P_ACK = 3,
	RPMI_قطار_IDX_MAX_گنتی,
};

شمار rpmi_reg_idx {
	RPMI_REG_IDX_DB_REG = 0, /* Doorbell register */
	RPMI_REG_IDX_MAX_گنتی,
};

/** Mailbox registers */
ساخت rpmi_mb_regs {
	/* doorbell from AP -> PuC*/
	متلون le32_t db_reg;
} __packed;

/** Single Queue Context Structure */
ساخت smq_قطار_ctx {
	u32 قطار_id;
	u32 عدد_slots;
	spinlock_t قطار_تالا;
	/* Type of queue - REQ or ACK */
	شمار rpmi_قطار_type قطار_type;
	/* Pointers to the queue shared memory */
	متلون le32_t *headptr PT_GUARDED_BY(&قطار_تالا);
	متلون le32_t *tailptr PT_GUARDED_BY(&قطار_تالا);
	متلون uint8_t *بفر PT_GUARDED_BY(&قطار_تالا);
	/* Name of the queue */
	حرف name[RPMI_NAME_CHARS_MAX];
};

ساخت rpmi_srvgrp_chan {
	u32 servicegroup_id;
	u32 servicegroup_نسخہ;
	ساخت mbox_chan chan;
};

#تعریف_کرو مقصد_srvgrp_chan(mbox_chan)	\
		container_کا(mbox_chan, ساخت rpmi_srvgrp_chan, chan);

ساخت rpmi_shmem_mbox_controller {
	/* Driver specific members */
	u32 خانہ_سائز;
	u32 قطار_گنتی;
	u32 p2a_doorbell_sysmsi_اشاریہ;
	u32 a2p_doorbell_قدر;
	ساخت rpmi_mb_regs *mb_regs;
	ساخت smq_قطار_ctx قطار_ctx_tbl[RPMI_قطار_IDX_MAX_گنتی];
	/* Mailbox framework related members */
	ساخت mbox_controller controller;
	ساخت mbox_chan *بنیاد_chan;
	u32 impl_نسخہ;
	u32 impl_id;
	u32 spec_نسخہ;
	u32 plat_info_len;
	حرف *plat_info;
	ساخت {
		u8 f0_priv_level;
		bool f0_ev_notif_en;
	} بنیاد_flags;
};

/**************** Shared Memory Queues Helpers **************/

ساکن bool __smq_queue_full(ساخت smq_قطار_ctx *qctx)
	MUST_HOLD(&qctx->قطار_تالا)
{
	واپس ((le32_مقصد_cpu(*qctx->tailptr) + 1) % qctx->عدد_slots ==
			le32_مقصد_cpu(*qctx->headptr)) ? true : false;
}

ساکن bool __smq_queue_empty(ساخت smq_قطار_ctx *qctx)
	MUST_HOLD(&qctx->قطار_تالا)
{
	واپس (le32_مقصد_cpu(*qctx->headptr) ==
		le32_مقصد_cpu(*qctx->tailptr)) ? true : false;
}

ساکن صحیح __smq_rx(ساخت smq_قطار_ctx *qctx, u32 خانہ_سائز,
		    u32 service_گروہ_id, ساخت mbox_xfer *xfer)
	MUST_HOLD(&qctx->قطار_تالا)
{
	خالی *dst, *src;
	ساخت rpmi_پیغام *پیغام;
	u32 i, tmp, pos, dlen, msgidn, headidx, tailidx;
	ساخت rpmi_پیغام_دلائل *دلائل = xfer->دلائل;
	bool no_rx_token = (دلائل->flags & RPMI_پیغام_FLAGS_NO_RX_TOKEN) ?
			   true : false;

	/* Rx sanity checks */
	اگر ((حجم(u32) * دلائل->rx_endian_الفاظ) >
	    (خانہ_سائز - حجم(ساخت rpmi_پیغام_سرخی)))
		واپس SBI_EINVAL;
	اگر ((حجم(u32) * دلائل->rx_endian_الفاظ) > xfer->rx_len)
		واپس SBI_EINVAL;

	/* There should be some message in the queue */
	اگر (__smq_queue_empty(qctx))
		واپس SBI_ENOENT;

	/* Get the head/read index and tail/write index */
	headidx = le32_مقصد_cpu(*qctx->headptr);
	tailidx = le32_مقصد_cpu(*qctx->tailptr);

	/*
	 * Compute msgidn expected in the incoming message
	 * NOTE: DOORBELL bit is not expected to be set.
	 */
	msgidn = MAKE_پیغام_ID(service_گروہ_id, دلائل->service_id, دلائل->type);

	/* Find the Rx message with matching token */
	pos = headidx;
	جب (pos != tailidx) {
		src = (خالی *)qctx->بفر + (pos * خانہ_سائز);
		اگر ((no_rx_token && GET_پیغام_ID(src) == msgidn) ||
		    (GET_TOKEN(src) == (xfer->seq & RPMI_پیغام_TOKEN_نقاب)))
			توڑو;
		pos = (pos + 1) % qctx->عدد_slots;
	}
	اگر (pos == tailidx)
		واپس SBI_ENOENT;

	/* If Rx message is not first message then make it first message */
	اگر (pos != headidx) {
		src = (خالی *)qctx->بفر + (pos * خانہ_سائز);
		dst = (خالی *)qctx->بفر + (headidx * خانہ_سائز);
		برائے (i = 0; i < خانہ_سائز / حجم(u32); i++) {
			tmp = ((u32 *)dst)[i];
			((u32 *)dst)[i] = ((u32 *)src)[i];
			((u32 *)src)[i] = tmp;
		}
	}

	/* Update rx_token if not available */
	پیغام = (خالی *)qctx->بفر + (headidx * خانہ_سائز);
	اگر (no_rx_token)
		دلائل->rx_token = GET_TOKEN(پیغام);

	/* Extract data from the first message */
	اگر (xfer->rx) {
		دلائل->rx_ڈیٹا_len = dlen = GET_DLEN(پیغام);
		اگر (dlen > xfer->rx_len)
			dlen = xfer->rx_len;
		src = (خالی *)پیغام + حجم(ساخت rpmi_پیغام_سرخی);
		dst = xfer->rx;
		برائے (i = 0; i < دلائل->rx_endian_الفاظ; i++)
			((u32 *)dst)[i] = le32_مقصد_cpu(((u32 *)src)[i]);
		dst += حجم(u32) * دلائل->rx_endian_الفاظ;
		src += حجم(u32) * دلائل->rx_endian_الفاظ;
		sbi_memcpy(dst, src,
			xfer->rx_len - (حجم(u32) * دلائل->rx_endian_الفاظ));
	}

	/* Update the head/read index */
	*qctx->headptr = cpu_مقصد_le32(headidx + 1) % qctx->عدد_slots;

	/* Make sure updates to head are immediately visible to PuC */
	smp_wmb();

	واپس SBI_OK;
}

ساکن صحیح __smq_tx(ساخت smq_قطار_ctx *qctx, ساخت rpmi_mb_regs *mb_regs,
		    u32 a2p_doorbell_قدر, u32 خانہ_سائز, u32 service_گروہ_id,
		    ساخت mbox_xfer *xfer) MUST_HOLD(&qctx->قطار_تالا)
{
	u32 i, tailidx;
	خالی *dst, *src;
	ساخت rpmi_پیغام_سرخی سرخی = { 0 };
	ساخت rpmi_پیغام_دلائل *دلائل = xfer->دلائل;

	/* Tx sanity checks */
	اگر ((حجم(u32) * دلائل->tx_endian_الفاظ) >
	    (خانہ_سائز - حجم(ساخت rpmi_پیغام_سرخی)))
		واپس SBI_EINVAL;
	اگر ((حجم(u32) * دلائل->tx_endian_الفاظ) > xfer->tx_len)
		واپس SBI_EINVAL;

	/* There should be some room in the queue */
	اگر (__smq_queue_full(qctx))
		واپس SBI_ENOMEM;

	/* Get the tail/write index */
	tailidx = le32_مقصد_cpu(*qctx->tailptr);

	/* Prepare the header to be written into the slot */
	سرخی.servicegroup_id = cpu_مقصد_le16(service_گروہ_id);
	سرخی.service_id = دلائل->service_id;
	سرخی.flags = دلائل->type;
	سرخی.datalen = cpu_مقصد_le16((u16)xfer->tx_len);
	سرخی.token = cpu_مقصد_le16((u16)xfer->seq);

	/* Write header into the slot */
	dst = (حرف *)qctx->بفر + (tailidx * خانہ_سائز);
	sbi_memcpy(dst, &سرخی, حجم(سرخی));
	dst += حجم(سرخی);

	/* Write data into the slot */
	اگر (xfer->tx) {
		src = xfer->tx;
		برائے (i = 0; i < دلائل->tx_endian_الفاظ; i++)
			((u32 *)dst)[i] = cpu_مقصد_le32(((u32 *)src)[i]);
		dst += حجم(u32) * دلائل->tx_endian_الفاظ;
		src += حجم(u32) * دلائل->tx_endian_الفاظ;
		sbi_memcpy(dst, src,
			xfer->tx_len - (حجم(u32) * دلائل->tx_endian_الفاظ));
	}

	/* Make sure queue chanages are visible to PuC before updating tail */
	smp_wmb();

	/* Update the tail/write index */
	*qctx->tailptr = cpu_مقصد_le32(tailidx + 1) % qctx->عدد_slots;

	/* Ring the RPMI doorbell if present */
	اگر (mb_regs)
		writel(a2p_doorbell_قدر, &mb_regs->db_reg);

	واپس SBI_OK;
}

ساکن صحیح smq_rx(ساخت rpmi_shmem_mbox_controller *mctl,
		  u32 قطار_id, u32 service_گروہ_id, ساخت mbox_xfer *xfer)
{
	صحیح ret, rxretry = 0;
	ساخت smq_قطار_ctx *qctx;

	اگر (mctl->قطار_گنتی < قطار_id) {
		sbi_printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		واپس SBI_EINVAL;
	}
	qctx = &mctl->قطار_ctx_tbl[قطار_id];

	/*
	 * Once the timeout happens and call this function is returned
	 * to the client then there is no way to deliver the response
	 * message after that if it comes later.
	 *
	 * REVISIT: In complete timeout duration how much duration
	 * it should wait(delay) before recv retry. udelay or mdelay
	 */
	کرو {
		spin_تالا(&qctx->قطار_تالا);
		ret = __smq_rx(qctx, mctl->خانہ_سائز, service_گروہ_id, xfer);
		spin_unlock(&qctx->قطار_تالا);
		اگر (!ret)
			واپس 0;

		sbi_timer_mdelay(1);
		rxretry += 1;
	} جب (rxretry < xfer->rx_timeout);

	واپس SBI_ETIMEDOUT;
}

ساکن صحیح smq_tx(ساخت rpmi_shmem_mbox_controller *mctl,
		  u32 قطار_id, u32 service_گروہ_id, ساخت mbox_xfer *xfer)
{
	صحیح ret, txretry = 0;
	ساخت smq_قطار_ctx *qctx;

	اگر (mctl->قطار_گنتی < قطار_id) {
		sbi_printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		واپس SBI_EINVAL;
	}
	qctx = &mctl->قطار_ctx_tbl[قطار_id];

	/*
	 * Ignoring the tx timeout since in RPMI has no mechanism
	 * with which other side can let know about the reception of
	 * message which marks as tx complete. For RPMI tx complete is
	 * marked as done when message in successfully copied in queue.
	 *
	 * REVISIT: In complete timeout duration how much duration
	 * it should wait(delay) before send retry. udelay or mdelay
	 */
	کرو {
		spin_تالا(&qctx->قطار_تالا);
		ret = __smq_tx(qctx, mctl->mb_regs, mctl->a2p_doorbell_قدر,
				mctl->خانہ_سائز, service_گروہ_id, xfer);
		spin_unlock(&qctx->قطار_تالا);
		اگر (!ret)
			واپس 0;

		sbi_timer_mdelay(1);
		txretry += 1;
	} جب (txretry < xfer->tx_timeout);

	واپس SBI_ETIMEDOUT;
}

ساکن صحیح rpmi_get_platform_info(ساخت rpmi_shmem_mbox_controller *mctl)
{
	صحیح ret = SBI_OK;

	/**
	 * platform string can occupy max possible size
	 * max possible space in the message data as
	 * per the format
	 */
	ساخت rpmi_بنیاد_get_platform_info_resp *resp =
			sbi_zalloc(RPMI_پیغام_ڈیٹا_سائز(mctl->خانہ_سائز));
	اگر (!resp)
		واپس SBI_ENOMEM;

	ret = rpmi_معمول_request_with_حالت(mctl->بنیاد_chan,
				 RPMI_بنیاد_SRV_GET_PLATFORM_INFO,
				 تہی, 0, 0,
				 resp,
				 RPMI_پیغام_ڈیٹا_سائز(mctl->خانہ_سائز)/4,
				 RPMI_پیغام_ڈیٹا_سائز(mctl->خانہ_سائز)/4);
	اگر (ret)
		جاؤ fail_free_resp;

	mctl->plat_info_len = resp->plat_info_len;
	mctl->plat_info = sbi_zalloc(mctl->plat_info_len);
	اگر (!mctl->plat_info) {
		ret = SBI_ENOMEM;
		جاؤ fail_free_resp;
	}

	sbi_strncpy(mctl->plat_info, resp->plat_info, mctl->plat_info_len);

fail_free_resp:
	sbi_free(resp);
	واپس ret;
}

ساکن صحیح smq_بنیاد_get_two_u32(ساخت rpmi_shmem_mbox_controller *mctl,
				u32 service_id, u32 *inarg, u32 *outvals)
{
	واپس rpmi_معمول_request_with_حالت(
			mctl->بنیاد_chan, service_id,
			inarg, (inarg) ? 1 : 0, (inarg) ? 1 : 0,
			outvals, 2, 2);
}

/**************** Mailbox Controller Functions **************/

ساکن صحیح rpmi_shmem_mbox_xfer(ساخت mbox_chan *chan, ساخت mbox_xfer *xfer)
{
	صحیح ret;
	u32 tx_qid = 0, rx_qid = 0;
	ساخت rpmi_shmem_mbox_controller *mctl =
			container_کا(chan->mbox,
				     ساخت rpmi_shmem_mbox_controller,
				     controller);
	ساخت rpmi_srvgrp_chan *srvgrp_chan = مقصد_srvgrp_chan(chan);

	ساخت rpmi_پیغام_دلائل *دلائل = xfer->دلائل;
	bool do_tx = (دلائل->flags & RPMI_پیغام_FLAGS_NO_TX) ? false : true;
	bool do_rx = (دلائل->flags & RPMI_پیغام_FLAGS_NO_RX) ? false : true;

	اگر (!do_tx && !do_rx)
		واپس SBI_EINVAL;

	انتخاب (دلائل->type) {
	حالت RPMI_پیغام_معمول_REQUEST:
		اگر (do_tx && do_rx) {
			tx_qid = RPMI_قطار_IDX_A2_P_REQ;
			rx_qid = RPMI_قطار_IDX_P2_A_ACK;
		} ورنہ اگر (do_tx) {
			tx_qid = RPMI_قطار_IDX_A2_P_REQ;
		} ورنہ اگر (do_rx) {
			rx_qid = RPMI_قطار_IDX_P2_A_REQ;
		}
		توڑو;
	حالت RPMI_پیغام_POSTED_REQUEST:
		اگر (do_tx && do_rx)
			واپس SBI_EINVAL;
		اگر (do_tx) {
			tx_qid = RPMI_قطار_IDX_A2_P_REQ;
		} ورنہ {
			rx_qid = RPMI_قطار_IDX_P2_A_REQ;
		}
		توڑو;
	حالت RPMI_پیغام_ACKNOWLDGEMENT:
		اگر (do_tx && do_rx)
			واپس SBI_EINVAL;
		اگر (do_tx) {
			tx_qid = RPMI_قطار_IDX_A2_P_ACK;
		} ورنہ {
			rx_qid = RPMI_قطار_IDX_P2_A_ACK;
		}
		توڑو;
	طےشدہ:
		واپس SBI_ENOTSUPP;
	}

	اگر (do_tx) {
		ret = smq_tx(mctl, tx_qid, srvgrp_chan->servicegroup_id, xfer);
		اگر (ret)
			واپس ret;
	}

	اگر (do_rx) {
		ret = smq_rx(mctl, rx_qid, srvgrp_chan->servicegroup_id, xfer);
		اگر (ret)
			واپس ret;
	}

	واپس 0;
}

ساکن صحیح rpmi_shmem_mbox_get_attribute(ساخت mbox_chan *chan,
					 صحیح attr_id, خالی *برآمد_قدر)
{
	ساخت rpmi_shmem_mbox_controller *mctl =
			container_کا(chan->mbox,
				     ساخت rpmi_shmem_mbox_controller,
				     controller);
	ساخت rpmi_srvgrp_chan *srvgrp_chan = مقصد_srvgrp_chan(chan);

	انتخاب (attr_id) {
	حالت RPMI_CHANNEL_ATTR_PROTOCOL_نسخہ:
		*((u32 *)برآمد_قدر) = mctl->spec_نسخہ;
		توڑو;
	حالت RPMI_CHANNEL_ATTR_MAX_ڈیٹا_LEN:
		*((u32 *)برآمد_قدر) = RPMI_پیغام_ڈیٹا_سائز(mctl->خانہ_سائز);
		توڑو;
	حالت RPMI_CHANNEL_ATTR_P2_A_DOORBELL_SYSMSI_اشاریہ:
		*((u32 *)برآمد_قدر) = mctl->p2a_doorbell_sysmsi_اشاریہ;
		توڑو;
	حالت RPMI_CHANNEL_ATTR_TX_TIMEOUT:
		*((u32 *)برآمد_قدر) = RPMI_DEF_TX_TIMEOUT;
		توڑو;
	حالت RPMI_CHANNEL_ATTR_RX_TIMEOUT:
		*((u32 *)برآمد_قدر) = RPMI_DEF_RX_TIMEOUT;
		توڑو;
	حالت RPMI_CHANNEL_ATTR_SERVICEGROUP_ID:
		*((u32 *)برآمد_قدر) = srvgrp_chan->servicegroup_id;
		توڑو;
	حالت RPMI_CHANNEL_ATTR_SERVICEGROUP_نسخہ:
		*((u32 *)برآمد_قدر) = srvgrp_chan->servicegroup_نسخہ;
		توڑو;
	حالت RPMI_CHANNEL_ATTR_IMPL_ID:
		*((u32 *)برآمد_قدر) = mctl->impl_id;
		توڑو;
	حالت RPMI_CHANNEL_ATTR_IMPL_نسخہ:
		*((u32 *)برآمد_قدر) = mctl->impl_نسخہ;
		توڑو;
	طےشدہ:
		واپس SBI_ENOTSUPP;
	}

	واپس 0;
}

ساکن ساخت mbox_chan *rpmi_shmem_mbox_request_chan(
						ساخت mbox_controller *mbox,
						u32 *chan_دلائل)
{
	صحیح ret;
	u32 tval[2] = { 0 };
	ساخت rpmi_srvgrp_chan *srvgrp_chan;
	ساخت rpmi_shmem_mbox_controller *mctl =
			container_کا(mbox,
				     ساخت rpmi_shmem_mbox_controller,
				     controller);

	/* Service group id not defined or in reserved range is invalid */
	اگر (chan_دلائل[0] >= RPMI_SRVGRP_ID_MAX_گنتی &&
		chan_دلائل[0] <= RPMI_SRVGRP_RESERVE_انتہا)
		واپس تہی;

	/* Base serivce group is always present so probe other groups */
	اگر (chan_دلائل[0] != RPMI_SRVGRP_بنیاد) {
		/* Probe service group */
		ret = smq_بنیاد_get_two_u32(mctl,
					   RPMI_بنیاد_SRV_PROBE_SERVICE_گروہ,
					   chan_دلائل, tval);
		اگر (ret || !tval[1])
			واپس تہی;
	}

	srvgrp_chan = sbi_zalloc(حجم(*srvgrp_chan));
	اگر (!srvgrp_chan)
		واپس تہی;

	srvgrp_chan->servicegroup_id = chan_دلائل[0];
	srvgrp_chan->servicegroup_نسخہ = tval[1];

	واپس &srvgrp_chan->chan;
}

ساکن خالی rpmi_shmem_mbox_free_chan(ساخت mbox_controller *mbox,
				      ساخت mbox_chan *chan)
{
	ساخت rpmi_srvgrp_chan *srvgrp_chan = مقصد_srvgrp_chan(chan);
	sbi_free(srvgrp_chan);
}

خارجی ساخت fdt_mailbox fdt_mailbox_rpmi_shmem;

ساکن صحیح rpmi_shmem_transport_init(ساخت rpmi_shmem_mbox_controller *mctl,
				     ثابت خالی *fdt,
				     صحیح nodeoff) NO_THREAD_SAFETY_ANALYSIS
{
	ثابت حرف *name;
	ثابت fdt32_t *prop;
	صحیح گنتی, len, ret, qid;
	uint64_t reg_addr, reg_سائز;
	ساخت smq_قطار_ctx *qctx;

	ret = fdt_گرہ_check_compatible(fdt, nodeoff,
					"riscv,rpmi-shmem-mbox");
	اگر (ret)
		واپس ret;

	/* get queue slot size in bytes */
	prop = fdt_getprop(fdt, nodeoff, "riscv,slot-size", &len);
	اگر (!prop)
		واپس SBI_ENOENT;

	mctl->خانہ_سائز = fdt32_مقصد_cpu(*prop);
	اگر (mctl->خانہ_سائز < RPMI_خانہ_سائز_MIN) {
		sbi_printf("%s: slot_size < mimnum required message size\n",
			   __func__);
		mctl->خانہ_سائز = RPMI_خانہ_سائز_MIN;
	}

	/* get p2a doorbell system MSI index */
	prop = fdt_getprop(fdt, nodeoff, "riscv,p2a-doorbell-sysmsi-index", &len);
	mctl->p2a_doorbell_sysmsi_اشاریہ = prop ? fdt32_مقصد_cpu(*prop) : -1U;

	/* get a2p doorbell value */
	prop = fdt_getprop(fdt, nodeoff, "riscv,a2p-doorbell-value", &len);
	mctl->a2p_doorbell_قدر = prop ? fdt32_مقصد_cpu(*prop) : 1;

	/*
	 * queue names count is taken as the number of queues
	 * supported which make it mandatory to provide the
	 * name of the queue.
	 */
	گنتی = fdt_stringlist_گنتی(fdt, nodeoff, "reg-names");
	اگر (گنتی < 0 ||
	    گنتی > (RPMI_قطار_IDX_MAX_گنتی + RPMI_REG_IDX_MAX_گنتی))
		واپس SBI_EINVAL;

	mctl->قطار_گنتی = گنتی - RPMI_REG_IDX_MAX_گنتی;

	/* parse all queues and populate queues context structure */
	برائے (qid = 0; qid < mctl->قطار_گنتی; qid++) {
		qctx = &mctl->قطار_ctx_tbl[qid];

		/* get each queue share-memory base address and size*/
		ret = fdt_get_گرہ_addr_سائز(fdt, nodeoff, qid,
					     &reg_addr, &reg_سائز);
		اگر (ret < 0 || !reg_addr || !reg_سائز)
			واپس SBI_ENOENT;

		ret = sbi_domain_جڑ_جوڑو_memrange(reg_addr, reg_سائز, reg_سائز,
						   (SBI_DOMAIN_MEMREGION_MMIO |
						    SBI_DOMAIN_MEMREGION_M_READABLE |
						    SBI_DOMAIN_MEMREGION_M_WRITABLE));
		اگر (ret)
			واپس ret;

		/* calculate number of slots in each queue */
		qctx->عدد_slots =
			(reg_سائز - (mctl->خانہ_سائز * RPMI_قطار_سرخی_SLOTS)) / mctl->خانہ_سائز;

		/* setup queue pointers */
		qctx->headptr = ((خالی *)(غیرعلامتی طویل)reg_addr) +
				RPMI_قطار_سر_خانہ * mctl->خانہ_سائز;
		qctx->tailptr = ((خالی *)(غیرعلامتی طویل)reg_addr) +
				RPMI_قطار_دم_خانہ * mctl->خانہ_سائز;
		qctx->بفر = ((خالی *)(غیرعلامتی طویل)reg_addr) +
				RPMI_قطار_سرخی_SLOTS * mctl->خانہ_سائز;

		/* get the queue name */
		name = fdt_stringlist_get(fdt, nodeoff, "reg-names",
					  qid, &len);
		اگر (!name || (name && len < 0))
			واپس len;

		sbi_memcpy(qctx->name, name, len);

		/* store the index as queue_id */
		qctx->قطار_id = qid;

		SPIN_تالا_INIT(qctx->قطار_تالا);
	}

	/* get the a2p-doorbell property name */
	name = fdt_stringlist_get(fdt, nodeoff, "reg-names", qid, &len);
	اگر (!name || (name && len < 0))
		واپس len;

	/* fetch doorbell register address*/
	ret = fdt_get_گرہ_addr_سائز(fdt, nodeoff, qid, &reg_addr,
				       &reg_سائز);
	اگر (!ret && !(لڑی_موازنہ(name, "a2p-doorbell", لڑی_لمبائی("a2p-doorbell")))) {
		mctl->mb_regs = (خالی *)(غیرعلامتی طویل)reg_addr;
		ret = sbi_domain_جڑ_جوڑو_memrange(reg_addr, reg_سائز, reg_سائز,
						   (SBI_DOMAIN_MEMREGION_MMIO |
						    SBI_DOMAIN_MEMREGION_M_READABLE |
						    SBI_DOMAIN_MEMREGION_M_WRITABLE));
		اگر (ret)
			واپس ret;
	}

	واپس SBI_SUCCESS;
}

ساکن صحیح rpmi_shmem_mbox_init(ثابت خالی *fdt, صحیح nodeoff,
				ثابت ساخت fdt_مطابقت *مطابقت)
{
	ساخت rpmi_بنیاد_get_اوصاف_resp resp;
	ساخت rpmi_shmem_mbox_controller *mctl;
	ساخت rpmi_srvgrp_chan *بنیاد_srvgrp;
	u32 tval[2], دلائل[1];
	صحیح ret = 0;

	mctl = sbi_zalloc(حجم(*mctl));
	اگر (!mctl)
		واپس SBI_ENOMEM;

	/* Initialization transport from device tree */
	ret = rpmi_shmem_transport_init(mctl, fdt, nodeoff);
	اگر (ret)
		جاؤ fail_free_controller;

	/* Register mailbox controller */
	mctl->controller.id = nodeoff;
	mctl->controller.max_xfer_len =
			mctl->خانہ_سائز - حجم(ساخت rpmi_پیغام_سرخی);
	mctl->controller.driver = &fdt_mailbox_rpmi_shmem;
	mctl->controller.request_chan = rpmi_shmem_mbox_request_chan;
	mctl->controller.free_chan = rpmi_shmem_mbox_free_chan;
	mctl->controller.xfer = rpmi_shmem_mbox_xfer;
	mctl->controller.get_attribute = rpmi_shmem_mbox_get_attribute;
	ret = mbox_controller_جوڑو(&mctl->controller);
	اگر (ret)
		جاؤ fail_free_controller;

	/* Request base service group channel */
	tval[0] = RPMI_SRVGRP_بنیاد;
	mctl->بنیاد_chan = mbox_controller_request_chan(&mctl->controller,
							tval);
	اگر (!mctl->بنیاد_chan) {
		ret = SBI_ENOENT;
		جاؤ fail_remove_controller;
	}

	/* Update base service group version */
	بنیاد_srvgrp = مقصد_srvgrp_chan(mctl->بنیاد_chan);
	دلائل[0] = RPMI_SRVGRP_بنیاد;
	ret = smq_بنیاد_get_two_u32(mctl, RPMI_بنیاد_SRV_PROBE_SERVICE_گروہ,
				   &دلائل[0], tval);
	اگر (ret)
		جاؤ fail_free_chan;
	بنیاد_srvgrp->servicegroup_نسخہ = tval[1];
	اگر (بنیاد_srvgrp->servicegroup_نسخہ < RPMI_بنیاد_نسخہ_MIN) {
		ret = SBI_EINVAL;
		جاؤ fail_free_chan;
	}

	/* Get implementation id */
	ret = smq_بنیاد_get_two_u32(mctl,
				   RPMI_بنیاد_SRV_GET_IMPLEMENTATION_نسخہ,
				   تہی, tval);
	اگر (ret)
		جاؤ fail_free_chan;
	mctl->impl_نسخہ = tval[1];

	/* Get implementation version */
	ret = smq_بنیاد_get_two_u32(mctl, RPMI_بنیاد_SRV_GET_IMPLEMENTATION_IDN,
				   تہی, tval);
	اگر (ret)
		جاؤ fail_free_chan;
	mctl->impl_id = tval[1];

	/* Get specification version */
	ret = smq_بنیاد_get_two_u32(mctl, RPMI_بنیاد_SRV_GET_SPEC_نسخہ,
				   تہی, tval);
	اگر (ret)
		جاؤ fail_free_chan;
	mctl->spec_نسخہ = tval[1];
	اگر (mctl->spec_نسخہ < RPMI_بنیاد_نسخہ_MIN ||
	    mctl->spec_نسخہ != بنیاد_srvgrp->servicegroup_نسخہ) {
		ret = SBI_EINVAL;
		جاؤ fail_free_chan;
	}

	/* Get optional features implementation flags */
	ret = rpmi_معمول_request_with_حالت(
			mctl->بنیاد_chan, RPMI_بنیاد_SRV_GET_اوصاف,
			تہی, 0, 0,
			&resp, rpmi_u32_گنتی(resp), rpmi_u32_گنتی(resp));
	اگر (ret)
		جاؤ fail_free_chan;

	/* 1: M-mode, 0: S-mode */
	mctl->بنیاد_flags.f0_priv_level =
			resp.f0 & RPMI_بنیاد_FLAGS_F0_PRIVILEGE ? 1 : 0;
	/* 1: Supported, 0: Not Supported */
	mctl->بنیاد_flags.f0_ev_notif_en =
			resp.f0 & RPMI_بنیاد_FLAGS_F0_EV_NOTIFY ? 1 : 0;

	/* We only use M-mode RPMI context in OpenSBI */
	اگر (!mctl->بنیاد_flags.f0_priv_level) {
		ret = SBI_ENODEV;
		جاؤ fail_free_chan;
	}

	/*
	 * Continue without platform information string if not
	 * available or if an error is encountered while fetching
	 */
	rpmi_get_platform_info(mctl);

	واپس 0;

fail_free_chan:
	mbox_controller_free_chan(mctl->بنیاد_chan);
fail_remove_controller:
	mbox_controller_remove(&mctl->controller);
fail_free_controller:
	sbi_free(mctl);
	واپس ret;
}

ساکن ثابت ساخت fdt_مطابقت rpmi_shmem_mbox_مطابقت[] = {
	{ .compatible = "riscv,rpmi-shmem-mbox" },
	{ },
};

ساخت fdt_mailbox fdt_mailbox_rpmi_shmem = {
	.driver = {
		.مطابقت_جدول = rpmi_shmem_mbox_مطابقت,
		.init = rpmi_shmem_mbox_init,
	},
	.xlate = fdt_mailbox_simple_xlate,
};
