/*
 * DamBOX is a software developed in C++ that intermittently releases the packet on the interface,
 * like a dam : using netfilter, incoming packets are intercepted and blocked until released at the interface goodput.
 * The user specifies the duration and frequency of the blocking and releasing function.
 *
 * DamBOX can be used to emulate beam hopping in satellite telecommunication system
 *
 * This file is part of the DamBOX software.
 * 
 * Copyright © 2020 CNES
 *
 *
 * DamBOX is free software : you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY, without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/.
 *
 * 
 * @author Clément RUCEL / <crucel@silicom.fr>
 * @author Benoit Gaillac / <bgaillac@silicom.fr>
 * @author Corentin Ormiere / <cormiere@silicom.fr>
 */

#include "nfq.h"

static int cb(struct nfq_q_handle *qh, struct nfgenmsg *nfmsg, struct nfq_data *nfa, void *data)
{
	u_int32_t id=0;
    struct nfqnl_msg_packet_hdr *ph;
	ph = nfq_get_msg_packet_hdr(nfa);
	id = ntohl(ph->packet_id);
	setId(id);
	return 1;
}

int initNfq (struct nfq_handle **h, struct nfq_q_handle **qh)
{
	int fd=0;

	*h = nfq_open();

	if (!*h)
	{
	    std::cerr << "[ERROR] Function nfq_open() failed" << std::endl;
		fd = -1;
	}

	if (nfq_unbind_pf(*h, AF_INET) < 0)
	{
	    std::cerr << "[ERROR] Function nfq_unbind_pf() has failed" << std::endl;
		fd = -2;
	}

	if (nfq_bind_pf(*h, AF_INET) < 0)
	{
		std::cerr << "[ERROR] Function nfq_bind_pf() has failed" << std::endl;
		fd = -3;
	}

	*qh = nfq_create_queue(*h,  0, &cb, NULL);
	if (!*qh)
	{
	    std::cerr << "[ERROR] Function nfq_create_queue() has failed" << std::endl;
		fd = -4;
	}

	if (nfq_set_mode(*qh, NFQNL_COPY_PACKET, 0xffff) < 0)
	{
	    std::cerr << "[ERROR] Can't set packet_copy mode" << std::endl;
		fd = -5;
	}

	nfnl_rcvbufsiz(nfq_nfnlh(*h),8172*8172); //Socket buffer size
	nfq_set_queue_maxlen(*qh, 10000); //Maximum number of packets the kernel will store before dropping upcoming packets
	fd = nfq_fd(*h);

	return fd;
}

int closeNfq (struct nfq_handle **h, struct nfq_q_handle **qh)
{
	nfq_destroy_queue(*qh);

#ifdef INSANE
	/* normally, applications SHOULD NOT issue this command, since
	 * it detaches other programs/sockets from AF_INET, too ! */
	printf("unbinding from AF_INET\n");
	nfq_unbind_pf(*h, AF_INET);
#endif
	nfq_close(*h);

	return 1;
}
