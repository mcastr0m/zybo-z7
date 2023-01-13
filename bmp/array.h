#include "stdio.h"
#include "iostream"
#include "iomanip"
#include "tgmath.h"
#include "time.h"

using namespace std;

struct ARRAY {
public:
    /*
     * Crea un arreglo por defecto de 3 x 3, en la que todos sus elementos son
     * son ceros.
     *
     */
    ARRAY() {
        m = 3; n = 3;
        data = new double *[3];
        for(int i = 0; i < 3; i++)
            data[i] = new double[3] { 0.0 };
    }
    /*
     * Crea un vector en la que se define el número de elementos que va a contener, 
     * y todos sus elementos son ceros.
     * 
     * uint _n -> número de elementos.
     *
     */
    ARRAY(unsigned int _n) {
        m = 1; n = _n;
        data = new double *[1];
        data[0] = new double[_n] { 0.0 };
    }
    /*
     * Crea una matriz de M x N en la que todos sus elementos son ceros.
     *
     * uint _m -> número de filas de la matriz.
     * uint _n -> número de columnas de la matriz.
     *  
     */
    ARRAY(unsigned int _m, unsigned int _n) {
        m = _m; n = _n;
        data = new double *[_m];
        for(int i = 0; i < _m; i++)
            data[i] = new double[_n] { 0.0 };
    }
    /*
     * Obtiene un elemento especificado en el indice i, j.
     *
     */
    double get(unsigned int _i, unsigned int _j) { 
        return data[_i][_j]; 
    } 
    /*
     * Obtiene un elemento especificado en el indice i.
     *
     */
    double get(unsigned int _i) {
        if (m == 1)      return data[0][_i];
        else if (n == 1) return data[_i][0];
        return 0;
    }
    /*
     * Obtiene una fila del arreglo.
     *
     */
    ARRAY getr(unsigned int _i) {
        ARRAY vector(n);
        for(int i = 0; i < n; i++) vector.set(i, data[_i][i]);
        return vector;
    }
    /*
     * Obtiene una columna del arreglo.
     *
     */
    ARRAY getc(unsigned int _i) {
        ARRAY vector(m, 1);
        for(int i = 0; i < m; i++) vector.set(i, data[i][_i]);
        return vector;
    }
    /*
     * Establece un elemento en el indice i, j.
     *
     */
    void set(unsigned int _i, unsigned int _j, double _data) {
        data[_i][_j] = _data;
    }
    /*
     * Establece un elemento en el indice i.
     *
     */ 
    void set(unsigned int _i, double _data) {
        if (m == 1)      data[0][_i] = _data;
        else if (n == 1) data[_i][0] = _data;
    }
    /*
     * Obtiene el numero de filas de la matriz.
     *
     */
    int M() { 
        return m; 
    }
    /*
     * Obtiene el numero de columnas de la matriz.
     *
     */ 
    int N() { 
        return n; 
    }
    /*
     * Obteiene el numero de elementos del arreglo.
     *
     */ 
    int size() { 
        return m * n; 
    }
    /*
     * Escribe en consola el arreglo formateado.
     *
     */
    void write() {
        cout << "[";
        // Vector fila.
        if (m == 1) {
            for (int i = 0; i < n; i++) {
                cout << setprecision(5) << data[0][i];
                if (n - i != 1) cout << ", ";
            }
            cout << "]";
        }
        // Vector columna o matriz.
        else {
            for (int i = 0; i < m; i++) {
                cout << "[";
                for (int j = 0; j < n; j++) {
                    cout << setprecision(5) << data[i][j];
                    if (n - j != 1) cout << ", ";
                }
                cout << "]" << endl;
                if (m - i != 1) cout << " ";
            }
            cout << "]";
        }
    }
    /*
     * Escribe en consola el arreglo formateado con salto de linea al final.
     *
     */
    void writeln() {
        write(); cout << endl;
    }
    /* 
    *  Barajea la información recibida de un vector, esto significa que la información contenida
    *  es desordenada y no crea una lista adicional. El algoritmo empleado es el de Fisher–Yates 
    *  shuffle obtenido de https://en.wikipedia.org/wiki/Fisher–Yates_shuffle.
    * 
    */
    void shuffle() {
        // Iniciar una semilla aleatoria inicial para la generación de números aleatorios.
        srand(time(NULL));
        // Recorrer toda la lista para desordenar la información, el algoritmo usa la función
        // swap para intercambiar los datos de un indice por otro. 
        for(int i = 0; i < m - 2; i++) {
            int j = i + rand() % (m - i);
            swap(data[j], data[i]);
        }
    }
    /*
     * Elimina el arreglo.
     *
     */
    void dispose() {
        delete[] data;
    }
private:
    // Atributos del arreglo.
    int m; int n;
    double **data;
};
/* 
 *  Carga las muestras de un archivo de texto y crear una lista que las contiene. La estrcutura debe ser tal
 *  que cada muestra es cada renglón, y cada variable separadas por comas. La última variable indica la clase 
 *  a la que pertenece. La función retorna una matriz de _n*_k.
 * 
 *  uchar* _filename -> nombre del archivo de texto.
 *  int _n -> número de muestras a tomar. 
 *  int _k -> número de variables a tomar, incluyendo la clase. 
 *
 */
ARRAY load(const char *_filename, const int _n, const int _k) {
    // Crear el vector usando las dimensiones especificadas.
    ARRAY samples = ARRAY(_n, _k);
    // Inicializar el lector del archivo.
    FILE *file;
    // Abrir el archivo en modo lectura.
    file = fopen(_filename, "r");
    // Recorrer las filas del archivo de texto.
    for(int row = 0; row < _n; row++) {
        // Indices para el caracter y la variable. 
        unsigned char i = 0;
        unsigned char j = 0;
        char character = 0;
        // Creación del vector que almacena temporalmente la cadena. 
        char *data = new char[64] { 0x00 };
        // recorre los caracteres de una fila para ir creando cada uno de los
        // datos separados por comas. Esto lo hace hasta el salto de línea. 
        while (character != '\n') {
            character = fgetc(file);
            // Verificar que no sea una coma o salto de linea. Se asume que sigue
            // siendo el mismo dato.
            if(character != ',' && character != '\n') {
                data[i] = character; i++;
            }
            // En caso de que si, terminar la cadena y agregarla al vector.
            else {
                // Si ya está en la última variable, entoces termina el ciclo. 
                if(j == _k) break;
                samples.set(row, j, atof(data));
                // Eliminar la cadena y crear una nueva. 
                data = new char[64] { 0x00 };
                // Incrementar el contador de variable y reiniciar el de caracter. 
                j++; i = 0;
            }
        }
        // Eliminar la cadena.
        delete[] data;
    }
    // Cierra el archivo para terminar la función, y retorna la matriz con las
    // muestras cargada. 
    fclose(file);
    return samples;
}
bool save(){
    return false;
}
/*
 * Obtiene la suma de dos arreglos. Hace la suma de cada uno de los elementos que contiene y 
 * devuelve una nueva matriz.
 * 
 * ARRAY _left -> matriz izquierda.
 * ARRAY _right -> matriz derecha.
 * 
 */
ARRAY sum(ARRAY _left, ARRAY _right) {
    // Verificar que las matrices sean compatibles.
    if(_left.M() != _right.M() &&  _left.N() != _right.N()) {
        cout << "Las matrices son incompatibles." << endl;
        return _left;
    }
    // Realizar la operación y almacenar los resultados en una nueva matriz.
    ARRAY result(_left.M(), _left.N());
    for(int i = 0; i < _left.M(); i++) {
        for(int j = 0; j < _left.N(); j++)
            result.set(i, j, _left.get(i, j) + _right.get(i, j));
    }
    return result;
}
/*
 * Obtiene la diferencia entre dos arreglos. Calcula la diferencia que hay entra cada uno de 
 * los elementos de los arreglos.
 *
 * ARRAY _left -> matriz izquierda.
 * ARRAY _right -> matriz derecha. 
 * 
 */
ARRAY dif(ARRAY _left, ARRAY _right) {
    // Verificar que los arreglos sean compatibles.
    if(_left.M() != _right.M() &&  _left.N() != _right.N()) {
        cout << "Las matrices son incompatibles." << endl;
        return _left;
    }
    // Realizar la operación y almacenar los resultados en una nueva matriz.
    ARRAY result(_left.M(), _left.N());
    for(int i = 0; i < _left.M(); i++) {
        for(int j = 0; j < _left.N(); j++)
            result.set(i, j, _left.get(i, j) - _right.get(i, j));
    }
    return result;
}
/*
 * Obtiene el producto de un escalar y un arreglo. Calcula la mutiplicación de cada
 * elemento del arreglo por el escalar.
 * 
 * double _escalar -> el escalar por el que se va a multiplicar.
 * ARRAY _matriz -> el arreglo que se va a multiplicar.
 *
 */
ARRAY mult(double _escalar, ARRAY _matriz) {
    for(int i = 0; i < _matriz.M(); i++) {
        for(int j = 0; j < _matriz.N(); j++)
            _matriz.set(i, j, _matriz.get(i, j) * _escalar);
    }
    return _matriz;
}
/*
 * Obtiene el producto de dos arreglos. El numero de columnas de la matriz izquierda 
 * tiene que ser el mismo numero de filas de la matris derecha.  
 * 
 * ARRAY _left -> matriz izquierda.
 * ARRAY _right -> matriz derecha. 
 * 
 */
ARRAY mult(ARRAY _left, ARRAY _right) {
    // Verificar que los arreglos sean compatibles.
    if (_left.N() != _right.M()) {
        cout << "Las matrices son incompatibles." << endl;
        return _left;
    }
    ARRAY result(_left.M(), _right.N());
    double escalar = 0;
    for (int i = 0; i < result.M(); i++) {
        for (int j = 0; j < result.N(); j++) {
            // Creación de dos subvectores.
            ARRAY vector_a(1, _left.N());
            ARRAY vector_b(_right.M(), 1);
            // Reiniciar el acumulador.
            escalar = 0;
            // Agregar a los vactores creados, los datos de las matrices de una fila
            // y columna determinada.
            for (int k = 0; k < _left.N(); k++)
                vector_a.set(0, k, _left.get(i, k));
            for (int k = 0; k < _right.M(); k++)
                vector_b.set(k, 0, _right.get(k, j));
            // Realizar la multiplicación de cada uno de los elementos de cada vector y 
            // acumularlos, ese resultado se asigna al elemento i, j de la nueva matriz
            for (int k = 0; k < vector_a.size(); k++) {
                double elemento_numerico_A = vector_a.get(k);
                double elemento_numerico_B = vector_b.get(k);
                escalar += elemento_numerico_A * elemento_numerico_B;
            }
            result.set(i, j, escalar);
        }
    }
    return result;
}

/* Obtiene la matriz minima de una matriz, usando los indices i, j como pivote. */
ARRAY min(ARRAY _matriz, int _i, int _j) {
    /* Verificar que los indices i, j no estpen fuera de los indices de la matriz. */
    if(_i < 0 || _i >= _matriz.M() ||  _j < 0 || _j >= _matriz.N()) {
        cout << "Los indices i, j están fuera de los indices de la matriz." << endl;
        return _matriz;
    }
    /* La matriz minima tiene como nuevo tamaño m - 1, n - 1 de la matriz original. */
    ARRAY matriz_min(_matriz.M() - 1, _matriz.N() - 1);
    int desplazamiento_fila = 0;
    for (int fila_matriz = 0; fila_matriz < _matriz.M(); fila_matriz++) {
        int desplazamiento_columna = 0;
        for (int columna_matriz = 0; columna_matriz < _matriz.N(); columna_matriz++) {
            if (fila_matriz == _i) {
                desplazamiento_fila = 1; continue;
            }
            if (columna_matriz == _j) {
                desplazamiento_columna = 1; continue;
            }
            matriz_min.set(
                fila_matriz - desplazamiento_fila, 
                columna_matriz - desplazamiento_columna, 
                _matriz.get(fila_matriz, columna_matriz)
            );
        }
    }
    return matriz_min;
}
/*
 * Obtiene la transpuesta de la matriz. Consiste en intercambiar las filas por 
 * las columnas.
 *
 */
ARRAY transpose(ARRAY _matriz) {
    ARRAY result(_matriz.N(), _matriz.M());
    for (int i = 0; i < _matriz.M(); i++) {
        for (int j = 0; j < _matriz.N(); j++)
            result.set(j, i, _matriz.get(i, j));
    }
    return result;
}

/* Obtiene el determinante de una matriz. */
double det(ARRAY _matriz) {
    /* Verificar que se trate de una amtriz cuadrada. */
    if(_matriz.M() != _matriz.N()) {
        cout << "La matriz debe de ser cuadrada." << endl;
        return 0.0f;
    }
    /* En caso de que se trate de una matriz de n = 1. */
    if (_matriz.M() == 1) return _matriz.get(0, 0);

    /* En caso de que se trate de una matriz de n = 2. */
    else if (_matriz.M() == 2) {
        double elemento00 = _matriz.get(0, 0);
        double elemento01 = _matriz.get(0, 1);
        double elemento10 = _matriz.get(1, 0);
        double elemento11 = _matriz.get(1, 1);

        /* Se calcula el determinante usando los elementos obtenidos anteriormente, la operacion
         * efectuada es |A| = (a00 * a11) - (a01 * a10)
         */
        return (elemento00 * elemento11) - (elemento01 * elemento10);
    }

    /* En caso de que  se trate de una matriz de n >= 3. */
    else {
        /* Se usa el procedimiento de cofactores para determinar el determinante. */
        int cofactor_matriz_min = 1;
        double determinante = 0;

        /* El ciclo hace la busqueda de las matrices minimas, y obtiene su determinante. */
        for (int columna_matriz = 0; columna_matriz < _matriz.N(); columna_matriz++) {
            double elemento_ij = _matriz.get(0, columna_matriz);
            cofactor_matriz_min = (0 + columna_matriz + 2) % 2 == 0 ? 1 : -1;
            double subdeterminante = det(min(_matriz, 0, columna_matriz));
            determinante += elemento_ij * subdeterminante * cofactor_matriz_min;
        }
        return determinante;
    }
    return 0.0f;
}

/* Obtiene la adjunta de una matriz. */
ARRAY adj(ARRAY _matriz) {
    /* Este procedimiento obtiene la matriz de cofactores primero, luego su transpuesta. */
    ARRAY matriz_adj(_matriz.M(), _matriz.N());
    int cofactor = 1;

    for (int fila_matriz = 0; fila_matriz < _matriz.M(); fila_matriz++) {
        for (int columna_matriz = 0; columna_matriz < _matriz.N(); columna_matriz++) {
            cofactor = (fila_matriz + columna_matriz + 2) % 2 == 0 ? 1 : -1;
            double determinante = det(min(_matriz, fila_matriz, columna_matriz));
            double elemento_ij = determinante * cofactor;
            matriz_adj.set(fila_matriz, columna_matriz, elemento_ij);
        }
    }
    return transpose(matriz_adj);
}

/* Obtiene la inversa de una matriz. */
ARRAY inv(ARRAY _matriz) {
    /* A^-1 = (1 / |A|) * adj(A)^t. La matriz debe de ser cuadrada. */
    if(_matriz.M() != _matriz.N()) {
        cout << "La matriz no es cuadrada." << endl;
        return _matriz;
    }

    /* Obtener el determinate y verificar que no sea cero. */
    double determinante = det(_matriz);
    if (determinante == 0.0) return _matriz;
    double inv_determinante = 1.0f / determinante;

    return mult(inv_determinante, adj(_matriz));
}

bool equals(ARRAY _left, ARRAY _right) {
    if(_left.M() != _right.M() && _left.N() != _right.N())
        return false;
    for(int i = 0; i < _left.M(); i++) {
        for(int j = 0; j < _left.N(); j++) {
            if(_left.get(i, j) != _right.get(i, j)) return false;
        }
    }
    return true;
}