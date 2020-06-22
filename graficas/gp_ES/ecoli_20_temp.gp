set xlabel "Número de evaluación"
set ylabel "Valor de la temperatura"
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

	set title "Ecoli con un 20% de restricciones con la semilla ".i
	set term pngcairo dashed size 1400,1050
	set output "salidas_png/ecoli_set_const_20_".i."_temp.png"


	plot "../datos/ecoli_set_const_20.const_".i."_BO-MOD.out" with lines title "Boltzmann modificado " ls 7,\
	"../datos/ecoli_set_const_20.const_".i."_BO.out" with lines title "Boltzmann " ls 8 , \
	"../datos/ecoli_set_const_20.const_".i."_CA-MOD.out" with lines title "Cauchy modificado " ls 9, \
	"../datos/ecoli_set_const_20.const_".i."_CA.out" with lines title "Cauchy " ls 10, \
	"../datos/ecoli_set_const_20.const_".i."_PRO.out" with lines title "Proporcional " ls 11, \
	"../datos/ecoli_set_const_20.const_".i."_CONST.out" with lines title "Constante " ls 12

}
