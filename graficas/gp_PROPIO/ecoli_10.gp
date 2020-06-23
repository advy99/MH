set title "Ecoli con un 10% de restricciones"
set xlabel "Número de evaluación"
set ylabel "Valor de la función objetivo"
set ytics 500
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
	set output "../salidas_png/ecoli_set_const_10_".i.".png"


	plot "../datos/ecoli_set_const_10.const_".i."_ALG-PRO_explorar.out" w lines title "Mejor población explorar" ls 1, \
	     "../datos/ecoli_set_const_10.const_".i."_ALG-PRO_explotar.out" using 1:2 w lines title "Mejor población explotar" ls 2, \
		  "../datos/ecoli_set_const_10.const_".i."_ALG-PRO_explotar.out" using 1:3 w lines title "Peor población explotar" ls 3
}
