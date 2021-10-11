# DamBOX

DamBOX is a software developed in C++ that intermittently releases the packet on the interface, like a dam : using netfilter, incoming packets are intercepted and blocked until released at the interface goodput. The user specifies the duration and frequency of the blocking and releasing function.

DamBOX can be used to emulate beam hopping in satellite telecommunication system.

- [Installation](#installation)
  - [Package installation](#package-installation)
  - [Manual installation](#manual-installation)
  - [Generate a debian package](#generate-a-debian-package)
- [Exploitation](#exploitation)
  - [Iptables configuration](#iptables-configuration)
  - [Execution](#execution)
  - [Exploitation with OpenSAND](#exploitation-with-opensand)
  - [Orchestration with OpenBACH](#orchestration-with-openbach)
- [Get Involved](#get-involved)
  - [Partners](#partners)
  - [Licence](#licence)

## Design document

TODO 

### Specific vocabularies

In order to understand all the documentation of the DamBOX, the following vocabulary is important:

* **DS** (Dam Slot): the smallest unit of time during which a beam can be considered as switched on.
* **TimeLine**: the lighting profile oef the recipient. It is composed of 1 and 0. State 1 corresponds to a beam illuminated by the satellite. During state 0, the receiver is not illuminated. The TimeLine will be repeated indefinitely until the end of the program execution.

## Installation

### Package installation

<details><summary><b>Show package installation</b></summary>

Add the OpenBACH repository :

Update sources.list with net4sat openbach repository
> echo "deb http://packages.net4sat.org/openbach xenial stable" | sudo tee /etc/apt/sources.list.d/dambox.list 

Add Net4Sat repository GPG key
> wget -O - http://packages.net4sat.org/support_net4sat_org_pub.gpg.key |sudo apt-key add -

Update the list of available packages:
>  sudo apt-get update 

Install Dambox
> sudo apt-get install dambox 

</details>

### Manual installation

<details><summary><b>Show manual installation</b></summary>

The rest of this README considers a package installation. 
If you want to proceed a manual installation, please replace
> dambox 

by
> ./dambox 

in the provided commands lines. Or simply edit the $PATH variable. 

It is compulsory to install the *netfilter* library on the computer where the DamBOX will be deployed. To do this, simply execute the following command: 

> sudo apt-get install libnetfilter-queue-dev

The maximum size of the FIFO system must be set to a high value. This size is located in the /proc/sys/fs folder and can be set using this command: 

> sudo sysctl fs.pipe-max-size=66781584

The source code of the software can be downloaded through the git repository.

To compile the source code, you must do so with at least C++11 version and including the following libraries for the compilation:

* **lpthread**
* **lnfnetlink**
* **lnetfilter_queue**

On Ubuntu 16.04, here are the requirements :

> sudo apt install make g++ libnetfilter-queue-dev

A makefile is available. Run the following command in the root folder to obtain the binary dambox:

> make

</details>

### Generate a debian package

<details><summary><b>Show debian package generation </b></summary>

You can generate a debian package (Ubuntu 16.04 only) with the following procedure.
Install the following dependences

> sudo apt-get install build-essential fakeroot devscripts libnetfilter-queue-dev

Then by issuing the following command in the root folder

> dpkg-buildpackage -us -uc

The package will be generated in the root folder

You can install it by running: 

> sudo dpkg -i ../dambox_1.0_amd64.deb

You can remove the unnecessary files as follows:

> rm ../dambox_1.0*

</details>

## Exploitation


### Iptables configuration

**Do not ignore this necessary step**

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

### Execution

<details><summary><b>Show DamBOX execution</b></summary>

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

</details>

### Exploitation with OpenSAND


<details><summary><b>Show OpenSAND exploitation</b></summary>

TODO

</details>

### Orchestration with OpenBACH


<details><summary><b>Show OpenBACH orchestration</b></summary>

TODO

</details>

## Get Involved

DamBox is part of OpenBACH project.
OpenBACH is funded and promoted by CNES (French Space Center) as a reference open-source software tool within its research and development studies and activities in the domain of satellite communication systems and networks.

### Partners

Silicom, CNES

### Licence

Copyright © 2020 CNES

OpenBACH is released under GPLv3 (see [COPYING](COPYING.txt) file).


