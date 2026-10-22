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
#define RPMI_BASE_VERSION_MIN		RPMI_VERSION(1, 0)

/**************** RPMI Transport Structures and Macros ***********/

#define GET_SERVICEGROUP_ID(رسالة)		\
({						\
	بنية rpmi_message *mbuf = رسالة;	\
	le16_to_cpu(mbuf->header.servicegroup_id);\
})

#define GET_SERVICE_ID(رسالة)			\
({						\
	بنية rpmi_message *mbuf = رسالة;	\
	mbuf->header.service_id;		\
})

#define GET_FLAGS(رسالة)				\
({						\
	بنية rpmi_message *mbuf = رسالة;	\
	mbuf->header.flags;			\
})

#define GET_MESSAGE_ID(رسالة)			\
({						\
	بنية rpmi_message *mbuf = رسالة;	\
	((u32)mbuf->header.flags << (RPMI_رسالة_FLAGS_OFFSET * 8)) | \
	((u32)mbuf->header.service_id << (RPMI_رسالة_SERVICE_ID_OFFSET * 8)) | \
	((u32)le16_to_cpu(mbuf->header.servicegroup_id)); \
})

#define MAKE_MESSAGE_ID(__group_id, __service_id, __flags)	\
({						\
	u32 __ret = 0;				\
	__ret |= (u32)(__group_id) << (RPMI_رسالة_SERVICEGROUP_ID_OFFSET * 8); \
	__ret |= (u32)(__service_id) << (RPMI_رسالة_SERVICE_ID_OFFSET * 8); \
	__ret |= (u32)(__flags) << (RPMI_رسالة_FLAGS_OFFSET * 8); \
	__ret;					\
})

#define GET_DLEN(رسالة)				\
({						\
	بنية rpmi_message *mbuf = رسالة;	\
	le16_to_cpu(mbuf->header.datalen);	\
})

#define GET_TOKEN(رسالة)				\
({						\
	بنية rpmi_message *mbuf = رسالة;	\
	le16_to_cpu(mbuf->header.token);	\
})

#define GET_MESSAGE_TYPE(رسالة)						\
({									\
	uint8_t flags = *((uint8_t *)رسالة + RPMI_رسالة_FLAGS_OFFSET);	\
	((flags & RPMI_رسالة_FLAGS_TYPE) >> RPMI_رسالة_FLAGS_TYPE_POS));	\
})

تعداد rpmi_queue_type {
	RPMI_QUEUE_TYPE_REQ = 0,
	RPMI_QUEUE_TYPE_ACK = 1,
};

تعداد rpmi_queue_idx {
	RPMI_QUEUE_IDX_A2P_REQ = 0,
	RPMI_QUEUE_IDX_P2A_ACK = 1,
	RPMI_QUEUE_IDX_P2A_REQ = 2,
	RPMI_QUEUE_IDX_A2P_ACK = 3,
	RPMI_QUEUE_IDX_MAX_تعداد,
};

تعداد rpmi_reg_idx {
	RPMI_REG_IDX_DB_REG = 0, /* Doorbell register */
	RPMI_REG_IDX_MAX_تعداد,
};

/** Mailbox registers */
بنية rpmi_mb_regs {
	/* doorbell from AP -> PuC*/
	متطاير le32_t db_reg;
} __packed;

/** Single Queue Context Structure */
بنية smq_queue_ctx {
	u32 queue_id;
	u32 رقم_slots;
	spinlock_t queue_lock;
	/* Type of queue - REQ or ACK */
	تعداد rpmi_queue_type queue_type;
	/* Pointers to the queue shared memory */
	متطاير le32_t *headptr PT_GUARDED_BY(&queue_lock);
	متطاير le32_t *tailptr PT_GUARDED_BY(&queue_lock);
	متطاير uint8_t *buffer PT_GUARDED_BY(&queue_lock);
	/* Name of the queue */
	حرف name[RPMI_NAME_CHARS_MAX];
};

بنية rpmi_srvgrp_chan {
	u32 servicegroup_id;
	u32 servicegroup_version;
	بنية mbox_chan chan;
};

#define to_srvgrp_chan(mbox_chan)	\
		container_of(mbox_chan, بنية rpmi_srvgrp_chan, chan);

بنية rpmi_shmem_mbox_controller {
	/* Driver specific members */
	u32 slot_size;
	u32 queue_تعداد;
	u32 p2a_doorbell_sysmsi_index;
	u32 a2p_doorbell_قيمة;
	بنية rpmi_mb_regs *mb_regs;
	بنية smq_queue_ctx queue_ctx_tbl[RPMI_QUEUE_IDX_MAX_تعداد];
	/* Mailbox framework related members */
	بنية mbox_controller controller;
	بنية mbox_chan *base_chan;
	u32 impl_version;
	u32 impl_id;
	u32 spec_version;
	u32 plat_info_len;
	حرف *plat_info;
	بنية {
		u8 f0_priv_level;
		bool f0_ev_notif_en;
	} base_flags;
};

/**************** Shared Memory Queues Helpers **************/

ساكن bool __smq_queue_full(بنية smq_queue_ctx *qctx)
	MUST_HOLD(&qctx->queue_lock)
{
	إرجاع ((le32_to_cpu(*qctx->tailptr) + 1) % qctx->رقم_slots ==
			le32_to_cpu(*qctx->headptr)) ? true : false;
}

ساكن bool __smq_queue_empty(بنية smq_queue_ctx *qctx)
	MUST_HOLD(&qctx->queue_lock)
{
	إرجاع (le32_to_cpu(*qctx->headptr) ==
		le32_to_cpu(*qctx->tailptr)) ? true : false;
}

ساكن صحيح __smq_rx(بنية smq_queue_ctx *qctx, u32 slot_size,
		    u32 service_group_id, بنية mbox_xfer *xfer)
	MUST_HOLD(&qctx->queue_lock)
{
	فراغ *dst, *src;
	بنية rpmi_message *رسالة;
	u32 i, tmp, pos, dlen, msgidn, headidx, tailidx;
	بنية rpmi_message_args *args = xfer->args;
	bool no_rx_token = (args->flags & RPMI_رسالة_FLAGS_NO_RX_TOKEN) ?
			   true : false;

	/* Rx sanity checks */
	إذا ((حجم(u32) * args->rx_endian_كلمات) >
	    (slot_size - حجم(بنية rpmi_message_header)))
		إرجاع SBI_EINVAL;
	إذا ((حجم(u32) * args->rx_endian_كلمات) > xfer->rx_len)
		إرجاع SBI_EINVAL;

	/* There should be some message in the queue */
	إذا (__smq_queue_empty(qctx))
		إرجاع SBI_ENOENT;

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
	بينما (pos != tailidx) {
		src = (فراغ *)qctx->buffer + (pos * slot_size);
		إذا ((no_rx_token && GET_MESSAGE_ID(src) == msgidn) ||
		    (GET_TOKEN(src) == (xfer->seq & RPMI_رسالة_TOKEN_MASK)))
			اكسر;
		pos = (pos + 1) % qctx->رقم_slots;
	}
	إذا (pos == tailidx)
		إرجاع SBI_ENOENT;

	/* If Rx message is not first message then make it first message */
	إذا (pos != headidx) {
		src = (فراغ *)qctx->buffer + (pos * slot_size);
		dst = (فراغ *)qctx->buffer + (headidx * slot_size);
		لكل (i = 0; i < slot_size / حجم(u32); i++) {
			tmp = ((u32 *)dst)[i];
			((u32 *)dst)[i] = ((u32 *)src)[i];
			((u32 *)src)[i] = tmp;
		}
	}

	/* Update rx_token if not available */
	رسالة = (فراغ *)qctx->buffer + (headidx * slot_size);
	إذا (no_rx_token)
		args->rx_token = GET_TOKEN(رسالة);

	/* Extract data from the first message */
	إذا (xfer->rx) {
		args->rx_data_len = dlen = GET_DLEN(رسالة);
		إذا (dlen > xfer->rx_len)
			dlen = xfer->rx_len;
		src = (فراغ *)رسالة + حجم(بنية rpmi_message_header);
		dst = xfer->rx;
		لكل (i = 0; i < args->rx_endian_كلمات; i++)
			((u32 *)dst)[i] = le32_to_cpu(((u32 *)src)[i]);
		dst += حجم(u32) * args->rx_endian_كلمات;
		src += حجم(u32) * args->rx_endian_كلمات;
		sbi_memcpy(dst, src,
			xfer->rx_len - (حجم(u32) * args->rx_endian_كلمات));
	}

	/* Update the head/read index */
	*qctx->headptr = cpu_to_le32(headidx + 1) % qctx->رقم_slots;

	/* Make sure updates to head are immediately visible to PuC */
	smp_wmb();

	إرجاع SBI_OK;
}

ساكن صحيح __smq_tx(بنية smq_queue_ctx *qctx, بنية rpmi_mb_regs *mb_regs,
		    u32 a2p_doorbell_قيمة, u32 slot_size, u32 service_group_id,
		    بنية mbox_xfer *xfer) MUST_HOLD(&qctx->queue_lock)
{
	u32 i, tailidx;
	فراغ *dst, *src;
	بنية rpmi_message_header header = { 0 };
	بنية rpmi_message_args *args = xfer->args;

	/* Tx sanity checks */
	إذا ((حجم(u32) * args->tx_endian_كلمات) >
	    (slot_size - حجم(بنية rpmi_message_header)))
		إرجاع SBI_EINVAL;
	إذا ((حجم(u32) * args->tx_endian_كلمات) > xfer->tx_len)
		إرجاع SBI_EINVAL;

	/* There should be some room in the queue */
	إذا (__smq_queue_full(qctx))
		إرجاع SBI_ENOMEM;

	/* Get the tail/write index */
	tailidx = le32_to_cpu(*qctx->tailptr);

	/* Prepare the header to be written into the slot */
	header.servicegroup_id = cpu_to_le16(service_group_id);
	header.service_id = args->service_id;
	header.flags = args->type;
	header.datalen = cpu_to_le16((u16)xfer->tx_len);
	header.token = cpu_to_le16((u16)xfer->seq);

	/* Write header into the slot */
	dst = (حرف *)qctx->buffer + (tailidx * slot_size);
	sbi_memcpy(dst, &header, حجم(header));
	dst += حجم(header);

	/* Write data into the slot */
	إذا (xfer->tx) {
		src = xfer->tx;
		لكل (i = 0; i < args->tx_endian_كلمات; i++)
			((u32 *)dst)[i] = cpu_to_le32(((u32 *)src)[i]);
		dst += حجم(u32) * args->tx_endian_كلمات;
		src += حجم(u32) * args->tx_endian_كلمات;
		sbi_memcpy(dst, src,
			xfer->tx_len - (حجم(u32) * args->tx_endian_كلمات));
	}

	/* Make sure queue chanages are visible to PuC before updating tail */
	smp_wmb();

	/* Update the tail/write index */
	*qctx->tailptr = cpu_to_le32(tailidx + 1) % qctx->رقم_slots;

	/* Ring the RPMI doorbell if present */
	إذا (mb_regs)
		writel(a2p_doorbell_قيمة, &mb_regs->db_reg);

	إرجاع SBI_OK;
}

ساكن صحيح smq_rx(بنية rpmi_shmem_mbox_controller *mctl,
		  u32 queue_id, u32 service_group_id, بنية mbox_xfer *xfer)
{
	صحيح ret, rxretry = 0;
	بنية smq_queue_ctx *qctx;

	إذا (mctl->queue_تعداد < queue_id) {
		sbi_printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		إرجاع SBI_EINVAL;
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
	افعل {
		spin_lock(&qctx->queue_lock);
		ret = __smq_rx(qctx, mctl->slot_size, service_group_id, xfer);
		spin_unlock(&qctx->queue_lock);
		إذا (!ret)
			إرجاع 0;

		sbi_timer_mdelay(1);
		rxretry += 1;
	} بينما (rxretry < xfer->rx_timeout);

	إرجاع SBI_ETIMEDOUT;
}

ساكن صحيح smq_tx(بنية rpmi_shmem_mbox_controller *mctl,
		  u32 queue_id, u32 service_group_id, بنية mbox_xfer *xfer)
{
	صحيح ret, txretry = 0;
	بنية smq_queue_ctx *qctx;

	إذا (mctl->queue_تعداد < queue_id) {
		sbi_printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		إرجاع SBI_EINVAL;
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
	افعل {
		spin_lock(&qctx->queue_lock);
		ret = __smq_tx(qctx, mctl->mb_regs, mctl->a2p_doorbell_قيمة,
				mctl->slot_size, service_group_id, xfer);
		spin_unlock(&qctx->queue_lock);
		إذا (!ret)
			إرجاع 0;

		sbi_timer_mdelay(1);
		txretry += 1;
	} بينما (txretry < xfer->tx_timeout);

	إرجاع SBI_ETIMEDOUT;
}

ساكن صحيح rpmi_get_platform_info(بنية rpmi_shmem_mbox_controller *mctl)
{
	صحيح ret = SBI_OK;

	/**
	 * platform string can occupy max possible size
	 * max possible space in the message data as
	 * per the format
	 */
	بنية rpmi_base_get_platform_info_resp *resp =
			sbi_zalloc(RPMI_رسالة_DATA_SIZE(mctl->slot_size));
	إذا (!resp)
		إرجاع SBI_ENOMEM;

	ret = rpmi_normal_request_with_status(mctl->base_chan,
				 RPMI_BASE_SRV_GET_PLATFORM_INFO,
				 عدم, 0, 0,
				 resp,
				 RPMI_رسالة_DATA_SIZE(mctl->slot_size)/4,
				 RPMI_رسالة_DATA_SIZE(mctl->slot_size)/4);
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
	إرجاع ret;
}

ساكن صحيح smq_base_get_two_u32(بنية rpmi_shmem_mbox_controller *mctl,
				u32 service_id, u32 *inarg, u32 *outvals)
{
	إرجاع rpmi_normal_request_with_status(
			mctl->base_chan, service_id,
			inarg, (inarg) ? 1 : 0, (inarg) ? 1 : 0,
			outvals, 2, 2);
}

/**************** Mailbox Controller Functions **************/

ساكن صحيح rpmi_shmem_mbox_xfer(بنية mbox_chan *chan, بنية mbox_xfer *xfer)
{
	صحيح ret;
	u32 tx_qid = 0, rx_qid = 0;
	بنية rpmi_shmem_mbox_controller *mctl =
			container_of(chan->mbox,
				     بنية rpmi_shmem_mbox_controller,
				     controller);
	بنية rpmi_srvgrp_chan *srvgrp_chan = to_srvgrp_chan(chan);

	بنية rpmi_message_args *args = xfer->args;
	bool do_tx = (args->flags & RPMI_رسالة_FLAGS_NO_TX) ? false : true;
	bool do_rx = (args->flags & RPMI_رسالة_FLAGS_NO_RX) ? false : true;

	إذا (!do_tx && !do_rx)
		إرجاع SBI_EINVAL;

	بدل (args->type) {
	حالة RPMI_رسالة_NORMAL_REQUEST:
		إذا (do_tx && do_rx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
			rx_qid = RPMI_QUEUE_IDX_P2A_ACK;
		} وإلا إذا (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
		} وإلا إذا (do_rx) {
			rx_qid = RPMI_QUEUE_IDX_P2A_REQ;
		}
		اكسر;
	حالة RPMI_رسالة_POSTED_REQUEST:
		إذا (do_tx && do_rx)
			إرجاع SBI_EINVAL;
		إذا (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
		} وإلا {
			rx_qid = RPMI_QUEUE_IDX_P2A_REQ;
		}
		اكسر;
	حالة RPMI_رسالة_ACKNOWLDGEMENT:
		إذا (do_tx && do_rx)
			إرجاع SBI_EINVAL;
		إذا (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_ACK;
		} وإلا {
			rx_qid = RPMI_QUEUE_IDX_P2A_ACK;
		}
		اكسر;
	افتراضي:
		إرجاع SBI_ENOTSUPP;
	}

	إذا (do_tx) {
		ret = smq_tx(mctl, tx_qid, srvgrp_chan->servicegroup_id, xfer);
		إذا (ret)
			إرجاع ret;
	}

	إذا (do_rx) {
		ret = smq_rx(mctl, rx_qid, srvgrp_chan->servicegroup_id, xfer);
		إذا (ret)
			إرجاع ret;
	}

	إرجاع 0;
}

ساكن صحيح rpmi_shmem_mbox_get_attribute(بنية mbox_chan *chan,
					 صحيح attr_id, فراغ *out_قيمة)
{
	بنية rpmi_shmem_mbox_controller *mctl =
			container_of(chan->mbox,
				     بنية rpmi_shmem_mbox_controller,
				     controller);
	بنية rpmi_srvgrp_chan *srvgrp_chan = to_srvgrp_chan(chan);

	بدل (attr_id) {
	حالة RPMI_CHANNEL_ATTR_PROTOCOL_VERSION:
		*((u32 *)out_قيمة) = mctl->spec_version;
		اكسر;
	حالة RPMI_CHANNEL_ATTR_MAX_DATA_LEN:
		*((u32 *)out_قيمة) = RPMI_رسالة_DATA_SIZE(mctl->slot_size);
		اكسر;
	حالة RPMI_CHANNEL_ATTR_P2A_DOORBELL_SYSMSI_INDEX:
		*((u32 *)out_قيمة) = mctl->p2a_doorbell_sysmsi_index;
		اكسر;
	حالة RPMI_CHANNEL_ATTR_TX_TIMEOUT:
		*((u32 *)out_قيمة) = RPMI_DEF_TX_TIMEOUT;
		اكسر;
	حالة RPMI_CHANNEL_ATTR_RX_TIMEOUT:
		*((u32 *)out_قيمة) = RPMI_DEF_RX_TIMEOUT;
		اكسر;
	حالة RPMI_CHANNEL_ATTR_SERVICEGROUP_ID:
		*((u32 *)out_قيمة) = srvgrp_chan->servicegroup_id;
		اكسر;
	حالة RPMI_CHANNEL_ATTR_SERVICEGROUP_VERSION:
		*((u32 *)out_قيمة) = srvgrp_chan->servicegroup_version;
		اكسر;
	حالة RPMI_CHANNEL_ATTR_IMPL_ID:
		*((u32 *)out_قيمة) = mctl->impl_id;
		اكسر;
	حالة RPMI_CHANNEL_ATTR_IMPL_VERSION:
		*((u32 *)out_قيمة) = mctl->impl_version;
		اكسر;
	افتراضي:
		إرجاع SBI_ENOTSUPP;
	}

	إرجاع 0;
}

ساكن بنية mbox_chan *rpmi_shmem_mbox_request_chan(
						بنية mbox_controller *mbox,
						u32 *chan_args)
{
	صحيح ret;
	u32 tval[2] = { 0 };
	بنية rpmi_srvgrp_chan *srvgrp_chan;
	بنية rpmi_shmem_mbox_controller *mctl =
			container_of(mbox,
				     بنية rpmi_shmem_mbox_controller,
				     controller);

	/* Service group id not defined or in reserved range is invalid */
	إذا (chan_args[0] >= RPMI_SRVGRP_ID_MAX_تعداد &&
		chan_args[0] <= RPMI_SRVGRP_RESERVE_END)
		إرجاع عدم;

	/* Base serivce group is always present so probe other groups */
	إذا (chan_args[0] != RPMI_SRVGRP_BASE) {
		/* Probe service group */
		ret = smq_base_get_two_u32(mctl,
					   RPMI_BASE_SRV_PROBE_SERVICE_GROUP,
					   chan_args, tval);
		إذا (ret || !tval[1])
			إرجاع عدم;
	}

	srvgrp_chan = sbi_zalloc(حجم(*srvgrp_chan));
	إذا (!srvgrp_chan)
		إرجاع عدم;

	srvgrp_chan->servicegroup_id = chan_args[0];
	srvgrp_chan->servicegroup_version = tval[1];

	إرجاع &srvgrp_chan->chan;
}

ساكن فراغ rpmi_shmem_mbox_free_chan(بنية mbox_controller *mbox,
				      بنية mbox_chan *chan)
{
	بنية rpmi_srvgrp_chan *srvgrp_chan = to_srvgrp_chan(chan);
	sbi_free(srvgrp_chan);
}

خارجي بنية fdt_mailbox fdt_mailbox_rpmi_shmem;

ساكن صحيح rpmi_shmem_transport_init(بنية rpmi_shmem_mbox_controller *mctl,
				     ثابت فراغ *fdt,
				     صحيح nodeoff) NO_THREAD_SAFETY_ANALYSIS
{
	ثابت حرف *name;
	ثابت fdt32_t *prop;
	صحيح تعداد_2, len, ret, qid;
	uint64_t reg_addr, reg_size;
	بنية smq_queue_ctx *qctx;

	ret = fdt_عقدة_check_compatible(fdt, nodeoff,
					"riscv,rpmi-shmem-mbox");
	إذا (ret)
		إرجاع ret;

	/* get queue slot size in bytes */
	prop = fdt_getprop(fdt, nodeoff, "riscv,slot-size", &len);
	إذا (!prop)
		إرجاع SBI_ENOENT;

	mctl->slot_size = fdt32_to_cpu(*prop);
	إذا (mctl->slot_size < RPMI_SLOT_SIZE_MIN) {
		sbi_printf("%s: slot_size < mimnum required message size\n",
			   __func__);
		mctl->slot_size = RPMI_SLOT_SIZE_MIN;
	}

	/* get p2a doorbell system MSI index */
	prop = fdt_getprop(fdt, nodeoff, "riscv,p2a-doorbell-sysmsi-index", &len);
	mctl->p2a_doorbell_sysmsi_index = prop ? fdt32_to_cpu(*prop) : -1U;

	/* get a2p doorbell value */
	prop = fdt_getprop(fdt, nodeoff, "riscv,a2p-doorbell-value", &len);
	mctl->a2p_doorbell_قيمة = prop ? fdt32_to_cpu(*prop) : 1;

	/*
	 * queue names count is taken as the number of queues
	 * supported which make it mandatory to provide the
	 * name of the queue.
	 */
	تعداد_2 = fdt_stringlist_تعداد(fdt, nodeoff, "reg-names");
	إذا (تعداد_2 < 0 ||
	    تعداد_2 > (RPMI_QUEUE_IDX_MAX_تعداد + RPMI_REG_IDX_MAX_تعداد))
		إرجاع SBI_EINVAL;

	mctl->queue_تعداد = تعداد_2 - RPMI_REG_IDX_MAX_تعداد;

	/* parse all queues and populate queues context structure */
	لكل (qid = 0; qid < mctl->queue_تعداد; qid++) {
		qctx = &mctl->queue_ctx_tbl[qid];

		/* get each queue share-memory base address and size*/
		ret = fdt_get_عقدة_addr_size(fdt, nodeoff, qid,
					     &reg_addr, &reg_size);
		إذا (ret < 0 || !reg_addr || !reg_size)
			إرجاع SBI_ENOENT;

		ret = sbi_domain_جذر_add_memrange(reg_addr, reg_size, reg_size,
						   (SBI_DOMAIN_MEMREGION_MMIO |
						    SBI_DOMAIN_MEMREGION_M_READABLE |
						    SBI_DOMAIN_MEMREGION_M_WRITABLE));
		إذا (ret)
			إرجاع ret;

		/* calculate number of slots in each queue */
		qctx->رقم_slots =
			(reg_size - (mctl->slot_size * RPMI_QUEUE_HEADER_SLOTS)) / mctl->slot_size;

		/* setup queue pointers */
		qctx->headptr = ((فراغ *)(unsigned long)reg_addr) +
				RPMI_QUEUE_HEAD_SLOT * mctl->slot_size;
		qctx->tailptr = ((فراغ *)(unsigned long)reg_addr) +
				RPMI_QUEUE_TAIL_SLOT * mctl->slot_size;
		qctx->buffer = ((فراغ *)(unsigned long)reg_addr) +
				RPMI_QUEUE_HEADER_SLOTS * mctl->slot_size;

		/* get the queue name */
		name = fdt_stringlist_get(fdt, nodeoff, "reg-names",
					  qid, &len);
		إذا (!name || (name && len < 0))
			إرجاع len;

		sbi_memcpy(qctx->name, name, len);

		/* store the index as queue_id */
		qctx->queue_id = qid;

		SPIN_LOCK_INIT(qctx->queue_lock);
	}

	/* get the a2p-doorbell property name */
	name = fdt_stringlist_get(fdt, nodeoff, "reg-names", qid, &len);
	إذا (!name || (name && len < 0))
		إرجاع len;

	/* fetch doorbell register address*/
	ret = fdt_get_عقدة_addr_size(fdt, nodeoff, qid, &reg_addr,
				       &reg_size);
	إذا (!ret && !(strncmp(name, "a2p-doorbell", strlen("a2p-doorbell")))) {
		mctl->mb_regs = (فراغ *)(unsigned long)reg_addr;
		ret = sbi_domain_جذر_add_memrange(reg_addr, reg_size, reg_size,
						   (SBI_DOMAIN_MEMREGION_MMIO |
						    SBI_DOMAIN_MEMREGION_M_READABLE |
						    SBI_DOMAIN_MEMREGION_M_WRITABLE));
		إذا (ret)
			إرجاع ret;
	}

	إرجاع SBI_SUCCESS;
}

ساكن صحيح rpmi_shmem_mbox_init(ثابت فراغ *fdt, صحيح nodeoff,
				ثابت بنية fdt_match *match)
{
	بنية rpmi_base_get_attributes_resp resp;
	بنية rpmi_shmem_mbox_controller *mctl;
	بنية rpmi_srvgrp_chan *base_srvgrp;
	u32 tval[2], args[1];
	صحيح ret = 0;

	mctl = sbi_zalloc(حجم(*mctl));
	إذا (!mctl)
		إرجاع SBI_ENOMEM;

	/* Initialization transport from device tree */
	ret = rpmi_shmem_transport_init(mctl, fdt, nodeoff);
	إذا (ret)
		اذهب fail_free_controller;

	/* Register mailbox controller */
	mctl->controller.id = nodeoff;
	mctl->controller.max_xfer_len =
			mctl->slot_size - حجم(بنية rpmi_message_header);
	mctl->controller.driver = &fdt_mailbox_rpmi_shmem;
	mctl->controller.request_chan = rpmi_shmem_mbox_request_chan;
	mctl->controller.free_chan = rpmi_shmem_mbox_free_chan;
	mctl->controller.xfer = rpmi_shmem_mbox_xfer;
	mctl->controller.get_attribute = rpmi_shmem_mbox_get_attribute;
	ret = mbox_controller_add(&mctl->controller);
	إذا (ret)
		اذهب fail_free_controller;

	/* Request base service group channel */
	tval[0] = RPMI_SRVGRP_BASE;
	mctl->base_chan = mbox_controller_request_chan(&mctl->controller,
							tval);
	إذا (!mctl->base_chan) {
		ret = SBI_ENOENT;
		اذهب fail_remove_controller;
	}

	/* Update base service group version */
	base_srvgrp = to_srvgrp_chan(mctl->base_chan);
	args[0] = RPMI_SRVGRP_BASE;
	ret = smq_base_get_two_u32(mctl, RPMI_BASE_SRV_PROBE_SERVICE_GROUP,
				   &args[0], tval);
	إذا (ret)
		اذهب fail_free_chan;
	base_srvgrp->servicegroup_version = tval[1];
	إذا (base_srvgrp->servicegroup_version < RPMI_BASE_VERSION_MIN) {
		ret = SBI_EINVAL;
		اذهب fail_free_chan;
	}

	/* Get implementation id */
	ret = smq_base_get_two_u32(mctl,
				   RPMI_BASE_SRV_GET_IMPLEMENTATION_VERSION,
				   عدم, tval);
	إذا (ret)
		اذهب fail_free_chan;
	mctl->impl_version = tval[1];

	/* Get implementation version */
	ret = smq_base_get_two_u32(mctl, RPMI_BASE_SRV_GET_IMPLEMENTATION_IDN,
				   عدم, tval);
	إذا (ret)
		اذهب fail_free_chan;
	mctl->impl_id = tval[1];

	/* Get specification version */
	ret = smq_base_get_two_u32(mctl, RPMI_BASE_SRV_GET_SPEC_VERSION,
				   عدم, tval);
	إذا (ret)
		اذهب fail_free_chan;
	mctl->spec_version = tval[1];
	إذا (mctl->spec_version < RPMI_BASE_VERSION_MIN ||
	    mctl->spec_version != base_srvgrp->servicegroup_version) {
		ret = SBI_EINVAL;
		اذهب fail_free_chan;
	}

	/* Get optional features implementation flags */
	ret = rpmi_normal_request_with_status(
			mctl->base_chan, RPMI_BASE_SRV_GET_ATTRIBUTES,
			عدم, 0, 0,
			&resp, rpmi_u32_تعداد(resp), rpmi_u32_تعداد(resp));
	إذا (ret)
		اذهب fail_free_chan;

	/* 1: M-mode, 0: S-mode */
	mctl->base_flags.f0_priv_level =
			resp.f0 & RPMI_BASE_FLAGS_F0_PRIVILEGE ? 1 : 0;
	/* 1: Supported, 0: Not Supported */
	mctl->base_flags.f0_ev_notif_en =
			resp.f0 & RPMI_BASE_FLAGS_F0_EV_NOTIFY ? 1 : 0;

	/* We only use M-mode RPMI context in OpenSBI */
	إذا (!mctl->base_flags.f0_priv_level) {
		ret = SBI_ENODEV;
		اذهب fail_free_chan;
	}

	/*
	 * Continue without platform information string if not
	 * available or if an error is encountered while fetching
	 */
	rpmi_get_platform_info(mctl);

	إرجاع 0;

fail_free_chan:
	mbox_controller_free_chan(mctl->base_chan);
fail_remove_controller:
	mbox_controller_remove(&mctl->controller);
fail_free_controller:
	sbi_free(mctl);
	إرجاع ret;
}

ساكن ثابت بنية fdt_match rpmi_shmem_mbox_match[] = {
	{ .compatible = "riscv,rpmi-shmem-mbox" },
	{ },
};

بنية fdt_mailbox fdt_mailbox_rpmi_shmem = {
	.driver = {
		.match_table = rpmi_shmem_mbox_match,
		.init = rpmi_shmem_mbox_init,
	},
	.xlate = fdt_mailbox_simple_xlate,
};
