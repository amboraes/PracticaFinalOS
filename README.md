# PracticaFinalOS
* Hoy en día la gran mayoría de los exámenes médicos son realizados automáticamente por computadores, estos se encargan de recibir un conjuntode muestras y realizar los análisis correspondientes. Muchos analizadores el procesamiento de las muestras es secuencial debido a que los analizadores son diseñados únicamente con un bandeja de entrada. Esta práctica busca simular la construcción de un nuevo sistema con múltiples bandejas de entrada para procesar más exámenes por unidad de tiempo.
## Funcionamiento
Esta práctica funciona con 5 comandos: init, reg, ctrl, rep y stop.
### Comando Init
Este comando inicializa el sistema (hilos, semaforos y memorias compartidas correspondientes)
````
$ ./evaluator init [-i <integer>] [-ie <integer>] [-oe <integer>] [-n <string>] [-b <integer>] [-d <integer>] [-s <integer>] [-q <integer>] 
````
Donde **-i** es el número de entradas que es es un valor positivo, **-ie** el núumero de posiciones en cada una de las colas de entrada, **-oe** el número de entradas en la cola de salida, **-n** indica el nombre del segmento de memoria compartida, **-b** el nivel de reactivo de sangre, **-d** el nivel de reactivo de detritos, **-s** el nivel de reactivo de piel, **-q** el tamaño de las colas internas.

Cada uno de estas entradas tiene valores por omisión indicados en el enunciado de este proyecto. 

### Comando Reg
Este comando se encarga de registrar los diferentes exámenes puede ser usado de forma interactiva o puede pasarse un archivo .txt donde estos se encuentren.
````
$ ./evaluator [-n <string>] {{<filename>} ... | - }
````
La opción **-** activa la forma interactiva y se verá se la siguiente forma:
````
> <integer> {B|D|S} <integer>
````
Donde el primer integer indica la bandeja de entrada, el segundo es una letra que indica que la muestra es: Blood, Detritus, o Skin y el
tercer elemento es un entero que indica la cantidad de la muestra. 

Al ingresarse un examen con éxito el programa devolverá el id de dicho examen, de no ser así el programa no devolverá este id.

### Comando Ctrl
Este comando esta compuesto por 2 sub-comandos adicionales: list y update. El subcomando list tiene a su vez 5 subcomands más (processing, waiting, reported, reactive y all) los cuales deben devolver los estados de los exámenes así: 
````
$ ./evaluator ctrl [-n <string>]
> list [ processing | waiting | reported | all ]
[Processing: | Waiting: | Reported:]
id i k q p 
.
.
.
````
El comando reactive devuelve el estado de los reactivos así:
````
> list reactive
Reactive:
B: <integer>
S: <integer>
D: <integer>
````
El comando update actualiza los niveles de reactivo actual así:
````
> update { B | D | S } <integer>
````
### Comando Rep
Este comando devuelve las muestras que ya han sido reportadas así:
````
$ ./evaluator rep [-n <string>]
> -i <integer>
> -m <integer>
````
-i indica que el reporte queda en modo interactivo esperando un tiempo indicado por el parámetro entero, -m indica que el reporte obtiene hasta un número determinado de exámenes, sino hay espera hasta completar la cifra.

### Comando Stop
Este comando permite parar a todo el sistema, borrando y devolviendo al sistema todos los recursos adquiridos.
````
$ ./evaluator stop [-n <string>]
````
## Compilación
La practica esta compuesta de 8 archivos .cpp y 8 archivos .h todos localizados en la carpeta /src, la forma de compialción es con un Makefile. Se ejecuta el comando make en la carpeta donde este se encuentre, este generará un ejecutable llamado evaluator localizado en la carpeta src con el cual se harán todos los demas comandos explicados previamente.
````
$ make
$ cd src
$ ./evaluator ....
````
## Integrantes
María Clara Sánchez V.

Juan Esteban Fonseca P.

Tomás Alvarez G.
