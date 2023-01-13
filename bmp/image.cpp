/**
 * @file    image.cpp
 * @author  Mario Alberto Castro Morgan
 * @brief   Hace el procesamiento de una imagen, recibida como un vector de valores de 8 bits
 *          para su implementación en un FPGA. El codigo está en C.
 * @version 0.1
 * @date    2022-08-03
 * 
 */

#include <iostream>
#include <stdio.h>
#include <cstring>
#include "timer.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define REAL_SIZE_IMAGE 17484
#define GRAY_SIZE_IMAGE 5828

#define WIDTH    62 
#define HEIGTH   94
#define CHANNELS 3

#define WINDOWS_S 3

using namespace std;

unsigned char *img_1 = new unsigned char[REAL_SIZE_IMAGE];
unsigned char *img_2 = new unsigned char[REAL_SIZE_IMAGE];

unsigned char *gry_1 = new unsigned char[GRAY_SIZE_IMAGE];
unsigned char *gry_2 = new unsigned char[GRAY_SIZE_IMAGE];

unsigned char *mdian = new unsigned char[REAL_SIZE_IMAGE];
unsigned char *__hsv = new unsigned char[GRAY_SIZE_IMAGE];

unsigned char lower[3] = { 100, 50,  50 };
unsigned char upper[3] = { 130, 255, 255};

char channels = CHANNELS;

void load(const char *, unsigned char *&);
void save(const char *, unsigned char *);
void grayscale(unsigned char *, unsigned char *);
void median(unsigned char *, unsigned char *);
void rgb2hsv(unsigned char *, unsigned char*);
double *moments(unsigned char *);
double *hu(double *);
void morp(unsigned char *, unsigned char *);
int partition(unsigned char *, int, int);
void quick_sort(unsigned char *, int, int);
void quick_sirt(unsigned char *, int);
unsigned char at(unsigned char *, int, int, int);
void at(unsigned char *, unsigned char, int, int, int);

void load(const char *_filename, unsigned char *&_dst) {
    FILE *file;
    file = fopen(_filename, "rb");
    if(file == NULL) 
        return;
    fread((void *)_dst, sizeof (char) * REAL_SIZE_IMAGE, 1, file);
    fclose(file);
    return;
}

void save(const char *_filename, unsigned char *_src) {
    FILE *file;
    file = fopen(_filename, "wb");
    if(file == NULL) 
        return;
    fwrite(_src, sizeof (char) * GRAY_SIZE_IMAGE, 1, file);
    fclose(file);
    return;
}

int partition(unsigned char* _A, int _p, int _r) {
    unsigned char x = _A[_r];
    int i = _p - 1;
    for(int j = _p; j < _r; j++) {
        if(_A[j] <= x) {
            i += 1;
            swap(_A[i], _A[j]);
        }
    }
    swap(_A[i + 1], _A[_r]);
    return i + 1;
}

void quick_sort(unsigned char* _A, int _p, int _r) {
    if(_p < _r) {
        int q = partition(_A, _p, _r);
        quick_sort(_A, _p, q - 1);
        quick_sort(_A, q + 1, _r);
    }
}

void quick_sort(unsigned char* _A, int _n) {
    quick_sort(_A, 1, _n - 1);
}

unsigned char at(unsigned char *_src, int _x, int _y, int _c) {
    int x = _x * channels;
    int y = _y * channels * WIDTH;
    return _src[x + y + _c];   
}

void at(unsigned char *_dst, unsigned char _value, int _x, int _y, int _c) {
    int x = _x * channels;
    int y = _y * channels * WIDTH;
    _dst[x + y + _c] = _value > 255 ? 255 : _value < 0 ? 0 : _value;
}

void grayscale(unsigned char *_src, unsigned char *_dst) {
    int c = 0;
    for(int i = 0; i < REAL_SIZE_IMAGE; i += 3) {
        unsigned char b00 = (_src[i] + _src[i + 1] * 4 + _src[i + 2] * 2) / 7;
        _dst[c] = b00;
        c++;
    }
}

void median(unsigned char *_src, unsigned char *_dst) {
    const unsigned char W = WINDOWS_S;
    const unsigned char MW = W / 2;
    const unsigned char FULL_SIZE = W * W;
    const unsigned char M = MW + 1;
    for(int y = W; y < HEIGTH - W; y++) {
        for(int x = W; x < WIDTH - W; x++) {
            for(int c = 0; c < CHANNELS; c++) {
                unsigned char *data = new unsigned char[FULL_SIZE] { 0x00 };
                unsigned char cont = 0;
                for(int i = -MW; i <= MW; i++) {
                    for(int j = -MW; j <= MW; j++) {
                        data[cont] += at(_src, x + i, y + j, c);
                        cont++;
                    }
                }
                quick_sort(data, FULL_SIZE);
                at(_dst, data[M],  x, y, c);
                delete[] data;
            }
        }
    }
}

void rgb2hsv(unsigned char *_src, unsigned char *_dst) {
    int c = 0;
    for(int i = 0; i < REAL_SIZE_IMAGE; i += 3) {
        // Obtener cada uno de los colores del pixel.
        double blue  = double(_src[i]) / 255.0;
        double green = double(_src[i + 1]) / 255.0;
        double red   = double(_src[i + 2]) / 255.0;
        // Obtener valores maximos y minimos entre los colores.
        double value = MAX(red, MAX(green, blue));
        double min   = MIN(red, MIN(green, blue));
        double dif   = value - min;

        // Obtener la saturación.
        double saturation = value != 0.0 ? dif / value : 0.0;
        // Obtener el valor de la matiz.
        double hue = 0.0;
        if(value == red)
            hue = (60.0 * (green - blue)) / dif;
        if(value == green)
            hue = 120.0 + (60 * (blue - red)) / dif;
        if(value == blue)
            hue = 240.0 + (60 * (red - green)) / dif;
        hue = hue == 0.0 ? hue + 360.0 : hue;
        // Normalizar los datos para que estén dentro del rango.
        unsigned int h2 = hue / 2;
        unsigned int s2 = saturation * 255;
        unsigned int v2 = value * 255;
        // Obtner el umbral.
        bool threshold = (h2 >= lower[0] && h2 <= upper[0]) &&
                         (s2 >= lower[1] && s2 <= upper[1]) && 
                         (v2 >= lower[2] && v2 <= upper[2]);
        // Asignar el umbral al pixel.
        _dst[c] = threshold ? 255 : 0;
        c++;
    }
    channels = 1;
}

double *moments(unsigned char *_src) {
    double x = 0.0;
    double y = 0.0;
    double m00 = 0.0;
    double m10 = 0.0;
    double m20 = 0.0;
    double m01 = 0.0;
    double m02 = 0.0;
    double m11 = 0.0;
    double m30 = 0.0;
    double m21 = 0.0;
    double m12 = 0.0;
    double m03 = 0.0;

    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGTH; j++) {
            m00 = m00 + (at(_src, i, j, 0) * pow(i, 0) * pow(j, 0));
            m10 = m10 + (at(_src, i, j, 0) * pow(i, 1) * pow(j, 0));
            m20 = m20 + (at(_src, i, j, 0) * pow(i, 2) * pow(j, 0));
            m01 = m01 + (at(_src, i, j, 0) * pow(i, 0) * pow(j, 1));
            m02 = m02 + (at(_src, i, j, 0) * pow(i, 0) * pow(j, 2));
            m11 = m11 + (at(_src, i, j, 0) * pow(i, 1) * pow(j, 1));
            m30 = m30 + (at(_src, i, j, 0) * pow(i, 3) * pow(j, 0));
            m21 = m21 + (at(_src, i, j, 0) * pow(i, 2) * pow(j, 1));
            m12 = m12 + (at(_src, i, j, 0) * pow(i, 1) * pow(j, 2));
            m03 = m03 + (at(_src, i, j, 0) * pow(i, 0) * pow(j, 3));

            x = m10 / (m00 != 0 ? m00 : 1);
            y = m01 / (m00 != 0 ? m00 : 1);
        }
    }

    double mu00 = m00;
    double mu10 = 0.0, mu01 = 0.0;
    double mu20 = m20 - (x * m10);
    double mu02 = m02 - (y * m01);
    double mu11 = m11 - (y - m10);
    double mu30 = m30 - ((3.0 * x * m20) + (2.0 * m10 * pow(x, 2.0)));
    double mu12 = m12 - ((2.0 * y * m11) - (x * m02) + (2.0 * pow(y, 2.0) * m10));
    double mu21 = m21 - ((2.0 * x * m11) - (y * m20) + (2.0 * pow(x, 2.0) * m01));
    double mu03 = m03 - ((3.0 * y * m02) + (2.0 * m01 * pow(y, 2.0)));

    double *momentos = new double[29] { 0.0 };
    momentos[0] = mu20 / pow(mu00, 2);
    momentos[1] = mu02 / pow(mu00, 2);
    momentos[2] = mu11 / pow(mu00, 2);
    momentos[3] = mu12 / pow(mu00, 2.5);
    momentos[4] = mu21 / pow(mu00, 2.5);
    momentos[5] = mu30 / pow(mu00, 2.5);
    momentos[6] = mu03 / pow(mu00, 2.5);

    momentos[7] = x;
    momentos[8] = y;
    momentos[9] = m00;
    momentos[10] = m10;
    momentos[11] = m20;
    momentos[12] = m01;
    momentos[13] = m11;
    momentos[14] = m02;
    momentos[15] = m30;
    momentos[16] = m12;
    momentos[17] = m21;
    momentos[18] = m03;

    momentos[19] = mu00;
    momentos[20] = mu10;
    momentos[21] = mu20;
    momentos[22] = mu01;
    momentos[23] = mu11;
    momentos[24] = mu02;
    momentos[25] = mu30;
    momentos[26] = mu12;
    momentos[27] = mu21;
    momentos[28] = mu03;

    return momentos;
}

double *hu(double *_momentos) {
    // Inicializar el arreglo.
    double *hu_moments = new double[7] { 0.0 };
    // Son los momentos geometricos que se usan.
    double n20 = _momentos[0];
    double n02 = _momentos[1];
    double n11 = _momentos[2];
    double n12 = _momentos[3];
    double n21 = _momentos[4];
    double n30 = _momentos[5];
    double n03 = _momentos[6];
    // Estimar los momentos usando las funciones.
    hu_moments[0] = n20 + n02;
    hu_moments[1] = pow(n20 - n02, 2) + (4 * pow(n11, 2));
    hu_moments[2] = pow(n30 - (3 * n12), 2) + pow((3 * n21) - n03, 2);
    hu_moments[3] = pow(n30 + n12, 2) + pow(n21 + n03, 2);
    hu_moments[4] = ((n30 - (3 * n12)) * (n30 + n12)) * ((pow(n30 + n12, 2) - (3 * pow(n21 + n03, 2)))) + ((((3 * n21) - n03) * (n21 + n03)) * ((3 * pow(n30 + n12, 2)) - pow(n21 + n03, 2)));
    hu_moments[5] = ((n20 - n02) * (pow(n30 + n12, 2) - pow(n21 + n03, 2))) + ((4 * n11) * (n30 + n12) * (n21 + n03));
    hu_moments[6] = (((3 * n21) - n03) * (n30 + n12) * (pow(n30 + n12, 2) - (3 * pow(n21 + n03, 2)))) + (((3 * n12) - n30) * (n21 + n03) * ((3 * pow(n30 + n12, 2)) - pow(n21 + n03, 2)));
    // Terminar y retornar el arreglo.
    return hu_moments;
}

int main() {
    Timer timer;
    double *momentos = new double[29] { 0.0 };
    double *m_hu = new double[7] { 0.0 };
    cout << &img_1 << endl;
    load("ae.bit", img_1);
    load("ae.bit", img_2);

    timer.start();
    grayscale(img_1, gry_1);
    grayscale(img_2, gry_2);
    median(img_1, mdian);
    rgb2hsv(mdian, __hsv);
    momentos = moments(__hsv);
    m_hu = hu(momentos);
    timer.stop();

    save("ee.bit", __hsv);

    for(int i = 0; i < 7; i++) {
        cout << m_hu[i] << endl;
    }

    cout << endl << timer.elapsedSeconds() << endl;
    return 0;
}

/*

    𒀭𒂗𒆤 𒈗 𒆳𒆳𒊏 𒀊𒁀 𒀭𒀭𒌷𒉈𒆤 𒅗 𒄀𒈾𒉌𒋫 𒀭𒊩𒌆𒄈𒋢 𒀭𒇋𒁉 𒆠 𒂊𒉈𒋩
    𒈨𒁲 𒈗𒆧𒆠𒆤 𒅗 𒀭𒅗𒁲𒈾𒋫 𒂠 𒃷 𒁉𒊏 𒆠𒁀 𒈾 𒉈𒆕
    𒍑 𒉺𒋼𒋛 𒄑𒆵𒆠𒆤 𒉆 𒅗𒈠 𒋛𒀀𒋛𒀀𒂠 𒂊𒀝
    𒈾𒆕𒀀𒁉 𒉌𒉻 𒂔 𒉢𒁓𒆷𒆠𒂠 𒉌𒁺
    𒀭𒊩𒌆𒄈𒋢 𒌨𒊕 𒀭𒂗𒆤𒇲𒆤 𒅗 𒋛𒁲𒉌𒋫 𒄑𒆵𒆠𒁕 𒁮𒄩𒊏 𒂊𒁕𒀝
    𒅗 𒀭𒂗𒆤𒇲𒋫 𒊓 𒌋 𒃲 𒉈𒌋 𒅖𒇯𒋺𒁉 𒂔𒈾𒆠 𒁀𒉌𒍑𒍑
    𒂍𒀭𒈾𒁺 𒉺𒋼𒋛 𒉢𒁓𒆷𒆠 𒉺𒄑𒉋𒂵 𒂗𒋼𒈨𒈾 𒉺𒋼𒋛 𒉢𒁓𒆷𒆠𒅗𒆤
    𒂗𒀉𒆗𒇷 𒉺𒋼𒋛 𒄑𒆵𒆠𒁕 𒆠 𒂊𒁕𒋩

    Enlil, rey de todas las tierras, padre de todos los dioses, por su firme mandato, fijó la frontera entre Ningirsu y Šara
    Mesilim, rey de Kiš, por orden de Ištaran, midió el campo y colocó una estela allí.
    Ush, gobernante de Umma, actuó de manera indescriptible.
    Arrancó esa estela y marchó hacia la llanura de Lagaš.
    Ningirsu, guerrero de Enlil, a su justa orden, hizo la guerra con Umma.
    A la orden de Enlil, arrojó su gran red de batalla sobre él y le amontonó túmulos en la llanura.
    Eannatum, gobernante de Lagash, tío de Entemena, gobernante de Lagaš
    arregló la frontera con Enakale, gobernante de Umma
    
    𐤀𐤉𐤔

*/