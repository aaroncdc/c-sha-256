# c-sha-256
 Implementación de SHA-256 en C

Esta es una implementación personal en C del algoritmo de hashing SHA-256. Es una demostración de cómo implementar dicho algoritmo en dicho lenguaje, y no está pensado para uso profesional.

Para compilarlo con gcc, ejecuta el makefile con GNU MAKE o utiliza el siguiente comando:

    gcc sha256.c -o sha256 -lm

Se puede compilar tanto en un entorno Linux/BSD nativo, como en MinGW/MSYS en Windows. También es posible hacerlo usando WSL. El código no compila en Visual Studio, pero es posible adaptarlo a dicho entorno.

Uso:

```
sha256 <mensaje / ruta de archivo>

- mensaje: Esto puede ser el mensaje a hashear,
o una ruta de archivo. Debe estar entre comillas.
```

A continuación una lista de mensajes y los hashes que produce:

| Mensaje | Hash |
|---|---|
|hello world|b94d27b9934d3e08a52e52d7da7dabfac484efe37a5380ee9088f7ace2efcde9|
|hola mundo!|9b8b048f951b60542c00a3f2754233fb339b60d0cddb5a73025897cf1af4e55f|
|1234ABCD4321|9084e174a2f8628daf352a51372bbb65971ab8cc22e5734f8ef3a9363080bbf9|
|1234ABCD4322|337bb6efeb78d06da8524a6b37f265d0b25daa9e0074723bcf11bcb29b90c4a1|
|$a$b|6c512aa233db08278faa6989d357f1dd2052c61cb8d4cfeafa132d4993a0999d|
||e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855|

La siguiente lista muestra los tiempos que tarda en procesar diferentes mensajes. Para medirlos, se ha ejecutado el programa en un ordenador con un procesador AMD Ryzen 7 2700 usando MinGW y compilado con GCC sin banderas de optimización. La tarea se ha ejecutado 5 veces y se ha anotado la media de los tiempos:

| Mensaje | Tamaño (JEDEC) | Tiempo |
|---|---|---|
|"Hola Mundo!"|11 bytes|62ms|
|Guión de la película "Bee movie" (https://gist.github.com/ElliotGluck/64b0b814293c09999f765e265aaa2ba1)|54KB|64ms|
|El Quijote (Texto Plano) (https://gramatica.usc.es/~gamallo/aulas/lingcomputacional/corpus/quijote-es.txt)|1,96MB|111ms|
|Foto de Rigel y la nébula de la cabeza de bruja (https://apod.nasa.gov/apod/astropix.html)|2,25MB|71ms|