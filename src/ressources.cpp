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

int iReadFdPkt=0;
int iWriteFdPkt=0;
int iReadFdSize=0;
int iWriteFdSize=0;
int iFdSocket=0;
int iCompteurPktIn=0;
int iCompteurPktOut=0;

std::mutex mtxDamOn;
std::condition_variable cvDamOn;

std::mutex mtxDataInFifo;
std::condition_variable cvDataInFifo;

std::mutex mtxStartTransmission;
std::condition_variable cvStartTransmission;
sem_t semStart;

std::mutex mtxBh;
int iDamState=0;

void setDamState(int iB)
{
	mtxBh.lock();
	if(iDamState != iB)
	{
		iDamState = iB;
	}
    mtxBh.unlock();
}

int getDamState(void)
{
	int ireturn = 0;
	mtxBh.lock();
	ireturn = iDamState;
	mtxBh.unlock();
	return ireturn;
}

std::mutex mtxFifo;
int iBoolSignalPktAdded=0;

void setSignalPktAdded(int iB){

    mtxFifo.lock();
    if(iBoolSignalPktAdded!=iB)
    {
        iBoolSignalPktAdded=iB;
    }
	mtxFifo.unlock();
}

int isSignalPktAddedON(void){

	int ireturn = 0;
    mtxFifo.lock();
	ireturn=iBoolSignalPktAdded;
	mtxFifo.unlock();
	return ireturn;
}

std::mutex mtxEnd;
int iBoolEnd=0;

void setEnd(int iB){
    mtxEnd.lock();
    if(iBoolEnd!=iB)
    {
       iBoolEnd=iB;
    }
    mtxEnd.unlock();
}

int isEnd(){
    int ireturn = 0;
    mtxEnd.lock();
    ireturn=iBoolEnd;
    mtxEnd.unlock();
    return ireturn;
}

std::mutex mtxSizeFifo;
long int iSizeFifoOctet=0;
int iSizeFifoPkt=0;

long int getSizeFifoOctet(){
    long int ireturn = 0;
    mtxSizeFifo.lock();
    ireturn = iSizeFifoOctet;
    mtxSizeFifo.unlock();
    return ireturn;
}

int getSizeFifoPkt(){
    int ireturn = 0;
    mtxSizeFifo.lock();
    ireturn = iSizeFifoPkt;
    mtxSizeFifo.unlock();
    return ireturn;
}

void addPacket(int taille){
    mtxSizeFifo.lock();
    iSizeFifoOctet = iSizeFifoOctet + taille;
    iSizeFifoPkt++;
    mtxSizeFifo.unlock();
}

void removePackets(int nbr_data, int nbr_pkt){
    mtxSizeFifo.lock();
    iSizeFifoOctet =  iSizeFifoOctet - nbr_data;
    iSizeFifoPkt = iSizeFifoPkt - nbr_pkt;
    mtxSizeFifo.unlock();

}

int iPacketId=0;
std::mutex mtxPacketId;

void setId(int iB){
    mtxPacketId.lock();
    iPacketId=iB;
    mtxPacketId.unlock();
}

int getId(){
    int ireturn = 0;
    mtxPacketId.lock();
    ireturn=iPacketId;
    mtxPacketId.unlock();
    return ireturn;
}


int iDuration=0;

void setDuration(int sec){
    iDuration=sec;
}

int getDuration(){
    return iDuration;
}

int iFrequency=1;

void setFrequency(int freq){
        iFrequency=freq;
}

int getFrequency(){
    return iFrequency;
}

std::string sTimelineFile;
void setTimelineFile(std::string timeline_config){
    sTimelineFile=timeline_config;
}

std::string getTimelineFile(){

    return sTimelineFile;
}

std::chrono::nanoseconds tDamSlot;

void setDamSlot(std::chrono::nanoseconds duration){
    tDamSlot=duration;
}

std::chrono::nanoseconds getDamSlot(){
    return tDamSlot;
}

std::vector<int> vTimeline;

void push_back_timeline(int i){
    vTimeline.push_back(i);
}

std::vector<int> getTimeline(){
    return vTimeline;
}

int iMaxSimultaneousVerdicts=1;

void setMaxSimultaneousVerdicts(int nbr){
    iMaxSimultaneousVerdicts=nbr;
}

int getMaxSimultaneousVerdicts(){
    return iMaxSimultaneousVerdicts;
}

int iBoolDebugMode=0;


void debugModeON(){
    iBoolDebugMode=1;
}

int isDebugMode(){
    return iBoolDebugMode;
}

int iBoolDurationFixed=0;

void DurationFixedON(){
    iBoolDurationFixed=1;
}

int isDurationFixed(){
    return iBoolDurationFixed;
}
