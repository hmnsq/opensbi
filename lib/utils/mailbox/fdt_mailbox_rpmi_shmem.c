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

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi/sbi_timer.h>
#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/riscv_locks.h>
#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/riscv_barrier.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_helper.h>
#അന്തര്ഭാവ <sbi_utils/mailbox/mailbox.h>
#അന്തര്ഭാവ <sbi_utils/mailbox/fdt_mailbox.h>
#അന്തര്ഭാവ <sbi_utils/mailbox/rpmi_mailbox.h>

/** Minimum Base group version required */
#define RPMI_BASE_VERSION_MIN		RPMI_VERSION(1, 0)

/**************** RPMI Transport Structures and Macros ***********/

#define GET_SERVICEGROUP_ID(സന്ദേശം)		\
({						\
	ഘടന rpmi_message *mbuf = സന്ദേശം;	\
	le16_to_cpu(mbuf->header.servicegroup_id);\
})

#define GET_SERVICE_ID(സന്ദേശം)			\
({						\
	ഘടന rpmi_message *mbuf = സന്ദേശം;	\
	mbuf->header.service_id;		\
})

#define GET_FLAGS(സന്ദേശം)				\
({						\
	ഘടന rpmi_message *mbuf = സന്ദേശം;	\
	mbuf->header.flags;			\
})

#define GET_MESSAGE_ID(സന്ദേശം)			\
({						\
	ഘടന rpmi_message *mbuf = സന്ദേശം;	\
	((u32)mbuf->header.flags << (RPMI_സന്ദേശം_FLAGS_OFFSET * 8)) | \
	((u32)mbuf->header.service_id << (RPMI_സന്ദേശം_SERVICE_ID_OFFSET * 8)) | \
	((u32)le16_to_cpu(mbuf->header.servicegroup_id)); \
})

#define MAKE_MESSAGE_ID(__group_id, __service_id, __flags)	\
({						\
	u32 __ret = 0;				\
	__ret |= (u32)(__group_id) << (RPMI_സന്ദേശം_SERVICEGROUP_ID_OFFSET * 8); \
	__ret |= (u32)(__service_id) << (RPMI_സന്ദേശം_SERVICE_ID_OFFSET * 8); \
	__ret |= (u32)(__flags) << (RPMI_സന്ദേശം_FLAGS_OFFSET * 8); \
	__ret;					\
})

#define GET_DLEN(സന്ദേശം)				\
({						\
	ഘടന rpmi_message *mbuf = സന്ദേശം;	\
	le16_to_cpu(mbuf->header.datalen);	\
})

#define GET_TOKEN(സന്ദേശം)				\
({						\
	ഘടന rpmi_message *mbuf = സന്ദേശം;	\
	le16_to_cpu(mbuf->header.token);	\
})

#define GET_MESSAGE_TYPE(സന്ദേശം)						\
({									\
	uint8_t flags = *((uint8_t *)സന്ദേശം + RPMI_സന്ദേശം_FLAGS_OFFSET);	\
	((flags & RPMI_സന്ദേശം_FLAGS_TYPE) >> RPMI_സന്ദേശം_FLAGS_TYPE_POS));	\
})

എണ്ണൽ rpmi_queue_type {
	RPMI_QUEUE_TYPE_REQ = 0,
	RPMI_QUEUE_TYPE_ACK = 1,
};

എണ്ണൽ rpmi_queue_idx {
	RPMI_QUEUE_IDX_A2P_REQ = 0,
	RPMI_QUEUE_IDX_P2A_ACK = 1,
	RPMI_QUEUE_IDX_P2A_REQ = 2,
	RPMI_QUEUE_IDX_A2P_ACK = 3,
	RPMI_QUEUE_IDX_MAX_എണ്ണം,
};

എണ്ണൽ rpmi_reg_idx {
	RPMI_REG_IDX_DB_REG = 0, /* Doorbell register */
	RPMI_REG_IDX_MAX_എണ്ണം,
};

/** Mailbox registers */
ഘടന rpmi_mb_regs {
	/* doorbell from AP -> PuC*/
	അസ്ഥിര le32_t db_reg;
} __packed;

/** Single Queue Context Structure */
ഘടന smq_queue_ctx {
	u32 queue_id;
	u32 അക്കം_slots;
	spinlock_t queue_lock;
	/* Type of queue - REQ or ACK */
	എണ്ണൽ rpmi_queue_type queue_type;
	/* Pointers to the queue shared memory */
	അസ്ഥിര le32_t *headptr PT_GUARDED_BY(&queue_lock);
	അസ്ഥിര le32_t *tailptr PT_GUARDED_BY(&queue_lock);
	അസ്ഥിര uint8_t *buffer PT_GUARDED_BY(&queue_lock);
	/* Name of the queue */
	അക്ഷരം name[RPMI_NAME_CHARS_MAX];
};

ഘടന rpmi_srvgrp_chan {
	u32 servicegroup_id;
	u32 servicegroup_version;
	ഘടന mbox_chan chan;
};

#define to_srvgrp_chan(mbox_chan)	\
		container_of(mbox_chan, ഘടന rpmi_srvgrp_chan, chan);

ഘടന rpmi_shmem_mbox_controller {
	/* Driver specific members */
	u32 slot_size;
	u32 queue_എണ്ണം;
	u32 p2a_doorbell_sysmsi_index;
	u32 a2p_doorbell_മൂല്യം;
	ഘടന rpmi_mb_regs *mb_regs;
	ഘടന smq_queue_ctx queue_ctx_tbl[RPMI_QUEUE_IDX_MAX_എണ്ണം];
	/* Mailbox framework related members */
	ഘടന mbox_controller controller;
	ഘടന mbox_chan *base_chan;
	u32 impl_version;
	u32 impl_id;
	u32 spec_version;
	u32 plat_info_len;
	അക്ഷരം *plat_info;
	ഘടന {
		u8 f0_priv_level;
		bool f0_ev_notif_en;
	} base_flags;
};

/**************** Shared Memory Queues Helpers **************/

സ്ഥിതം bool __smq_queue_full(ഘടന smq_queue_ctx *qctx)
	MUST_HOLD(&qctx->queue_lock)
{
	തിരികെ ((le32_to_cpu(*qctx->tailptr) + 1) % qctx->അക്കം_slots ==
			le32_to_cpu(*qctx->headptr)) ? true : false;
}

സ്ഥിതം bool __smq_queue_empty(ഘടന smq_queue_ctx *qctx)
	MUST_HOLD(&qctx->queue_lock)
{
	തിരികെ (le32_to_cpu(*qctx->headptr) ==
		le32_to_cpu(*qctx->tailptr)) ? true : false;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ __smq_rx(ഘടന smq_queue_ctx *qctx, u32 slot_size,
		    u32 service_group_id, ഘടന mbox_xfer *xfer)
	MUST_HOLD(&qctx->queue_lock)
{
	ശൂന്യം *dst, *src;
	ഘടന rpmi_message *സന്ദേശം;
	u32 i, tmp, pos, dlen, msgidn, headidx, tailidx;
	ഘടന rpmi_message_args *args = xfer->args;
	bool no_rx_token = (args->flags & RPMI_സന്ദേശം_FLAGS_NO_RX_TOKEN) ?
			   true : false;

	/* Rx sanity checks */
	എങ്കിൽ ((വലുപ്പം(u32) * args->rx_endian_വാക്കുകൾ) >
	    (slot_size - വലുപ്പം(ഘടന rpmi_message_header)))
		തിരികെ SBI_EINVAL;
	എങ്കിൽ ((വലുപ്പം(u32) * args->rx_endian_വാക്കുകൾ) > xfer->rx_len)
		തിരികെ SBI_EINVAL;

	/* There should be some message in the queue */
	എങ്കിൽ (__smq_queue_empty(qctx))
		തിരികെ SBI_ENOENT;

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
	ആകുമ്പോൾ (pos != tailidx) {
		src = (ശൂന്യം *)qctx->buffer + (pos * slot_size);
		എങ്കിൽ ((no_rx_token && GET_MESSAGE_ID(src) == msgidn) ||
		    (GET_TOKEN(src) == (xfer->seq & RPMI_സന്ദേശം_TOKEN_MASK)))
			നിർത്തുക;
		pos = (pos + 1) % qctx->അക്കം_slots;
	}
	എങ്കിൽ (pos == tailidx)
		തിരികെ SBI_ENOENT;

	/* If Rx message is not first message then make it first message */
	എങ്കിൽ (pos != headidx) {
		src = (ശൂന്യം *)qctx->buffer + (pos * slot_size);
		dst = (ശൂന്യം *)qctx->buffer + (headidx * slot_size);
		വേണ്ടി (i = 0; i < slot_size / വലുപ്പം(u32); i++) {
			tmp = ((u32 *)dst)[i];
			((u32 *)dst)[i] = ((u32 *)src)[i];
			((u32 *)src)[i] = tmp;
		}
	}

	/* Update rx_token if not available */
	സന്ദേശം = (ശൂന്യം *)qctx->buffer + (headidx * slot_size);
	എങ്കിൽ (no_rx_token)
		args->rx_token = GET_TOKEN(സന്ദേശം);

	/* Extract data from the first message */
	എങ്കിൽ (xfer->rx) {
		args->rx_data_len = dlen = GET_DLEN(സന്ദേശം);
		എങ്കിൽ (dlen > xfer->rx_len)
			dlen = xfer->rx_len;
		src = (ശൂന്യം *)സന്ദേശം + വലുപ്പം(ഘടന rpmi_message_header);
		dst = xfer->rx;
		വേണ്ടി (i = 0; i < args->rx_endian_വാക്കുകൾ; i++)
			((u32 *)dst)[i] = le32_to_cpu(((u32 *)src)[i]);
		dst += വലുപ്പം(u32) * args->rx_endian_വാക്കുകൾ;
		src += വലുപ്പം(u32) * args->rx_endian_വാക്കുകൾ;
		sbi_memcpy(dst, src,
			xfer->rx_len - (വലുപ്പം(u32) * args->rx_endian_വാക്കുകൾ));
	}

	/* Update the head/read index */
	*qctx->headptr = cpu_to_le32(headidx + 1) % qctx->അക്കം_slots;

	/* Make sure updates to head are immediately visible to PuC */
	smp_wmb();

	തിരികെ SBI_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ __smq_tx(ഘടന smq_queue_ctx *qctx, ഘടന rpmi_mb_regs *mb_regs,
		    u32 a2p_doorbell_മൂല്യം, u32 slot_size, u32 service_group_id,
		    ഘടന mbox_xfer *xfer) MUST_HOLD(&qctx->queue_lock)
{
	u32 i, tailidx;
	ശൂന്യം *dst, *src;
	ഘടന rpmi_message_header header = { 0 };
	ഘടന rpmi_message_args *args = xfer->args;

	/* Tx sanity checks */
	എങ്കിൽ ((വലുപ്പം(u32) * args->tx_endian_വാക്കുകൾ) >
	    (slot_size - വലുപ്പം(ഘടന rpmi_message_header)))
		തിരികെ SBI_EINVAL;
	എങ്കിൽ ((വലുപ്പം(u32) * args->tx_endian_വാക്കുകൾ) > xfer->tx_len)
		തിരികെ SBI_EINVAL;

	/* There should be some room in the queue */
	എങ്കിൽ (__smq_queue_full(qctx))
		തിരികെ SBI_ENOMEM;

	/* Get the tail/write index */
	tailidx = le32_to_cpu(*qctx->tailptr);

	/* Prepare the header to be written into the slot */
	header.servicegroup_id = cpu_to_le16(service_group_id);
	header.service_id = args->service_id;
	header.flags = args->type;
	header.datalen = cpu_to_le16((u16)xfer->tx_len);
	header.token = cpu_to_le16((u16)xfer->seq);

	/* Write header into the slot */
	dst = (അക്ഷരം *)qctx->buffer + (tailidx * slot_size);
	sbi_memcpy(dst, &header, വലുപ്പം(header));
	dst += വലുപ്പം(header);

	/* Write data into the slot */
	എങ്കിൽ (xfer->tx) {
		src = xfer->tx;
		വേണ്ടി (i = 0; i < args->tx_endian_വാക്കുകൾ; i++)
			((u32 *)dst)[i] = cpu_to_le32(((u32 *)src)[i]);
		dst += വലുപ്പം(u32) * args->tx_endian_വാക്കുകൾ;
		src += വലുപ്പം(u32) * args->tx_endian_വാക്കുകൾ;
		sbi_memcpy(dst, src,
			xfer->tx_len - (വലുപ്പം(u32) * args->tx_endian_വാക്കുകൾ));
	}

	/* Make sure queue chanages are visible to PuC before updating tail */
	smp_wmb();

	/* Update the tail/write index */
	*qctx->tailptr = cpu_to_le32(tailidx + 1) % qctx->അക്കം_slots;

	/* Ring the RPMI doorbell if present */
	എങ്കിൽ (mb_regs)
		writel(a2p_doorbell_മൂല്യം, &mb_regs->db_reg);

	തിരികെ SBI_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ smq_rx(ഘടന rpmi_shmem_mbox_controller *mctl,
		  u32 queue_id, u32 service_group_id, ഘടന mbox_xfer *xfer)
{
	പൂർണ്ണസംഖ്യ ret, rxretry = 0;
	ഘടന smq_queue_ctx *qctx;

	എങ്കിൽ (mctl->queue_എണ്ണം < queue_id) {
		sbi_printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		തിരികെ SBI_EINVAL;
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
	ചെയ്യുക {
		spin_lock(&qctx->queue_lock);
		ret = __smq_rx(qctx, mctl->slot_size, service_group_id, xfer);
		spin_unlock(&qctx->queue_lock);
		എങ്കിൽ (!ret)
			തിരികെ 0;

		sbi_timer_mdelay(1);
		rxretry += 1;
	} ആകുമ്പോൾ (rxretry < xfer->rx_timeout);

	തിരികെ SBI_ETIMEDOUT;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ smq_tx(ഘടന rpmi_shmem_mbox_controller *mctl,
		  u32 queue_id, u32 service_group_id, ഘടന mbox_xfer *xfer)
{
	പൂർണ്ണസംഖ്യ ret, txretry = 0;
	ഘടന smq_queue_ctx *qctx;

	എങ്കിൽ (mctl->queue_എണ്ണം < queue_id) {
		sbi_printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		തിരികെ SBI_EINVAL;
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
	ചെയ്യുക {
		spin_lock(&qctx->queue_lock);
		ret = __smq_tx(qctx, mctl->mb_regs, mctl->a2p_doorbell_മൂല്യം,
				mctl->slot_size, service_group_id, xfer);
		spin_unlock(&qctx->queue_lock);
		എങ്കിൽ (!ret)
			തിരികെ 0;

		sbi_timer_mdelay(1);
		txretry += 1;
	} ആകുമ്പോൾ (txretry < xfer->tx_timeout);

	തിരികെ SBI_ETIMEDOUT;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ rpmi_get_platform_info(ഘടന rpmi_shmem_mbox_controller *mctl)
{
	പൂർണ്ണസംഖ്യ ret = SBI_OK;

	/**
	 * platform string can occupy max possible size
	 * max possible space in the message data as
	 * per the format
	 */
	ഘടന rpmi_base_get_platform_info_resp *resp =
			sbi_zalloc(RPMI_സന്ദേശം_DATA_SIZE(mctl->slot_size));
	എങ്കിൽ (!resp)
		തിരികെ SBI_ENOMEM;

	ret = rpmi_normal_request_with_status(mctl->base_chan,
				 RPMI_BASE_SRV_GET_PLATFORM_INFO,
				 ശൂന്യ, 0, 0,
				 resp,
				 RPMI_സന്ദേശം_DATA_SIZE(mctl->slot_size)/4,
				 RPMI_സന്ദേശം_DATA_SIZE(mctl->slot_size)/4);
	എങ്കിൽ (ret)
		പോകുക fail_free_resp;

	mctl->plat_info_len = resp->plat_info_len;
	mctl->plat_info = sbi_zalloc(mctl->plat_info_len);
	എങ്കിൽ (!mctl->plat_info) {
		ret = SBI_ENOMEM;
		പോകുക fail_free_resp;
	}

	sbi_strncpy(mctl->plat_info, resp->plat_info, mctl->plat_info_len);

fail_free_resp:
	sbi_free(resp);
	തിരികെ ret;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ smq_base_get_two_u32(ഘടന rpmi_shmem_mbox_controller *mctl,
				u32 service_id, u32 *inarg, u32 *outvals)
{
	തിരികെ rpmi_normal_request_with_status(
			mctl->base_chan, service_id,
			inarg, (inarg) ? 1 : 0, (inarg) ? 1 : 0,
			outvals, 2, 2);
}

/**************** Mailbox Controller Functions **************/

സ്ഥിതം പൂർണ്ണസംഖ്യ rpmi_shmem_mbox_xfer(ഘടന mbox_chan *chan, ഘടന mbox_xfer *xfer)
{
	പൂർണ്ണസംഖ്യ ret;
	u32 tx_qid = 0, rx_qid = 0;
	ഘടന rpmi_shmem_mbox_controller *mctl =
			container_of(chan->mbox,
				     ഘടന rpmi_shmem_mbox_controller,
				     controller);
	ഘടന rpmi_srvgrp_chan *srvgrp_chan = to_srvgrp_chan(chan);

	ഘടന rpmi_message_args *args = xfer->args;
	bool do_tx = (args->flags & RPMI_സന്ദേശം_FLAGS_NO_TX) ? false : true;
	bool do_rx = (args->flags & RPMI_സന്ദേശം_FLAGS_NO_RX) ? false : true;

	എങ്കിൽ (!do_tx && !do_rx)
		തിരികെ SBI_EINVAL;

	തിരഞ്ഞെടുപ്പ് (args->type) {
	അവസ്ഥ RPMI_സന്ദേശം_NORMAL_REQUEST:
		എങ്കിൽ (do_tx && do_rx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
			rx_qid = RPMI_QUEUE_IDX_P2A_ACK;
		} അല്ലെങ്കിൽ എങ്കിൽ (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
		} അല്ലെങ്കിൽ എങ്കിൽ (do_rx) {
			rx_qid = RPMI_QUEUE_IDX_P2A_REQ;
		}
		നിർത്തുക;
	അവസ്ഥ RPMI_സന്ദേശം_POSTED_REQUEST:
		എങ്കിൽ (do_tx && do_rx)
			തിരികെ SBI_EINVAL;
		എങ്കിൽ (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
		} അല്ലെങ്കിൽ {
			rx_qid = RPMI_QUEUE_IDX_P2A_REQ;
		}
		നിർത്തുക;
	അവസ്ഥ RPMI_സന്ദേശം_ACKNOWLDGEMENT:
		എങ്കിൽ (do_tx && do_rx)
			തിരികെ SBI_EINVAL;
		എങ്കിൽ (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_ACK;
		} അല്ലെങ്കിൽ {
			rx_qid = RPMI_QUEUE_IDX_P2A_ACK;
		}
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
		തിരികെ SBI_ENOTSUPP;
	}

	എങ്കിൽ (do_tx) {
		ret = smq_tx(mctl, tx_qid, srvgrp_chan->servicegroup_id, xfer);
		എങ്കിൽ (ret)
			തിരികെ ret;
	}

	എങ്കിൽ (do_rx) {
		ret = smq_rx(mctl, rx_qid, srvgrp_chan->servicegroup_id, xfer);
		എങ്കിൽ (ret)
			തിരികെ ret;
	}

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ rpmi_shmem_mbox_get_attribute(ഘടന mbox_chan *chan,
					 പൂർണ്ണസംഖ്യ attr_id, ശൂന്യം *out_മൂല്യം)
{
	ഘടന rpmi_shmem_mbox_controller *mctl =
			container_of(chan->mbox,
				     ഘടന rpmi_shmem_mbox_controller,
				     controller);
	ഘടന rpmi_srvgrp_chan *srvgrp_chan = to_srvgrp_chan(chan);

	തിരഞ്ഞെടുപ്പ് (attr_id) {
	അവസ്ഥ RPMI_CHANNEL_ATTR_PROTOCOL_VERSION:
		*((u32 *)out_മൂല്യം) = mctl->spec_version;
		നിർത്തുക;
	അവസ്ഥ RPMI_CHANNEL_ATTR_MAX_DATA_LEN:
		*((u32 *)out_മൂല്യം) = RPMI_സന്ദേശം_DATA_SIZE(mctl->slot_size);
		നിർത്തുക;
	അവസ്ഥ RPMI_CHANNEL_ATTR_P2A_DOORBELL_SYSMSI_INDEX:
		*((u32 *)out_മൂല്യം) = mctl->p2a_doorbell_sysmsi_index;
		നിർത്തുക;
	അവസ്ഥ RPMI_CHANNEL_ATTR_TX_TIMEOUT:
		*((u32 *)out_മൂല്യം) = RPMI_DEF_TX_TIMEOUT;
		നിർത്തുക;
	അവസ്ഥ RPMI_CHANNEL_ATTR_RX_TIMEOUT:
		*((u32 *)out_മൂല്യം) = RPMI_DEF_RX_TIMEOUT;
		നിർത്തുക;
	അവസ്ഥ RPMI_CHANNEL_ATTR_SERVICEGROUP_ID:
		*((u32 *)out_മൂല്യം) = srvgrp_chan->servicegroup_id;
		നിർത്തുക;
	അവസ്ഥ RPMI_CHANNEL_ATTR_SERVICEGROUP_VERSION:
		*((u32 *)out_മൂല്യം) = srvgrp_chan->servicegroup_version;
		നിർത്തുക;
	അവസ്ഥ RPMI_CHANNEL_ATTR_IMPL_ID:
		*((u32 *)out_മൂല്യം) = mctl->impl_id;
		നിർത്തുക;
	അവസ്ഥ RPMI_CHANNEL_ATTR_IMPL_VERSION:
		*((u32 *)out_മൂല്യം) = mctl->impl_version;
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
		തിരികെ SBI_ENOTSUPP;
	}

	തിരികെ 0;
}

സ്ഥിതം ഘടന mbox_chan *rpmi_shmem_mbox_request_chan(
						ഘടന mbox_controller *mbox,
						u32 *chan_args)
{
	പൂർണ്ണസംഖ്യ ret;
	u32 tval[2] = { 0 };
	ഘടന rpmi_srvgrp_chan *srvgrp_chan;
	ഘടന rpmi_shmem_mbox_controller *mctl =
			container_of(mbox,
				     ഘടന rpmi_shmem_mbox_controller,
				     controller);

	/* Service group id not defined or in reserved range is invalid */
	എങ്കിൽ (chan_args[0] >= RPMI_SRVGRP_ID_MAX_എണ്ണം &&
		chan_args[0] <= RPMI_SRVGRP_RESERVE_END)
		തിരികെ ശൂന്യ;

	/* Base serivce group is always present so probe other groups */
	എങ്കിൽ (chan_args[0] != RPMI_SRVGRP_BASE) {
		/* Probe service group */
		ret = smq_base_get_two_u32(mctl,
					   RPMI_BASE_SRV_PROBE_SERVICE_GROUP,
					   chan_args, tval);
		എങ്കിൽ (ret || !tval[1])
			തിരികെ ശൂന്യ;
	}

	srvgrp_chan = sbi_zalloc(വലുപ്പം(*srvgrp_chan));
	എങ്കിൽ (!srvgrp_chan)
		തിരികെ ശൂന്യ;

	srvgrp_chan->servicegroup_id = chan_args[0];
	srvgrp_chan->servicegroup_version = tval[1];

	തിരികെ &srvgrp_chan->chan;
}

സ്ഥിതം ശൂന്യം rpmi_shmem_mbox_free_chan(ഘടന mbox_controller *mbox,
				      ഘടന mbox_chan *chan)
{
	ഘടന rpmi_srvgrp_chan *srvgrp_chan = to_srvgrp_chan(chan);
	sbi_free(srvgrp_chan);
}

ബാഹ്യ ഘടന fdt_mailbox fdt_mailbox_rpmi_shmem;

സ്ഥിതം പൂർണ്ണസംഖ്യ rpmi_shmem_transport_init(ഘടന rpmi_shmem_mbox_controller *mctl,
				     സ്ഥിരം ശൂന്യം *fdt,
				     പൂർണ്ണസംഖ്യ nodeoff) NO_THREAD_SAFETY_ANALYSIS
{
	സ്ഥിരം അക്ഷരം *name;
	സ്ഥിരം fdt32_t *prop;
	പൂർണ്ണസംഖ്യ എണ്ണം, len, ret, qid;
	uint64_t reg_addr, reg_size;
	ഘടന smq_queue_ctx *qctx;

	ret = fdt_കണ്ണി_check_compatible(fdt, nodeoff,
					"riscv,rpmi-shmem-mbox");
	എങ്കിൽ (ret)
		തിരികെ ret;

	/* get queue slot size in bytes */
	prop = fdt_getprop(fdt, nodeoff, "riscv,slot-size", &len);
	എങ്കിൽ (!prop)
		തിരികെ SBI_ENOENT;

	mctl->slot_size = fdt32_to_cpu(*prop);
	എങ്കിൽ (mctl->slot_size < RPMI_SLOT_SIZE_MIN) {
		sbi_printf("%s: slot_size < mimnum required message size\n",
			   __func__);
		mctl->slot_size = RPMI_SLOT_SIZE_MIN;
	}

	/* get p2a doorbell system MSI index */
	prop = fdt_getprop(fdt, nodeoff, "riscv,p2a-doorbell-sysmsi-index", &len);
	mctl->p2a_doorbell_sysmsi_index = prop ? fdt32_to_cpu(*prop) : -1U;

	/* get a2p doorbell value */
	prop = fdt_getprop(fdt, nodeoff, "riscv,a2p-doorbell-value", &len);
	mctl->a2p_doorbell_മൂല്യം = prop ? fdt32_to_cpu(*prop) : 1;

	/*
	 * queue names count is taken as the number of queues
	 * supported which make it mandatory to provide the
	 * name of the queue.
	 */
	എണ്ണം = fdt_stringlist_എണ്ണം(fdt, nodeoff, "reg-names");
	എങ്കിൽ (എണ്ണം < 0 ||
	    എണ്ണം > (RPMI_QUEUE_IDX_MAX_എണ്ണം + RPMI_REG_IDX_MAX_എണ്ണം))
		തിരികെ SBI_EINVAL;

	mctl->queue_എണ്ണം = എണ്ണം - RPMI_REG_IDX_MAX_എണ്ണം;

	/* parse all queues and populate queues context structure */
	വേണ്ടി (qid = 0; qid < mctl->queue_എണ്ണം; qid++) {
		qctx = &mctl->queue_ctx_tbl[qid];

		/* get each queue share-memory base address and size*/
		ret = fdt_get_കണ്ണി_addr_size(fdt, nodeoff, qid,
					     &reg_addr, &reg_size);
		എങ്കിൽ (ret < 0 || !reg_addr || !reg_size)
			തിരികെ SBI_ENOENT;

		ret = sbi_domain_വേര്_add_memrange(reg_addr, reg_size, reg_size,
						   (SBI_DOMAIN_MEMREGION_MMIO |
						    SBI_DOMAIN_MEMREGION_M_READABLE |
						    SBI_DOMAIN_MEMREGION_M_WRITABLE));
		എങ്കിൽ (ret)
			തിരികെ ret;

		/* calculate number of slots in each queue */
		qctx->അക്കം_slots =
			(reg_size - (mctl->slot_size * RPMI_QUEUE_HEADER_SLOTS)) / mctl->slot_size;

		/* setup queue pointers */
		qctx->headptr = ((ശൂന്യം *)(unsigned long)reg_addr) +
				RPMI_QUEUE_HEAD_SLOT * mctl->slot_size;
		qctx->tailptr = ((ശൂന്യം *)(unsigned long)reg_addr) +
				RPMI_QUEUE_TAIL_SLOT * mctl->slot_size;
		qctx->buffer = ((ശൂന്യം *)(unsigned long)reg_addr) +
				RPMI_QUEUE_HEADER_SLOTS * mctl->slot_size;

		/* get the queue name */
		name = fdt_stringlist_get(fdt, nodeoff, "reg-names",
					  qid, &len);
		എങ്കിൽ (!name || (name && len < 0))
			തിരികെ len;

		sbi_memcpy(qctx->name, name, len);

		/* store the index as queue_id */
		qctx->queue_id = qid;

		SPIN_LOCK_INIT(qctx->queue_lock);
	}

	/* get the a2p-doorbell property name */
	name = fdt_stringlist_get(fdt, nodeoff, "reg-names", qid, &len);
	എങ്കിൽ (!name || (name && len < 0))
		തിരികെ len;

	/* fetch doorbell register address*/
	ret = fdt_get_കണ്ണി_addr_size(fdt, nodeoff, qid, &reg_addr,
				       &reg_size);
	എങ്കിൽ (!ret && !(strncmp(name, "a2p-doorbell", strlen("a2p-doorbell")))) {
		mctl->mb_regs = (ശൂന്യം *)(unsigned long)reg_addr;
		ret = sbi_domain_വേര്_add_memrange(reg_addr, reg_size, reg_size,
						   (SBI_DOMAIN_MEMREGION_MMIO |
						    SBI_DOMAIN_MEMREGION_M_READABLE |
						    SBI_DOMAIN_MEMREGION_M_WRITABLE));
		എങ്കിൽ (ret)
			തിരികെ ret;
	}

	തിരികെ SBI_SUCCESS;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ rpmi_shmem_mbox_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff,
				സ്ഥിരം ഘടന fdt_match *match)
{
	ഘടന rpmi_base_get_attributes_resp resp;
	ഘടന rpmi_shmem_mbox_controller *mctl;
	ഘടന rpmi_srvgrp_chan *base_srvgrp;
	u32 tval[2], args[1];
	പൂർണ്ണസംഖ്യ ret = 0;

	mctl = sbi_zalloc(വലുപ്പം(*mctl));
	എങ്കിൽ (!mctl)
		തിരികെ SBI_ENOMEM;

	/* Initialization transport from device tree */
	ret = rpmi_shmem_transport_init(mctl, fdt, nodeoff);
	എങ്കിൽ (ret)
		പോകുക fail_free_controller;

	/* Register mailbox controller */
	mctl->controller.id = nodeoff;
	mctl->controller.max_xfer_len =
			mctl->slot_size - വലുപ്പം(ഘടന rpmi_message_header);
	mctl->controller.driver = &fdt_mailbox_rpmi_shmem;
	mctl->controller.request_chan = rpmi_shmem_mbox_request_chan;
	mctl->controller.free_chan = rpmi_shmem_mbox_free_chan;
	mctl->controller.xfer = rpmi_shmem_mbox_xfer;
	mctl->controller.get_attribute = rpmi_shmem_mbox_get_attribute;
	ret = mbox_controller_add(&mctl->controller);
	എങ്കിൽ (ret)
		പോകുക fail_free_controller;

	/* Request base service group channel */
	tval[0] = RPMI_SRVGRP_BASE;
	mctl->base_chan = mbox_controller_request_chan(&mctl->controller,
							tval);
	എങ്കിൽ (!mctl->base_chan) {
		ret = SBI_ENOENT;
		പോകുക fail_remove_controller;
	}

	/* Update base service group version */
	base_srvgrp = to_srvgrp_chan(mctl->base_chan);
	args[0] = RPMI_SRVGRP_BASE;
	ret = smq_base_get_two_u32(mctl, RPMI_BASE_SRV_PROBE_SERVICE_GROUP,
				   &args[0], tval);
	എങ്കിൽ (ret)
		പോകുക fail_free_chan;
	base_srvgrp->servicegroup_version = tval[1];
	എങ്കിൽ (base_srvgrp->servicegroup_version < RPMI_BASE_VERSION_MIN) {
		ret = SBI_EINVAL;
		പോകുക fail_free_chan;
	}

	/* Get implementation id */
	ret = smq_base_get_two_u32(mctl,
				   RPMI_BASE_SRV_GET_IMPLEMENTATION_VERSION,
				   ശൂന്യ, tval);
	എങ്കിൽ (ret)
		പോകുക fail_free_chan;
	mctl->impl_version = tval[1];

	/* Get implementation version */
	ret = smq_base_get_two_u32(mctl, RPMI_BASE_SRV_GET_IMPLEMENTATION_IDN,
				   ശൂന്യ, tval);
	എങ്കിൽ (ret)
		പോകുക fail_free_chan;
	mctl->impl_id = tval[1];

	/* Get specification version */
	ret = smq_base_get_two_u32(mctl, RPMI_BASE_SRV_GET_SPEC_VERSION,
				   ശൂന്യ, tval);
	എങ്കിൽ (ret)
		പോകുക fail_free_chan;
	mctl->spec_version = tval[1];
	എങ്കിൽ (mctl->spec_version < RPMI_BASE_VERSION_MIN ||
	    mctl->spec_version != base_srvgrp->servicegroup_version) {
		ret = SBI_EINVAL;
		പോകുക fail_free_chan;
	}

	/* Get optional features implementation flags */
	ret = rpmi_normal_request_with_status(
			mctl->base_chan, RPMI_BASE_SRV_GET_ATTRIBUTES,
			ശൂന്യ, 0, 0,
			&resp, rpmi_u32_എണ്ണം(resp), rpmi_u32_എണ്ണം(resp));
	എങ്കിൽ (ret)
		പോകുക fail_free_chan;

	/* 1: M-mode, 0: S-mode */
	mctl->base_flags.f0_priv_level =
			resp.f0 & RPMI_BASE_FLAGS_F0_PRIVILEGE ? 1 : 0;
	/* 1: Supported, 0: Not Supported */
	mctl->base_flags.f0_ev_notif_en =
			resp.f0 & RPMI_BASE_FLAGS_F0_EV_NOTIFY ? 1 : 0;

	/* We only use M-mode RPMI context in OpenSBI */
	എങ്കിൽ (!mctl->base_flags.f0_priv_level) {
		ret = SBI_ENODEV;
		പോകുക fail_free_chan;
	}

	/*
	 * Continue without platform information string if not
	 * available or if an error is encountered while fetching
	 */
	rpmi_get_platform_info(mctl);

	തിരികെ 0;

fail_free_chan:
	mbox_controller_free_chan(mctl->base_chan);
fail_remove_controller:
	mbox_controller_remove(&mctl->controller);
fail_free_controller:
	sbi_free(mctl);
	തിരികെ ret;
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_match rpmi_shmem_mbox_match[] = {
	{ .compatible = "riscv,rpmi-shmem-mbox" },
	{ },
};

ഘടന fdt_mailbox fdt_mailbox_rpmi_shmem = {
	.driver = {
		.match_table = rpmi_shmem_mbox_match,
		.init = rpmi_shmem_mbox_init,
	},
	.xlate = fdt_mailbox_simple_xlate,
};
