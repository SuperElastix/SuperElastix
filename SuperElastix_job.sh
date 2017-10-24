#!/bin/bash 
#$ -S /bin/bash 
#$ -q all.q 
#$ -N SuperElastix_job
#$ -pe BWA 1
#$ -l h_vmem=1g
#$ -cwd
#$ -j Y
#$ -V
#$ -m be
#$ -M N.Dekker@lumc.nl 

echo Start time : `date` 
echo 'Starting job...' 
sleep 10 
echo '10 seconds, end of script.' 
echo End time : `date` 
