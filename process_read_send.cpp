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

#include "process_read_send.h"

int getSizeToRead(byte bBufSize[3]){

    return ((int) bBufSize[0]) + ((int) bBufSize[1])*256 + ((int) bBufSize[2])*65536;
}

void sendPackets(struct nfq_handle *h, struct nfq_q_handle *qh, int maxVerdict){

	int iPktToSend=0;
	int iSizePktToRead=0;
	int iCptrDataSent=0;
	char cBufPkt[4096] __attribute__ ((aligned));
	byte bBufSize[3] __attribute__ ((aligned));

    //Until the FIFO is not empty and beam is ON, read the FIFO and send packets
	while((getSizeFifoPkt() !=0) && (getBeamState()) && (!isEnd())){

            //Number of packet that will be send simultaneously
            int iIteration=std::min(getSizeFifoPkt(),maxVerdict);
            iCptrDataSent=0;

            for(int i=0; i<iIteration;i++){

                //Read the size of the next packet to read
                iSizePktToRead = read(iReadFdSize, &bBufSize, 3);

                if (iSizePktToRead>0){


                    //Read the data of the packet and store ti in cBufPkt
                    int iSizeToRead = getSizeToRead(bBufSize);
                    iPktToSend = read(iReadFdPkt, &cBufPkt, iSizeToRead);

                    //Call the callback function
                    if (iPktToSend>0){
                        iCptrDataSent+=iSizeToRead;
                        nfq_handle_packet(h, cBufPkt, iPktToSend);
                    }
                }
            }

            //Send all the packets that have been reading previously
            nfq_set_verdict_batch(qh, getId(), NF_ACCEPT);
            iCompteurPktOut+=iIteration;
            removePackets(iCptrDataSent, iIteration);
	}
}


void processNfqHandle (struct nfq_handle *h, struct nfq_q_handle *qh)
{
    int maxVerdict = getMaxSimultaneousVerdicts();

	while(!isEnd())
	{
	    //Wait signal that inform that the beam in ON
	    std::unique_lock<std::mutex> lockBeamOn(mtxBeamOn);
	    cvBeamOn.wait(lockBeamOn);
	    lockBeamOn.unlock();

        //Call sendPackets funtion
		sendPackets(h,qh,maxVerdict);

        //As long as the beam is ON
		while((getBeamState()) && (!isEnd()))
		{
		    //Wait signal that inform that a packet have been stored in the FIFO

		    std::unique_lock<std::mutex> lockDataInFifo(mtxDataInFifo);
            cvDataInFifo.wait(lockDataInFifo);
			lockDataInFifo.unlock();

			//If beam is ON
			if(getBeamState()){
                sendPackets(h,qh,maxVerdict);
			}
		}

        //Beam is ON, signal exchange is no longer useful
		setSignalPktAdded(SIGNAL_PKT_OFF);
	}

	std::cout << "[INFO] Thread process_read_send closed" << std::endl;
}
