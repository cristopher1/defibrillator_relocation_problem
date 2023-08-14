---------------------- FALTA ACTUALIZAR EL INFORME ----------------------

Para ejecutar el presente programa, utilice ./pruebas para ejecutar el script bash que se encargará de ejecutar
las 168 pruebas utilizadas en el informe.

Parámetros: los parámetros utilizados por el algoritmo son.

1. Nombre de archivo: nombre del archivo que contiene la instancia del problema a resolver, se debe incluir
la ruta donde se encuentra ubicado.

2. Número de restart: cantidad de restart utilizadas por el algoritmo, utilizado para realizar exploración.

3. Semilla: número que indica la semilla utilizada para generar los números aleatorios que serán usados para
crear las soluciones iniciales.

Ejemplos de comandos de ejecución:

1. En caso de usar el script bash incoporado, usar: ./pruebas

2. En caso de necesitar llamar manualmente o usar el algoritmo desde otro script bash, utilizar:

/usr/bin/time -f "Tiempo de ejecución: %e [s]" ./proyecto $nombre_archivo $n_restart $semilla &> $archivo_salida

Donde:

1. /usr/bin/time -f "Tiempo de ejecución: %e [s]": es usado para medir el tiempo de ejecución en [s].

2. $nombre_archivo: nombre del archivo que contiene la instancia del problema a resolver.

3. $n_restart: número de restart.

4. $semilla: semilla utilizada.

5. $archivo_salida: archivo donde se guardará la salida del programa.

Ejemplos de llamada:

    /usr/bin/time -f "Tiempo de ejecución: %e [s]" ./proyecto ./DRP-INSTANCIAS/SJC-DRP/SJC324-3.txt 1 120 &> ./RESULTADOS/DRP-INSTANCIAS/SJC-DRP/SJC324-3/SJC324-3-1-120.txt

    /usr/bin/time -f "Tiempo de ejecución: %e [s]" ./proyecto ./DRP-INSTANCIAS/SJC-DRP/SJC402-3.txt 2 300 &> ./RESULTADOS/DRP-INSTANCIAS/SJC-DRP/SJC402-3/SJC402-3-2-300.txt

    /usr/bin/time -f "Tiempo de ejecución: %e [s]" ./proyecto ./DRP-INSTANCIAS/SJC-DRP/SJC500-3.txt 3 500 &> ./RESULTADOS/DRP-INSTANCIAS/SJC-DRP/SJC500-3/SJC500-3-3-500.txt

Detalles del output:

    Se incluyen la mayor parte de los datos solicitados para la salida exceptuando lo requerido en la sección 5.b.
    Debido a algunas complicaciones para obtener dicha información no se ha agregado al output.

    Datos adicionales: Se incluye la cantidad de AEDs agregados y movidos.

Supuestos:

    El presente script bash esta pensado para ejecutar el programa con las instancias de problemas dadas para
    realizar el informe. En caso de querer probar el algoritmo con otras instancias con el mismo formato,
    será necesario modificar el script bash o crear uno nuevo para realizar dichas pruebas.
