set title "Newthyroid con un 10% de restricciones"
set xlabel "Número de generación"
set ylabel "Valor de la función objetivo"
set ytics 100
set grid

semillas = "123452244 9398429 12321 213566 3939021"

set style line 1 lc rgb "#00ffff" lt 1 lw 1.7
set style line 2 lc rgb "#99cc00" lt 1 lw 1.7
set style line 3 lc rgb "#000000" lt 1 lw 1.7

set style line 4 lc rgb "#ff9933" lt 1 lw 1.7
set style line 5 lc rgb "#996600" lt 1 lw 1.7

set style line 6 lc rgb "#0066ff" lt 1 lw 1.7
set style line 7 lc rgb "#ff0000" lt 1 lw 1.7

set style line 8 lc rgb "#660033" lt 1 lw 1.7
set style line 9 lc rgb "#000099" lt 1 lw 1.7
set style line 10 lc rgb "#cc00cc" lt 1 lw 1.7
do for[i in semillas] {

	set terminal png size 1280,960
	set output "salidas_png/ecoli_set_const_10.const_".i


	plot "datos/newthyroid_set_const_10.const_".i."_AM-1_UN.out" with lines title "AM-1 semilla ".i,\
	"datos/newthyroid_set_const_10.const_".i."_AM-0_1_UN.out" with lines title "AM-0.1 semilla ".i, \
	"datos/newthyroid_set_const_10.const_".i."_AM-0_1mej_UN.out" with lines title "AM-0.1mej semilla ".i, \
	"datos/newthyroid_set_const_10.const_".i."_AGG_UN.out" with lines title "AGG-UN semilla ".i, \
	"datos/newthyroid_set_const_10.const_".i."_AGG_SF.out" with lines title "AGG-SF semilla ".i, \
	"datos/newthyroid_set_const_10.const_".i."_AGE_UN.out" with lines title "AGE-UN semilla ".i, \
	"datos/newthyroid_set_const_10.const_".i."_AGE_SF.out" with lines title "AGE-SF semilla ".i, \
	"datos/newthyroid_set_const_10.const_".i."_BL.out" with lines title "BL semilla ".i, \
	"datos/newthyroid_set_const_10.const_".i."_AM-BL-1_UN.out" with lines title "AM-BL-1 semilla ".i,\
	"datos/newthyroid_set_const_10.const_".i."_AM-BL-0_1_UN.out" with lines title "AM-BL-0.1 semilla ".i, \
	"datos/newthyroid_set_const_10.const_".i."_AM-BL-0_1mej_UN.out" with lines title "AM-BL-0.1mej semilla ".i

}
