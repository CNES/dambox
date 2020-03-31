#!/bin/bash

sudo iptables -F

sudo rm Resultat/*.png Resultat/tracer_courbe.conf Resultat/profil_fifo.txt Resultat/profil_bh.txt

#Parametre $1 - Durée d'un timeslot en us
let "time_bhs=$1*1000"

#Parametre $2 - Chemin ou nom de fichier pour une timeline personnalisée

#Parametre $3 - Duree de vie de notre soft beam-hopping à saisir en seconde
let "simu_time=$3"

#Création de la NFQUEUE en sortie de la machine
sudo iptables -I FORWARD -o ens192 -j NFQUEUE

#On lance le beam hopping
sudo nice -20 ./run_bh -bs $time_bhs --timeline $2 --debug -t $simu_time 

sudo iptables -F

#GNUPLOT : Tracer les fonctions (TimeLine, fifo, debit)

echo "set terminal png size 1600,1200" >> Resultat/tracer_courbe.conf

#Tracer l'évolution de la timeLine au cours du temps
echo "set output \"timeline.png\"" >> Resultat/tracer_courbe.conf
echo "set ylabel \"Etat du Beam\"" >> Resultat/tracer_courbe.conf
echo "set yrange[0:1.05]" >> Resultat/tracer_courbe.conf
echo "set xrange[1:1.5]" >> Resultat/tracer_courbe.conf
echo "set xtics 0.01" >> Resultat/tracer_courbe.conf
echo "set xlabel \"Temps en secondes\"" >> Resultat/tracer_courbe.conf
echo "plot \"profil_bh.txt\" using 1:2 with steps title \"Etat du BH \", \"profil_bh.txt\" using 1:2 with points title \"\"" >> Resultat/tracer_courbe.conf

#Tracer l'évolution du remplissage de la FIFO au cours du temps
echo "set output \"fifo.png\"" >> Resultat/tracer_courbe.conf
echo "set nokey" >> Resultat/tracer_courbe.conf
echo "set ylabel \"Remplissage de la fifo en octet\"" >> Resultat/tracer_courbe.conf
echo "set xrange[1:1.5]" >> Resultat/tracer_courbe.conf 
echo "set yrange[*:*]" >> Resultat/tracer_courbe.conf
echo "set xtics 0.01" >> Resultat/tracer_courbe.conf
echo "set xlabel \"Temps en secondes\"" >> Resultat/tracer_courbe.conf
echo "set border linewidth 1.5" >> Resultat/tracer_courbe.conf
echo "set pointsize 1.5" >> Resultat/tracer_courbe.conf
echo "set style line 1 lc rgb '#ff0000' pt 7" >> Resultat/tracer_courbe.conf
echo "plot \"profil_fifo.txt\" using 1:2 with p ls 1" >> Resultat/tracer_courbe.conf

cat Resultat/tracer_courbe.conf | gnuplot
mv timeline.png fifo.png profil_fifo.txt profil_bh.txt Resultat/

#Installation du man (fichier run-bh.1) pour run-bh

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
 
