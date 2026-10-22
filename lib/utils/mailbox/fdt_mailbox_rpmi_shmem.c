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

#समावेश <libfdt.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi/sbi_timer.h>
#समावेश <sbi/riscv_io.h>
#समावेश <sbi/riscv_locks.h>
#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/riscv_barrier.h>
#समावेश <sbi_utils/fdt/fdt_helper.h>
#समावेश <sbi_utils/mailbox/mailbox.h>
#समावेश <sbi_utils/mailbox/fdt_mailbox.h>
#समावेश <sbi_utils/mailbox/rpmi_mailbox.h>

/** Minimum Base group version required */
#घोषणा RPMI_BASE_VERSION_MIN		RPMI_VERSION(1, 0)

/**************** RPMI Transport Structures and Macros ***********/

#घोषणा GET_SERVICEGROUP_ID(संदेश)		\
({						\
	काष्ठा rpmi_message *mbuf = संदेश;	\
	le16_to_cpu(mbuf->header.servicegroup_id);\
})

#घोषणा GET_SERVICE_ID(संदेश)			\
({						\
	काष्ठा rpmi_message *mbuf = संदेश;	\
	mbuf->header.service_id;		\
})

#घोषणा GET_FLAGS(संदेश)				\
({						\
	काष्ठा rpmi_message *mbuf = संदेश;	\
	mbuf->header.flags;			\
})

#घोषणा GET_MESSAGE_ID(संदेश)			\
({						\
	काष्ठा rpmi_message *mbuf = संदेश;	\
	((u32)mbuf->header.flags << (RPMI_संदेश_FLAGS_OFFSET * 8)) | \
	((u32)mbuf->header.service_id << (RPMI_संदेश_SERVICE_ID_OFFSET * 8)) | \
	((u32)le16_to_cpu(mbuf->header.servicegroup_id)); \
})

#घोषणा MAKE_MESSAGE_ID(__group_id, __service_id, __flags)	\
({						\
	u32 __ret = 0;				\
	__ret |= (u32)(__group_id) << (RPMI_संदेश_SERVICEGROUP_ID_OFFSET * 8); \
	__ret |= (u32)(__service_id) << (RPMI_संदेश_SERVICE_ID_OFFSET * 8); \
	__ret |= (u32)(__flags) << (RPMI_संदेश_FLAGS_OFFSET * 8); \
	__ret;					\
})

#घोषणा GET_DLEN(संदेश)				\
({						\
	काष्ठा rpmi_message *mbuf = संदेश;	\
	le16_to_cpu(mbuf->header.datalen);	\
})

#घोषणा GET_TOKEN(संदेश)				\
({						\
	काष्ठा rpmi_message *mbuf = संदेश;	\
	le16_to_cpu(mbuf->header.token);	\
})

#घोषणा GET_MESSAGE_TYPE(संदेश)						\
({									\
	uint8_t flags = *((uint8_t *)संदेश + RPMI_संदेश_FLAGS_OFFSET);	\
	((flags & RPMI_संदेश_FLAGS_TYPE) >> RPMI_संदेश_FLAGS_TYPE_POS));	\
})

क्रमागत rpmi_queue_type {
	RPMI_QUEUE_TYPE_REQ = 0,
	RPMI_QUEUE_TYPE_ACK = 1,
};

क्रमागत rpmi_queue_idx {
	RPMI_QUEUE_IDX_A2P_REQ = 0,
	RPMI_QUEUE_IDX_P2A_ACK = 1,
	RPMI_QUEUE_IDX_P2A_REQ = 2,
	RPMI_QUEUE_IDX_A2P_ACK = 3,
	RPMI_QUEUE_IDX_MAX_गिनती,
};

क्रमागत rpmi_reg_idx {
	RPMI_REG_IDX_DB_REG = 0, /* Doorbell register */
	RPMI_REG_IDX_MAX_गिनती,
};

/** Mailbox registers */
काष्ठा rpmi_mb_regs {
	/* doorbell from AP -> PuC*/
	अस्थिर le32_t db_reg;
} __packed;

/** Single Queue Context Structure */
काष्ठा smq_queue_ctx {
	u32 queue_id;
	u32 अंक_slots;
	spinlock_t queue_lock;
	/* Type of queue - REQ or ACK */
	क्रमागत rpmi_queue_type queue_type;
	/* Pointers to the queue shared memory */
	अस्थिर le32_t *headptr PT_GUARDED_BY(&queue_lock);
	अस्थिर le32_t *tailptr PT_GUARDED_BY(&queue_lock);
	अस्थिर uint8_t *बफर PT_GUARDED_BY(&queue_lock);
	/* Name of the queue */
	अक्षर name[RPMI_NAME_CHARS_MAX];
};

काष्ठा rpmi_srvgrp_chan {
	u32 servicegroup_id;
	u32 servicegroup_version;
	काष्ठा mbox_chan chan;
};

#घोषणा to_srvgrp_chan(mbox_chan)	\
		container_of(mbox_chan, काष्ठा rpmi_srvgrp_chan, chan);

काष्ठा rpmi_shmem_mbox_controller {
	/* Driver specific members */
	u32 slot_आकार;
	u32 queue_गिनती;
	u32 p2a_doorbell_sysmsi_सूचकांक;
	u32 a2p_doorbell_मूल्य;
	काष्ठा rpmi_mb_regs *mb_regs;
	काष्ठा smq_queue_ctx queue_ctx_tbl[RPMI_QUEUE_IDX_MAX_गिनती];
	/* Mailbox framework related members */
	काष्ठा mbox_controller controller;
	काष्ठा mbox_chan *base_chan;
	u32 impl_version;
	u32 impl_id;
	u32 spec_version;
	u32 plat_info_len;
	अक्षर *plat_info;
	काष्ठा {
		u8 f0_priv_level;
		bool f0_ev_notif_en;
	} base_flags;
};

/**************** Shared Memory Queues Helpers **************/

अटल bool __smq_queue_full(काष्ठा smq_queue_ctx *qctx)
	MUST_HOLD(&qctx->queue_lock)
{
	वापस ((le32_to_cpu(*qctx->tailptr) + 1) % qctx->अंक_slots ==
			le32_to_cpu(*qctx->headptr)) ? true : false;
}

अटल bool __smq_queue_empty(काष्ठा smq_queue_ctx *qctx)
	MUST_HOLD(&qctx->queue_lock)
{
	वापस (le32_to_cpu(*qctx->headptr) ==
		le32_to_cpu(*qctx->tailptr)) ? true : false;
}

अटल पूर्णांक __smq_rx(काष्ठा smq_queue_ctx *qctx, u32 slot_आकार,
		    u32 service_group_id, काष्ठा mbox_xfer *xfer)
	MUST_HOLD(&qctx->queue_lock)
{
	व्योम *dst, *src;
	काष्ठा rpmi_message *संदेश;
	u32 i, tmp, pos, dlen, msgidn, headidx, tailidx;
	काष्ठा rpmi_message_args *args = xfer->args;
	bool no_rx_token = (args->flags & RPMI_संदेश_FLAGS_NO_RX_TOKEN) ?
			   true : false;

	/* Rx sanity checks */
	अगर ((माप(u32) * args->rx_endian_शब्दगण) >
	    (slot_आकार - माप(काष्ठा rpmi_message_header)))
		वापस SBI_EINVAL;
	अगर ((माप(u32) * args->rx_endian_शब्दगण) > xfer->rx_len)
		वापस SBI_EINVAL;

	/* There should be some message in the queue */
	अगर (__smq_queue_empty(qctx))
		वापस SBI_ENOENT;

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
	जबतक (pos != tailidx) {
		src = (व्योम *)qctx->बफर + (pos * slot_आकार);
		अगर ((no_rx_token && GET_MESSAGE_ID(src) == msgidn) ||
		    (GET_TOKEN(src) == (xfer->seq & RPMI_संदेश_TOKEN_MASK)))
			अवरोध;
		pos = (pos + 1) % qctx->अंक_slots;
	}
	अगर (pos == tailidx)
		वापस SBI_ENOENT;

	/* If Rx message is not first message then make it first message */
	अगर (pos != headidx) {
		src = (व्योम *)qctx->बफर + (pos * slot_आकार);
		dst = (व्योम *)qctx->बफर + (headidx * slot_आकार);
		क्रम (i = 0; i < slot_आकार / माप(u32); i++) {
			tmp = ((u32 *)dst)[i];
			((u32 *)dst)[i] = ((u32 *)src)[i];
			((u32 *)src)[i] = tmp;
		}
	}

	/* Update rx_token if not available */
	संदेश = (व्योम *)qctx->बफर + (headidx * slot_आकार);
	अगर (no_rx_token)
		args->rx_token = GET_TOKEN(संदेश);

	/* Extract data from the first message */
	अगर (xfer->rx) {
		args->rx_तथ्य_len = dlen = GET_DLEN(संदेश);
		अगर (dlen > xfer->rx_len)
			dlen = xfer->rx_len;
		src = (व्योम *)संदेश + माप(काष्ठा rpmi_message_header);
		dst = xfer->rx;
		क्रम (i = 0; i < args->rx_endian_शब्दगण; i++)
			((u32 *)dst)[i] = le32_to_cpu(((u32 *)src)[i]);
		dst += माप(u32) * args->rx_endian_शब्दगण;
		src += माप(u32) * args->rx_endian_शब्दगण;
		sbi_memcpy(dst, src,
			xfer->rx_len - (माप(u32) * args->rx_endian_शब्दगण));
	}

	/* Update the head/read index */
	*qctx->headptr = cpu_to_le32(headidx + 1) % qctx->अंक_slots;

	/* Make sure updates to head are immediately visible to PuC */
	smp_wmb();

	वापस SBI_OK;
}

अटल पूर्णांक __smq_tx(काष्ठा smq_queue_ctx *qctx, काष्ठा rpmi_mb_regs *mb_regs,
		    u32 a2p_doorbell_मूल्य, u32 slot_आकार, u32 service_group_id,
		    काष्ठा mbox_xfer *xfer) MUST_HOLD(&qctx->queue_lock)
{
	u32 i, tailidx;
	व्योम *dst, *src;
	काष्ठा rpmi_message_header header = { 0 };
	काष्ठा rpmi_message_args *args = xfer->args;

	/* Tx sanity checks */
	अगर ((माप(u32) * args->tx_endian_शब्दगण) >
	    (slot_आकार - माप(काष्ठा rpmi_message_header)))
		वापस SBI_EINVAL;
	अगर ((माप(u32) * args->tx_endian_शब्दगण) > xfer->tx_len)
		वापस SBI_EINVAL;

	/* There should be some room in the queue */
	अगर (__smq_queue_full(qctx))
		वापस SBI_ENOMEM;

	/* Get the tail/write index */
	tailidx = le32_to_cpu(*qctx->tailptr);

	/* Prepare the header to be written into the slot */
	header.servicegroup_id = cpu_to_le16(service_group_id);
	header.service_id = args->service_id;
	header.flags = args->type;
	header.datalen = cpu_to_le16((u16)xfer->tx_len);
	header.token = cpu_to_le16((u16)xfer->seq);

	/* Write header into the slot */
	dst = (अक्षर *)qctx->बफर + (tailidx * slot_आकार);
	sbi_memcpy(dst, &header, माप(header));
	dst += माप(header);

	/* Write data into the slot */
	अगर (xfer->tx) {
		src = xfer->tx;
		क्रम (i = 0; i < args->tx_endian_शब्दगण; i++)
			((u32 *)dst)[i] = cpu_to_le32(((u32 *)src)[i]);
		dst += माप(u32) * args->tx_endian_शब्दगण;
		src += माप(u32) * args->tx_endian_शब्दगण;
		sbi_memcpy(dst, src,
			xfer->tx_len - (माप(u32) * args->tx_endian_शब्दगण));
	}

	/* Make sure queue chanages are visible to PuC before updating tail */
	smp_wmb();

	/* Update the tail/write index */
	*qctx->tailptr = cpu_to_le32(tailidx + 1) % qctx->अंक_slots;

	/* Ring the RPMI doorbell if present */
	अगर (mb_regs)
		writel(a2p_doorbell_मूल्य, &mb_regs->db_reg);

	वापस SBI_OK;
}

अटल पूर्णांक smq_rx(काष्ठा rpmi_shmem_mbox_controller *mctl,
		  u32 queue_id, u32 service_group_id, काष्ठा mbox_xfer *xfer)
{
	पूर्णांक ret, rxretry = 0;
	काष्ठा smq_queue_ctx *qctx;

	अगर (mctl->queue_गिनती < queue_id) {
		sbi_printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		वापस SBI_EINVAL;
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
	करो {
		spin_lock(&qctx->queue_lock);
		ret = __smq_rx(qctx, mctl->slot_आकार, service_group_id, xfer);
		spin_unlock(&qctx->queue_lock);
		अगर (!ret)
			वापस 0;

		sbi_timer_mdelay(1);
		rxretry += 1;
	} जबतक (rxretry < xfer->rx_timeout);

	वापस SBI_ETIMEDOUT;
}

अटल पूर्णांक smq_tx(काष्ठा rpmi_shmem_mbox_controller *mctl,
		  u32 queue_id, u32 service_group_id, काष्ठा mbox_xfer *xfer)
{
	पूर्णांक ret, txretry = 0;
	काष्ठा smq_queue_ctx *qctx;

	अगर (mctl->queue_गिनती < queue_id) {
		sbi_printf("%s: invalid queue_id or service_group_id\n",
			   __func__);
		वापस SBI_EINVAL;
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
	करो {
		spin_lock(&qctx->queue_lock);
		ret = __smq_tx(qctx, mctl->mb_regs, mctl->a2p_doorbell_मूल्य,
				mctl->slot_आकार, service_group_id, xfer);
		spin_unlock(&qctx->queue_lock);
		अगर (!ret)
			वापस 0;

		sbi_timer_mdelay(1);
		txretry += 1;
	} जबतक (txretry < xfer->tx_timeout);

	वापस SBI_ETIMEDOUT;
}

अटल पूर्णांक rpmi_get_platform_info(काष्ठा rpmi_shmem_mbox_controller *mctl)
{
	पूर्णांक ret = SBI_OK;

	/**
	 * platform string can occupy max possible size
	 * max possible space in the message data as
	 * per the format
	 */
	काष्ठा rpmi_base_get_platform_info_resp *resp =
			sbi_zalloc(RPMI_संदेश_तथ्य_आकार(mctl->slot_आकार));
	अगर (!resp)
		वापस SBI_ENOMEM;

	ret = rpmi_normal_request_with_status(mctl->base_chan,
				 RPMI_BASE_SRV_GET_PLATFORM_INFO,
				 शून्य, 0, 0,
				 resp,
				 RPMI_संदेश_तथ्य_आकार(mctl->slot_आकार)/4,
				 RPMI_संदेश_तथ्य_आकार(mctl->slot_आकार)/4);
	अगर (ret)
		जाओ fail_free_resp;

	mctl->plat_info_len = resp->plat_info_len;
	mctl->plat_info = sbi_zalloc(mctl->plat_info_len);
	अगर (!mctl->plat_info) {
		ret = SBI_ENOMEM;
		जाओ fail_free_resp;
	}

	sbi_strncpy(mctl->plat_info, resp->plat_info, mctl->plat_info_len);

fail_free_resp:
	sbi_free(resp);
	वापस ret;
}

अटल पूर्णांक smq_base_get_two_u32(काष्ठा rpmi_shmem_mbox_controller *mctl,
				u32 service_id, u32 *inarg, u32 *outvals)
{
	वापस rpmi_normal_request_with_status(
			mctl->base_chan, service_id,
			inarg, (inarg) ? 1 : 0, (inarg) ? 1 : 0,
			outvals, 2, 2);
}

/**************** Mailbox Controller Functions **************/

अटल पूर्णांक rpmi_shmem_mbox_xfer(काष्ठा mbox_chan *chan, काष्ठा mbox_xfer *xfer)
{
	पूर्णांक ret;
	u32 tx_qid = 0, rx_qid = 0;
	काष्ठा rpmi_shmem_mbox_controller *mctl =
			container_of(chan->mbox,
				     काष्ठा rpmi_shmem_mbox_controller,
				     controller);
	काष्ठा rpmi_srvgrp_chan *srvgrp_chan = to_srvgrp_chan(chan);

	काष्ठा rpmi_message_args *args = xfer->args;
	bool do_tx = (args->flags & RPMI_संदेश_FLAGS_NO_TX) ? false : true;
	bool do_rx = (args->flags & RPMI_संदेश_FLAGS_NO_RX) ? false : true;

	अगर (!do_tx && !do_rx)
		वापस SBI_EINVAL;

	चयन (args->type) {
	हाल RPMI_संदेश_NORMAL_REQUEST:
		अगर (do_tx && do_rx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
			rx_qid = RPMI_QUEUE_IDX_P2A_ACK;
		} अन्यथा अगर (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
		} अन्यथा अगर (do_rx) {
			rx_qid = RPMI_QUEUE_IDX_P2A_REQ;
		}
		अवरोध;
	हाल RPMI_संदेश_POSTED_REQUEST:
		अगर (do_tx && do_rx)
			वापस SBI_EINVAL;
		अगर (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_REQ;
		} अन्यथा {
			rx_qid = RPMI_QUEUE_IDX_P2A_REQ;
		}
		अवरोध;
	हाल RPMI_संदेश_ACKNOWLDGEMENT:
		अगर (do_tx && do_rx)
			वापस SBI_EINVAL;
		अगर (do_tx) {
			tx_qid = RPMI_QUEUE_IDX_A2P_ACK;
		} अन्यथा {
			rx_qid = RPMI_QUEUE_IDX_P2A_ACK;
		}
		अवरोध;
	शेष:
		वापस SBI_ENOTSUPP;
	}

	अगर (do_tx) {
		ret = smq_tx(mctl, tx_qid, srvgrp_chan->servicegroup_id, xfer);
		अगर (ret)
			वापस ret;
	}

	अगर (do_rx) {
		ret = smq_rx(mctl, rx_qid, srvgrp_chan->servicegroup_id, xfer);
		अगर (ret)
			वापस ret;
	}

	वापस 0;
}

अटल पूर्णांक rpmi_shmem_mbox_get_attribute(काष्ठा mbox_chan *chan,
					 पूर्णांक attr_id, व्योम *out_मूल्य)
{
	काष्ठा rpmi_shmem_mbox_controller *mctl =
			container_of(chan->mbox,
				     काष्ठा rpmi_shmem_mbox_controller,
				     controller);
	काष्ठा rpmi_srvgrp_chan *srvgrp_chan = to_srvgrp_chan(chan);

	चयन (attr_id) {
	हाल RPMI_CHANNEL_ATTR_PROTOCOL_VERSION:
		*((u32 *)out_मूल्य) = mctl->spec_version;
		अवरोध;
	हाल RPMI_CHANNEL_ATTR_MAX_तथ्य_LEN:
		*((u32 *)out_मूल्य) = RPMI_संदेश_तथ्य_आकार(mctl->slot_आकार);
		अवरोध;
	हाल RPMI_CHANNEL_ATTR_P2_A_DOORBELL_SYSMSI_सूचकांक:
		*((u32 *)out_मूल्य) = mctl->p2a_doorbell_sysmsi_सूचकांक;
		अवरोध;
	हाल RPMI_CHANNEL_ATTR_TX_TIMEOUT:
		*((u32 *)out_मूल्य) = RPMI_DEF_TX_TIMEOUT;
		अवरोध;
	हाल RPMI_CHANNEL_ATTR_RX_TIMEOUT:
		*((u32 *)out_मूल्य) = RPMI_DEF_RX_TIMEOUT;
		अवरोध;
	हाल RPMI_CHANNEL_ATTR_SERVICEGROUP_ID:
		*((u32 *)out_मूल्य) = srvgrp_chan->servicegroup_id;
		अवरोध;
	हाल RPMI_CHANNEL_ATTR_SERVICEGROUP_VERSION:
		*((u32 *)out_मूल्य) = srvgrp_chan->servicegroup_version;
		अवरोध;
	हाल RPMI_CHANNEL_ATTR_IMPL_ID:
		*((u32 *)out_मूल्य) = mctl->impl_id;
		अवरोध;
	हाल RPMI_CHANNEL_ATTR_IMPL_VERSION:
		*((u32 *)out_मूल्य) = mctl->impl_version;
		अवरोध;
	शेष:
		वापस SBI_ENOTSUPP;
	}

	वापस 0;
}

अटल काष्ठा mbox_chan *rpmi_shmem_mbox_request_chan(
						काष्ठा mbox_controller *mbox,
						u32 *chan_args)
{
	पूर्णांक ret;
	u32 tval[2] = { 0 };
	काष्ठा rpmi_srvgrp_chan *srvgrp_chan;
	काष्ठा rpmi_shmem_mbox_controller *mctl =
			container_of(mbox,
				     काष्ठा rpmi_shmem_mbox_controller,
				     controller);

	/* Service group id not defined or in reserved range is invalid */
	अगर (chan_args[0] >= RPMI_SRVGRP_ID_MAX_गिनती &&
		chan_args[0] <= RPMI_SRVGRP_RESERVE_END)
		वापस शून्य;

	/* Base serivce group is always present so probe other groups */
	अगर (chan_args[0] != RPMI_SRVGRP_BASE) {
		/* Probe service group */
		ret = smq_base_get_two_u32(mctl,
					   RPMI_BASE_SRV_PROBE_SERVICE_GROUP,
					   chan_args, tval);
		अगर (ret || !tval[1])
			वापस शून्य;
	}

	srvgrp_chan = sbi_zalloc(माप(*srvgrp_chan));
	अगर (!srvgrp_chan)
		वापस शून्य;

	srvgrp_chan->servicegroup_id = chan_args[0];
	srvgrp_chan->servicegroup_version = tval[1];

	वापस &srvgrp_chan->chan;
}

अटल व्योम rpmi_shmem_mbox_free_chan(काष्ठा mbox_controller *mbox,
				      काष्ठा mbox_chan *chan)
{
	काष्ठा rpmi_srvgrp_chan *srvgrp_chan = to_srvgrp_chan(chan);
	sbi_free(srvgrp_chan);
}

बाह्य काष्ठा fdt_mailbox fdt_mailbox_rpmi_shmem;

अटल पूर्णांक rpmi_shmem_transport_init(काष्ठा rpmi_shmem_mbox_controller *mctl,
				     स्थिर व्योम *fdt,
				     पूर्णांक nodeoff) NO_THREAD_SAFETY_ANALYSIS
{
	स्थिर अक्षर *name;
	स्थिर fdt32_t *prop;
	पूर्णांक गिनती, len, ret, qid;
	uint64_t reg_addr, reg_आकार;
	काष्ठा smq_queue_ctx *qctx;

	ret = fdt_गाँठ_check_compatible(fdt, nodeoff,
					"riscv,rpmi-shmem-mbox");
	अगर (ret)
		वापस ret;

	/* get queue slot size in bytes */
	prop = fdt_getprop(fdt, nodeoff, "riscv,slot-size", &len);
	अगर (!prop)
		वापस SBI_ENOENT;

	mctl->slot_आकार = fdt32_to_cpu(*prop);
	अगर (mctl->slot_आकार < RPMI_SLOT_आकार_MIN) {
		sbi_printf("%s: slot_size < mimnum required message size\n",
			   __func__);
		mctl->slot_आकार = RPMI_SLOT_आकार_MIN;
	}

	/* get p2a doorbell system MSI index */
	prop = fdt_getprop(fdt, nodeoff, "riscv,p2a-doorbell-sysmsi-index", &len);
	mctl->p2a_doorbell_sysmsi_सूचकांक = prop ? fdt32_to_cpu(*prop) : -1U;

	/* get a2p doorbell value */
	prop = fdt_getprop(fdt, nodeoff, "riscv,a2p-doorbell-value", &len);
	mctl->a2p_doorbell_मूल्य = prop ? fdt32_to_cpu(*prop) : 1;

	/*
	 * queue names count is taken as the number of queues
	 * supported which make it mandatory to provide the
	 * name of the queue.
	 */
	गिनती = fdt_stringlist_गिनती(fdt, nodeoff, "reg-names");
	अगर (गिनती < 0 ||
	    गिनती > (RPMI_QUEUE_IDX_MAX_गिनती + RPMI_REG_IDX_MAX_गिनती))
		वापस SBI_EINVAL;

	mctl->queue_गिनती = गिनती - RPMI_REG_IDX_MAX_गिनती;

	/* parse all queues and populate queues context structure */
	क्रम (qid = 0; qid < mctl->queue_गिनती; qid++) {
		qctx = &mctl->queue_ctx_tbl[qid];

		/* get each queue share-memory base address and size*/
		ret = fdt_get_गाँठ_addr_आकार(fdt, nodeoff, qid,
					     &reg_addr, &reg_आकार);
		अगर (ret < 0 || !reg_addr || !reg_आकार)
			वापस SBI_ENOENT;

		ret = sbi_domain_मूल_add_memrange(reg_addr, reg_आकार, reg_आकार,
						   (SBI_DOMAIN_MEMREGION_MMIO |
						    SBI_DOMAIN_MEMREGION_M_READABLE |
						    SBI_DOMAIN_MEMREGION_M_WRITABLE));
		अगर (ret)
			वापस ret;

		/* calculate number of slots in each queue */
		qctx->अंक_slots =
			(reg_आकार - (mctl->slot_आकार * RPMI_QUEUE_HEADER_SLOTS)) / mctl->slot_आकार;

		/* setup queue pointers */
		qctx->headptr = ((व्योम *)(अचिन्हित दीर्घ)reg_addr) +
				RPMI_QUEUE_शीर्ष_SLOT * mctl->slot_आकार;
		qctx->tailptr = ((व्योम *)(अचिन्हित दीर्घ)reg_addr) +
				RPMI_QUEUE_पुच्छ_SLOT * mctl->slot_आकार;
		qctx->बफर = ((व्योम *)(अचिन्हित दीर्घ)reg_addr) +
				RPMI_QUEUE_HEADER_SLOTS * mctl->slot_आकार;

		/* get the queue name */
		name = fdt_stringlist_get(fdt, nodeoff, "reg-names",
					  qid, &len);
		अगर (!name || (name && len < 0))
			वापस len;

		sbi_memcpy(qctx->name, name, len);

		/* store the index as queue_id */
		qctx->queue_id = qid;

		SPIN_LOCK_INIT(qctx->queue_lock);
	}

	/* get the a2p-doorbell property name */
	name = fdt_stringlist_get(fdt, nodeoff, "reg-names", qid, &len);
	अगर (!name || (name && len < 0))
		वापस len;

	/* fetch doorbell register address*/
	ret = fdt_get_गाँठ_addr_आकार(fdt, nodeoff, qid, &reg_addr,
				       &reg_आकार);
	अगर (!ret && !(म_भेदन(name, "a2p-doorbell", म_माप("a2p-doorbell")))) {
		mctl->mb_regs = (व्योम *)(अचिन्हित दीर्घ)reg_addr;
		ret = sbi_domain_मूल_add_memrange(reg_addr, reg_आकार, reg_आकार,
						   (SBI_DOMAIN_MEMREGION_MMIO |
						    SBI_DOMAIN_MEMREGION_M_READABLE |
						    SBI_DOMAIN_MEMREGION_M_WRITABLE));
		अगर (ret)
			वापस ret;
	}

	वापस SBI_SUCCESS;
}

अटल पूर्णांक rpmi_shmem_mbox_init(स्थिर व्योम *fdt, पूर्णांक nodeoff,
				स्थिर काष्ठा fdt_match *match)
{
	काष्ठा rpmi_base_get_attributes_resp resp;
	काष्ठा rpmi_shmem_mbox_controller *mctl;
	काष्ठा rpmi_srvgrp_chan *base_srvgrp;
	u32 tval[2], args[1];
	पूर्णांक ret = 0;

	mctl = sbi_zalloc(माप(*mctl));
	अगर (!mctl)
		वापस SBI_ENOMEM;

	/* Initialization transport from device tree */
	ret = rpmi_shmem_transport_init(mctl, fdt, nodeoff);
	अगर (ret)
		जाओ fail_free_controller;

	/* Register mailbox controller */
	mctl->controller.id = nodeoff;
	mctl->controller.max_xfer_len =
			mctl->slot_आकार - माप(काष्ठा rpmi_message_header);
	mctl->controller.driver = &fdt_mailbox_rpmi_shmem;
	mctl->controller.request_chan = rpmi_shmem_mbox_request_chan;
	mctl->controller.free_chan = rpmi_shmem_mbox_free_chan;
	mctl->controller.xfer = rpmi_shmem_mbox_xfer;
	mctl->controller.get_attribute = rpmi_shmem_mbox_get_attribute;
	ret = mbox_controller_add(&mctl->controller);
	अगर (ret)
		जाओ fail_free_controller;

	/* Request base service group channel */
	tval[0] = RPMI_SRVGRP_BASE;
	mctl->base_chan = mbox_controller_request_chan(&mctl->controller,
							tval);
	अगर (!mctl->base_chan) {
		ret = SBI_ENOENT;
		जाओ fail_remove_controller;
	}

	/* Update base service group version */
	base_srvgrp = to_srvgrp_chan(mctl->base_chan);
	args[0] = RPMI_SRVGRP_BASE;
	ret = smq_base_get_two_u32(mctl, RPMI_BASE_SRV_PROBE_SERVICE_GROUP,
				   &args[0], tval);
	अगर (ret)
		जाओ fail_free_chan;
	base_srvgrp->servicegroup_version = tval[1];
	अगर (base_srvgrp->servicegroup_version < RPMI_BASE_VERSION_MIN) {
		ret = SBI_EINVAL;
		जाओ fail_free_chan;
	}

	/* Get implementation id */
	ret = smq_base_get_two_u32(mctl,
				   RPMI_BASE_SRV_GET_IMPLEMENTATION_VERSION,
				   शून्य, tval);
	अगर (ret)
		जाओ fail_free_chan;
	mctl->impl_version = tval[1];

	/* Get implementation version */
	ret = smq_base_get_two_u32(mctl, RPMI_BASE_SRV_GET_IMPLEMENTATION_IDN,
				   शून्य, tval);
	अगर (ret)
		जाओ fail_free_chan;
	mctl->impl_id = tval[1];

	/* Get specification version */
	ret = smq_base_get_two_u32(mctl, RPMI_BASE_SRV_GET_SPEC_VERSION,
				   शून्य, tval);
	अगर (ret)
		जाओ fail_free_chan;
	mctl->spec_version = tval[1];
	अगर (mctl->spec_version < RPMI_BASE_VERSION_MIN ||
	    mctl->spec_version != base_srvgrp->servicegroup_version) {
		ret = SBI_EINVAL;
		जाओ fail_free_chan;
	}

	/* Get optional features implementation flags */
	ret = rpmi_normal_request_with_status(
			mctl->base_chan, RPMI_BASE_SRV_GET_ATTRIBUTES,
			शून्य, 0, 0,
			&resp, rpmi_u32_गिनती(resp), rpmi_u32_गिनती(resp));
	अगर (ret)
		जाओ fail_free_chan;

	/* 1: M-mode, 0: S-mode */
	mctl->base_flags.f0_priv_level =
			resp.f0 & RPMI_BASE_FLAGS_F0_PRIVILEGE ? 1 : 0;
	/* 1: Supported, 0: Not Supported */
	mctl->base_flags.f0_ev_notif_en =
			resp.f0 & RPMI_BASE_FLAGS_F0_EV_NOTIFY ? 1 : 0;

	/* We only use M-mode RPMI context in OpenSBI */
	अगर (!mctl->base_flags.f0_priv_level) {
		ret = SBI_ENODEV;
		जाओ fail_free_chan;
	}

	/*
	 * Continue without platform information string if not
	 * available or if an error is encountered while fetching
	 */
	rpmi_get_platform_info(mctl);

	वापस 0;

fail_free_chan:
	mbox_controller_free_chan(mctl->base_chan);
fail_remove_controller:
	mbox_controller_remove(&mctl->controller);
fail_free_controller:
	sbi_free(mctl);
	वापस ret;
}

अटल स्थिर काष्ठा fdt_match rpmi_shmem_mbox_match[] = {
	{ .compatible = "riscv,rpmi-shmem-mbox" },
	{ },
};

काष्ठा fdt_mailbox fdt_mailbox_rpmi_shmem = {
	.driver = {
		.match_table = rpmi_shmem_mbox_match,
		.init = rpmi_shmem_mbox_init,
	},
	.xlate = fdt_mailbox_simple_xlate,
};
