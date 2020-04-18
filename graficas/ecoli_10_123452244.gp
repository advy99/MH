set title "Avance de la mejor solución encontrada en ecoli con un 10% de restricciones tras las generaciones"
set xlabel "Número de generación"
set ylabel "Valor de la función objetivo"
set ytics 500
set grid
plot "datos/ecoli_set_const_10.const_123452244_AM-1_UN.out" with lines title "AM-1 semilla 123452244",\
	  "datos/ecoli_set_const_10.const_123452244_AM-0_1_UN.out" with lines title "AM-0.1 semilla 123452244",\
	  "datos/ecoli_set_const_10.const_123452244_AM-0_1mej_UN.out" with lines title "AM-0.1mej semilla 123452244",\
	  "datos/ecoli_set_const_10.const_123452244_AGG_UN.out" with lines title "AGG-UN semilla 123452244", \
	  "datos/ecoli_set_const_10.const_123452244_AGG_SF.out" with lines title "AGG-SF semilla 123452244", \
	  "datos/ecoli_set_const_10.const_123452244_AGE_UN.out" with lines title "AGE-UN semilla 123452244", \
	  "datos/ecoli_set_const_10.const_123452244_AGE_SF.out" with lines title "AGE-SF semilla 123452244"
