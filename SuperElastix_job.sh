#!/bin/bash 
#$ -S /bin/bash 
#$ -q all.q 
#$ -N my_first_job
#$ -cwd
#$ -j Y
#$ -V
#$ -m be
#$ -M email@address.lumc 

echo Start time : `date` 
echo 'Starting job...' 
sleep 10 
echo '10 seconds, end of script.' 
echo End time : `date` 
