#include <iostream>
#include <stdio.h>
#include <cstring>
#include "timer.h"
#include "bmp.h"
#include "nn.h"

#define FOLDER      "result/%d.bmp"
#define MAX_FRAMES  100

using namespace std;

unsigned char lower_blue[3] = { 95 , 0,   0  };
unsigned char upper_blue[3] = { 185, 255, 255 };

unsigned char lower_green[3] = { 40, 20, 0   };
unsigned char upper_green[3] = { 70, 60, 255 };

unsigned char lower_red[3] = { 150, 50,  50  };
unsigned char upper_red[3] = { 180, 255, 255 };

int images() {
    Timer timer;
    IMAGE src = IMAGE("f24.bmp");
    IMAGE dst = IMAGE(src.width(), src.height());
    timer.start();
    grayscale(src);
    conv(src, dst, LAPLACIAN3x3, 8);
    timer.stop();
    dst.save("bordes3x3.bmp", img_format::BMP);

    // Terminar el programa y mostrar resultados.s
    cout << "Finalizado" << endl;
    cout << "Tiempo: " << timer.elapsedMilliseconds() << " ms" << endl;
    return 0;
}

int convert() {
    IMAGE src = IMAGE("out.bmp");
    normalize(src);
    src.save("out2.bmp", img_format::BMP);
    
    return 0;
}

int process() {
    Timer timer;
    IMAGE src = IMAGE("stereo/Tsukuba/rgh.bit");
    timer.start();

    inrange_threshold(src, lower_blue, upper_blue);
    
    float *momentos;
    float *hu_momentos;

    momentos = moments(src);
    hu_momentos = hu(momentos);
    
    cout << "Posicion: " << momentos[7] << ", " << momentos[8] << endl;
    
    for(int i = 0; i < 7; i++) {
        cout << hu_momentos[i] << endl;
    }

    timer.stop();

    src.save("src.bmp", img_format::BMP);

    // Terminar el programa y mostrar resultados.s
    cout << "Finalizado" << endl;
    cout << "Tiempo: " << timer.elapsedMilliseconds() << " ms" << endl;
    return 0;
}

int show() {
    Timer timer;
    int count = 0;
    IMAGE src = IMAGE(320, 240, 1, "stereo/Tsukuba/lf.bit");
    for(int i = 0; i < 3840; i++) {
        count++;
        cout << (int)src.first()[i] << ",";
        if(count == 320) {
            cout << endl << endl;
            count = 0;
        }
    }

    // Terminar el programa y mostrar resultados.s
    cout << "Finalizado" << endl;
    cout << "Tiempo: " << timer.elapsedMilliseconds() << " ms" << endl;
    return 0;
}

int figuras() {
    // Crear las constantes que contiene la información para
    // controlar el programa.
    const int N_FIGURAS = 210;
    // Crear el contador
    Timer timer;
    // Crear los buffer que contiene los momentos calculados
    float *momentos;
    float *hu_m;
    // Iniciar el punto critico del programa y contar el tiempo.
    timer.start();
    char images[] = "C:/Users/mario/Desktop/bmp/figuras/basura/mask%d.bmp";
    char file_image_name[55];
    // Recorrer todas lass imágenes que se encuentran en la carpeta. 
    for(int i = 0; i < N_FIGURAS; i++) {
        sprintf(file_image_name, images, i);
        IMAGE src = IMAGE(file_image_name);
        if(src.null())
            continue;
        src.to_8bits();
        momentos = moments(src);
        hu_m = hu(momentos);
        // Mostrar los reultados obtenidos.
        for(int i = 0; i < 7; i++)  cout << hu_m[i] << ", ";
        cout << endl;
    }

    /* Terminar el programa y mostrar resultados. */
    cout << "Finalizado" << endl;
    cout << "Tiempo: " << timer.elapsedMilliseconds() << " ms" << endl;
    return 0;
}

int stereo() {
    // Crear las variables que se van a usar.
    Timer timer;
    IMAGE left = IMAGE("zybo/Books/lf.bmp");
    IMAGE right = IMAGE("zybo/Books/rg.bmp");
    IMAGE dl = IMAGE(left.width(), left.height());
    IMAGE dr = IMAGE(left.width(), left.height());
    IMAGE deph = IMAGE(left.width(), left.height());
    IMAGE dst = IMAGE(left.width(), left.height());

    //median(left, dl, 3);
    //median(right, dr, 3);

    //inrange_threshold(dl, lower_blue, upper_blue);
    //inrange_threshold(dr, lower_blue, upper_blue);

    timer.start();
    grayscale(left);
    grayscale(right);

    dl.to_8bits();
    dr.to_8bits();
    deph.to_8bits();
    dst.to_8bits();

    match(left, right, deph, 3, 64);
    median(deph, dst, 5);
    // open(deph, dst, MORPH_RECT5x5);
    timer.stop();


    // Guardar los resultados.
    deph.save("zybo/Books/dp.bmp", img_format::BMP);
    dst.save("zybo/Books/ds.bmp", img_format::BMP);

    // Terminar el programa y mostrar resultados.
    cout << "Finalizado" << endl;
    cout << "Tiempo: " << timer.elapsedMilliseconds() << " ms" << endl;

    // Eliminar las imágenes
    left.dispose();
    right.dispose();
    deph.dispose();
    dst.dispose();

    // Terminar el programa
    return 0;
}

int nn() {
    // Función para probar la red neuronal. 
    // Variables que definen el numero y las dimensiones de las muestras. K es
    // el número de variables, mientras que N es el número de muestras.
    const unsigned int K = 8;
    const unsigned int N = 90;
    // Función para cargar los datos. Obtiene una matriz de N * K.
    ARRAY samples = load("samples/circulos.data", N, K);
    // shuffle(samples, N);

    NN neural(7, 10, 1);
    neural.write();
    cout << "---------------" << endl;
    neural.train(samples, 30);
    neural.write();
    cout << "Prueba. " << endl;
    ARRAY example(7);
    example.set(0, 0.00133474);
    example.set(1, 1.72729e-005);
    example.set(2, 3.99643e-008);
    example.set(3, 5.34699e-007);
    example.set(4, 3.98299e-014);
    example.set(5, 2.10726e-009);
    example.set(6, -6.72533e-014);
    cout << "--------------" << endl;
    neural.predict(example).writeln();
}

void momentos() {

}

int main() {
    images();
    return 0;
}

