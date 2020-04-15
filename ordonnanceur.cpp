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
#define LIMIT_PKT 50

void timeLineManagement ()
{
        //Definition of BH parameters (BHS, Frequency)
        std::chrono::nanoseconds tDamSlot=getDamSlot();
        int iDuration=getDuration();

		//Definition of timeLine
		std::vector<int> vTimeline = getTimeline();

		//If duration is set, waiting of the first packet
        if (isDurationFixed()){
            std::unique_lock<std::mutex> lockStart(mtxStartTransmission);
            cvStartTransmission.wait(lockStart);
            lockStart.unlock();
        }

        //Time
		auto tNextTimeSlot = std::chrono::high_resolution_clock::now();

		//TimeLine Index
		int iIndexTimeline=0;
		setDamState(DAM_ON);

		// List to save data over time if debug mode is activated
        std::list<data> lDataStored;

		//Start time
		auto tStartPoint = std::chrono::high_resolution_clock::now();

		//If duration is set, increment the semaphore
		if(isDurationFixed()){
            sem_post(&semStart);
		}

		while (!isEnd()){

            //Time since start
            auto tTimeStamp = std::chrono::duration_cast<std::chrono::microseconds> (std::chrono::high_resolution_clock::now() - tStartPoint);

            //Save data if debug mode activated
            if(isDebugMode()){
                lDataStored.push_back({tTimeStamp.count(), vTimeline[iIndexTimeline], getSizeFifoOctet()});
            }

            //If dam state=0, update the variable
			if(vTimeline[iIndexTimeline]==DAM_OFF){
                int iLastDamState=getDamState();
				setDamState(DAM_OFF);
				if((iLastDamState==DAM_ON) && (getSizeFifoPkt()>=LIMIT_PKT)){ //Limite à discuter
                    std::cout << "[WARNING] Program not enough fast to send back packets. Some packets (" << getSizeFifoPkt() << ") may have been sent during state OFF or next dam-slot" << std::endl;
				}
			}

            //If dam state=1, send signal to process_read_send and update variables
			if(vTimeline[iIndexTimeline]==DAM_ON){
                setDamState(DAM_ON);
                setSignalPktAdded(SIGNAL_PKT_ON);
                std::unique_lock<std::mutex> lock(mtxDamOn);
                cvDamOn.notify_one();
                lock.unlock();
			}

			//Increment index of timeLine
			iIndexTimeline++;
			if ((unsigned int)iIndexTimeline==vTimeline.size()){
				iIndexTimeline=0;
			}

            //If duration time is over, update variable
			if((isDurationFixed()) && (tTimeStamp.count()>iDuration*1e6)){
				setEnd(END);
				setDamState(DAM_OFF);
			}

            //Wait until the next timeslot
            tNextTimeSlot += tDamSlot;
			std::this_thread::sleep_until(tNextTimeSlot);
		}

		//Save all data in files if debug mode is activated
		if(isDebugMode()){

            //File which store the fifo filling over time
            FILE* fFifo = fopen("profil_fifo.txt", "w");
            if (fFifo==NULL){
                std::cerr << "[ERROR] Cannot open file profil_fifo.txt" << std::endl;
            }

            //File which store the dam_state over time
            FILE* fDam = fopen("profil_dam.txt", "w");
            if (fDam==NULL){
                std::cerr << "[ERROR] Cannot open file profil_dam.txt" << std::endl;
            }

            for(std::list<data>::iterator it=lDataStored.begin(); it!=lDataStored.end(); ++it)
            {
                fprintf(fFifo, "%f %li\n", ((*it).iTimeStamp)/1e6,  (*it).iFifoSize);
                fprintf(fDam, "%f %i \n", ((*it).iTimeStamp)/1e6, (*it).iDamState);
            }
            fclose(fFifo);
            fclose(fDam);
		}

		lDataStored.clear();

        std::cout << "[INFO] Thread scheduler closed" << std::endl;
}
