---
layout: page
title: "Página principal"
permalink: /
filename: index.md
---

# Prácticas de la asignatura Metaheurísticas

## Autor: Antonio David Villegas Yeguas

En este proyecto podrá encontrar mi implementación para resolver el problema de agrupación con restricciones (de ahora en adelante PAR) escrita en C/C++.


# Descripción de los archivos por carpetas

## Carpeta bin

En esta carpeta encontrará el binario generado a partir del código.

## Carpeta datos

En esta carpeta encontrará los datos del PAR dados, existen dos tipos de ficheros:

### Ficheros de datos

Estos ficheros almacenan las características de los datos del problema.

Tienen la siguiente forma: `<dataset>_set.dat`

En nuestro caso `<dataset>` podrá ser:

- Ecoli
- Iris
- Rand
- Newthyroid

### Ficheros de restricciones

Estos ficheros almacenan las restricciones entre los distintos datos del problema.

Tienen la siguiente forma: `<dataset>_set_const_<porcentaje>.const`

En este caso `<dataset>` será el mismo que en el apartado anterior, mientras que `<porcentaje>` podrá ser:

- 10
- 20


Además cada ejecución del programa creará ficheros con la siguiente forma dentro de esta carpeta: `<dataset>_set_const_<porcentaje>.const_<semilla>_<algoritmo>.dat`

Donde cada elemento entre llaves significa:

- `<dataset>`: Conjunto de datos usado
- `<porcentaje>`: Porcentaje de restricciones sobre el conjunto de datos
- `<semilla>`: Semilla aleatoria sobre la que se ha ejecutado el programa
- `<algoritmo>`: Algoritmo usado para su resolución


El programa creará un fichero por algoritmo usado, donde almacenará los resultados obtenidos con dicho algoritmo, conjunto de datos y restricciones y semilla.


## Carpeta graficas

En esta carpeta se almacenan scripts de gnuplot que generan gráficas con las distintas semillas, porcentajes y conjuntos de datos para poder comparar como se comportan los algoritmos genéticos, la búsqueda local y sobre enfriamiento simulado.

Dentro existe una carpeta llamada datos donde el programa almacenará los datos para gnuplot y otra llamada salidas_png donde se guardarán las imagenes tras ejecutar:

```
gnuplot <fichero>.gp
```

Es necesario ejecutar el programa para obtener los ficheros con los que se generarán las gráficas.

## Carpeta include

Carpeta con las cabeceras de los ficheros en C/C++ usados.

## Carpeta memorias

Carpeta con los ficheros fuente de LaTex usados para generar la documentación.

## Carpeta obj

Carpeta donde crearemos los ficheros objeto a partir del código, para finalmente obtener el ejecutable.

## Carpeta src

Implementación del código descrito en las cabeceras y el fichero `practica1.cpp` con el `main`.

## Carpeta .

En esta carpeta encontramos:

- Este propio fichero de documentación.
- `Makefile`: Fichero para compilar el programa
- `ejecuciones.sh`: Fichero para ejecutar las 30 ejecuciones pedidas en la práctica.
