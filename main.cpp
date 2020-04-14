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

#include "ordonnanceur.h"
#include "ressources.h"
#include "process_rcv_write.h"
#include "process_read_send.h"
#include "nfq.h"
#include "args.h"

void stopExecution (int sig){
    setEnd(END);
    if (isDebugMode()){
            std::unique_lock<std::mutex> lockStart(mtxStartTransmission);
            cvStartTransmission.notify_one();
            lockStart.unlock();
    }
}

int main (int ac, char **av)
{
    processArguments(ac, av);

	struct nfq_handle *h;
	struct nfq_q_handle *qh;

	//Initialize semaphore start
	sem_init(&semStart, 0, 0);

	//Link ctrl+z ctrl+c ctrl+\ to interruption of the program
	signal(SIGQUIT, stopExecution);
	signal(SIGINT, stopExecution);
	signal(SIGTSTP, stopExecution);

	//Initialize NFQUEUE
	if ( (iFdSocket = initNfq(&h, &qh)) < 0){
		std::cerr << "[ERROR] The initialization of NFQ has failed" << std::endl;
		exit (1);
	}

	//Initialize FIFO to store packets data
	char cPipePkt[] = "/tmp/pipe_pkt";
	if ((mkfifo(cPipePkt, 0777) == -1) && (errno != EEXIST)){
		std::cerr << "[ERROR] Creation of FIFO pipe has failed" << std::endl;
	}
	iReadFdPkt = open(cPipePkt, O_RDONLY|O_NONBLOCK);
	iWriteFdPkt = open(cPipePkt, O_WRONLY|O_NONBLOCK);
	if ((fcntl(iWriteFdPkt, 1024+7 ,8172*8172) < 0)){
		std::cerr << "[WARNING] Impossible to set the size of FIFO to 66781584 bytes" << std::endl;
	}

	//Initialize FIFO to store packets size
	char cPipeSize[] = "/tmp/pipe_size";
	if ((mkfifo(cPipeSize, 0777) == -1) && (errno != EEXIST)) {
		std::cerr << "[ERROR] Creation of FIFO pipe has failed" << std::endl;
	}
	iReadFdSize = open(cPipeSize, O_RDONLY|O_NONBLOCK);
	iWriteFdSize = open(cPipeSize, O_WRONLY|O_NONBLOCK);
	if ((fcntl(iWriteFdSize, 1024+7 ,8172*8172) < 0)){
		std::cerr << "[WARNING] Impossible to set the size of FIFO to 66781584 bytes" << std::endl;
	}

	//Initialize timeout using in the recv() function
	struct timeval tRecvTimeout;
    tRecvTimeout.tv_sec=1;
    tRecvTimeout.tv_usec=0;
    setsockopt(iFdSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tRecvTimeout, sizeof tRecvTimeout);

    //Run the 3 threads
    std::thread thrOrdonnanceur(timeLineManagement);
    std::thread thrRecvWrite(processNfqRecv);
    std::thread thrReadSend(processNfqHandle, h, qh);

    //Wait until the end of the execution of the 3 threads
    thrOrdonnanceur.join();
    thrRecvWrite.join();
    thrReadSend.join();

	//Close NFQUEUE
	if (closeNfq(&h, &qh) < 0){
        std::cerr << "[ERROR] The closing of NFQ has failed" << std::endl;
		exit (1);
	}

    //If mode debug is activated, show the number of packets received by the BHBox
	if(isDebugMode()){
	    std::cout << "Number of packets received by the software = " << iCompteurPktIn << std::endl;
        std::cout << "Number of packets sent by the software = " << iCompteurPktOut << std::endl;
	}

	std::cout << "[INFO] DamBOX stopped" << std::endl;

	return 0;
}

