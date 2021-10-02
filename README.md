# Protocolos de Comunicación TPE1

## Autores

- [Julián Arce](https://github.com/juarce)
- [Roberto Catalán](https://github.com/rcatalan98)
- [Paula Domingues](https://github.com/pdomins)
- [Gian Luca Pecile](https://github.com/glpecile)

## Objetivo

El objetivo del TPE1 es el diseño de un protocolo extensión del protocolo `ECHO`, a su vez se tiene que implementar un servidor concurrente.

## Funcionalidades
*   Conexión **TCP** que acepta:
    *   El recibir líneas de texto US-ASCII (se ignoran hasta el final de la linea si no contiene) finalizadas con `\r\n`.
    *   Cada linea tiene que ser de menos de 100 lineas (no se tienen en cuenta \n\r). Si se supera dicho largo los caracteres son ignoroados.
    *    Se aceptan únicamente los siguientes comandos:
        *   ```ECHO texto\r\n``` 
        donde el servidor le enviará como respuesta al cliente el texto recibido, finalizado con `\r\n`.
        *   ```GET date\r\n```
        donde el servidor responde con una línea de texto con la fecha actual, por defecto en formato dd/MM/yyyy.
        *   ```GET time\r\n```
        el servidor responde con una línea de texto con la hora en forma hh:mm:ss.

*   Conexión **UDP** que acepta los siguientes datagramas:
    *   `SET locale en` - cambia el formato de las fechas a MM/dd/yyyy.
    *   `SET local es` - cambia el formato de las fechas a dd/MM/yyyy.
    *   `STATS` - se obtiene un datagrama con las siguientes estadísticas en modo texto (cada una separada por `\r\n`):
        *   Cantidad de conexiones realizadas desde que inició la ejecución.
        *   Cantidad de líneas incorrectas recibidas.
        *   Cantidad de líneas correctas recibidas.
        *   Cantidad de datagramas incorrectos recibidos (incluye comandos inválidos o inexistentes).
*   El servidor para tanto UDP como TCP se encuentra en el puerto 9999, esto se puede cambiar por linea de comando.


## Compilación

La compilación se realiza con el siguiente comando:

```bash
make all
```

## Ejecución

La ejecución se realiza con el siguiente comando:

```bash
./server <port>
```
Por defecto, el puerto utilizado es 9999.

## Testeo
Para el testeo tanto con **Cppcheck** como **Valgrind**. Correr el siguiente comando:

```bash
 make test
```


## Limpieza

La limpieza de los archivos generados se realiza con el siguiente comando:

```bash
make clean
```
La limpieza de los achivos generados durante el testeo se realiza con el siguiente comando:

```bash
make cleanTest
```
