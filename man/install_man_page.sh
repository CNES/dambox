#!/bin/bash

#Verification de l'existance du dossier  man perso local
if [ -d "/usr/local/man/man1" ]; then
sudo rm /usr/local/man/man1/run_bh*
sudo install -g 0 -o 0 -m 0644 run_bh.1 /usr/local/man/man1/
sudo gzip /usr/local/man/man1/run_bh.1

else
sudo mkdir /usr/local/man/man1
sudo install -g 0 -o 0 -m 0644 run_bh.1 /usr/local/man/man1/
sudo gzip /usr/local/man/man1/run_bh.1
fi

