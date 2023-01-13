#include <iostream>
#include <stdio.h>
#include <cstring>
#include "timer.h"
#include "bmp.h"

#define FOLDER      "result/%d.bmp"
#define MAX_FRAMES  100

using namespace std;

int main(int argc, char **argv) {
    if (argc != 5) {
        cout << "No hay nada que hacer, me voy alv." << endl;
        return 0;
    }
    char *file_name = argv[1];
    int width = stoi(argv[2]);
    int height = stoi(argv[3]);
    int channels = stoi(argv[4]);
    Timer timer;
    IMAGE src = IMAGE(width, height, channels, file_name);
    src.save("_ss.bmp", img_format::BMP);

    // Terminar el programa y mostrar resultados.s
    cout << "Finalizado" << endl;
    cout << "Tiempo: " << timer.elapsedMilliseconds() << " ms" << endl;
    return 0;
}