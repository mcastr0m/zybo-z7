#include "platform.h"
#include "xparameters.h"
#include "xil_printf.h"
#include "ff.h"
#include "xil_types.h"
#include "xtime_l.h"
#include "xil_exception.h"
#include <stdio.h>
#include <math.h>

// Macros para obtener los valores maximos y minimos entre dos
// numeros.
#define MAX(a, b)   ((a) > (b) ? (a) : (b))
#define MIN(a, b)   ((a) < (b) ? (a) : (b))

// Atributos de la imagen.
#define WIDTH       320
#define HEIGHT      240
#define CHANNELS    1
#define PARTS       2

// Longitudes de los buffer que contiene las imagenes. En bytes.
#define LENGHTx1C   WIDTH * HEIGHT
#define LENGHTx3C   WIDTH * HEIGHT * 3

// Tamano de la ventana.
#define WINDOWS_S   5

// Dimensiones de la red neuronal
#define N_IN        7
#define N_HIDDEN    10
#define N_OUT       1

// Direcciones de las imagenes.
#define IMG_OR_LEFT    0x10000000
#define IMG_OR_RIGHT   0x10038400
#define IMG_GR_LEFT    0x10070800
#define IMG_GR_RIGHT   0x10083400
#define IMG_DEPH       0x10096000
#define IMG_MEDIAN     0x100A8C00

// Direcciones de los registros de memoria.
#define REG_SLV_0   0x00
#define REG_SLV_1   0x01
#define REG_SLV_2   0x02
#define REG_SLV_3   0x03
#define REG_SLV_4   0x04
#define REG_SLV_5   0x05

// Valores que se toman los diferentes estados para controlar
/// el programa principal
#define VOID        0x00000000
#define REINICIO    0x00000001
#define INICIO      0x00000002
#define ESCRITURA   0x00000004
#define LECTURA     0x00000008

// Funciones prototipos de las funciones principales.
u8 mount();
u8 unmount();
u8 load(u8 *_src, const char *_filename, u32 _lenght);
u8 save(u8 *_src, const char *_filename, u32 _lenght);
void swap(u8 *_a, u8 *_b);
u8 partition(u8 _a[], u8 _p, u8 _r);
void quick(u8 _A[], u8 _p, u8 _r);
void quick_sort(u8 _A[], u8 _n);
void median(u8 *_src, u8 *_dst) ;
void grayscale(u8 *_src, u8 *_dst);
void hsv(u8 *_src, u8 *_dst);
void moments(u8 *_src, float _momentos[29]);
void hu(float _momentos[29], float _hu_momentos[7]);
float predict(float _sample[7]);
float mult(float _a[], float _b[], u32 _n);
u8 get_value(u8 *_src, u32 _x, u32 _y);
void set_value(u8 *_dst, u32 _x, u32 _y, u8 _value);
u32 get_ip_value(u8 _register);
void set_ip_value(u8 _register, u32 _value);

/*
################################################################################
# Automatically-generated file. Do not edit!
################################################################################

USER_OBJS :=

LIBS := -Wl,--start-group,-lxil,-lgcc,-lc,-lm,--end-group -Wl,--start-group,-lxilffs,-lxil,-lgcc,-lc,-lm,--end-group -Wl,--start-group,-lrsa,-lxil,-lgcc,-lc,-lm,--end-group
*/
