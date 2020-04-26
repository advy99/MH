set title "Iris con un 20% de restricciones"
set xlabel "Número de generación (evaluación en caso de BL)"
set ylabel "Valor de la función objetivo"
set grid

semillas = "123452244 9398429 12321 213566 3939021"

set style line 1 lc rgb "blue" lt 1 lw 1.5
set style line 2 lc rgb "#00FFFF" lt 1 lw 1.5
set style line 3 lc rgb "#7FFFD4" lt 1 lw 1.5
set style line 4 lc rgb "#D2691E" lt 1 lw 1.5
set style line 5 lc rgb "#FF7F50" lt 1 lw 1.5
set style line 6 lc rgb "#FF8C00" lt 1 lw 1.5
set style line 7 lc rgb "#B8860B" lt 1 lw 1.5
set style line 8 lc rgb "#6B8E23" lt 1 lw 1.5
set style line 9 lc rgb "#2E8B57" lt 1 lw 1.5
set style line 10 lc rgb "#808000" lt 1 lw 1.5


do for[i in semillas] {

	set terminal png size 1280,960
	set output "salidas_png/ecoli_set_const_10.const_".i


	plot "datos/iris_set_const_20.const_".i."_AM-1_UN.out" with lines title "AM-1 semilla ".i,\
	"datos/iris_set_const_20.const_".i."_AM-0_1_UN.out" with lines title "AM-0.1 semilla ".i, \
	"datos/iris_set_const_20.const_".i."_AM-0_1mej_UN.out" with lines title "AM-0.1mej semilla ".i, \
	"datos/iris_set_const_20.const_".i."_AGG_UN.out" with lines title "AGG-UN semilla ".i, \
	"datos/iris_set_const_20.const_".i."_AGG_SF.out" with lines title "AGG-SF semilla ".i, \
	"datos/iris_set_const_20.const_".i."_AGE_UN.out" with lines title "AGE-UN semilla ".i, \
	"datos/iris_set_const_20.const_".i."_AGE_SF.out" with lines title "AGE-SF semilla ".i, \
	"datos/iris_set_const_20.const_".i."_BL.out" with lines title "BL semilla ".i, \
	"datos/iris_set_const_20.const_".i."_AM-BL-1_UN.out" with lines title "AM-BL-1 semilla ".i,\
	"datos/iris_set_const_20.const_".i."_AM-BL-0_1_UN.out" with lines title "AM-BL-0.1 semilla ".i, \
	"datos/iris_set_const_20.const_".i."_AM-BL-0_1mej_UN.out" with lines title "AM-BL-0.1mej semilla ".i

}
