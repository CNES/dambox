DamBOX
=====

DamBOX is a software developed in C++ that intermittently releases the packet on the interface, like a dam : using netfilter, incoming packets are intercepted and blocked until released at the interface goodput. The user specifies the duration and frequency of the blocking and releasing function.

DamBOX can be used to emulate beam hopping in satellite telecommunication system.

The documentation can be found at https://wiki.net4sat.org/doku.php?id=dambox:index

Specific vocabularies:
----------------------
In order to understand all the documentation of the DamBOX, the following vocabulary is important:

* **DS** (Dam Slot): the smallest unit of time during which a beam can be considered as switched on.
* **TimeLine**: the lighting profile of the recipient. It is composed of 1 and 0. State 1 corresponds to a beam illuminated by the satellite. During state 0, the receiver is not illuminated. The TimeLine will be repeated indefinitely until the end of the program execution.

Installation of the DamBOX:
--------------------------

It is compulsory to install the *netfilter* library on the computer where the DamBOX will be deployed. To do this, simply execute the following command: 

> sudo apt-get install libnetfilter-queue-dev

The maximum size of the FIFO system must be set to a high value. This size is located in the /proc/sys/fs folder and can be set using this command: 

> sudo sysctl fs.pipe-max-size=66781584

The source code of the software can be downloaded from the address below:

> git clone https://forge.net4sat.org/dambox/dambox.git/

To compile the source code, you must do so with at least C++11 version and including the following libraries for the compilation:

* **lpthread**
* **lnfnetlink**
* **lnetfilter_queue**

A makefile is available. Run the following command to obtain the binary dambox:

> make -f DamBOX.mak

Iptables configuration :
------------------------

Filtering rules based on *iptables* are necessary to redirect incoming packets to the DamBOX.

> iptables -I FORWARD -j NFQUEUE

Thanks to this *iptables* rules and *netfilter*, the traffic will be redirected to the DamBOX interface.

It may be interesting to add some parameters to the filtering rule :

* **-p** : Filter only such types of protocols (e.g.: UDP)
* **-o** : Filter only packets intended for an interface
* **-i** : Filter only packets arriving from an interface
* **-d** : Filter only packets destined to an IP address 
* **-s** : Filter only packets sent by an IP address

To delete the filter rules, simply execute the following command:

> sudo iptables -F

Execution of the DamBOX
----------------------

In order to launch the DamBOX, and only after setting up the above filtering, the following command can be executed

> sudo ./dambox -ds $damslot -f $freq (-d $duration --debug) 

The launching of the DamBOX therefore requires at least 2 parameters:

* **damslot** : duration  in us (microsecond) of a timeslot.
* **freq**: Frequency of beam illumination. The beam will be switched on once every freq timeslot (Example: for freq=6, we will have a timeline [100000]).
* If you want to operate the DamBOX for a defined time, you can enter a **duration**. This duration must be entered in second. Otherwise, the executable will run until the user stop the program manually (ctrl+c, ctrl+z, ctrl+\)
* You can also activate the **debug mode** to follow the evolution of the timeline and fifo filling over time

The program exploits these parameters to establish the timeline. Then all the incoming packets are stored on the FIFO. They are only retransmitted to the recipient when the beam is considered switched ON. (See Architecture of DamBOX for more information about the parameters and the design of the program).

At the end of the execution, if the debug mode is activated, the program gives access to two output files:

* **profil_dam.txt** : Evolution of the dam state over time
* **profil_fifo.txt** : Evolution of the FIFO filling over time. The unit is the bytes.

The help for setting the executable can be obtained by using the command:

> ./dambox --help

Get Involved
============
DamBox is part of OpenBACH project
See OpenBACH web site : http://www.openbach.org/ 
Mailing list are available : users@opensand.org

Authors
=======
OpenBACH is funded and promoted by CNES (French Space Center) as a reference open-source software tool within its research and development studies and activities in the domain of satellite communication systems and networks.

Partners
=======
Silicom 


Licence
=======
Copyright © 2020 CNES
OpenBACH is released under GPLv3 (see [COPYING](COPYING.txt) file).


