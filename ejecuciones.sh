#!/bin/sh


semillas=( 12345224113467 34332110049 456454542 3493875848 982731928361823)

for semilla in ${semillas[*]}
do
	echo "Ejecutando los distintos datasets con la semilla $semilla"

	./bin/practica1 datos/iris_set.dat datos/iris_set_const_10.const 3 $semilla
	./bin/practica1 datos/iris_set.dat datos/iris_set_const_20.const 3 $semilla

	./bin/practica1 datos/rand_set.dat datos/rand_set_const_10.const 3 $semilla
	./bin/practica1 datos/rand_set.dat datos/rand_set_const_20.const 3 $semilla

	./bin/practica1 datos/ecoli_set.dat datos/ecoli_set_const_10.const 8 $semilla
	./bin/practica1 datos/ecoli_set.dat datos/ecoli_set_const_20.const 8 $semilla

done
