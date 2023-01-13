#pragma once
#include "quicksort.h"
#include "iostream"
#include "stdio.h"
#include "math.h"

/* Definir los canales de color en imagenes BGR. */
#define BLUE_CHANNEL  0
#define GREEN_CHANNEL 1
#define RED_CHANNEL   2

/* Definir funciones para obtener maximos y minimos. */
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

/*  */
#define VO 255
#define VB 0

/* Kernel para el desenfoque usando promedio. k = 9. */
const int BOX3x3[3][3] = { { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } };
/* Kernel para el resalte de esquinas. k = 4. */
const int CORNER3x3[3][3] = { { 1, -2, 1 }, { -2, 4, -2 }, { 1, -2, 1 } };
/* Kernel para el desenfoque. k = 16. */
const int BILINEAR3x3[3][3] = { { 1, 2, 1 }, { 2, 4, 2 }, { 1, 2, 1 } };
/* Kernel para el enfoque. k = 9. */
const int ENFOQUE3x3[3][3] = { { 0, -1, 0 }, { -1, 5, -1 }, { 0, -1, 0 } };
/* Kernel para el resalte de bordes. k = 2. */
const int R_BORDES3x3[3][3] = { { 0, 0, 0 }, { -1, 1, 0 }, { 0, 0, 0 } };
/* Kernel para el repujado. k = 9. */
const int REPUJADO3x3[3][3] = { { -1, -2, -1 }, { 0, 1, 0 }, { 1, 2, 1 } };
/* Kernel para resalte de bordes laplaciano. k = 1.*/
const int LAPLACIAN3x3[3][3] = { { 0, -1, 0 }, { -1, 4, -1 }, { 0, -1, 0 } };

/* Kernel para el elemento estructurante rectangular de 3x3. */
const int MORPH_RECT5x5[5][5] = { { 1, 1, 1, 1, 1 }, { 1, 1, 1, 1, 1 }, { 1, 1, 1, 1, 1 }, { 1, 1, 1, 1, 1 }, { 1, 1, 1, 1, 1 } };
/* Kernel para el elemento estructurante de forma cruz de 3x3. */
const int MORPH_CROSS5x5[5][5] = { { 0, 0, 1, 0, 0 }, { 0, 0, 1, 0, 0 }, { 1, 1, 1, 1, 1 }, { 0, 0, 1, 0, 0 }, { 0, 0, 1, 0, 0 } };
/* Kernel para el elemento estructurante tipo eliptico de 3x3. */
const int MORPH_ELLIPSE5x5[5][5] = { { 0, 0, 1, 0, 0 }, { 0, 1, 1, 1, 0 }, { 1, 1, 1, 1, 1 }, { 0, 1, 1, 1, 0 }, { 0, 0, 1, 0, 0 } };

using namespace std;

#pragma pack(push, 1)
struct img_bmp_file_header {
    uint16_t  FILE_TYPE     {0x4D42};           /* Tipo de archivo. Normalmente es BM (para BMP). */
    uint32_t  FILE_SIZE     {0};                /* Tamaño del archivo (incluye tamaño de cabecera y del vector). */
    uint32_t  RESERVED      {0};                /* Reservado, normalmente 0 bytes. */
    uint32_t  OFFSET_DATA   {0x00000036};       /* Indice donde indica desde donde comienzan los pixeles. */
};

struct img_bmp_info_header {
    uint32_t HEADER_SIZE    {0x00000028};       /* Tamaño de esta cabecera. Normalmente 40 bytes. */
    uint32_t IMG_WIDTH      {0};                /* Anchura de la imagen. */
    uint32_t IMG_HEIGHT     {0};                /* Altura de la imagen. */
    uint16_t IMG_PLANES     {0x0001};           /* Numero de planos de la imagen. Siempre es 1. */
    uint16_t BITS_PER_PIXEL {0};                /* Profundidad de bits. */
    uint32_t COMPRESSION    {0};                /* Métoto de compresión de la imagen. Con 0 indica que la immagen no está comprimida. */
    uint32_t SIZE_IMAGE     {0};                /* Tamaño total de la imagen. */
    int32_t  X_RESOLUTION   {0x00000EC4};       /* Resolución horizontal. */
    int32_t  Y_RESOLUTION   {0x00000EC4};       /* Resolución vertical. */
    uint32_t COLORS_USED    {0};                /* Número de colores por paleta. */
    uint32_t COLORS_IMPT    {0};                /* Número de colores importantes por paleta. */
};

struct img_bmp_color_header {
    uint32_t RED            {0x00FF0000};
    uint32_t GREEN          {0x0000FF00};
    uint32_t BLUE           {0x000000FF};
    uint32_t ALPHA          {0xFF000000};
    uint32_t SPCOLOR_TYPE   {0x73524742};
    uint32_t UNUSED[16]     {0};
};
#pragma pack(pop)

enum img_format {
    NONE = 0,
    BMP  = 1,
    BIT  = 2
};

struct IMAGE {
public:
    /*
     *  Definir una nueva imagen en blanco establecieno inicialmente las dimensiones de la imagen. Los
     *  valores por defecto son una imagen 320 x 240 pixeles.
     *
     *  _width -> anchura de la imagen en pixeles.
     *  _heigth -> altura de la imagen en pixeles.
     * 
     */
    IMAGE(unsigned int _width = 320, unsigned int _height = 240) {
        // Estimar las dimensiones del vector de datos, usando las dimensiones de la imagen
        // y el número de canales.
        img_synchrony_byts = _width % 4;
        img_channels = 3;
        real_image_size = _width * _height * 3;
        or_image_size = _width * _height * 3 + (img_synchrony_byts * _height);
        img_width = _width;
        img_height = _height;
        // Inicializar el vector de datos.
        data = new unsigned char[real_image_size];
        for(int i = 0; i < real_image_size; i++) data[i] = 0x00;
    }
    /*
     *  Definir una nueva definiendo la anchura y altura de la imagen, además de especificar
     *  un archuvo donde contiene vector de datos.
     *
     *  int _width -> Anchura de la imagen en pixeles.
     *  int _heigth -> Altura de la imagen en pixeles.
     *  char *_file_name -> Archivo que contiene el stream de datos.
     * 
     */
    IMAGE(unsigned int _width, unsigned int _height, unsigned char _channels, const char *_filename) {
        img_synchrony_byts = _width % 4;
        img_channels = _channels;
        real_image_size = _width * _height * _channels;
        or_image_size = _width * _height * _channels + (img_synchrony_byts * _height);
        img_width = _width;
        img_height = _height;

        FILE *file;
        file = fopen(_filename, "rb");
        data = new unsigned char[real_image_size];
        fread((unsigned char*)&*data, sizeof(char) * real_image_size, 1, file);
        fclose(file);
        img_null = false;
    }
    /*
     *  Definir una imagen usando un nombre de archivo. Esta función busca el archivo y hace el volcado de
     *  los datos para crear la definición de la imagen.
     * 
     *  _file_name -> nombre del archivo.
     *
     */
    IMAGE(const char *_file_name) {
        read(_file_name);
    }
    /*
        Función que por medio de un nombre de archivo especificado, hace la carga del archivo en memoria.

        _file_name -> nombre del archivo.
    */
    void read(const char *_filename) {
        // Cabeceras.
        img_bmp_file_header  file_header;
        img_bmp_info_header  info_header;
        img_bmp_color_header color_header;
        // Puntero al archivo.
        FILE *file;
        file = fopen(_filename, "rb");
        if(file == NULL) {
            // Si el archivo no existe, no se puede abrir el archivo.
            img_null = true;
            return;
        }
        else {
            // Leer la primera parte de la cabecera. La que corresponde a los datos del archivo.
            fread((char *)&file_header, sizeof (file_header), 1, file);
            if(file_header.FILE_TYPE == 0x4D42) {
                // Leer los datos de la segunda cabecera, los que contiene los datos de la imagen.
                fread((char *)&info_header, sizeof (info_header), 1, file);
                // En caso de que la imagen sea de 32 bits, no va a poder abrir la imagen.
                if(info_header.BITS_PER_PIXEL != 24) {
                    img_null = true;
                    return;
                }
                // En caso de que la imagen si sea de 24 bits, hay que hacer un vaciado de la información.
                else {
                    info_header.HEADER_SIZE = sizeof (info_header);
                    file_header.OFFSET_DATA = sizeof (file_header) + sizeof (info_header);
                    // Obtener los atributos de la imagen.
                    or_image_size = file_header.FILE_SIZE - (sizeof(file_header) + sizeof(info_header));
                    real_image_size = info_header.IMG_WIDTH * info_header.IMG_HEIGHT * 3;
                    img_synchrony_byts = ((file_header.FILE_SIZE - real_image_size - (sizeof(file_header) + sizeof(info_header)))) / info_header.IMG_HEIGHT;
                    img_channels = 3;
                    img_height = info_header.IMG_HEIGHT;
                    img_width = info_header.IMG_WIDTH;
                }
                fseek(file, file_header.OFFSET_DATA, SEEK_SET);
                unsigned char *buffer = new unsigned char[or_image_size];
                fread((unsigned char *)&*buffer, sizeof (char) * or_image_size, 1, file);
                trim(buffer);
            }
            fclose(file);
            img_null = false;
        }
    }
    /*
     *  Función que hace el guardado de un archivo. Se tiene que especificar el formato
     *  del archivo.
     * 
     *  char *_file_name -> Nombre del archivo.
     *  img_format _format -> Formato del archivo. 
     * 
     */
    bool save(const char *_filename, img_format _format) {
        switch (_format) {
            case img_format::BMP: return save_bmp(_filename);
            case img_format::BIT: return save_bit(_filename);
            default: return false;
        }
    }
    /*
     *  Obtiene la anchura en pixeles de la imagen.
     *
     */
    unsigned int width() {
        return img_width;
    }
    /*
     * Obtiene la altura en pixeles de la imagen.
     *
     */
    unsigned int height() {
        return img_height;
    }
    /*
     *  Obtiene una bandera que indica si la imagen es nula o no.
     *
     */
    bool null() {
        return img_null;
    }
    /*
     *  Obtiene el número de bits que se añaden al final de una fila para completar el múltiplo de 4.
     *
     */
    unsigned char synchrony_bytes() {
        return img_synchrony_byts;
    }
    /*
     *  Obtiene el numero de canales de la imagen.
     *
     */
    unsigned char channels() {
        return img_channels;
    }
    /*
     *  Obtiene el tamaño original de la imagen haciendo la multiplicación de la resolución horizontal 
     *  y vertical tomando en cuenta los bits que se añaden al final de a fila para completar el múltiplo 
     *  de 4.
     * 
     */
    unsigned int get_or_image_size() {
        return or_image_size;
    }
    /*
     *  Obtiene el tamaño real de la imagen haciendo la multiplicación de la resolución horizontal y 
     *  vertical de la imagen. Representa el tamaño total del vector de bits de los pixeles.
     * 
     */
    unsigned int get_real_image_size() {
        return real_image_size;
    }
    /*
     *  Obtiene un puntero que apunta al primer bit del primer pixel. Las imágenes se almacenan en 
     *  memoria al revés.
     * 
     */
    unsigned char *first() {
        return data;
    }
    /*
     *  Cambia el valor de un bit del vector de bits de la imagen. Los valores que se ingresan se 
     *  convierten a tipo char y se hace una operación para mantenerlo dentro del rango de 0 a 255.
     * 
     *  int _x -> Posición x del pixel que se va a cambiar.
     *  int _y -> Posición y del pixel que se va a cambiar.
     *  int _x -> Canal c del pixel que se va a cambiar
     *  int _value -> valor del elemento nuevo.
     * 
     */    
    void at(unsigned int _x, unsigned int _y, unsigned int _c, unsigned int _value) {
        int x = _x * img_channels;
        int y = _y * img_channels * img_width;
        data[x + y + _c] = _value > 255 ? 255 : _value < 0 ? 0 : _value;
    }
    /*
     *  Obtiene un pixel en un indice determinada.
     * 
     *  int _x -> Posición x del pixel que se va a cambiar.
     *  int _y -> Posición y del pixel que se va a cambiar.
     *  int _x -> Canal c del pixel que se va a cambiar
     * 
     */   
    unsigned char at(unsigned int _x, unsigned int _y, unsigned int _c) {
        int x = _x * img_channels;
        int y = _y * img_channels * img_width;
        return data[x + y + _c];
    }
    /*
     *  Convierte la imagen en una versión de 8 bits, especificando el canal de color que se quiere 
     *  conservar.
     *
     * _channel -> canal de color a preservar. 
     * 
     */
    void to_8bits(unsigned char _channel = 0U) {
        if(img_channels != 3)
            return;
        unsigned int new_real_size = img_height * img_width;
        unsigned char *buffer = new unsigned char[new_real_size] { 0x00 };
        unsigned int bytes_count = 0;
        for(int i = _channel; i < real_image_size; i += 3) {
            buffer[bytes_count] = data[i];
            bytes_count++;
        }
        real_image_size = new_real_size;
        or_image_size = new_real_size + ((img_width % 4) * img_height);
        img_channels = 1;
        data = buffer;
    }
    /*
     *  Convierte la imagen en una versión de 24 bits. Si la imagen es de 8 bits, este copia
     *  el byte en cada uno de los canales de color, generando una imagen a escala de grises.
     * 
     */
    void to_24bits() {
        if(img_channels != 1)
            return;
        unsigned int new_real_size = img_height * img_width * 3;
        unsigned char *buffer = new unsigned char[new_real_size] { 0x00 };
        unsigned int bytes_count = 0;
        for(int i = 0; i < new_real_size; i += 3) {
            buffer[i] = data[bytes_count];
            buffer[i + 1] = data[bytes_count];
            buffer[i + 2] = data[bytes_count];
            bytes_count++;
        }
        real_image_size = new_real_size;
        or_image_size = new_real_size + ((img_width % 4) * img_height);
        img_channels = 3;
        data = buffer;
    }
    /*
     *  Libera lo recursos usados por la imagen. 
     *  
     */
    void dispose() {
        delete[] data;
    }

private: 
    /*
     *  Indica si la imagen pudo ser leida o no. La bandera se posicionara en nullo si la imagen 
     *  no pudo ser abierta o hubo un error en la creación de la misma. En caso contrario la 
     *  bandera estará negativa.
     * 
     */
    bool img_null;
    /*
     *  Vector que contiene todos los bytes de la imagen, correspondiente a cada uno de los valores 
     *  de cada pixel. Para recorrerse la imagen tiene que hacerse como si fuese un vector. 
     * 
     */
    unsigned char *data;
    /*
     *  Número de bits que se añaden al final de una fila para acompletar el múltiplo de cuatro.
     *  Las resoluciones de las imágenes tienen que estar en multiplo de cuatro, cuando no es el 
     *  caso, añade al final de la fila el número de bits necesarios para acompletar. El valor de 
     *  estos bits están en cero.
     *
     */
    unsigned char img_synchrony_byts;
    /*
     *  Número de canales de color de la imagen. Las imagenes a color RGB manejan 3 canales de color 
     *  (una por cada color del espectro RGB) y las imagenes que son en escala de grises son 
     *  normalmente de 1 solo canal.
     * 
     */
    unsigned char img_channels;
    /* 
     *  Tamaño original de la imagen. Corresponde a las dimensiones originales de la imagen tomando 
     *  en cuenta los bits que se  agregan al final para completar el multiplo de 4.
     * 
     */
    unsigned int or_image_size;
    /*
     *  Tamaño real de la imagen. Es el tamaño original del vector de bits de la imagen, corresponde 
     *  a la multiplicación de  las dimensiones de la matriz por el espacio de color.
     * 
     */
    unsigned int real_image_size;
    /*
     *
     */
    unsigned int img_width;
    /*
     *
     *
     */
    unsigned int img_height;
    /*
     *  Función que extrae el vector original de bits de la imagen. Se encarga de retirar los bits 
     *  extra que se añaden al final de cada renglón para la sincronía vertical y trabajar mas facil 
     *  la imagen.
     * 
     * _buffer -> vector original que contiene todos los pixeles. 
     * 
     */
    void trim(unsigned char *_buffer) {
        unsigned int idx_vector = 0;
        unsigned int rows_count = 0;
        data = new unsigned char[real_image_size] { 0x00 };

        for(int i = 0; i < or_image_size; i++) {
            data[idx_vector] = _buffer[i];
            if((i + 1) > img_channels && 
               (i + 1 - (rows_count * img_synchrony_byts)) % (img_width * img_channels) == 0) {
                i += img_synchrony_byts;
                rows_count++;
            }
            idx_vector++;
        }
    }
    /* 
     *  Añada los bytes faltantes para la sincronia vertical, en caso de que haya bits que añadir.
     *  
     */
    void syncronize() {
        // Si la imagen no es de 24 bits, hay que convertirla. Para guardarla se requiere este formato.
        if(img_channels != 3)
            to_24bits();

        // Si no hay bits de sincronia, no hay nada que sincornizar.
        if(img_synchrony_byts == 0)
            return;

        unsigned char *buffer = new unsigned char[or_image_size] { 0x00 };
        int idx_vector = 0;

        for(int i = 0; i < real_image_size; i++) {
            buffer[idx_vector] = data[i];
            if((i + 1) > img_channels && 
               (i + 1) % (img_width * img_channels) == 0) {
                for(int j = 0; j < img_synchrony_byts; j++) {
                    idx_vector++;
                    buffer[idx_vector] = 0;
                }
            }
            idx_vector++;
        }

        data = buffer;
    }
    /*
     *  Función que hace el guardado del archivo mediante un nombre especificado.
     *
     * _file_name -> nombre del archivo.
     * 
     */
    bool save_bmp(const char *_filename) {
        // Cabeceras.
        img_bmp_file_header  file_header;
        img_bmp_info_header  info_header;
        img_bmp_color_header color_header;
        FILE *file;
        file = fopen(_filename, "wb");
        // Verificar que el archivo se pudo abrir.
        if(file == NULL)
            return false;
        // Reacomodar el vector de los datos de bytes.
        syncronize();
        // Crear las cabeceras de los archivos.
        file_header.FILE_SIZE = or_image_size + 54;
        info_header.IMG_WIDTH = img_width;
        info_header.IMG_HEIGHT = img_height;
        info_header.BITS_PER_PIXEL = 24;
        // Guardar las cabezeras del archivo.
        fwrite((char *)&file_header, sizeof(char), sizeof(file_header), file);
        fwrite((char *)&info_header, sizeof(char), sizeof(info_header), file);
        fseek(file, file_header.OFFSET_DATA, SEEK_SET);
        fwrite(data, sizeof (char) * or_image_size, 1, file);
        fclose(file);
        return true;
    }
    /*
     *  Función que hace el guardado del archivo mediante un nombre especificado.
     *
     * _file_name -> nombre del archivo.
     * 
     */
    bool save_bit(const char *_filename) {
        FILE *file;
        file = fopen(_filename, "wb");
        if(file == NULL)
            return false;
        fwrite(data, sizeof (char) * real_image_size, 1, file);
        fclose(file);
        return true;
    }
};
/*
    Duplica la información de una imagen en otra, creando una copia de la original. Las dos imágenes tienen
    que tener las mismas dimensiones.

    _src -> imagen a duplicar.
    _dst -> copia de la imagen original.
*/
void copy(IMAGE _src, IMAGE _dst) {
    if(_src.get_real_image_size() != _dst.get_real_image_size())
        return;

    for(int i = 0; i < _src.get_real_image_size(); i++)
        _dst.first()[i] = _src.first()[i];
}
/* 
    COnvierte la imagen a escala de grises.

    _src -> imagen a color de 24 bits.
*/
void grayscale(IMAGE _src) {
    /* Solo se convierten imágenes de 24 bits. */
    if(_src.channels() != 3)
        return;

    for(int i = 0; i < _src.get_real_image_size(); i += 3) {
        unsigned char b00 = (_src.first()[i] + _src.first()[i + 1] * 4 + _src.first()[i + 2] * 2) / 7;
        _src.first()[i] = b00;
        _src.first()[i + 1] = b00;
        _src.first()[i + 2] = b00;
    }
}
/*
    Invierte los colores de la imagen.

    _src -> imagen a ajustar. 
*/
void negative(IMAGE _src) {
    for(int i = 0; i < _src.get_real_image_size(); i++)
        _src.first()[i] = 255 - _src.first()[i];
}
/*
    Binariza la immagen, esto convierte en una immagen de dos colores separadas por un umbral.

    _src -> imagen de 8 bits a umbralizar.
    _threshold -> umbral.
*/
void binary_threshold(IMAGE _src, unsigned int _threshold) {
    if(_src.channels() != 1)
        return;
    
    for(int i = 0; i < _src.get_real_image_size(); i++)
        _src.first()[i] = _src.first()[i] > _threshold ? 255 : 0;
}
void truncate_threshold(IMAGE _src, unsigned int _threshold) {
    if(_src.channels() != 1)
        return;
    
    for(int i = 0; i < _src.get_real_image_size(); i++) {
        unsigned char b00 = _src.first()[i];
        _src.first()[i] = b00 > _threshold ? _threshold : b00;
    }
}
void tozero_threshold(IMAGE _src, unsigned int _threshold) {
    if(_src.channels() != 1)
        return;

    for(int i = 0; i < _src.get_real_image_size(); i++) {
        unsigned char b00 = _src.first()[i];
        _src.first()[i] = b00 > _threshold ? b00 : 0;
    }
}
/*
    Obtiene una imagen umbralizada, conservando los colores que están dentro de un intervalo de colores. El
    rango tiene que ser especificado en el espacio de color HSV.

    _src -> imagen de 24 bits a umbralizar.
    _lower_mask -> rango HSV inferior.
    _upper_mask -> rango HSV superior.
*/
void inrange_threshold(IMAGE _src, unsigned char _lower_mask[3], unsigned char _upper_mask[3]) {
    /* Solo se reciben imágenes a color. */
    if(_src.channels() != 3)
        return;

    for(int i = 0; i < _src.get_real_image_size(); i += 3) {
        /* Obtener cada uno de los colores del pixel. */
        float blue = float(_src.first()[i]) / 255.0;
        float green = float(_src.first()[i + 1]) / 255.0;
        float red = float(_src.first()[i + 2]) / 255.0;

        /* Obtener valores mamximos y minimos entre los colores. */
        float value = MAX(red, MAX(green, blue));
        float min = MIN(red, MIN(green, blue));
        float dif = value - min;

        /* Obtner la saturación. */
        float saturation = value != 0.0 ? dif / value : 0.0;

        /* Obtener el valor de la matiz. */
        float hue = 0.0;
        if(value == red)
            hue = (60.0 * (green - blue)) / dif;
        if(value == green)
            hue = 120.0 + (60 * (blue - red)) / dif;
        if(value == blue)
            hue = 240.0 + (60 * (red - green)) / dif;
        hue = hue == 0.0 ? hue + 360.0 : hue;

        /* Normalizar los datos para que estén dentro del rango. */
        unsigned int h2 = hue / 2;
        unsigned int s2 = saturation * 255;
        unsigned int v2 = value * 255;

        /* Obtner el umbral. */
        bool threshold = (h2 >= _lower_mask[0] && h2 <= _upper_mask[0]) &&
                         (s2 >= _lower_mask[1] && s2 <= _upper_mask[1]) && 
                         (v2 >= _lower_mask[2] && v2 <= _upper_mask[2]);

        /* Asignar el umbral al pixel. */
        _src.first()[i] = threshold ? _src.first()[i] : 0;
        _src.first()[i + 1] = threshold ? _src.first()[i + 1] : 0;
        _src.first()[i + 2] = threshold ? _src.first()[i + 2] : 0;
    }
}

/*
    Función que convoluciona una imagen, usando el kernel especificado y una nueva imagen en donde
    se almacena la información nueva.

    _src -> imagen a convolucionar.
    _dst -> imagen nueva donde se almacena la información (tiene que ser del mismo tamaño que la original).
    _kernel -> matriz de convolución.
    _k -> coeficiente de escalado.
*/
void conv(IMAGE _src, IMAGE _dst, const int _kernel[3][3], int _k) {
    /* Validar que el coeficiente no sea cero. */
    if(_k < 1) 
        return;

    /* Validar que la imágenes tengan el mismo tamaño. */
    if(_src.get_real_image_size() != _dst.get_real_image_size())
        return;

    /* Iterador. Este recorre cada pixel de la imagen, por canal y además la matriz convolutiva. */
    for(int y = 1; y < _src.height() - 1; y++) {
        for(int x = 1; x < _src.width() - 1; x++) {
            for(int c = 0; c < _src.channels(); c++) {
                /* Acumulador que obtiene el nuevo valor del pixel. */
                int data = 0;
                for(int i = -1; i < 2; i++) {
                    for(int j = -1; j < 2; j++) {
                        data += _src.at(x + i, y + j, c) * _kernel[i + 1][j + 1];
                    }
                }
                /* Establecer el nuevo valor en el pixel especificado. */
                _dst.at(x, y, c, data / _k);
            }
        }
    }
}

void census(IMAGE _src, IMAGE _dst) {
    /* Verificar que las imágenes tengan las mismas dimensiones. */
    if(_src.get_real_image_size() != _dst.get_real_image_size())
        return;

    /* Verificar que las imágenes tengan un solo canal de color. */
    if(_src.channels() != 1 && _dst.channels() != 1)
        return;

    /* Variable en donde se almacena el nuevo valor de gris. */
    unsigned char gray = 0x00;

    for(int y = 1; y < _src.height(); y++) {
        for(int x = 1; x < _src.width(); x++) {
            /* Recorrer la ventana dentro de la imagen. */
            for(int i = -1; i < 2; i++) {
                for(int j = -1; j < 2; j++) {
                    /* Esta verificación es para que solo se tome el color central
                       la ventana. */
                    if(i == 0 && j == 0) continue;
                    if(_src.at(x + i, y + j, 0) > _src.at(x, y, 0)) gray |= 1; else gray |= 0;
                    gray <<= 1;
                }
            }
            /* Asignar el valor al pixel. */
            _dst.at(x, y, 0, gray);
            gray = 0x00;
        }
    }
}
/*
    Aplica una erosión a la imagen. Esto hace que los elementos en la imagen que tengan valores maximos se 
    vea reducidos en su tamaño.

    _src -> imagen original a erosionar.
    _dst -> imagen donde se va a copiar la información.
    _kernel -> elemento estructurante a usar como ventana.
*/
void erode(IMAGE _src, IMAGE _dst, const int _kernel[5][5]) {
    /* Verificar que las imágenes tengan las mismas dimensiones. */
    if(_src.get_real_image_size() != _dst.get_real_image_size())
        return;

    /* Iterador. Este recorre cada pixel de la imagen, por canal y además la matriz convolutiva. */
    for(int y = 2; y < _src.height() - 2; y++) {
        for(int x = 2; x < _src.width() - 2; x++) {
            for(int c = 0; c < _src.channels(); c++) {
                /* Acumulador que obtiene el nuevo valor del pixel. */
                int minimo = _src.at(x, y, c);
                for(int i = -2; i < 3; i++) {
                    for(int j = -2; j < 3; j++) {
                        if(_kernel[i + 2][j + 2] == 0)
                            continue;
                        minimo = MIN(_src.at(x + i, y + j, c), minimo);
                    }
                }
                /* Establecer el nuevo valor en el pixel especificado. */
                _dst.at(x, y, c, minimo);
            }
        }
    }
}
/*
    Aplica una dilatación a la imagen. Esto hace que los elementos en la imagen que tengan valores máximos 
    se vea incrementados en su tamaño.

    _src -> imagen original a dilatar.
    _dst -> imagen donde se va a copiar la información.
    _kernel -> elemento estructurante a usar como ventana.
*/
void dilate(IMAGE _src, IMAGE _dst, const int _kernel[5][5]) {
    /* Verificar que las imágenes tengan las mismas dimensiones. */
    if(_src.get_real_image_size() != _dst.get_real_image_size())
        return;

    /* Iterador. Este recorre cada pixel de la imagen, por canal y además la matriz convolutiva. */
    for(int y = 2; y < _src.height() - 2; y++) {
        for(int x = 2; x < _src.width() - 2; x++) {
            for(int c = 0; c < _src.channels(); c++) {
                /* Acumulador que obtiene el nuevo valor del pixel. */
                int minimo = _src.at(x, y, c);
                for(int i = -2; i < 3; i++) {
                    for(int j = -2; j < 3; j++) {
                        if(_kernel[i + 2][j + 2] == 0)
                            continue;
                        minimo = MAX(_src.at(x + i, y + j, c), minimo);
                    }
                }
                /* Establecer el nuevo valor en el pixel especificado. */
                _dst.at(x, y, c, minimo);
            }
        }
    }
}
void open(IMAGE _src, IMAGE _dst, const int _kernel[5][5]) {
    erode(_src, _dst, _kernel);
    dilate(_dst, _src, _kernel);
}

void close(IMAGE _src, IMAGE _dst, const int _kernel[5][5]) {
    dilate(_src, _dst, _kernel);
    erode(_dst, _src, _kernel);
}

/*
    Esta webada no queda.
*/
void connected_components(IMAGE _src) {
    /* Verifica que la imagen solo sea de un solo canal de color. */
    if(_src.channels() != 1)
        return;

    cout << "iniciar esto... " << endl;

    // Primera etiqueta
    unsigned char m = 10;
    unsigned int l = 0;

    for(int y = _src.height() - 3; y > 1; y--) {
        for(int x = 2; x < _src.width() - 2; x++) {

            if(_src.at(x, y, 0) == 0)
                continue;

            unsigned char c1 = _src.at(x - 1, y, 0);
            unsigned char c2 = _src.at(x - 1, y + 1, 0);
            unsigned char c3 = _src.at(x, y + 1, 0);
            unsigned char c4 = _src.at(x + 1, y + 1, 0);

            // bool trr = (c3 != 0) | (c4 == 0) | (c1 == 0) & (c2 == 0);

            if(c3 != 0) {
                // cout << "c3: " << c3 << endl;
                _src.at(x, y, 0, c3);
            }
            else if(c1 != 0) {
                //cout << "c1: " << c1 << endl;
                if(c4 != 0) {
                    if(c4 != c1) {
                        _src.at(x - 1, y, 0, c4);
                        _src.at(x, y, 0, c4);
                    }
                    else {
                        _src.at(x, y, 0, c1);
                    }
                }
                else {
                    _src.at(x, y, 0, c1);
                }
            }
            else if(c2 != 0) {
                //cout << "c2: " << c2 << endl;
                if(c4 != 0) {
                    if(c4 != c2) {
                        _src.at(x - 1, y + 1, 0, c4);
                        _src.at(x, y, 0, c4);
                    }
                    else {
                        _src.at(x, y, 0, c1);
                    }
                }
                else {
                    _src.at(x, y, 0, c2);
                }
            }
            else if(c4 != 0) {
                //cout << "c4: " << c4 << endl;
                _src.at(x, y, 0, c4);
            }
            else {
                cout << (int)_src.at(x, y, 0)<< endl;
                cout << "x: " << x << " y: " << y << ": c1 " << (int)c1 << ": c2 " << (int)c2 << ": c3 " << (int)c3 << ": c4 " << (int)c4 << ": "; 
                _src.at(x, y, 0, m);
                m += 1;
                l += 1;
                cout << (int)m << endl;
            }

        }
    }

    cout << "segunda vuelta..." << endl;

/*
    for(int y = _src.height() - 3; y > 1; y--) {
        for(int x = 2; x < _src.width() - 2; x++) {

            if(_src.at(x, y, 0) == 0)
                continue;

            unsigned char c1 = _src.at(x + 1, y, 0);
            unsigned char c2 = _src.at(x + 1, y - 1, 0);
            unsigned char c3 = _src.at(x, y - 1, 0);
            unsigned char c4 = _src.at(x - 1, y - 1, 0);

            // bool trr = (c3 != 0) | (c4 == 0) | (c1 == 0) & (c2 == 0);

            if(c3 != 0) {
                // cout << "c3: " << c3 << endl;
                _src.at(x, y, 0, c3);
            }
            else if(c1 != 0) {
                //cout << "c1: " << c1 << endl;
                if(c4 != 0) {
                    if(c4 != c1) {
                        _src.at(x + 1, y, 0, c4);
                        _src.at(x, y, 0, c4);
                    }
                    else {
                        _src.at(x, y, 0, c1);
                    }
                }
                else {
                    _src.at(x, y, 0, c1);
                }
            }
            else if(c2 != 0) {
                //cout << "c2: " << c2 << endl;
                if(c4 != 0) {
                    if(c4 != c2) {
                        _src.at(x + 1, y - 1, 0, c4);
                        _src.at(x, y, 0, c4);
                    }
                    else {
                        _src.at(x, y, 0, c1);
                    }
                }
                else {
                    _src.at(x, y, 0, c2);
                }
            }
            else if(c4 != 0) {
                //cout << "c4: " << c4 << endl;
                _src.at(x, y, 0, c4);
            }

        }
    }
*/

    cout << "finalizado..." << l << endl;
}

/*
 *  Función que estima el mapa de disparidad entre dos imágenes capturada poruna cámara 
 *  configurada en estereo. Las imagenes tienen que estar rectificadas. Las tres imágenes 
 *  tienen que tener el mismo tamaño.
 *
 *  IMAGE _left -> imagen izquierda.
 *  IMAGE _right -> imagen derecha.
 *  IMAGE _deph -> buffer donde se almacena el mapa de disparidad.
 *  uchar _w -> tamaño de la ventana, por defecto es 7.
 *  uchar _d -> niveles de disparidad, por defecto son 64.
 * 
 */
void match(IMAGE _left, IMAGE _right, IMAGE _deph, unsigned char _w = 7U, unsigned char _d = 64U) {
    if(_left.channels() != 1 && _right.channels() != 1 && _deph.channels() != 1)
        return;

    /* Establecer parametros de cámaras. */
    const double FX = 942.8f;
    const double BASELINE = 54.8f;
    const double SCALE = 0.001f;

    /* Establecer los parametros de búsqueda. */
    const int DISPARITY = _d;
    const int W = _w;
    const int MW = W / 2;

    /* Establecer variables usadas para la búsqueda. */
    double energia = 0.0f;
    double min = 0.0f;
    int absmin = 0;
    
    /* Crear los buffer que contiene las ventanas. */
    double **LEFT_W = new double *[W];
    double **RGHT_W = new double *[W];
    for(int i = 0; i < W; i++) LEFT_W[i] = new double[W] { 0.0 };
    for(int i = 0; i < W; i++) RGHT_W[i] = new double[W] { 0.0 };

    /* Crear el buffer que contiene las disparidades estimadas. */
    double *BUFFER = new double[DISPARITY] { 0x00 };

    for(int x = W; x < _left.width() - W; x++) {
        for(int y = W; y < _left.height() - W; y++) {

            /* Reiniciar los valores que contienen los valores minimos. */
            absmin = 0;
            min = 0;

            /* Obtener el bloque de la imagen izquierda. */
            for(int i = -MW; i <= MW; i++) {
                for(int j = -MW; j <= MW; j++) {
                    LEFT_W[i + MW][j + MW] = (double)_left.at(x + i, y + j, 0);
                }
            }

            /* Obtener los bloque de la imagen derecha, por cada nivel. */
            for(int d = 0; d < DISPARITY; d++) {

                /* Reinciar la variable que contiene el coste estimado. */
                energia = 0.0;

                /* Obtener el bloque de la imagen derecha. */
                for(int i = -MW; i <= MW; i++) {
                    for(int j = -MW; j <= MW; j++) {
                        RGHT_W[i + MW][j + MW] = (double)_right.at(x + i + d, y + j, 0);
                    }
                }

                /* Estimar con la función de coste. */
                for(int i = 0; i < W; i++) {
                    for(int j = 0; j < W; j++) {
                        energia += abs(LEFT_W[i][j] - RGHT_W[i][j]);
                    }
                }
                // Añadir el valor obtenido al buffer.
                BUFFER[d] = energia;
            }

            /* Realizar la buqueda del minimo. */
            min = BUFFER[DISPARITY - 1];
            absmin = 0;
            for(int i = DISPARITY - 2; i > 0; i--){
                min = MIN(min, BUFFER[i]);
                absmin = min < BUFFER[i] ? absmin : i;
            }

            /* Establecerlo en el mapa final. */
            _deph.at(x, y, 0, absmin);
        }
    }

    /* Normalizar la imagen, para que se pueda visualizar. */
    for(int i = 0; i < _deph.get_real_image_size(); i++)
        _deph.first()[i] = ((float)_deph.first()[i] / (float)DISPARITY) * 256;

    delete[] BUFFER;
    delete[] LEFT_W;
    delete[] RGHT_W;
}

/*
 *  Función que estima el mapa de disparidad entre dos imágenes capturada poruna cámara 
 *  configurada en estereo. Las imagenes tienen que estar rectificadas. Las tres imágenes 
 *  tienen que tener el mismo tamaño.
 *
 *  IMAGE _left -> imagen izquierda.
 *  IMAGE _right -> imagen derecha.
 *  IMAGE _deph -> buffer donde se almacena el mapa de disparidad.
 *  uchar _w -> tamaño de la ventana, por defecto es 7.
 *  uchar _d -> niveles de disparidad, por defecto son 64.
 * 
 */
void match2(IMAGE _left , IMAGE _right , IMAGE _deph , unsigned char _w = 7U, unsigned char _d = 64U) {
    if(_left.channels() != 1 && _right.channels() != 1 && _deph.channels()!= 1)
        return;

    const int DISPARITY = _d;
    const int W = _w;
    const int MW = W / 2;
    double energia = 0.0f;
    double min = 0.0f;
    int absmin = 0;
    int left_0 = 0;
    int right_0 = 0;

    double *BUFFER = new double[DISPARITY] { 0x00 };

    for(int x = 0; x < _left.width() - W - 1 ; x++) {
        for(int y = 0; y < _left.height() - W - 1; y++) {
            for(int i = 0; i < W; i++) {
                for(int j = 0; j < W; j++) {
                    for(int d = 0; d < DISPARITY; d++) {
                        left_0 = ((y + j) * _left.width()) + x + i;
                        right_0 = ((y + j) * _left.width()) + x + i + d;
                        energia = abs(_left.first()[left_0] - _right.first()[right_0]);
                        BUFFER[d] += energia;
                    }
                }
            }

            min = BUFFER[0];
            absmin = 0;
            for(int i = 1; i < DISPARITY; i++){
                min = MIN(min, BUFFER[i]);
                absmin = min < BUFFER[i] ? absmin : i;
            }

            _deph.at(x, y, 0, absmin);

            for(int d = 0; d < DISPARITY; d++) {
                BUFFER[d] = 0;
            }
        }
    }

    for(int i = 0; i < _deph.get_real_image_size(); i++)
        _deph.first()[i] = ((float)_deph.first()[i] / (float)DISPARITY) * 256;
    delete[] BUFFER;
}

/*
 *  Función que hace que el rango de colores de la imagen esté entre 
 *  0 y 255.
 *
 *  IMAGE _src -> imagen a normalizar.
 * 
 */
void normalize(IMAGE _src) {
    for(int i = 0; i < _src.get_real_image_size(); i++)
        _src.first()[i] = ((float)_src.first()[i] / (float)64) * 256;
}

unsigned int hamming_distance(unsigned int _x, unsigned int _y) {
    int dist = 0;

    // The ^ operators sets to 1 only the bits that are different
    for (unsigned val = _x ^ _y; val > 0; ++dist) {
        // We then count the bit set to 1 using the Peter Wegner way
        val = val & (val - 1); // Set to zero val's lowest-order 1
    }

    // Return the number of differing bits
    return dist;
}

/*
 *  Función que estima los momentos geometricos, usando una imagen binaria.
 *
 *  IMAGE _src -> imagen binaria.
 * 
 */
float *moments(IMAGE _src) {
    float x = 0.0;
    float y = 0.0;
    float m00 = 0.0;
    float m10 = 0.0;
    float m20 = 0.0;
    float m01 = 0.0;
    float m02 = 0.0;
    float m11 = 0.0;
    float m30 = 0.0;
    float m21 = 0.0;
    float m12 = 0.0;
    float m03 = 0.0;

    for (float i = 0; i < _src.width(); i++) {
        for (float j = 0; j < _src.height(); j++) {
            float b00 = _src.at(i, j, 0);
            m00 += b00 * powf(i, 0.0) * powf(j, 0.0);
            m10 += b00 * powf(i, 1.0) * powf(j, 0.0);
            m20 += b00 * powf(i, 2.0) * powf(j, 0.0);
            m01 += b00 * powf(i, 0.0) * powf(j, 1.0);
            m02 += b00 * powf(i, 0.0) * powf(j, 2.0);
            m11 += b00 * powf(i, 1.0) * powf(j, 1.0);
            m30 += b00 * powf(i, 3.0) * powf(j, 0.0);
            m21 += b00 * powf(i, 2.0) * powf(j, 1.0);
            m12 += b00 * powf(i, 1.0) * powf(j, 2.0);
            m03 += b00 * powf(i, 0.0) * powf(j, 3.0);
        }
    }

    x = m10 / (m00 != 0.0 ? m00 : 1);
    y = m01 / (m00 != 0.0 ? m00 : 1);

    float mu00 = m00;
    float mu10 = 0.0;
    float mu01 = 0.0;
    float mu20 = m20 - (x * m10);
    float mu02 = m02 - (y * m01);
    float mu11 = m11 - (y - m10);
    float mu30 = m30 - ((3.0 * x * m20) + (2.0 * m10 * powf(x, 2.0)));
    float mu12 = m12 - ((2.0 * y * m11) - (x * m02) + (2.0 * powf(y, 2.0) * m10));
    float mu21 = m21 - ((2.0 * x * m11) - (y * m20) + (2.0 * powf(x, 2.0) * m01));
    float mu03 = m03 - ((3.0 * y * m02) + (2.0 * m01 * powf(y, 2.0)));

    float *momentos = new float[29] { 0.0 };
    momentos[0] = (float)mu20 / powf(mu00, 2.0);
    momentos[1] = (float)mu02 / powf(mu00, 2.0);
    momentos[2] = (float)mu11 / powf(mu00, 2.0);
    momentos[3] = (float)mu12 / powf(mu00, 2.5);
    momentos[4] = (float)mu21 / powf(mu00, 2.5);
    momentos[5] = (float)mu30 / powf(mu00, 2.5);
    momentos[6] = (float)mu03 / powf(mu00, 2.5);

    momentos[7] = (float)x;
    momentos[8] = (float)y;
    momentos[9] = (float)m00;
    momentos[10] = (float)m10;
    momentos[11] = (float)m20;
    momentos[12] = (float)m01;
    momentos[13] = (float)m11;
    momentos[14] = (float)m02;
    momentos[15] = (float)m30;
    momentos[16] = (float)m12;
    momentos[17] = (float)m21;
    momentos[18] = (float)m03;

    momentos[19] = (float)mu00;
    momentos[20] = (float)mu10;
    momentos[21] = (float)mu20;
    momentos[22] = (float)mu01;
    momentos[23] = (float)mu11;
    momentos[24] = (float)mu02;
    momentos[25] = (float)mu30;
    momentos[26] = (float)mu12;
    momentos[27] = (float)mu21;
    momentos[28] = (float)mu03;

    /* Terminar y retornar los momentos estimados. */
    return momentos;
}

/*
 *  Función que obtiene los momentos invariantes de Hu.
 *
 *  double[] _momentos -> momentos geometricos previamente calculados.
 * 
 */
float *hu(float *_momentos) {
    /* Inicializar el arreglo. */
    float *hu_moments = new float[7] { 0.0 };

    /* Son los momentos geometricos que se usan. */
    float n20 = _momentos[0];
    float n02 = _momentos[1];
    float n11 = _momentos[2];
    float n12 = _momentos[3];
    float n21 = _momentos[4];
    float n30 = _momentos[5];
    float n03 = _momentos[6];

    /* Estimar los momentos usando las funciones. */
    hu_moments[0] = n20 + n02;
    hu_moments[1] = powf(n20 - n02, 2) + (4 * powf(n11, 2));
    hu_moments[2] = powf(n30 - (3 * n12), 2) + powf((3 * n21) - n03, 2);
    hu_moments[3] = powf(n30 + n12, 2) + powf(n21 + n03, 2);
    hu_moments[4] = ((n30 - (3 * n12)) * (n30 + n12)) * ((powf(n30 + n12, 2) - (3 * powf(n21 + n03, 2)))) + ((((3 * n21) - n03) * (n21 + n03)) * ((3 * powf(n30 + n12, 2)) - powf(n21 + n03, 2)));
    hu_moments[5] = ((n20 - n02) * (powf(n30 + n12, 2) - powf(n21 + n03, 2))) + ((4 * n11) * (n30 + n12) * (n21 + n03));
    hu_moments[6] = (((3 * n21) - n03) * (n30 + n12) * (powf(n30 + n12, 2) - (3 * powf(n21 + n03, 2)))) + (((3 * n12) - n30) * (n21 + n03) * ((3 * powf(n30 + n12, 2)) - powf(n21 + n03, 2)));

    /* Terminar y retornar el arreglo. */
    return hu_moments;
}

/*
 *  Función que aplica una ventana, y asigna el valor medio de esa ventana, 
 *  dando como resultado una imagen suavizada.
 *
 *  IMAGE _src -> Imagen fuente.
 *  IMAGE _dst -> Imagen destino, donde se almacena la imagen resultante.
 *  uchar _w -> Tamaño de la ventana, por defecto es 3.
 * 
 */
void median(IMAGE _src, IMAGE _dst, unsigned char _w = 3U) {
    /* Validar que la imágenes tengan el mismo tamaño. */
    if(_src.get_real_image_size() != _dst.get_real_image_size())
        return;

    /* Variables para el tamaño de la ventana a usar. */
    const unsigned char W = _w;
    const unsigned char MW = W / 2;
    const unsigned char FULL_SIZE = W * W;
    const unsigned char M = (FULL_SIZE / 2) + 1;

    /* Este iterador recorre cada pixel de la imagen aplicando la ventana. */
    for(int y = W; y < _src.height() - W; y++) {
        for(int x = W; x < _src.width() - W; x++) {
            for(int c = 0; c < _src.channels(); c++) {

                /* Vector que contiene todos los elementos de la ventana. */
                unsigned char *data = new unsigned char[FULL_SIZE] { 0x00 };
                unsigned char cont = 0;

                for(int i = -MW; i <= MW; i++) {
                    for(int j = -MW; j <= MW; j++) {
                        data[cont] = _src.at(x + i, y + j, c);
                        cont++;
                    }
                }
                /* Ordenar la lista y obtener el valor central. */
                quick_sort(data, FULL_SIZE);

                /* Establecer el nuevo valor en el pixel especificado. */
                _dst.at(x, y, c, data[M]);

                /* Eliminar el vector creado. */
                delete[] data;
            }
        }
    }
}

/*
void adaptative_threshold(IMAGE _image, IMAGE _new) {
    if(_image.get_real_image_size() != _new.get_real_image_size())
        return;

    if(_image.get_channels() != 1 && _new.get_channels() != 1)
        return;
    
    for(int i = _image.width(); i < _image.get_real_image_size() - _image.width(); i += _image.width()) {
        for(int j = 1; j < _image.width(); j++) {
            int b00 = _image.first()[(i + j) - 1 - _image.width()];
            int b01 = _image.first()[(i + j) + _image.width()];
            int b02 = _image.first()[(i + j) + 1 - _image.width()];
            int b10 = _image.first()[(i + j) - 1];
            int b11 = _image.first()[i + j];
            int b12 = _image.first()[(i + j) + 1];
            int b20 = _image.first()[(i + j) - 1 + _image.width()];
            int b21 = _image.first()[(i + j) + _image.width()];
            int b22 = _image.first()[(i + j) + 1 + _image.width()];

            int T = ((b00 + b01 + b02 + b10 + b11 + b12 + b20 + b21 + b22) / 9) + 10;
            _new.at(i + j, b11 > T ? 255 : 0);
        }
    }
}

אין לך כלום, שיקרת לי

*/