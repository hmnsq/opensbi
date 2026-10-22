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

#ضمن <libfdt.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi/sbi_timer.h>
#ضمن <sbi/riscv_io.h>
#ضمن <sbi/riscv_locks.h>
#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/riscv_barrier.h>
#ضمن <sbi_utils/fdt/fdt_helper.h>
#ضمن <sbi_utils/mailbox/mailbox.h>
#ضمن <sbi_utils/mailbox/fdt_mailbox.h>
#ضمن <sbi_utils/mailbox/rpmi_mailbox.h>

/** Minimum Base group version required */
#تعریف_کن RPMI_پایه_نسخه_MIN		RPMI_نسخه(1, 0)

/**************** RPMI Transport Structures and Macros ***********/

#تعریف_کن GET_SERVICEGROUP_ID(پیام)		\
({						\
	بنية rpmi_پیام *mbuf = پیام;	\
	le16_مقصد_cpu(mbuf->سرآیند.servicegroup_id);\
})

#تعریف_کن GET_SERVICE_ID(پیام)			\
({						\
	بنية rpmi_پیام *mbuf = پیام;	\
	mbuf->سرآیند.service_id;		\
})

#تعریف_کن GET_FLAGS(پیام)				\
({						\
	بنية rpmi_پیام *mbuf = پیام;	\
	mbuf->سرآیند.flags;			\
})

#تعریف_کن GET_پیام_ID(پیام)			\
({						\
	بنية rpmi_پیام *mbuf = پیام;	\
	((u32)mbuf->سرآیند.flags << (RPMI_پیام_FLAGS_جابه_جایی * 8)) | \
	((u32)mbuf->سرآیند.service_id << (RPMI_پیام_SERVICE_ID_جابه_جایی * 8)) | \
	((u32)le16_مقصد_cpu(mbuf->سرآیند.servicegroup_id)); \
})

#تعریف_کن MAKE_پیام_ID(__group_id, __service_id, __flags)	\
({						\
	u32 __ret = 0;				\
	__ret |= (u32)(__group_id) << (RPMI_پیام_SERVICEGROUP_ID_جابه_جایی * 8); \
	__ret |= (u32)(__service_id) << (RPMI_پیام_SERVICE_ID_جابه_جایی * 8); \
	__ret |= (u32)(__flags) << (RPMI_پیام_FLAGS_جابه_جایی * 8); \
	__ret;					\
})

#تعریف_کن GET_DLEN(پیام)				\
({						\
	بنية rpmi_پیام *mbuf = پیام;	\
	le16_مقصد_cpu(mbuf->سرآیند.datalen);	\
})

#تعریف_کن GET_TOKEN(پیام)				\
({						\
	بنية rpmi_پیام *mbuf = پیام;	\
	le16_مقصد_cpu(mbuf->سرآیند.token);	\
})

#تعریف_کن GET_پیام_TYPE(پیام)						\
({									\
	uint8_t flags = *((uint8_t *)پیام + RPMI_پیام_FLAGS_جابه_جایی);	\
	((flags & RPMI_پیام_FLAGS_TYPE) >> RPMI_پیام_FLAGS_TYPE_POS));	\
})

تعداد rpmi_صف_type {
	RPMI_صف_TYPE_REQ = 0,
	RPMI_صف_TYPE_ACK = 1,
};

تعداد rpmi_صف_idx {
	RPMI_صف_IDX_A2_P_REQ = 0,
	RPMI_صف_IDX_P2_A_ACK = 1,
	RPMI_صف_IDX_P2_A_REQ = 2,
	RPMI_صف_IDX_A2_P_ACK = 3,
	RPMI_صف_IDX_MAX_شمار,
};

تعداد rpmi_reg_idx {
	RPMI_REG_IDX_DB_REG = 0, /* Doorbell register */
	RPMI_REG_IDX_MAX_شمار,
};

/** Mailbox registers */
بنية rpmi_mb_regs {
	/* doorbell from AP -> PuC*/
	متطاير le32_t db_reg;
} __packed;

/** Single Queue Context Structure */
بنية smq_صف_ctx {
	u32 صف_id;
	u32 رقم_slots;
	spinlock_t صف_قفل;
	/* Type of queue - REQ or ACK */
	تعداد rpmi_صف_type صف_type;
	/* Pointers to the queue shared memory */
	متطاير le32_t *headptr PT_GUARDED_BY(&صف_قفل);
	متطاير le32_t *tailptr PT_GUARDED_BY(&صف_قفل);
	متطاير uint8_t *میانگیر PT_GUARDED_BY(&صف_قفل);
	/* Name of the queue */
	حرف name[RPMI_NAME_CHARS_MAX];
};

بنية rpmi_srvgrp_chan {
	u32 servicegroup_id;
	u32 servicegroup_نسخه;
	بنية mbox_chan chan;
};

#تعریف_کن مقصد_srvgrp_chan(mbox_chan)	\
		container_از(mbox_chan, بنية rpmi_srvgrp_chan, chan);

بنية rpmi_shmem_mbox_controller {
	/* Driver specific members */
	u32 جایگاه_اندازه;
	u32 صف_شمار;
	u32 p2a_doorbell_sysmsi_شاخص;
	u32 a2p_doorbell_مقدار;
	بنية rpmi_mb_regs *mb_regs;
	بنية smq_صف_ctx صف_ctx_tbl[RPMI_صف_IDX_MAX_شمار];
	/* Mailbox framework related members */
	بنية mbox_controller controller;
	بنية mbox_chan *پایه_chan;
	u32 impl_نسخه;
	u32 impl_id;
	u32 spec_نسخه;
	u32 plat_info_len;
	حرف *plat_info;
	بنية {
		u8 f0_priv_level;
		bool f0_ev_notif_en;
	} پایه_flags;
};

/**************** Shared Memory Queues Helpers **************/

ساكن bool __smq_queue_full(بنية smq_صف_ctx *qctx)
	MUST_HOLD(&qctx->صف_قفل)
{
	أرجع ((le32_مقصد_cpu(*qctx->tailptr) + 1) % qctx->رقم_slots ==
			le32_مقصد_cpu(*qctx->headptr)) ? true : false;
}

ساكن bool __smq_queue_empty(بنية smq_صف_ctx *qctx)
	MUST_HOLD(&qctx->صف_قفل)
{
	أرجع (le32_مقصد_cpu(*qctx->headptr) ==
		le32_مقصد_cpu(*qctx->tailptr)) ? true : false;
}

ساكن صحيح __smq_rx(بنية smq_صف_ctx *qctx, u32 جایگاه_اندازه,
		    u32 service_گروه_id, بنية mbox_xfer *xfer)
	MUST_HOLD(&qctx->صف_قفل)
{
	فراغ *dst, *src;
	بنية rpmi_پیام *پیام;
	u32 i, tmp, pos, dlen, msgidn, headidx, tailidx;
	بنية rpmi_پیام_آرگومانها *آرگومانها = xfer->آرگومانها;
	bool no_rx_token = (آرگومانها->flags & RPMI_پیام_FLAGS_NO_RX_TOKEN) ?
			   true : false;

	/* Rx sanity checks */
	إذا ((حجم(u32) * آرگومانها->rx_endian_واژگان) >
	    (جایگاه_اندازه - حجم(بنية rpmi_پیام_سرآیند)))
		أرجع SBI_EINVAL;
	إذا ((حجم(u32) * آرگومانها->rx_endian_واژگان) > xfer->rx_len)
		أرجع SBI_EINVAL;

	/* There should be some message in the queue */
	إذا (__smq_queue_empty(qctx))
		أرجع SBI_ENOENT;

	/* Get the head/read index and tail/write index */
	headidx = le32_مقصد_cpu(*qctx->headptr);
	tailidx = le32_مقصد_cpu(*qctx->tailptr);

	/*
	 * Compute msgidn expected in the incoming message
	 * NOTE: DOORBELL bit is not expected to be set.
	 */
	msgidn = MAKE_پیام_ID(service_گروه_id, آرگومانها->service_id, آرگومانها->type);

	/* Find the Rx message with matching token */
	pos = headidx;
	طالما (pos != tailidx) {
		src = (فراغ *)qctx->میانگیر + (pos * جایگاه_اندازه);
		إذا ((no_rx_token && GET_پیام_ID(src) == msgidn) ||
		    (GET_TOKEN(src) == (xfer->seq & RPMI_پیام_TOKEN_ماسک)))
			اكسر;
		pos = (pos + 1) % qctx->رقم_slots;
	}
	إذا (pos == tailidx)
		أرجع SBI_ENOENT;

	/* If Rx message is not first message then make it first message */
	إذا (pos != headidx) {
		src = (فراغ *)qctx->میانگیر + (pos * جایگاه_اندازه);
		dst = (فراغ *)qctx->میانگیر + (headidx * جایگاه_اندازه);
		لكل (i = 0; i < جایگاه_اندازه / حجم(u32); i++) {
			tmp = ((u32 *)dst)[i];
			((u32 *)dst)[i] = ((u32 *)src)[i];
			((u32 *)src)[i] = tmp;
		}
	}

	/* Update rx_token if not available */
	پیام = (فراغ *)qctx->میانگیر + (headidx * جایگاه_اندازه);
	إذا (no_rx_token)
		آرگومانها->rx_token = GET_TOKEN(پیام);

	/* Extract data from the first message */
	إذا (xfer->rx) {
		آرگومانها->rx_داده_len = dlen = GET_DLEN(پیام);
		إذا (dlen > xfer->rx_len)
			dlen = xfer->rx_len;
		src = (فراغ *)پیام + حجم(بنية rpmi_پیام_سرآیند);
		dst = xfer->rx;
		لكل (i = 0; i < آرگومانها->rx_endian_واژگان; i++)
			((u32 *)dst)[i] = le32_مقصد_cpu(((u32 *)src)[i]);
		dst += حجم(u32) * آرگومانها->rx_endian_واژگان;
		src += حجم(u32) * آرگومانها->rx_endian_واژگان;
		sbi_memcpy(dst, src,
			xfer->rx_len - (حجم(u32) * آرگومانها->rx_endian_واژگان));
	}

	/* Update the head/read index */
	*qctx->headptr = cpu_مقصد_le32(headidx + 1) % qctx->رقم_slots;

	/* Make sure updates to head are immediately visible to PuC */
	smp_wmb();

	أرجع SBI_OK;
}

ساكن صحيح __smq_tx(بنية smq_صف_ctx *qctx, بنية rpmi_mb_regs *mb_regs,
		    u32 a2p_doorbell_مقدار, u32 جایگاه_اندازه, u32 service_گروه_id,
		    بنية mbox_xfer *xfer) MUST_HOLD(&qctx->صف_قفل)
{
	u32 i, tailidx;
	فراغ *dst, *src;
	بنية rpmi_پیام_سرآیند سرآیند = { 0 };
	بنية rpmi_پیام_آرگومانها *آرگومانها = xfer->آرگومانها;

	/* Tx sanity checks */
	إذا ((حجم(u32) * آرگومانها->tx_endian_واژگان) >
	    (جایگاه_اندازه - حجم(بنية rpmi_پیام_سرآیند)))
		أرجع SBI_EINVAL;
	إذا ((حجم(u32) * آرگومانها->tx_endian_واژگان) > xfer->tx_len)
		أرجع SBI_EINVAL;

	/* There should be some room in the queue */
	إذا (__smq_queue_full(qctx))
		أرجع SBI_ENOMEM;

	/* Get the tail/write index */
	tailidx = le32_مقصد_cpu(*qctx->tailptr);

	/* Prepare the header to be written into the slot */
	سرآیند.servicegroup_id = cpu_مقصد_le16(service_گروه_id);
	سرآیند.service_id = آرگومانها->service_id;
	سرآیند.flags = آرگومانها->type;
	سرآیند.datalen = cpu_مقصد_le16((u16)xfer->tx_len);
	سرآیند.token = cpu_مقصد_le16((u16)xfer->seq);

	/* Write header into the slot */
	dst = (حرف *)qctx->میانگیر + (tailidx * جایگاه_اندازه);
	sbi_memcpy(dst, &سرآیند, حجم(سرآیند));
	dst += حجم(سرآیند);

	/* Write data into the slot */
	إذا (xfer->tx) {
		src = xfer->tx;
		لكل (i = 0; i < آرگومانها->tx_endian_واژگان; i++)
			((u32 *)dst)[i] = cpu_مقصد_le32(((u32 *)src)[i]);
		dst += حجم(u32) * آرگومانها->tx_endian_واژگان;
		src += حجم(u32) * آرگومانها->tx_endian_واژگان;
		sbi_memcpy(dst, src,
			xfer->tx_len - (حجم(u32) * آرگومانها->tx_endian_واژگان));
	}

	/* Make sure queue chanages are visible to PuC before updating tail */
	smp_wmb();

	/* Update the tail/write index */
	*qctx->tailptr = cpu_مقصد_le32(tailidx + 1) % qctx->رقم_slots;

	/* Ring the RPMI doorbell if present */
	إذا (mb_regs)
		writel(a2p_doorbell_مقدار, &mb_regs->db_reg);

	أرجع SBI_OK;
}

ساكن صحيح smq_rx(بنية rpmi_shmem_mbox_controller *mctl,
		  u32 صف_id, u32 service_گروه_id, بنية mbox_xfer *xfer)
{
	صحيح ret, rxretry = 0;
	بنية smq_صف_ctx *qctx;

	إذا (mctl->صف_شمار < صف_id) {
		sbi_printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		أرجع SBI_EINVAL;
	}
	qctx = &mctl->صف_ctx_tbl[صف_id];

	/*
	 * Once the timeout happens and call this function is returned
	 * to the client then there is no way to deliver the response
	 * message after that if it comes later.
	 *
	 * REVISIT: In complete timeout duration how much duration
	 * it should wait(delay) before recv retry. udelay or mdelay
	 */
	افعل {
		spin_قفل(&qctx->صف_قفل);
		ret = __smq_rx(qctx, mctl->جایگاه_اندازه, service_گروه_id, xfer);
		spin_unlock(&qctx->صف_قفل);
		إذا (!ret)
			أرجع 0;

		sbi_timer_mdelay(1);
		rxretry += 1;
	} طالما (rxretry < xfer->rx_timeout);

	أرجع SBI_ETIMEDOUT;
}

ساكن صحيح smq_tx(بنية rpmi_shmem_mbox_controller *mctl,
		  u32 صف_id, u32 service_گروه_id, بنية mbox_xfer *xfer)
{
	صحيح ret, txretry = 0;
	بنية smq_صف_ctx *qctx;

	إذا (mctl->صف_شمار < صف_id) {
		sbi_printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		أرجع SBI_EINVAL;
	}
	qctx = &mctl->صف_ctx_tbl[صف_id];

	/*
	 * Ignoring the tx timeout since in RPMI has no mechanism
	 * with which other side can let know about the reception of
	 * message which marks as tx complete. For RPMI tx complete is
	 * marked as done when message in successfully copied in queue.
	 *
	 * REVISIT: In complete timeout duration how much duration
	 * it should wait(delay) before send retry. udelay or mdelay
	 */
	افعل {
		spin_قفل(&qctx->صف_قفل);
		ret = __smq_tx(qctx, mctl->mb_regs, mctl->a2p_doorbell_مقدار,
				mctl->جایگاه_اندازه, service_گروه_id, xfer);
		spin_unlock(&qctx->صف_قفل);
		إذا (!ret)
			أرجع 0;

		sbi_timer_mdelay(1);
		txretry += 1;
	} طالما (txretry < xfer->tx_timeout);

	أرجع SBI_ETIMEDOUT;
}

ساكن صحيح rpmi_get_platform_info(بنية rpmi_shmem_mbox_controller *mctl)
{
	صحيح ret = SBI_OK;

	/**
	 * platform string can occupy max possible size
	 * max possible space in the message data as
	 * per the format
	 */
	بنية rpmi_پایه_get_platform_info_resp *resp =
			sbi_zalloc(RPMI_پیام_داده_اندازه(mctl->جایگاه_اندازه));
	إذا (!resp)
		أرجع SBI_ENOMEM;

	ret = rpmi_نرمال_request_with_وضعیت(mctl->پایه_chan,
				 RPMI_پایه_SRV_GET_PLATFORM_INFO,
				 عدم, 0, 0,
				 resp,
				 RPMI_پیام_داده_اندازه(mctl->جایگاه_اندازه)/4,
				 RPMI_پیام_داده_اندازه(mctl->جایگاه_اندازه)/4);
	إذا (ret)
		اذهب fail_free_resp;

	mctl->plat_info_len = resp->plat_info_len;
	mctl->plat_info = sbi_zalloc(mctl->plat_info_len);
	إذا (!mctl->plat_info) {
		ret = SBI_ENOMEM;
		اذهب fail_free_resp;
	}

	sbi_strncpy(mctl->plat_info, resp->plat_info, mctl->plat_info_len);

fail_free_resp:
	sbi_free(resp);
	أرجع ret;
}

ساكن صحيح smq_پایه_get_two_u32(بنية rpmi_shmem_mbox_controller *mctl,
				u32 service_id, u32 *inarg, u32 *outvals)
{
	أرجع rpmi_نرمال_request_with_وضعیت(
			mctl->پایه_chan, service_id,
			inarg, (inarg) ? 1 : 0, (inarg) ? 1 : 0,
			outvals, 2, 2);
}

/**************** Mailbox Controller Functions **************/

ساكن صحيح rpmi_shmem_mbox_xfer(بنية mbox_chan *chan, بنية mbox_xfer *xfer)
{
	صحيح ret;
	u32 tx_qid = 0, rx_qid = 0;
	بنية rpmi_shmem_mbox_controller *mctl =
			container_از(chan->mbox,
				     بنية rpmi_shmem_mbox_controller,
				     controller);
	بنية rpmi_srvgrp_chan *srvgrp_chan = مقصد_srvgrp_chan(chan);

	بنية rpmi_پیام_آرگومانها *آرگومانها = xfer->آرگومانها;
	bool do_tx = (آرگومانها->flags & RPMI_پیام_FLAGS_NO_TX) ? false : true;
	bool do_rx = (آرگومانها->flags & RPMI_پیام_FLAGS_NO_RX) ? false : true;

	إذا (!do_tx && !do_rx)
		أرجع SBI_EINVAL;

	بدل (آرگومانها->type) {
	حالة RPMI_پیام_نرمال_REQUEST:
		إذا (do_tx && do_rx) {
			tx_qid = RPMI_صف_IDX_A2_P_REQ;
			rx_qid = RPMI_صف_IDX_P2_A_ACK;
		} وإلا إذا (do_tx) {
			tx_qid = RPMI_صف_IDX_A2_P_REQ;
		} وإلا إذا (do_rx) {
			rx_qid = RPMI_صف_IDX_P2_A_REQ;
		}
		اكسر;
	حالة RPMI_پیام_POSTED_REQUEST:
		إذا (do_tx && do_rx)
			أرجع SBI_EINVAL;
		إذا (do_tx) {
			tx_qid = RPMI_صف_IDX_A2_P_REQ;
		} وإلا {
			rx_qid = RPMI_صف_IDX_P2_A_REQ;
		}
		اكسر;
	حالة RPMI_پیام_ACKNOWLDGEMENT:
		إذا (do_tx && do_rx)
			أرجع SBI_EINVAL;
		إذا (do_tx) {
			tx_qid = RPMI_صف_IDX_A2_P_ACK;
		} وإلا {
			rx_qid = RPMI_صف_IDX_P2_A_ACK;
		}
		اكسر;
	افتراضي:
		أرجع SBI_ENOTSUPP;
	}

	إذا (do_tx) {
		ret = smq_tx(mctl, tx_qid, srvgrp_chan->servicegroup_id, xfer);
		إذا (ret)
			أرجع ret;
	}

	إذا (do_rx) {
		ret = smq_rx(mctl, rx_qid, srvgrp_chan->servicegroup_id, xfer);
		إذا (ret)
			أرجع ret;
	}

	أرجع 0;
}

ساكن صحيح rpmi_shmem_mbox_get_attribute(بنية mbox_chan *chan,
					 صحيح attr_id, فراغ *بیرون_مقدار)
{
	بنية rpmi_shmem_mbox_controller *mctl =
			container_از(chan->mbox,
				     بنية rpmi_shmem_mbox_controller,
				     controller);
	بنية rpmi_srvgrp_chan *srvgrp_chan = مقصد_srvgrp_chan(chan);

	بدل (attr_id) {
	حالة RPMI_CHANNEL_ATTR_PROTOCOL_نسخه:
		*((u32 *)بیرون_مقدار) = mctl->spec_نسخه;
		اكسر;
	حالة RPMI_CHANNEL_ATTR_MAX_داده_LEN:
		*((u32 *)بیرون_مقدار) = RPMI_پیام_داده_اندازه(mctl->جایگاه_اندازه);
		اكسر;
	حالة RPMI_CHANNEL_ATTR_P2_A_DOORBELL_SYSMSI_شاخص:
		*((u32 *)بیرون_مقدار) = mctl->p2a_doorbell_sysmsi_شاخص;
		اكسر;
	حالة RPMI_CHANNEL_ATTR_TX_TIMEOUT:
		*((u32 *)بیرون_مقدار) = RPMI_DEF_TX_TIMEOUT;
		اكسر;
	حالة RPMI_CHANNEL_ATTR_RX_TIMEOUT:
		*((u32 *)بیرون_مقدار) = RPMI_DEF_RX_TIMEOUT;
		اكسر;
	حالة RPMI_CHANNEL_ATTR_SERVICEGROUP_ID:
		*((u32 *)بیرون_مقدار) = srvgrp_chan->servicegroup_id;
		اكسر;
	حالة RPMI_CHANNEL_ATTR_SERVICEGROUP_نسخه:
		*((u32 *)بیرون_مقدار) = srvgrp_chan->servicegroup_نسخه;
		اكسر;
	حالة RPMI_CHANNEL_ATTR_IMPL_ID:
		*((u32 *)بیرون_مقدار) = mctl->impl_id;
		اكسر;
	حالة RPMI_CHANNEL_ATTR_IMPL_نسخه:
		*((u32 *)بیرون_مقدار) = mctl->impl_نسخه;
		اكسر;
	افتراضي:
		أرجع SBI_ENOTSUPP;
	}

	أرجع 0;
}

ساكن بنية mbox_chan *rpmi_shmem_mbox_request_chan(
						بنية mbox_controller *mbox,
						u32 *chan_آرگومانها)
{
	صحيح ret;
	u32 tval[2] = { 0 };
	بنية rpmi_srvgrp_chan *srvgrp_chan;
	بنية rpmi_shmem_mbox_controller *mctl =
			container_از(mbox,
				     بنية rpmi_shmem_mbox_controller,
				     controller);

	/* Service group id not defined or in reserved range is invalid */
	إذا (chan_آرگومانها[0] >= RPMI_SRVGRP_ID_MAX_شمار &&
		chan_آرگومانها[0] <= RPMI_SRVGRP_RESERVE_انتها)
		أرجع عدم;

	/* Base serivce group is always present so probe other groups */
	إذا (chan_آرگومانها[0] != RPMI_SRVGRP_پایه) {
		/* Probe service group */
		ret = smq_پایه_get_two_u32(mctl,
					   RPMI_پایه_SRV_PROBE_SERVICE_گروه,
					   chan_آرگومانها, tval);
		إذا (ret || !tval[1])
			أرجع عدم;
	}

	srvgrp_chan = sbi_zalloc(حجم(*srvgrp_chan));
	إذا (!srvgrp_chan)
		أرجع عدم;

	srvgrp_chan->servicegroup_id = chan_آرگومانها[0];
	srvgrp_chan->servicegroup_نسخه = tval[1];

	أرجع &srvgrp_chan->chan;
}

ساكن فراغ rpmi_shmem_mbox_free_chan(بنية mbox_controller *mbox,
				      بنية mbox_chan *chan)
{
	بنية rpmi_srvgrp_chan *srvgrp_chan = مقصد_srvgrp_chan(chan);
	sbi_free(srvgrp_chan);
}

خارجي بنية fdt_mailbox fdt_mailbox_rpmi_shmem;

ساكن صحيح rpmi_shmem_transport_init(بنية rpmi_shmem_mbox_controller *mctl,
				     ثابت فراغ *fdt,
				     صحيح nodeoff) NO_THREAD_SAFETY_ANALYSIS
{
	ثابت حرف *name;
	ثابت fdt32_t *prop;
	صحيح شمار, len, ret, qid;
	uint64_t reg_addr, reg_اندازه;
	بنية smq_صف_ctx *qctx;

	ret = fdt_گره_check_compatible(fdt, nodeoff,
					"riscv,rpmi-shmem-mbox");
	إذا (ret)
		أرجع ret;

	/* get queue slot size in bytes */
	prop = fdt_getprop(fdt, nodeoff, "riscv,slot-size", &len);
	إذا (!prop)
		أرجع SBI_ENOENT;

	mctl->جایگاه_اندازه = fdt32_مقصد_cpu(*prop);
	إذا (mctl->جایگاه_اندازه < RPMI_جایگاه_اندازه_MIN) {
		sbi_printf("%s: slot_size < mimnum required message size\n",
			   __func__);
		mctl->جایگاه_اندازه = RPMI_جایگاه_اندازه_MIN;
	}

	/* get p2a doorbell system MSI index */
	prop = fdt_getprop(fdt, nodeoff, "riscv,p2a-doorbell-sysmsi-index", &len);
	mctl->p2a_doorbell_sysmsi_شاخص = prop ? fdt32_مقصد_cpu(*prop) : -1U;

	/* get a2p doorbell value */
	prop = fdt_getprop(fdt, nodeoff, "riscv,a2p-doorbell-value", &len);
	mctl->a2p_doorbell_مقدار = prop ? fdt32_مقصد_cpu(*prop) : 1;

	/*
	 * queue names count is taken as the number of queues
	 * supported which make it mandatory to provide the
	 * name of the queue.
	 */
	شمار = fdt_stringlist_شمار(fdt, nodeoff, "reg-names");
	إذا (شمار < 0 ||
	    شمار > (RPMI_صف_IDX_MAX_شمار + RPMI_REG_IDX_MAX_شمار))
		أرجع SBI_EINVAL;

	mctl->صف_شمار = شمار - RPMI_REG_IDX_MAX_شمار;

	/* parse all queues and populate queues context structure */
	لكل (qid = 0; qid < mctl->صف_شمار; qid++) {
		qctx = &mctl->صف_ctx_tbl[qid];

		/* get each queue share-memory base address and size*/
		ret = fdt_get_گره_addr_اندازه(fdt, nodeoff, qid,
					     &reg_addr, &reg_اندازه);
		إذا (ret < 0 || !reg_addr || !reg_اندازه)
			أرجع SBI_ENOENT;

		ret = sbi_domain_ریشه_جمع_کن_memrange(reg_addr, reg_اندازه, reg_اندازه,
						   (SBI_DOMAIN_MEMREGION_MMIO |
						    SBI_DOMAIN_MEMREGION_M_READABLE |
						    SBI_DOMAIN_MEMREGION_M_WRITABLE));
		إذا (ret)
			أرجع ret;

		/* calculate number of slots in each queue */
		qctx->رقم_slots =
			(reg_اندازه - (mctl->جایگاه_اندازه * RPMI_صف_سرآیند_SLOTS)) / mctl->جایگاه_اندازه;

		/* setup queue pointers */
		qctx->headptr = ((فراغ *)(بیعلامت دراز)reg_addr) +
				RPMI_صف_سر_جایگاه * mctl->جایگاه_اندازه;
		qctx->tailptr = ((فراغ *)(بیعلامت دراز)reg_addr) +
				RPMI_صف_دم_جایگاه * mctl->جایگاه_اندازه;
		qctx->میانگیر = ((فراغ *)(بیعلامت دراز)reg_addr) +
				RPMI_صف_سرآیند_SLOTS * mctl->جایگاه_اندازه;

		/* get the queue name */
		name = fdt_stringlist_get(fdt, nodeoff, "reg-names",
					  qid, &len);
		إذا (!name || (name && len < 0))
			أرجع len;

		sbi_memcpy(qctx->name, name, len);

		/* store the index as queue_id */
		qctx->صف_id = qid;

		SPIN_قفل_INIT(qctx->صف_قفل);
	}

	/* get the a2p-doorbell property name */
	name = fdt_stringlist_get(fdt, nodeoff, "reg-names", qid, &len);
	إذا (!name || (name && len < 0))
		أرجع len;

	/* fetch doorbell register address*/
	ret = fdt_get_گره_addr_اندازه(fdt, nodeoff, qid, &reg_addr,
				       &reg_اندازه);
	إذا (!ret && !(مقایسه_رشته(name, "a2p-doorbell", طول_رشته("a2p-doorbell")))) {
		mctl->mb_regs = (فراغ *)(بیعلامت دراز)reg_addr;
		ret = sbi_domain_ریشه_جمع_کن_memrange(reg_addr, reg_اندازه, reg_اندازه,
						   (SBI_DOMAIN_MEMREGION_MMIO |
						    SBI_DOMAIN_MEMREGION_M_READABLE |
						    SBI_DOMAIN_MEMREGION_M_WRITABLE));
		إذا (ret)
			أرجع ret;
	}

	أرجع SBI_SUCCESS;
}

ساكن صحيح rpmi_shmem_mbox_init(ثابت فراغ *fdt, صحيح nodeoff,
				ثابت بنية fdt_تطابق *تطابق)
{
	بنية rpmi_پایه_get_صفات_resp resp;
	بنية rpmi_shmem_mbox_controller *mctl;
	بنية rpmi_srvgrp_chan *پایه_srvgrp;
	u32 tval[2], آرگومانها[1];
	صحيح ret = 0;

	mctl = sbi_zalloc(حجم(*mctl));
	إذا (!mctl)
		أرجع SBI_ENOMEM;

	/* Initialization transport from device tree */
	ret = rpmi_shmem_transport_init(mctl, fdt, nodeoff);
	إذا (ret)
		اذهب fail_free_controller;

	/* Register mailbox controller */
	mctl->controller.id = nodeoff;
	mctl->controller.max_xfer_len =
			mctl->جایگاه_اندازه - حجم(بنية rpmi_پیام_سرآیند);
	mctl->controller.driver = &fdt_mailbox_rpmi_shmem;
	mctl->controller.request_chan = rpmi_shmem_mbox_request_chan;
	mctl->controller.free_chan = rpmi_shmem_mbox_free_chan;
	mctl->controller.xfer = rpmi_shmem_mbox_xfer;
	mctl->controller.get_attribute = rpmi_shmem_mbox_get_attribute;
	ret = mbox_controller_جمع_کن(&mctl->controller);
	إذا (ret)
		اذهب fail_free_controller;

	/* Request base service group channel */
	tval[0] = RPMI_SRVGRP_پایه;
	mctl->پایه_chan = mbox_controller_request_chan(&mctl->controller,
							tval);
	إذا (!mctl->پایه_chan) {
		ret = SBI_ENOENT;
		اذهب fail_remove_controller;
	}

	/* Update base service group version */
	پایه_srvgrp = مقصد_srvgrp_chan(mctl->پایه_chan);
	آرگومانها[0] = RPMI_SRVGRP_پایه;
	ret = smq_پایه_get_two_u32(mctl, RPMI_پایه_SRV_PROBE_SERVICE_گروه,
				   &آرگومانها[0], tval);
	إذا (ret)
		اذهب fail_free_chan;
	پایه_srvgrp->servicegroup_نسخه = tval[1];
	إذا (پایه_srvgrp->servicegroup_نسخه < RPMI_پایه_نسخه_MIN) {
		ret = SBI_EINVAL;
		اذهب fail_free_chan;
	}

	/* Get implementation id */
	ret = smq_پایه_get_two_u32(mctl,
				   RPMI_پایه_SRV_GET_IMPLEMENTATION_نسخه,
				   عدم, tval);
	إذا (ret)
		اذهب fail_free_chan;
	mctl->impl_نسخه = tval[1];

	/* Get implementation version */
	ret = smq_پایه_get_two_u32(mctl, RPMI_پایه_SRV_GET_IMPLEMENTATION_IDN,
				   عدم, tval);
	إذا (ret)
		اذهب fail_free_chan;
	mctl->impl_id = tval[1];

	/* Get specification version */
	ret = smq_پایه_get_two_u32(mctl, RPMI_پایه_SRV_GET_SPEC_نسخه,
				   عدم, tval);
	إذا (ret)
		اذهب fail_free_chan;
	mctl->spec_نسخه = tval[1];
	إذا (mctl->spec_نسخه < RPMI_پایه_نسخه_MIN ||
	    mctl->spec_نسخه != پایه_srvgrp->servicegroup_نسخه) {
		ret = SBI_EINVAL;
		اذهب fail_free_chan;
	}

	/* Get optional features implementation flags */
	ret = rpmi_نرمال_request_with_وضعیت(
			mctl->پایه_chan, RPMI_پایه_SRV_GET_صفات,
			عدم, 0, 0,
			&resp, rpmi_u32_شمار(resp), rpmi_u32_شمار(resp));
	إذا (ret)
		اذهب fail_free_chan;

	/* 1: M-mode, 0: S-mode */
	mctl->پایه_flags.f0_priv_level =
			resp.f0 & RPMI_پایه_FLAGS_F0_PRIVILEGE ? 1 : 0;
	/* 1: Supported, 0: Not Supported */
	mctl->پایه_flags.f0_ev_notif_en =
			resp.f0 & RPMI_پایه_FLAGS_F0_EV_NOTIFY ? 1 : 0;

	/* We only use M-mode RPMI context in OpenSBI */
	إذا (!mctl->پایه_flags.f0_priv_level) {
		ret = SBI_ENODEV;
		اذهب fail_free_chan;
	}

	/*
	 * Continue without platform information string if not
	 * available or if an error is encountered while fetching
	 */
	rpmi_get_platform_info(mctl);

	أرجع 0;

fail_free_chan:
	mbox_controller_free_chan(mctl->پایه_chan);
fail_remove_controller:
	mbox_controller_remove(&mctl->controller);
fail_free_controller:
	sbi_free(mctl);
	أرجع ret;
}

ساكن ثابت بنية fdt_تطابق rpmi_shmem_mbox_تطابق[] = {
	{ .compatible = "riscv,rpmi-shmem-mbox" },
	{ },
};

بنية fdt_mailbox fdt_mailbox_rpmi_shmem = {
	.driver = {
		.تطابق_جدول = rpmi_shmem_mbox_تطابق,
		.init = rpmi_shmem_mbox_init,
	},
	.xlate = fdt_mailbox_simple_xlate,
};
