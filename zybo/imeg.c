/*
 * main.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */
#include "imeg.h"

u32 *baseaddr_p = (u32 *)XPAR_STEREO_MATCH_0_S00_AXI_BASEADDR;

// Buferes donde se estan almacenando la informacion de las
// imagenes.
u8 *img_or_left;
u8 *img_or_right;
u8 *img_gr_left;
u8 *img_gr_right;
u8 *img_deph;
u8 *img_median;

// Nombre de los archivos almacenados en la SD.
static char load_img_left[7] = "lf.bit";
static char load_img_right[7] = "rg.bit";
static char save_deph[7] = "dp.bit";
// static char src_name[7] = "gs.bit";
// Sstatic char dst_name[7] = "gd.bit";

// Mascaras para la extraccion de color.
u8 lower_mask[3] = { 100, 50,  50 };
u8 upper_mask[3] = { 130, 255, 255};

// Vectores de pesos para la predicci�n
float wj[N_HIDDEN][N_IN];
float wk[N_OUT][N_HIDDEN];
float w0[N_HIDDEN + N_OUT];

// Definiciones de los apuntadores para los archivos.
static FATFS fatfs;
static FIL file;

// Funcion para montar la unidad de la SD (SD0).
u8 mount() {
    FRESULT result;
    TCHAR *path = "0:/";
    result = f_mount(&fatfs, path, 0);

    if(result != FR_OK) {
        xil_printf("\n\rNo se pudo montar la unidad %s.", path);
        return 0;
    }

    return 1;
}

// Funcion para desmontar la unidad SD (SD0).
u8 unmount() {
    FRESULT result;
    TCHAR *path = "0:/";
    result = f_mount(NULL, path, 0);

    if(result != FR_OK) {
        xil_printf("\n\rNo se pudo desmontar la unidad %s.", path);
        return 0;
    }

    return 1;
}

// Cargar una imagen de 24 bits desde la SD, en un bufer.
u8 load(u8 *_src, const char *_filename, u32 _lenght) {
    xil_printf("\n\rSe va a abrir el archivo %s de una longitud de %d.", _filename, _lenght);
    FRESULT result;
    UINT num_bytes_read;

    for(int i = 0; i < LENGHTx1C; i++) {
        *(_src + i) = 0x00;
    }

    result = f_open(&file, (char *)_filename, FA_READ);
    if (result != FR_OK) {
        xil_printf("\n\rNo se pudo abrir el archivo %s.", _filename);
        return 0;
    }

    result = f_lseek(&file, 0);
    if(result != FR_OK) {
        xil_printf("\n\rNo se pudo direccionar el archivo %s.", _filename);
        return 0;
    }

    result = f_read(&file, (void*)_src, _lenght, &num_bytes_read);
    if(result != FR_OK) {
        xil_printf("\n\rNo se pudo leer la informacion del archivo %s.", _filename);
        return 0;
    }

    result = f_close(&file);
    if(result != FR_OK) {
        xil_printf("\n\rNo se pudo cerrar el archivo %s.", _filename);
        return 0;
    }

    return 1;
}

// Almacenar una imagen de 8 bits en la SD.
u8 save(u8 *_src, const char *_filename, u32 _lenght) {
    xil_printf("\n\rSe va a abrir el archivo %s de una longitud de %d.", _filename, _lenght);
    FRESULT result;
    UINT num_bytes_write;

    result = f_open(&file, (char *)_filename, FA_WRITE);
    if (result != FR_OK) {
        xil_printf("\n\rNo se pudo abrir el archivo %s.", _filename);
        return 0;
    }

    result = f_lseek(&file, 0);
    if(result != FR_OK) {
        xil_printf("\n\rNo se pudo direccionar el archivo %s.", _filename);
        return 0;
    }

    result = f_write(&file, (const void*)_src, _lenght, &num_bytes_write);
    if(result != FR_OK) {
        xil_printf("\n\rNo se pudo leer la informacion del archivo %s.", _filename);
        return 0;
    }

    result = f_close(&file);
    if(result != FR_OK) {
        xil_printf("\n\rNo se pudo cerrar el archivo %s.", _filename);
        return 0;
    }

    return 1;
}


void swap(u8 *_a, u8 *_b) {
    u8 c = *_a;
    *_a = *_b;
    *_b = c;
}

u8 partition(u8 _A[], u8 _p, u8 _r) {
    u8 x = _A[_r];
    u8 i = _p - 1;
    for(u8 j = _p; j < _r; j++) {
        if(_A[j] <= x) {
            i += 1;
            swap(&_A[i], &_A[j]);
        }
    }
    swap(&_A[i + 1], &_A[_r]);
    return i + 1;
}

void quick(u8 _A[], u8 _p, u8 _r) {
    if (_p >= _r) {
        return;
    }
    u8 q = partition(_A, _p, _r);
    quick(_A, _p, q - 1);
    quick(_A, q + 1, _r);
}

void quick_sort(u8 _A[], u8 _n) {
    quick(_A, 1, _n - 1);
}

void median(u8 *_src, u8 *_dst) {
    u8 W = WINDOWS_S;
    u8 FULL_SIZE = W * W;
    u8 MEDIANA = FULL_SIZE / 2;

    u8 data[FULL_SIZE];
    u8 cont = 0;
    for(u32 y = 0; y < HEIGHT - W - 2; y++) {
        for(u32 x = 0; x < WIDTH - W - 2; x++) {
        	cont = 0;

            for(u8 j = 0; j < W; j++) {
            	for(u8 i = 0; i < W; i++) {
            		data[cont] = get_value(_src, x + i, y + j);
            		cont++;
                }
            }

            quick_sort(data, FULL_SIZE);
            set_value(_dst, x, y, data[MEDIANA]);
        }
    }
}

// Recibe una imagen de 24 bits y produce una imagen de 8 bits.
void grayscale(u8 *_src, u8 *_dst) {
    u32 i = 0;
    u32 j = 0;
    for(i = 0; i < LENGHTx3C; i += 3) {
        _dst[j] = (_src[i] + _src[i + 1] * 4 + _src[i + 2] * 2) / 7;
        j += 1;
    }
}

void hsv(u8 *_src, u8 *_dst) {
    float blue = 0.0;
    float green = 0.0;
    float red = 0.0;
    float value = 0.0;
    float saturation = 0.0;
    float hue = 0.0;
    float dif = 0.0;

    u32 h2 = 0;
    u32 s2 = 0;
    u32 v2 = 0;

    u8 min = 0;
    u8 max = 0;
    u8 threshold = 0;

    u32 i = 0;
    u32 j = 0;

    for(i = 0; i < LENGHTx3C; i += 3) {
        blue = (float)_src[i] / 255.0;
        green = (float)_src[i + 1] / 255.0;
        red = (float)_src[i + 2] / 255.0;

        min = MIN(_src[i], MIN(_src[i + 1], _src[i +2]));
        max = MAX(_src[i], MAX(_src[i + 1], _src[i +2]));

        value = (float)max / 255.0;
        dif = value - ((float)min / 255.0);

        saturation = value != 0.0 ? dif / value : 0.0;

        if(value == red) {
            hue = (60.0 * (green - blue)) / dif;
        }
        else if(value == green) {
            hue = 120.0 + (60 * (blue - red)) / dif;
        }
        else if(value == blue) {
            hue = 240.0 + (60 * (red - green)) / dif;
        }

        hue = hue == 0.0 ? hue + 360.0 : hue;

        hue = hue / 2;
        saturation = saturation * 255.0;
        value = value * 255.0;

        h2 = (u8)hue;
        s2 = (u8)saturation;
        v2 = (u8)value;

        threshold = (h2 >= lower_mask[0] && h2 <= upper_mask[0]) &&
                    (s2 >= lower_mask[1] && s2 <= upper_mask[1]) &&
                    (v2 >= lower_mask[2] && v2 <= upper_mask[2]);

        _dst[j] = threshold == 1 ? 255 : 0;
        j += 1;
    }
}

void moments(u8 *_src, float _momentos[29]) {
    float mx = 0.0;
    float my = 0.0;
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

    u8 p = 0;
    u32 x = 0;
    u32 y = 0;

    for(x = 0; x < WIDTH; x++) {
        for(y = 0; y < HEIGHT; y++) {
            p = get_value(_src, x, y);

            m00 += (float)p * powf(x, 0.0) * powf(y, 0.0);
            m10 += (float)p * powf(x, 1.0) * powf(y, 0.0);
            m20 += (float)p * powf(x, 2.0) * powf(y, 0.0);
            m01 += (float)p * powf(x, 0.0) * powf(y, 1.0);
            m02 += (float)p * powf(x, 0) * powf(y, 2);
            m11 += (float)p * powf(x, 1.0) * powf(y, 1.0);
            m30 += (float)p * powf(x, 3) * powf(y, 0);
            m21 += (float)p * powf(x, 2) * powf(y, 1);
            m12 += (float)p * powf(x, 1) * powf(y, 2);
            m03 += (float)p * powf(x, 0) * powf(y, 3);
        }
    }

    mx = m10 / (m00 != 0 ? m00 : 1);
    my = m01 / (m00 != 0 ? m00 : 1);

    float mu00 = m00;
    float mu10 = 0.0;
    float mu01 = 0.0;
    float mu20 = m20 - (mx * m10);
    float mu02 = m02 - (my * m01);
    float mu11 = m11 - (my - m10);
    float mu30 = m30 - ((3.0 * mx * m20) + (2.0 * m10 * powf(mx, 2.0)));
    float mu12 = m12 - ((2.0 * my * m11) - (mx * m02) + (2.0 * powf(my, 2.0) * m10));
    float mu21 = m21 - ((2.0 * mx * m11) - (my * m20) + (2.0 * powf(mx, 2.0) * m01));
    float mu03 = m03 - ((3.0 * my * m02) + (2.0 * m01 * powf(my, 2.0)));

    _momentos[0] = mu20 / powf(mu00, 2);
    _momentos[1] = mu02 / powf(mu00, 2);
    _momentos[2] = mu11 / powf(mu00, 2);
    _momentos[3] = mu12 / powf(mu00, 2.5);
    _momentos[4] = mu21 / powf(mu00, 2.5);
    _momentos[5] = mu30 / powf(mu00, 2.5);
    _momentos[6] = mu03 / powf(mu00, 2.5);

    _momentos[7] = mx;
    _momentos[8] = my;
    _momentos[9] = m00;
    _momentos[10] = m10;
    _momentos[11] = m20;
    _momentos[12] = m01;
    _momentos[13] = m11;
    _momentos[14] = m02;
    _momentos[15] = m30;
    _momentos[16] = m12;
    _momentos[17] = m21;
    _momentos[18] = m03;

    _momentos[19] = mu00;
    _momentos[20] = mu10;
    _momentos[21] = mu20;
    _momentos[22] = mu01;
    _momentos[23] = mu11;
    _momentos[24] = mu02;
    _momentos[25] = mu30;
    _momentos[26] = mu12;
    _momentos[27] = mu21;
    _momentos[28] = mu03;
}

void hu(float _momentos[29], float _hu_momentos[7]) {
    float n20 = _momentos[0];
    float n02 = _momentos[1];
    float n11 = _momentos[2];
    float n12 = _momentos[3];
    float n21 = _momentos[4];
    float n30 = _momentos[5];
    float n03 = _momentos[6];

    _hu_momentos[0] = n20 + n02;
    _hu_momentos[1] = powf(n20 - n02, 2) + (4 * powf(n11, 2));
    _hu_momentos[2] = powf(n30 - (3 * n12), 2) + powf((3 * n21) - n03, 2);
    _hu_momentos[3] = powf(n30 + n12, 2) + powf(n21 + n03, 2);
    _hu_momentos[4] = ((n30 - (3 * n12)) * (n30 + n12)) * ((powf(n30 + n12, 2) - (3 * powf(n21 + n03, 2)))) + ((((3 * n21) - n03) * (n21 + n03)) * ((3 * powf(n30 + n12, 2)) - powf(n21 + n03, 2)));
    _hu_momentos[5] = ((n20 - n02) * (powf(n30 + n12, 2) - powf(n21 + n03, 2))) + ((4 * n11) * (n30 + n12) * (n21 + n03));
    _hu_momentos[6] = (((3 * n21) - n03) * (n30 + n12) * (powf(n30 + n12, 2) - (3 * powf(n21 + n03, 2)))) + (((3 * n12) - n30) * (n21 + n03) * ((3 * powf(n30 + n12, 2)) - powf(n21 + n03, 2)));
}

float predict(float _sample[7]) {
    float yi[N_HIDDEN];
    float zi[N_OUT];

    for (int j = 0; j < N_HIDDEN; j++) {
        yi[j] = mult(wj[j], _sample, N_HIDDEN) + w0[j];
    }
    for(int k = 0; k < N_OUT; k++) {
        zi[k] = mult(wk[k], yi, N_OUT) + w0[N_HIDDEN + k];
    }

    return zi[0];
}

float mult(float _a[], float _b[], u32 _n) {
    float result = 0.0;
    for(int i = 0; i < _n; i++) {
        result += _a[i] * _b[i];
    }

    return result;
}

// Obtiene un valor de una direccion determinada.
u8 get_value(u8 *_src, u32 _x, u32 _y) {
    u32 x = _x;
    u32 y = _y * WIDTH;
    return _src[x + y];
}

// Establece un valor en una direccion determinada.
void set_value(u8 *_src, u32 _x, u32 _y, u8 _value) {
    u32 x = _x;
    u32 y = _y * WIDTH;
    _src[x + y] = _value;
}

u32 get_ip_value(u8 _register) {
    return *(baseaddr_p + _register);
}

void set_ip_value(u8 _register, u32 _value) {
    *(baseaddr_p + _register) = _value;
}

int main() {
	XTime start;
	XTime end;

    u32 image_index = 0;
    u32 deph_index = 0;
    u8 finished = 0;
    u8 image_part = 0;
    u8 loaded = 0;

    init_platform();

    img_or_left = (u8 *)IMG_OR_LEFT;
    img_or_right = (u8 *)IMG_OR_RIGHT;
    img_gr_left = (u8 *)IMG_GR_LEFT;
    img_gr_right = (u8 *)IMG_GR_RIGHT;
    img_deph = (u8 *)IMG_DEPH;
    img_median = (u8 *)IMG_MEDIAN;

    loaded = mount();
    // En caso de que no haya sido posible montar la unidad, terminar.
    if(!loaded) return 0;

    loaded = load(img_or_left, load_img_left, LENGHTx3C);
    // En caso de que no haya sido posible abrir la primera imagen, terminar.
    if(!loaded) return 0;

    loaded = load(img_or_right, load_img_right, LENGHTx3C);
    // En caso de que no haya sido posible abrir la segunda imagen, terminar.
    if(!loaded) return 0;


    // Inicializar los valores de las imagenes resultantes a 0.
    for(int n = 0; n < LENGHTx1C; n++) {
        *(img_deph + n) = 0;
        *(img_median + n) = 0;
    }

    XTime_GetTime(&start);
    // Convertir ambas a escala de grises.
    grayscale(img_or_left, img_gr_left);
    grayscale(img_or_right, img_gr_right);

    while(image_part < PARTS) {
    	// 1. Transferir ambas imagenes a los bloques de memoria del FPGA.
        for(u32 i = 0; i < (LENGHTx1C / PARTS) + 1600; i++) {
            set_ip_value(REG_SLV_0, ESCRITURA);
            set_ip_value(REG_SLV_2, i);
            set_ip_value(REG_SLV_3, *(img_gr_left + image_index));
            set_ip_value(REG_SLV_4, *(img_gr_right + image_index));
            image_index += 1;
        }

        // 2. Esperar hasta que el proceso termine.
        while(!finished) {
            set_ip_value(REG_SLV_0, INICIO);
            finished = get_ip_value(REG_SLV_1);
        }

        // 3. Recuperar la informacion obtenida.
        for(u32 i = 0; i < (LENGHTx1C / PARTS) + 1600; i++) {
            set_ip_value(REG_SLV_0, LECTURA);
            set_ip_value(REG_SLV_2, i);
            *(img_deph + deph_index) = get_ip_value(REG_SLV_5);
            deph_index += 1;
        }

        // 4. Reinciar los valores a sus estados iniciales.
        while(finished) {
            set_ip_value(REG_SLV_0, REINICIO);
            finished = get_ip_value(REG_SLV_1);
        }

        image_part += 1;
        image_index -= 3200;
        deph_index -= 3200;
    }

    // Applicar un filtro de mediana a el resultado final.
    median(img_deph, img_median);

    XTime_GetTime(&end);
    save(img_median, save_deph, LENGHTx1C);

    xil_printf("\n\r----------FINALIZADO----------");
    printf("\n\rEl proceso tomo %llu ciclos de reloj.", 2 * (end - start));
    printf("\n\rTiempo total:  %.2f us.\n", 1.0 * (end - start) / (COUNTS_PER_SECOND / 1000000));
    return 0;
}