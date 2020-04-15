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

#ifndef RESSOURCES_H_INCLUDED
#define RESSOURCES_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <netinet/in.h>
#include <linux/netfilter.h>
#include <libnetfilter_queue/libnetfilter_queue.h>
#include <libnfnetlink/linux_nfnetlink.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <list>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <iostream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <atomic>
#include <iostream>
#include <fstream>
#include <string>

#define DAM_ON 1
#define DAM_OFF 0
#define END 1
#define SIGNAL_PKT_ON 1
#define SIGNAL_PKT_OFF 0

typedef unsigned char byte;

/*
 * Set the state of the dam to iB
 */
void setDamState(int iB);

/*
 * Get the state of the dam slot {0,1}
 */
int getDamState(void);

/*
 * Activation or deactivation of the signal communication between
 * processNfqRecv and processNfqHandle
 */
void setSignalPktAdded(int iB);

/*
 * Get the state of the signal communication between
 * processNfqRecv and processNfqHandle
 * 0 = No signal communication
 * 1 = Signal communication
 */
int isSignalPktAddedON(void);

/*
 * Set the value of the integer iBoolEnd when the
 * time of the simulation has been exceeded
 */
void setEnd(int iB);

/*
 * Get the integer iBoolEnd to know if the
 * time of the simulation has been exceeded
 */
int isEnd(void);

/*
 * Get the actual filling rate of the FIFO in bytes
 */
long int getSizeFifoOctet();

/*
 * Get the actual filling rate of the FIFO in packets
 */
int getSizeFifoPkt();

/*
 * Increase the filling rate of the FIFO by a size of the packet added
 */
void addPacket(int taille);

/*
 * Decrease the filling rate of the FIFO by a size of the packet removed
 */
void removePackets(int nbr_data, int nbr_pkt);

/*
 * Activation of the debug mode
 */
void debugModeON();

/*
 * To know if the debug mode activated or not
 */
int isDebugMode();

/*
 * Set the ID of the last packet that the verdict should be applied to
 */
void setId(int iB);

/*
 * Get the ID of the last packet that the verdict should be applied to
 */
int getId();

/*
 * Set the duration in seconds of the execution when the debug mode is activated
 */
void setDuration(int sec);

/*
 * Get the duration in seconds of the execution when the debug mode is activated
 */
int getDuration();

/*
 * Set the dam activation frequency
 */
void setFrequency(int freq);

/*
 * Get the dam activation frequency
 */
int getFrequency();

/*
 * Set the duration in nanoseconds of a dam slot
 */
void setDamSlot(std::chrono::nanoseconds duration);

/*
 * Set the duration in nanoseconds of a dam slot
 */
std::chrono::nanoseconds getDamSlot();

/*
 * Add a value [0,1] to the timeline. Push the value add the end of the vector
 */
void push_back_timeline(int i);

/*
 * Get the timeline vector
 */
std::vector<int> getTimeline();

/*
 * Set the maximum number of packets that can be sent out the BH box simultaneously
 */
void setMaxSimultaneousVerdicts(int nbr);

/*
 * Set the name of the file which define the timeline parameter
 */
void setTimelineFile(std::string timeline_config);

/*
 * Get the name of the file which define the timeline parameter
 */
std::string getTimelineFile();


/*
 * Get the value of the maximum number of packets that can be sent out the BH box simultaneously
 */
int getMaxSimultaneousVerdicts();

/*
 * Activate the mode with a fixed execution time
 */
void DurationFixedON();

/*
 * To know if the mode with a fixed execution time is activated
 */
int isDurationFixed();

#endif // RESSOURCES_H_INCLUDED
