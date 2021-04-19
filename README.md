# SOR-Semáforos-1S-2021
Trabajo Práctico Semáforos primer semestre año 2021
Alumno : Espinillo Elías
INTRODUCCIÓN
En éste trabajo, se buscó la resolución de una competencia de cocina en base a la programación, aplicaciones del lenguaje C y utilización de semáforos. Se vieron principalmente la aplicación de éstos ultimos y su aplicación apra poder resolver problemas de recursos compartidos, como en éste caso fue el horno, un salero y una cocina para poder realizar la comida. 

PROCESO DE CREACIÓN DEL SISTEMA
En primera instancia, compile y ejecute el template brindado para ver su comportamiento y la salida que tenía, para poder empezar a ver lo que se podía requerir y ver el funcionamiento de las funciones dadas.

Una vez visto ésto, se empezó a realizar un pseudocodigo con los procesos que serían necesarios y los posibles semáforos que se necesitarían, de acuerdo a lo solicitado en el trabajo práctico.

A continuación, adjunto una imagen con el pseudocodigo:
![PSEUDOCODIGO](https://raw.githubusercontent.com/EliasEspinillo/TP-SOR/master/assets/PSEUDOCODIGO.png)

Una vez comprendido, empecé a utilizar la función que dieron de base (cortar) para poder empezar a replicarlo en las demás que tenían que aplicarse en el programa, que son:
-mezclar
-salar
-mezclar_milanesa
-empanar
-cocinar_milanesa
-cortarAditivos
-cocinarPan
-armarMilanesa
Las mismas primeramente fueron aplicadas a 1 sólo hilo, y de forma progresiva,  para asegurarme que las mismas fueron bien ejecutadas y su formato base estuvo bien hecho. 
