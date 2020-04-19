set title "Iris con un 10% de restricciones"
set xlabel "Número de generación"
set ylabel "Valor de la función objetivo"
set grid

semillas = "123452244 9398429 12321 213566 3939021"


do for[i in semillas] {

	plot "datos/iris_set_const_10.const_".i."_AM-1_UN.out" with lines title "AM-1 semilla ".i,\
	"datos/iris_set_const_10.const_".i."_AM-0_1_UN.out" with lines title "AM-0.1 semilla ".i, \
	"datos/iris_set_const_10.const_".i."_AM-0_1mej_UN.out" with lines title "AM-0.1mej semilla ".i, \
	"datos/iris_set_const_10.const_".i."_AGG_UN.out" with lines title "AGG-UN semilla ".i, \
	"datos/iris_set_const_10.const_".i."_AGG_SF.out" with lines title "AGG-SF semilla ".i, \
	"datos/iris_set_const_10.const_".i."_AGE_UN.out" with lines title "AGE-UN semilla ".i, \
	"datos/iris_set_const_10.const_".i."_AGE_SF.out" with lines title "AGE-SF semilla ".i

	pause mouse any
}
