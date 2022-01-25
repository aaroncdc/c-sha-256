/*
* Implementación del algoritmo SHA-256
* Por Aarón C.d.C
* https://elinformati.co / https://github.com/aaroncdc
*
* Este programa es una demostración de implementación del algoritmo SHA-256
* en el lenguaje de programación C y no está pensado para uso profesional.
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

/* Macros para invertir la endianidad de diferentes tipos de valores. */
#define reverseEndian16(x) (((x&0x00ff)<<8) | ((x&0xff00)>>8))
#define reverseEndian24(x) (((x&0x0000ff)<< 16) | ((x&0xff0000)>>16))
#define reverseEndian32(x) (((x&0x000000ff)<<24) | ((x&0x0000ff00)<<8) | ((x&0x00ff0000)>>8) | ((x&0xff000000)>>24))
#define reverseEndian64(x) (((x&0x00000000000000ff)<<56) | ((x&0x000000000000ff00)<<40) | ((x&0x0000000000ff0000)<<24) | ((x&0x00000000ff000000)<<8) | ((x&0x000000ff00000000)>>8) | ((x&0x0000ff0000000000)>>24) | ((x&0x00ff000000000000)>>40) | ((x&0xff00000000000000)>>56))

/* Implementación de rotacion circular de bits (rol = Left Circular Shift, ror = Right Circular Shift) */
#define rol32(x,n) ((x << n) | (x >> (32-n)))
#define ror32(x,n) ((x >> n) | (x << (32-n)))

/* Tipo que contiene un hash SHA-256 */
typedef struct _sha256_hash {
    uint32_t h0;
    uint32_t h1;
    uint32_t h2;
    uint32_t h3;
    uint32_t h4;
    uint32_t h5;
    uint32_t h6;
    uint32_t h7;
}(SHA256_HASH);

/* Valores del hash (h). Se inicializa mediante la función initialize() */
uint32_t h[8] = { 0x0 };

/* Constantes (k) */
static const uint32_t k[64] = {
   0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
   0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
   0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
   0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
   0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
   0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
   0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
   0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

/* Establecer los valores del hash a sus valores de inicio */
void initialize()
{
    h[0] = 0x6a09e667;
    h[1] = 0xbb67ae85;
    h[2] = 0x3c6ef372;
    h[3] = 0xa54ff53a;
    h[4] = 0x510e527f;
    h[5] = 0x9b05688c;
    h[6] = 0x1f83d9ab;
    h[7] = 0x5be0cd19;
    return;
}

/* Calcula el número de bytes requeridos para hacer un bloque múltiplo de 512 bits. */
int next512Mul(int val)
{
    // Transformar bytes en bits.
    int valBits = (val) * 8;
    // Calcular el número de bits requeridos para hacer un múltiplo de 512.
    int nextBits = ((valBits%512==0) ? valBits: 512-(valBits%512));
    // Debe haber al menos 64 bits reservados para poder almacenar el tamaño del mensaje.
    // Si no los hay, incrementar el tamaño del bloque en otros 512 bits.
    if(nextBits < 64)
        nextBits += 512;
    // Retorna el resultado, transformado de nuevo en bytes y teniendo en cuenta los valores decimales.
    return ceil((double)((double)nextBits/8));
}

/* Genera el bloque de datos inicial que se va a hashear. 
* message - El mensaje a hashear.
* msgLen - La longitúd del mensaje.
* sz - Parametro de salida con el tamaño final del bloque. */
void* makeInitialBlock(char* message, int msgLen, int* sz)
{
    // Calcular el número de bytes requeridos para el bloque.
    int mulReq = next512Mul(msgLen);
    // Solicitar memoria para el bloque.
    char* data = (char*)calloc(1, msgLen + mulReq);
    // Generar un íntegro sin signo de 64 bits que contiene la longitúd del mensaje.
    uint64_t length = msgLen * 8;

    // Revertir la endianidad de la longitúd. NOTA: NO ES NECESARIO EN SISTEMAS BIG ENDIAN.
    length = reverseEndian64(length);
    
    // Copia el mensaje en el bloque.
    memcpy(data, message, msgLen);

    /* Añade un byte b'10000000' justo a continuación del mensaje.
    * El algoritmo especifica que debe ser un 1 y tantos ceros como
    * para hacer un múltiplo de 512 bits, pero aquí estamos trabajando
    * con bytes enteros. */
    data[msgLen] = 0x80;

    // Copia la longitúd del mensaje justo al final del bloque de datos.
    memcpy(data + msgLen + mulReq - 8, &length, 8);

    // Obtener la longitúd del bloque y copiarla en el correspondiente parámetro.
    if(sz)
        *sz = msgLen + mulReq;
    // Retorna el puntero al bloque de datos.
    return data;
}

/* Calcular el estado del hash (h) por cada sub-bloque del bloque de datos (512 bits).
*  data - Puntero al sub-blñoque de datos de 512 bits. */
void doChunkLoop(char* data)
{
    uint32_t s0 = 0, s1 = 0, EP0 = 0, EP1 = 0, ch = 0, maj = 0, temp1 = 0, temp2 = 0;
    uint32_t a,b,c,d,e,f,g,hv;
    uint32_t w[64] = { 0x0 };

    // Copia el bloque de datos a w (words). 64 bytes = 512 bits.
    // Con esto dividimos el sub-bloque de datos en palabras (words, w)
    // de 32 bits.
    memcpy(&w, data, 64);

    // Invierte la endianidad del sub-bloque de datos.
    for(int i = 0; i < 64; i++)
        w[i] = reverseEndian32(w[i]);

    /* Extender las 16 palabras (words). 1 palabra (word) = 32 bits,
    *  16 palabras = 16 * 32 = 512 bits. El algoritmo usado aquí está
    *  especificado en el estandard. Para mayor claridad, visita:
    *  https://en.wikipedia.org/wiki/SHA-2 */
    
    for(int i = 16; i < 64; i++)
    {
        s0 = ror32(w[i-15],7) ^ ror32(w[i-15],18) ^ (w[i-15] >> 3);
        s1 = ror32(w[i-2],17) ^ ror32(w[i-2],19) ^ (w[i-2] >> 10);
        w[i] = w[i-16] + s0 + w[i-7] + s1;
    }

    /* Copia los valores de h (hash) a las diferentes variables a-h.
    * Aquí he llamado a h 'hv' para distinguirlo del array h (hash). */
    a = h[0];
    b = h[1];
    c = h[2];
    d = h[3];
    e = h[4];
    f = h[5];
    g = h[6];
    hv = h[7];

    /* Función de compresión de un solo paso.
    * El algoritmo usado aquí está especificado en el estandard. Para mayor
    * claridad, visita: https://en.wikipedia.org/wiki/SHA-2 */

    for(int i = 0; i < 64; i++)
    {
        EP1 = ror32(e,6) ^ ror32(e,11) ^ ror32(e,25);
        ch = (e & f) ^ ((~e)&g);
        temp1 = hv + EP1 + ch + k[i] + w[i];
        EP0 = ror32(a,2) ^ ror32(a,13) ^ ror32(a,22);
        maj = (a&b) ^ (a&c) ^ (b&c);
        temp2 = EP0 + maj;

        hv = g; g = f; f = e; e = d + temp1;
        d = c; c = b; b = a; a = temp1 + temp2;
    }

    /* Pasar los valores finales al estado del hash (h) */
    h[0] = h[0] + a;
    h[1] = h[1] + b;
    h[2] = h[2] + c;
    h[3] = h[3] + d;
    h[4] = h[4] + e;
    h[5] = h[5] + f;
    h[6] = h[6] + g;
    h[7] = h[7] + hv;
    
    // Final
    return;
}

/* Función que copia los valores del hash (h) a su correspondiente tipo. */
int makeHash(SHA256_HASH* hash)
{
    if(!hash)
        return 0;

    hash->h0 = h[0];
    hash->h1 = h[1];
    hash->h2 = h[2];
    hash->h3 = h[3];
    hash->h4 = h[4];
    hash->h5 = h[5];
    hash->h6 = h[6];
    hash->h7 = h[7];

    return 1;
}

/* Función que genera un texto con el valor de un hash. */
char* makeHashString(SHA256_HASH* hash)
{
    char* finalHash = (char*)calloc(1, 66);

    if(!finalHash)
        return 0L;

    sprintf(finalHash, "%08x%08x%08x%08x%08x%08x%08x%08x\0", hash->h0, hash->h1, hash->h2,
        hash->h3, hash->h4, hash->h5, hash->h6, hash->h7);

    return finalHash;
}

/* Función para cargar un archivo. */
char* loadFile(char* filename)
{
    char* buffer = 0L;
    size_t sz, rd;

    // Abre el archivo.
    FILE* f = fopen(filename, "r");
    if(!f)
        return 0L;

    // Obtener el tamaño total del archivo.
    fseek(f, 0, SEEK_END);
    sz = ftell(f);
    fseek(f, 0, SEEK_SET);

    // Alojar en la memoria espacio para el contenido del archivo.
    buffer = (char*) calloc (1,sz);

    if(!buffer)
    {
        fclose(f);
        return 0L;
    }

    // Copia los datos del archivo en la memoria.
    rd = fread(buffer, sz, 1, f);

    // Cierra el archivo.
    fclose(f);

    // Devuelve un puntero a la dirección de la copia del contenido del archivo en la memoria.
    return buffer;
}

/* Función principal */
int main(int argc, char** argv)
{
    char* finalHash = 0L;
    uint32_t w[64] = {0x0};
    int dataSz = 0, retVal = 0;
    char* data = 0L;
    char* fcontent = 0L;
    SHA256_HASH hash;

    // Comprueba si hay argumentos.
    if(argc > 1)
    {
        // El primer y único argumento puede ser un mensaje, o una ruta
        // a un archivo. Comprueba si es una ruta primero.
        if((argv[1][1] == ':' && argv[1][2] == '\\') ||
            argv[1][0] == '/')
        {
            // Si es una ruta, intenta cargar el archivo.
            fcontent = loadFile(argv[1]);
            if(!fcontent)
            {
                printf("Error reading file: %s\n", argv[1]);
                return -1;
            }
            // Genera un bloque de datos usando el contenido del archivo como mensaje.
            data = (char*)makeInitialBlock(fcontent, strlen(fcontent), &dataSz);
        }else{
            // Si es un mensaje, genera un bloque de datos con el contenido del mensaje.
            data = (char*)makeInitialBlock(argv[1], strlen(argv[1]), &dataSz);
        }
    }
    else{
        // Si no hay argumentos, muestra un mensaje con información sobre el uso de la aplicación y finaliza.
        printf("Uso:\nsha256 <mensaje / ruta de archivo>\n\n- mensaje: Esto puede ser el mensaje a hashear,\no una ruta de archivo. Debe estar entre comillas.\n");
        return 1;
    }

    // Inicializa el valor del hash (h)
    initialize();

    /* Divide el bloque de datos en sub-bloques de 512 bits (64 bytes), y procesa cada uno
    *  de manera individual hasta calcular el valor final del hash. */
    for(int x = 0; x < dataSz/64; x++)
        doChunkLoop(data + (64 * x));

    // Copia el valor final del hash en su correspondiente espacio en la memoria.
    retVal = makeHash(&hash);

    // Si falla, sale con error.
    if(!retVal)
    {
        printf("Failed to allocate memory for the hash\n");
        free(data);
        free(finalHash);
        if(fcontent)
            free(fcontent);
        return -1;
    }

    // Genera un texto con el valor del hash.
    finalHash = makeHashString(&hash);

    // Muestra el valor del hash.
    printf("SHA-256: %s\n", finalHash);

    // Libera la memoria usada.
    free(data);
    free(finalHash);
    if(fcontent)
        free(fcontent);

    // Fin del programa.
    return 0;
}