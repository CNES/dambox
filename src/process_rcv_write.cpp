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

#include "process_rcv_write.h"

void addSizeToBuffer(byte bBufSize[3], int iSize){

    bBufSize[0]=(byte)((byte)iSize & 0xFF);
    iSize = iSize>>8;
    bBufSize[1]=(byte)((byte)iSize & 0xFF);
    iSize = iSize>>8;
    bBufSize[2]=(byte)((byte)iSize & 0xFF);
}

int addPktToFifo(){

	int iPktRecv;
	char cBufPkt[4096] __attribute__ ((aligned));
	byte bBufSize[3] __attribute__ ((aligned));

    //Wait for the reception of next packet and store it in cBufPkt
	iPktRecv = recv(iFdSocket, cBufPkt, sizeof(cBufPkt), 0);

	//Inform user of the reception of the first packet by the software
    if((iCompteurPktIn == 0) && (iPktRecv != -1)){

        std::cout << "[INFO] First packet received by the software" << std::endl;
        if(isDurationFixed()){
            std::unique_lock<std::mutex> lockStart(mtxStartTransmission);
            cvStartTransmission.notify_one();
            lockStart.unlock();
            sem_wait(&semStart);
        }
	}

	//Store packet and size of the packet in two pipe FIFO
	if(iPktRecv>0){

            iCompteurPktIn++;

			//Store data of the packet in FIFO_PKT
			if ((write(iWriteFdPkt, (char*)(&cBufPkt), iPktRecv) <= 0)){
                std::cerr << "[WARNING] Maximum FIFO size " << getSizeFifoOctet() << " reached - Packet lost" << std::endl;
			}else{
                addPacket(iPktRecv);
			}

            //Store size of the packet in FIFO_SIZE
			addSizeToBuffer(bBufSize, iPktRecv);
			if ((write(iWriteFdSize, (byte*)(&bBufSize), 3) <= 0)){
                std::cerr << "[WARNING] Maximum FIFO size reached" << std::endl;
			}
			return 1;
		}else{
			return 0;
		}
}

void processNfqRecv ()
{

	while(!isEnd()){

		if(addPktToFifo()){

			//If variable bool_signal=1, we send signal to process_read_send each time a packet have been stored in FIFO
			if(isSignalPktAddedON()){
                std::unique_lock<std::mutex> lockDataInFifo(mtxDataInFifo);
                cvDataInFifo.notify_one();
                lockDataInFifo.unlock();
			}
		}
	}

	std::cout << "[INFO] Thread process_rcv_write closed" << std::endl;

	//Send signal to process_read_send to avoid indefinitely waiting
	std::unique_lock<std::mutex> lockEnd(mtxStartTransmission);
    cvStartTransmission.notify_one();
    lockEnd.unlock();

	std::unique_lock<std::mutex> lockDataInFifo(mtxDataInFifo);
    cvDataInFifo.notify_one();
    lockDataInFifo.unlock();

    std::unique_lock<std::mutex> lockDamOn(mtxDamOn);
    cvDamOn.notify_one();
    lockDamOn.unlock();


}
