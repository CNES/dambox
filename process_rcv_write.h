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

extern std::mutex mtxDataInFifo;
extern std::condition_variable cvDataInFifo;

extern std::mutex mtxStartTransmission;
extern std::condition_variable cvStartTransmission;
extern sem_t semStart;

extern std::mutex mtxDamOn;
extern std::condition_variable cvDamOn;

extern int iFdSocket;
extern int iWriteFdPkt;
extern int iWriteFdSize;
extern int iCompteurPktIn;

/*
 * This function listen the socket. When a packed is received
 * it writes the content and the size of the packet in the FIFO_PKT
 * and FIFO_SIZE
 *
 * @return 1 is success, 0 else
 */
int addPktToFifo();

/*
 * Thread which call addPktToifo. When the dam state is ON, it send a signal
 * to process_nfq_handle every time a packet is received and added to the FIFO
 */
void processNfqRecv();

void addSizeToBuffer(byte bBufSize[3], int iSize);

