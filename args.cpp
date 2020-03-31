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

#include "args.h"

std::string getHelpEn(void){

    std::stringstream ssHelp;

    ssHelp << "Usage: ./dam_box [-bs bhs -f freq |--timeline file] [options]" << std::endl;
    ssHelp << "       ./dam_box [-h|--help] [-v|--version]" << std::endl << std::endl;
    ssHelp << "BH Box options :" << std::endl << std::endl;
    ssHelp << "Available information :" << std::endl << std::endl;
    ssHelp << "-h, --help                       Print this message and quit" << std::endl;
    ssHelp << "-v, --version                    Print version information and quit" << std::endl << std::endl;
    ssHelp << "Beam-Hopping parameters (REQUIRED) : " << std::endl << std::endl;
    ssHelp << "-bs, --beamslot     #bhs[us]     Configuration of the beam-hopping slot duration in us (microsecond)" << std::endl;
    ssHelp << "-f, --frequency     #freq        Configuration of the freq corresponding to the beam illumination frequency (If freq = 4, the beam is activated 1 time-slot out of 4)" << std::endl;
    ssHelp << "-t, --timeline      #file_path   Use of a configuration file to define a custom timeline (BPH) of DamBOX" << std::endl;
    ssHelp << "                                     * Replace the usage of -f, the file must only containt 0 and 1 on his first line" << std::endl;
    ssHelp << "Execution options (OPTIONAL) :" << std::endl << std::endl;
    ssHelp << "-d, --duration      #d[sec]      Determination of a program execution duration" << std::endl;
    ssHelp << "                                     /!\\ Without this parameter, the binary runs continuously until a system interruption (ctrl+c, ctrl+z, ctrl+\\)" << std::endl;
    ssHelp << "-s, --simultaneous_verdict #nbr  Determination of the maximum number of packets that can be sent out the DamBOX simultaneously" << std::endl;
    ssHelp << "--debug                          Activation of the saving of the evolution of timeline and fifo filling over time" << std::endl;
    ssHelp << "                                     * Generating files profil_fifo.txt and profil_bh.txt at the end of the execution" << std::endl;

    return ssHelp.str();

}

std::string getErrorParameters(void){

    std::stringstream ssError;

    ssError << "dam_box: invalid options" << std::endl;
    ssError << "Try ./dam_box --help for more informations" << std::endl;

    return ssError.str();
}

std::string getVersion(void){

    std::stringstream ssVersion;

    ssVersion << "Version 1.2" << std::endl;

    return ssVersion.str();
}

std::string getErrorNoBHParameters(void){

    std::stringstream ssError;

    ssError << "dam_box: Missing beam-hopping parameters settings " << std::endl;
    ssError << "Usage: ./dam_box [-f|-m] [options]" << std::endl;
    ssError << "Try ./dam_box --help for more informations" << std::endl;

    return ssError.str();
}

std::string getRunningMessage(int i){

    std::stringstream ssRun;
    if(i==1){
             ssRun << "Timeline = " << getTimelineFile() << std::endl;
    }else{
            ssRun << "Illumination Frequency = " << getFrequency() << std::endl;
    }
    ssRun << "Beam-Hopping Slot =  " << getBeamSlot().count() << " ns" << std::endl;
    ssRun << "Running beam-hopping ..." << std::endl;

    return ssRun.str();
}

void processManualTimeline(int freq){

        push_back_timeline(1);
		for(int i=1; i<freq; i++){
            push_back_timeline(0);
		}

}

void processFileTimeline(std::string config){

//Read every character of the string and push the timeline
for(std::string::size_type i=0;i<config.size();++i){
    if (config[i]=='1'){
        push_back_timeline(1);
    }else if (config[i]=='0'){
        push_back_timeline(0);
    }
}

}

void checkTimelineFile(std::string config){

    for(std::string::size_type i=0;i<config.size();++i){
        //Check the timeline config for only 0 and 1
        if (config[i]=='1'|| config[i]=='0')
        {
        }else{
            std::cout << "[ERROR] The timeline file is not correct, only interger 0 and 1 are accepted. See help or man for more details"<< std::endl;
            exit(1);
        }
    }
}

std::string getFileTimeline(std::string path){

    std::string line;
    std::ifstream timeline_file (path);
    if (timeline_file.is_open())
    {
        getline (timeline_file,line);
        timeline_file.close();
        //Check the first line of the file
        if(line==""){
            std::cout << "[ERROR] No timeline in the first line of the file " << path <<std::endl;
            exit(1);
        }
        checkTimelineFile(line);
    }
    else{
        std::cout << "[ERROR] Cannot open the file "<< path <<std::endl;
        exit(1);
    }

    return line;
}

void processArguments(int ac, char **av){

    int iParameterIndex=1;
    int iBoolBsParameterDefined=0;
    int iBoolFrequencyParameterDefined=0;
    int iBoolFileParameterDefined=0;
    int iBoolSimultaneousVerdictDefined=0;
    int iBoolDurationDefined=0;
    int iBoolDebugParameterDefined=0;

    while(iParameterIndex<ac){

        if((strcmp(av[iParameterIndex],"-h")==0 || strcmp(av[iParameterIndex], "--help")==0)){
            std::cout << getHelpEn();
            exit(1);
        }
        else if((strcmp(av[iParameterIndex],"-v")==0 || strcmp(av[iParameterIndex], "--version")==0)){
            std::cout << getVersion();
            exit(1);
        }
        else if(((strcmp(av[iParameterIndex],"-f")==0) || (strcmp(av[iParameterIndex],"--frequency")==0)) && (iBoolFrequencyParameterDefined==0) && (iBoolFileParameterDefined==0) && ((iParameterIndex+1)<ac) && (atoi(av[iParameterIndex+1])!=0)){
            if((atoi(av[iParameterIndex+1]) <= 0)){
                std::cout << "[ERROR] Frequency must be an integer strictly higher than 0" << std::endl;
                exit(1);
            }
            setFrequency(atoi(av[iParameterIndex+1]));
            processManualTimeline(getFrequency());
            iParameterIndex=iParameterIndex+2;
            iBoolFrequencyParameterDefined=1;
        }
        else if(((strcmp(av[iParameterIndex],"-bs")==0) || (strcmp(av[iParameterIndex],"--beamslot")==0)) && ((iParameterIndex+1<ac) && (iBoolBsParameterDefined==0)&& (atoi(av[iParameterIndex+1])!=0))){
            if((atoi(av[iParameterIndex+1]) <= 500)){
                std::cout << "[WARNING] It is not recommended to use beamslot less than 500us. The duration of some beamslot may be incorrect" << std::endl;
            }
            setBeamSlot(std::chrono::nanoseconds ((atoi(av[iParameterIndex+1]))*1000));
            iBoolBsParameterDefined=1;
            iParameterIndex=iParameterIndex+2;
        }
        else if(((strcmp(av[iParameterIndex],"-d")==0) || (strcmp(av[iParameterIndex],"--duration")==0)) && ((iParameterIndex+1)<ac)&& (iBoolDurationDefined==0) && (atoi(av[iParameterIndex+1])!=0)){
            if((atoi(av[iParameterIndex+1]) <= 0)){
                std::cout << "[ERROR] Duration must be an integer strictly higher than 0" << std::endl;
                exit(1);
            }
            DurationFixedON();
            setDuration(atoi(av[iParameterIndex+1]));
            iBoolDurationDefined=1;
            iParameterIndex=iParameterIndex+2;
        }
        else if(((strcmp(av[iParameterIndex],"-s")==0) || (strcmp(av[iParameterIndex],"--simultaneous_verdict")==0)) && ((iParameterIndex+1)<ac) && (iBoolSimultaneousVerdictDefined==0)){
            if((atoi(av[iParameterIndex+1]) <= 0)){
                std::cout << "[ERROR] The maximum number of simultaneous verdict must be an integer strictly higher than 0" << std::endl;
                exit(1);
            }
            setMaxSimultaneousVerdicts(atoi(av[iParameterIndex+1]));
            iBoolSimultaneousVerdictDefined=1;
            iParameterIndex=iParameterIndex+2;
        }
        else if(((strcmp(av[iParameterIndex],"-t")==0) || (strcmp(av[iParameterIndex],"--timeline")==0))  && ((iParameterIndex+1)<ac) && (iBoolFileParameterDefined==0) && (iBoolFrequencyParameterDefined==0)){
            setTimelineFile(getFileTimeline(av[iParameterIndex+1]));
            processFileTimeline(getTimelineFile());
            iBoolFileParameterDefined=1;
            iParameterIndex=iParameterIndex+2;
        }
        else if((strcmp(av[iParameterIndex],"--debug")==0) && (iBoolDebugParameterDefined==0)){
            debugModeON();
            iBoolDebugParameterDefined=1;
            iParameterIndex=iParameterIndex+1;
        }
        else{
            std::cout << getErrorParameters();
            exit(1);
        }

    }

    if (((iBoolBsParameterDefined==1) && (iBoolFrequencyParameterDefined==1)) || ((iBoolFileParameterDefined==1) && (iBoolBsParameterDefined==1))){
        std::cout << getRunningMessage(iBoolFileParameterDefined);
    }else{
        std::cout << getErrorNoBHParameters();
        exit(1);
    }

}
