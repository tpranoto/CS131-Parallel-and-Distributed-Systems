#!/bin/bash
#
#$ -cwd
#$ -j y
#$ -S /bin/bash
#$ -pe openmpi 8
#$ -o output_image5.out
#
#
# Use modules to setup the runtime environment
#
module load sge
module load openmpi
#
# Execute the run
#
mpirun -np $NSLOTS ./lab2A aniketsh_tc5.pgm output_image5.pgm
