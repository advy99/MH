set xlabel "Número de evaluación"
set ylabel "Valor de la función objetivo"
set ytics 500
set xrange [0:100000]
set grid
set termoption dashed

semillas = "123452244 9398429 12321 213566 3939021"

set style line 1 lc rgb "#00ffff" lt 1 lw 1.7 dashtype 2
set style line 2 lc rgb "#99cc00" lt 2 lw 1.7 dashtype '--.'
set style line 3 lc rgb "#000000" lt 3 lw 1.7 dashtype 4
set style line 4 lc rgb "#ff9933" lt 4 lw 1.7 dashtype 5
set style line 5 lc rgb "#996600" lt 5 lw 1.7 dashtype 1
set style line 6 lc rgb "#0066ff" lt 1 lw 1.7 dashtype '-.-'


set style line 7 lc rgb "#00ffff" lt 1 lw 1.7 dashtype 2
set style line 8 lc rgb "#99cc00" lt 2 lw 1.7 dashtype '--.'
set style line 9 lc rgb "#000000" lt 3 lw 1.7 dashtype 4
set style line 10 lc rgb "#ff9933" lt 4 lw 1.7 dashtype 5
set style line 11 lc rgb "#996600" lt 5 lw 1.7 dashtype 1
set style line 12 lc rgb "#0066ff" lt 1 lw 1.7 dashtype '-.-'


do for[i in semillas] {

	set title "Newthyroid con un 20% de restricciones con la semilla ".i
	set term pngcairo dashed size 1400,1050
	set output "salidas_png/newthyroid_set_const_20_".i."_es.png"


	plot "datos/newthyroid_set_const_20.const_".i."_BO-MOD_fobjetivo.out" with lines title "Boltzmann modificado (Valor F. objetivo) " ls 1,\
	"datos/newthyroid_set_const_20.const_".i."_BO_fobjetivo.out" with lines title "Boltzmann (Valor F. objetivo) " ls 2 , \
	"datos/newthyroid_set_const_20.const_".i."_CA-MOD_fobjetivo.out" with lines title "Cauchy modificado (Valor F. objetivo) " ls 3, \
	"datos/newthyroid_set_const_20.const_".i."_CA_fobjetivo.out" with lines title "Cauchy (Valor F. objetivo) " ls 4, \
	"datos/newthyroid_set_const_20.const_".i."_PRO_fobjetivo.out" with lines title "Proporcional (Valor F. objetivo) " ls 5, \
	"datos/newthyroid_set_const_20.const_".i."_CONST_fobjetivo.out" with lines title "Constante (Valor F. objetivo) " ls 6

}
