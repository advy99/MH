layout: page
title: "Manual de uso"
permalink: /manual_uso/


# Manual de uso

En esta sección explicaré como compilar y ejecutar el programa para probar los distintos algoritmos implementados.



## Compilar

Para compilar el programa basta con abir una terminal y situarse en la carpeta donde se encuentra este fichero README, tras eso compilar ejecutando make:

`make`


## Ejecutar

El binario generado en la sección anterior recibirá 4 parámetros:

`./bin/practica1 <fichero_datos> <fichero_restricciones> <num_clusters> <semilla>`

Esto hará que se realice una ejecución de cada algoritmo con la semilla dada.

Para automatizar las distintas ejecuciones con las semillas aleatorias también es posible ejecutar el script en shell `ejecuciones.sh`:

`./ejecuciones.sh`

Con esto ejecutaremos el programa con todos los datasets para las distintas semillas, haciendo un total de 30 ejecuciones.

**Importante:** Este script ejecutará ocho instancias a la vez para aprovechar todos los núcleos de mi ordenador y obtener los resultados de forma más rápida, si no tienes ocho núcleos te recomiedo que modifiques este script para hacer las ejecuciones con menos o incluso secuenciales.

Este fichero ya está preparado para ejecutar todos los datasets con las distintas restricciones y semillas.


## Obtener gráficas

Una vez ejecutamos los algoritmos, tanto para la práctica dos, tres y cuatro, podemos generar gráficos que muestren el comportamiento de los distintos algoritmos.

Para obtener la gráficas tenemos que movernos al directorio donde están contenidos los scripts de gnuplot, existen tres directorios:

- gp_AG : Gráficas de la práctica 2.
- gp_ES : Gráficas de la práctica 3.
- gp_PROPIO : Gráficas de la práctica 4.

Y una vez dentro de estos directorios ejecutamos gnuplot:

```sh
cd graficas/<carpeta>
gnuplot <fichero>.gp
```

Cada fichero generará las gráficas de un conjunto de datos.

Las salidas en png están en la carpeta `graficas/salidas_png`.
