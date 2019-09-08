#!/bin/bash

pasta="ArquivosGerados"

if [ ! -d $pasta ] 
then
    mkdir $pasta
fi

tempo_exec=0
threads=""
gcc pi_2.c -pthread

for ((num_threads=1;$num_threads<=1000;num_threads++)); do
    > $pasta/$num_threads.txt
    /usr/bin/time -v ./a.out $num_threads &> $pasta/$num_threads.txt
    threads=$threads$num_threads$'\n'
done
echo "$threads" > eixo_x.txt


valores=""
tempo=""

for arquivo in `ls $pasta`; do
    valores="$(echo "$tempo" | egrep "[0-9]+:[0-9]+.[0-9]+" $pasta/$arquivo -o | cut -d "." -f2 )"
    tempo=$tempo$valores$'\n'
done

echo "$tempo" > eixo_y.txt