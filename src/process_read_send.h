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

#include "ressources.h"

extern int iReadFdPkt;
extern int iReadFdSize;
extern int iCompteurPktOut;

extern std::mutex mtxDamOn;
extern std::condition_variable cvDamOn;

extern std::mutex mtxDataInFifo;
extern std::condition_variable cvDataInFifo;

/*
 * This function empties the contents of the fifo and handle the packet
 * as long as the dam is on.
 */
void sendPackets(struct nfq_handle *h, struct nfq_q_handle *qh, int max_verdict);

/*
 * Thread which call sendPackets when the dam slot reach the state ON
 * After this step, the thread call vider_fifo each time it receive
 * a signal of the thread process_nfq_recv (Signal that informs that
 * content has been written in the FIFO)
 */
void processNfqHandle(struct nfq_handle *h, struct nfq_q_handle *qh);

int getSizeToRead(byte bBufSize[3]);


