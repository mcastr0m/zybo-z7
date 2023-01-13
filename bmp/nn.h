#include "stdio.h"
#include "iostream"
#include "tgmath.h"
#include "time.h"
#include "array.h"
#define BIAS    0.5  /* Bias inicial (en desuso).*/
#define ETA     0.1  /* Factor de entrenamiento. */
#define EPOCH   200 /* Numero de epocas para el entrenamiento. */
#define PARAM_A 0.01 /* Parametro alfa.*/
#define PARAM_B 0.01 /* Parametro beta. */
#define THETA   4.0  /* Margen de error. */

using namespace std;
enum activation_function { 
    IDENTITY  = 0,
    SIGNUM    = 1,
    SIGMOID   = 2,
    TANH      = 3,
    GAUSSIAN  = 4,
    RELU      = 5,
    LEAKYRELU = 6  
};

struct NN {
public:
    /*
     *  Constuctor que permite la construcción de una red neuronal, defininendo el número de neuronas 
     *  que la componen.
     * 
     *  uchar _input -> número de neuronas de entrada.
     *  uchar _hidden -> número de neuronas ocultas.
     *  uchar _output -> número de neuronas de salida.
     * 
     */
    NN(unsigned char _input = 5U, unsigned char _hidden = 10U, unsigned char _output = 2U) {
        // Crear los nodes que va a contener.
        wj = ARRAY(_hidden, _input);
        wk = ARRAY(_output, _hidden);
        // Establecer el total de sesgos que contiene la red.
        w0 = ARRAY(_hidden + _output);
        // Establecer las dimensiones de la red.
        n_wi = _input;
        n_wj = _hidden;
        n_wk = _output;
        /* Establcer la función de activación por defecto. */
        a_function = activation_function::IDENTITY;
        /* Inicializar los pesos a valores por defecto. */
        starting_weights();
    }
    /*
     *  Contructor que carga una red neuronal desde un archivo de texto.
     *
     *  uchar* _filename -> nombre del archivo.
     * 
     */
    NN(const char *_filename) {
        load(_filename);
    }
    /*
     *  Funcion que carga una red neuronal desde un archiov de texto. Esta función
     *  crea los vectores de la red.
     * 
     */
    void load(const char *_filename) {
        return;
    }
    /*
     *  Funcion que guarda una red neuronal.
     *
     */
    void save(const char *_filename) {
        return;
    }
    /*
     *  Funcion que entrana la red usando el algoritmo de backpropagation. El algoritmo
     *  obtiene el nuevo valor de los pesos para que pueda se usada despues para la 
     *  predicción.
     * 
     *  double** _samples-> todas las muestras.
     *  int _n -> número de muestras.
     * 
     */
    void train(ARRAY _samples, unsigned int _n) {
        /* Variable que mantiene el nivel de error de la red. */
        double Jw = 100.0;

        /* Metodo de retropropagación por lotes, en el que se entrena usando epocas. */
        for (int e = 0; (e < EPOCH && Jw > THETA); e++) {
            ARRAY Dwij(n_wi, n_wj);
            ARRAY Dwjk(n_wj, n_wk);
            // Inicializar esta wea.
            Jw = 0.0;
            // Hacer el recorrido de la lista de muestras, y tomar cada una.
            for (int i = 0; i < _n; i++) {
                // Tomar una muestra de la lista de muestras.
                ARRAY xi(n_wi);
                // Obtiene la respuesta, la cual está almacenada en la última columna (n_wi).
                double correct = _samples.get(i, n_wi);
                for (int j = 0; j < n_wi; j++) xi.set(j, _samples.get(i, j));
                // Creación de las variables de sensibilidad.
                ARRAY sk(n_wk);
                ARRAY yi(n_wj);
                ARRAY yi_net(n_wj);
                ARRAY zi(n_wk);
                ARRAY zi_net(n_wk);
                // Obtener los valores de las capas ocultas.
                for (int j = 0; j < n_wj; j++) {
                    double ji = mult(wj.getr(j), transpose(xi)).get(0) + w0.get(j);
                    yi.set(j, f(ji)); yi_net.set(j, ji);
                }
                // Obtener los valores de las capas de salida.
                for(int j = 0; j < n_wk; j++) {
                    double ki = mult(wk.getr(j), transpose(yi)).get(0) + w0.get(n_wj + j);
                    zi.set(j, f(ki)); zi_net.set(j, ki);
                }
                // Hacer el ajuste de los pesos de los nodos de salida.
                for(int j = 0; j < n_wk; j++) {
                    sk.set(j, (correct - zi.get(j)) * df(zi_net.get(j)));
                    for(int k = 0; k < n_wj; k++)
                        Dwjk.set(k, j, Dwjk.get(k, j) + (ETA * sk.get(j) * yi.get(k)));
                }
                // Hacer el ajuste de los pesos de los nodos ocultos.
                for(int j = 0; j < n_wj; j++) {
                    double sj = 0.0;
                    for(int k = 0; k < n_wk; k++) sj += wk.getc(k).get(j) * sk.get(k);
                    sj *= df(yi_net.get(j));
                    for(int k = 0; k < n_wi; k++) 
                        Dwij.set(k, j, Dwij.get(k, j) + (ETA * sj * xi.get(k)));
                }
                // Acumular el error.
                for(int j = 0; j < n_wk; j++) { Jw += (0.5) * pow(correct - zi.get(j), 2); }
            }
            for(int i = 0; i < n_wj; i++) {
                for(int j = 0; j < n_wi; j++) wj.set(i, j, wj.get(j, i) + Dwij.get(j, i));
            }
            for(int i = 0; i < n_wk; i++) {
                for(int j = 0; j < n_wj; j++) wk.set(i, j, wk.get(i, j) + Dwjk.get(j, i));
            }
        }
    }
    /* 
     *  Función para hacer la predicción de un vector de datos de entrada.
     *
     *  double* _sample-> vector de datos, cada elemento es una variable.
     *
     */
    ARRAY predict(ARRAY _sample) {
        ARRAY yi = ARRAY(1, n_wj);
        ARRAY zi = ARRAY(1, n_wk);
        for (int j = 0; j < n_wj; j++) {
            double ji = mult(wj.getr(j), transpose(_sample)).get(0) + w0.get(j);
            yi.set(j, f(ji));
        }
        for(int j = 0; j < n_wk; j++) {
            double ki = mult(wk.getr(j), transpose(yi)).get(0) + w0.get(n_wj + j);
            zi.set(j, f(ki));
        }
        return zi;
    }
    /* 
     *  Obtiene el vector de los pesos de las neuronas ocultas. 
     *
     */
    ARRAY get_wj() { 
        return wj; 
    }
    /* 
     *  Obtiene el vector de los pesos de las neuronas de salida. 
     *
     */
    ARRAY get_wk() { 
        return wk; 
    }
    /*  
     *  Obtiene el vector de sesgos. Contiene en total los sesgos de las neuronas ocultas y de 
     *  salida. 
     *
     */
    ARRAY get_w0() { 
        return w0; 
    }
    /* 
     * Obtiene la función de activación establecida para esta red. 
     *
     */
    activation_function get_activation_function() { 
        return a_function; 
    }
    /*
     *  Obtiene el número de neuronas de entrada.
     *
     */
    unsigned char get_n_wi() {
        return n_wi;
    }
    /*
     *  Obtiene el número de neuronas ocultas.
     *
     */
    unsigned char get_n_wj() {
        return n_wj;
    }
    /*
     *  Obtiene el número de neuronas de salida.
     *
     */
    unsigned char get_n_wk() {
        return n_wk;
    }
    /*
     *  Muestra los pesos que contiene la red.
     *
     */
    void write() {
        cout << "wj" << endl;
        // Desplegar los pesos de las neuronas de entrada a las ocultas.
        for (int i = 0; i < n_wj; i++) {
            for (int j = 0; j < n_wi; j++) cout << wj.get(i, j) << ", ";
            cout << endl;
        }
        // Desplegar los pesos de las neuronas ocultas a las de salida.
        cout << "--------" << endl << "wk" << endl;
        for (int i = 0; i < n_wk; i++) {
            for (int j = 0; j < n_wj; j++) cout << wk.get(i, j) << ", ";
            cout << endl;
        }
        // Desplegar los pesos de los sesgos. 
        cout << "--------" << endl << "w0" << endl;
        for(int i = 0; i < n_wj + n_wk; i++) cout << w0.get(i) << ", ";
    }
    /*
     *  Elimina la red neuronal.
     *
     */
    void dispose() {
        wj.dispose();
        wk.dispose();
        w0.dispose();
    }
private:
    /* 
     *  Funcion que selecciona la funcion de activación establecida de la red. 
     *
     */
    double f(double _x) {
        switch (a_function) {
            case activation_function::IDENTITY:
                return identity(_x);
            case activation_function::SIGNUM:
                return signum(_x);
            case activation_function::SIGMOID:
                return sigmoid(_x);
            case activation_function::TANH:
                return tanh(_x);
            case activation_function::GAUSSIAN:
                return gaussian(_x);
            case activation_function::RELU:
                return relu(_x);
            case activation_function::LEAKYRELU:
                return leakyrelu(_x);
            default:
                return _x;
        }
    }
    /*  
     *  Funcón que selecciona la derivada de la funcion de activacion establecida
     *  en la red.
     *  
     */
    double df(double _x) {
        switch (a_function) {
            case activation_function::IDENTITY:
                return derivative_identity(_x);
            case activation_function::SIGNUM:
                return derivative_signum(_x);
            case activation_function::SIGMOID:
                return derivative_sigmoid(_x);
            case activation_function::TANH:
                return derivative_tanh(_x);
            case activation_function::GAUSSIAN:
                return derivative_gaussian(_x);
            case activation_function::RELU:
                return derivative_relu(_x);
            case activation_function::LEAKYRELU:
                return derivative_leakyrelu(_x);
            default:
                return _x;
        }
    }
    /* 
     *  Función de activacion de identidad. 
     *
     */
    double identity(double _x) { 
        return _x; 
    }
    /* 
     *  Derivada de la función de activación de identidad. 
     *
     */
    double derivative_identity(double _x) { 
        return 1.0;
    }
    /* 
     *  Función de activación de paso binario. 
     *
     */
    double signum(double _x) { 
        return _x >= 0 ? 1.0 : -1.0; 
    }
    /* 
     *  Derivada de la función de activación de paso binario, devuelve 0.
     *
     */
    double derivative_signum(double _x) { 
        return 0.0; 
    }
    /* 
     *  Función de acivación sigmoide. 
     *
     */
    double sigmoid(double _x) { 
        return 1.0 / (1.0 + exp(-_x)); 
    }
    /* 
     *  Derivada de la función de activación sigmoide. 
     *
     */
    double derivative_sigmoid(double _x) { 
        return sigmoid(_x) * (1 - sigmoid(_x)); 
    }
    /* 
     *  Función de activación tangente hiperbolica. 
     *
     */
    double tanh(double _x) { 
        return (2.0 / (1 + exp(-2.0 * _x))) - 1.0; 
    }
    /* 
     *  Derivada de la función de activación tangente hiperbolica. 
     *
     */
    double derivative_tanh(double _x) { 
        return 1 - pow(tanh(_x), 2); 
    }
    /* 
     *  Función de activación gaussiana (no implementada, devuelve 0).
     *
     */
    double gaussian(double _x) { 
        return 0.0; 
    }
    /* 
     *  Derivada de la función de actvación gaussiana (no implementada, devuelve 0). 
     *
     */
    double derivative_gaussian(double _x) { 
        return 0.0; 
    }
    /* 
     *  Función de activación RELU. 
     *
     */
    double relu(double _x) { 
        return _x < 0 ? 0.0 : _x; 
    }
    /* 
     *  Derivada de la función de activación RELU. 
     *
     */
    double derivative_relu(double _x) { 
        return _x < 0 ? 0.0 : 1.0; 
    }
    /* 
     *  Función de activación RELU con fugas. 
     *
     */
    double leakyrelu(double _x) { 
        return _x < 0 ? PARAM_A * _x : _x; 
    }
    /* 
     *  Derivada de la función de activación RELU con fugas. 
     * 
     */
    double derivative_leakyrelu(double _x) { 
        return _x < 0 ? PARAM_A : _x; 
    }
    /* 
     *  Función que obtiene los valores aleatorios para los pesos de la red. Los 
     *  vectores ya están inicializados desde el momento que la red fue creada. 
     *
     */
    void starting_weights() {
        srand(time(NULL));
        // Pesos de los sesgos.
        for (int i = 0; i < n_wj + n_wk; i++) w0.set(i, rand01());
        // Pesos de las neuronas de entrada a las ocultas.
        for (int i = 0; i < n_wj; i++) {
            for (int j = 0; j < n_wi; j++) wj.set(i, j, rand01());
        }
        // Pesos de las neuronas ocultas a las de salida. 
        for (int i = 0; i < n_wk; i++) {
            for (int j = 0; j < n_wj; j++) wk.set(i, j, rand01());
        }
    }
    /* 
     *  Funcion que obtiene un numero pseudoaleatorio con rango de entre 0 y 1. 
     *  Tambien genera números negativos.  
     *
     */
    double rand01() {
        double r = (double)(rand() / (double)INT16_MAX);
        if (rand() % 2 == 0) return -1.0 * r;
        else return r;
    }
    /*
     *  Vector de pesos de las neuronas de entrada a las ocultas. Cada uno de los elementos
     *  es un vector que conecta por cada nodo de entrada.
     * 
     */
    ARRAY wj;
    /*
     *  Vector de pesos de las neuronas ocultas a las de salida. Cada uno de los elementos
     *  es un vector que conecta por cada nodo oculto.
     * 
     */
    ARRAY wk;
    /*
     *  Vector de sesgos.
     *
     */
    ARRAY w0;
    /*
     *  Número de neuronas de entrada.
     *
     */
    unsigned char n_wi;
    /*
     *  Número de neuronas ocultas. 
     *
     */
    unsigned char n_wj;
    /*
     *  Número de neuronas de salida. 
     *
     */
    unsigned char n_wk;
    /*
     *  Función de activación de la red.
     *
     */
    activation_function a_function;
};

/* Parte una lista de vectores y crea una nueva lista de vectores, seleccionando los elementos en
 * un intervalo definido.
 */
/*
void split(ARRAY *_original,  ARRAY *&_samples, ARRAY *&_testing, int _n, int _ns, int _nt, int _k, int _c) {
    int ns = _ns / _c;
    int nt = _nt / _c;
    ARRAY count_samp = ARRAY(1, _c, 0);
    ARRAY count_test = ARRAY(1, _c, 0);
    int j = 0, k = 0;
    for(int i = 0; i < _n; i++) {
        if(count_samp.get(_original[i].get(0) - 1) < ns) {
            _samples[j] = ARRAY(1, _k);
            for(int ii = 0; ii < _k; ii++) _samples[j].set(ii, _original[i].get(ii));
            j++;
            count_samp.set(_original[i].get(0) - 1, count_samp.get(_original[i].get(0) - 1) + 1);
        }
        else if(count_test.get(_original[i].get(0) - 1) < nt) {
            _testing[k] = ARRAY(1, _k);
            for(int ii = 0; ii < _k; ii++) _testing[k].set(ii, _original[i].get(ii));
            k++;
            count_test.set(_original[i].get(0) - 1, count_test.get(_original[i].get(0) - 1) + 1);
        }
    }
}
*/

/* Reemplaza imformación incompleta o faltante de una lista de datos, por el de los vecinos mas
   próximos, usando la media. */
/*
void replace(ARRAY &_vector) {
    double double_sigma = 2 * var(_vector);
    double media = mean(_vector);
    double x_min = media - double_sigma;
    double x_max = media + double_sigma;
    for(int i = 0; i < _vector.size(); i++) {
        if(_vector.get(i) < x_min || _vector.get(i) > x_max) {
            double x = 0.0;
            if(i == 0)
                x = (_vector.get(i + 1) + _vector.get(_vector.size() - 1)) / 2.0;
            else if(i == _vector.size() - 1)
                x = (_vector.get(i - 1) + _vector.get(0)) / 2.0;
            else
                x = (_vector.get(i - 1) + _vector.get(i + 1)) / 2.0;
            _vector.set(i, x);
        }
    }
}
*/
/* Normaliza los datos de un vector para que estén en el mismo intervalo de valores. */
/*
void normalize(ARRAY &_vector) {
    double media = mean(_vector);
    double desv = stnd(_vector);
    for(int i = 0; i < _vector.size(); i++)
        _vector.set(i, (_vector.get(i) - media) / desv);
}
*/
/* Reduce el número de caracteristicas de un conjunto de muestras. */
/*
ARRAY *reduce(ARRAY *_samples, int _n, int &_k) {
    int selected = 0;
    ARRAY means = ARRAY(1, _k);
    ARRAY stnds = ARRAY(1, _k);
    ARRAY deleted = ARRAY(1, _k);
    for(int i = 0; i < _k; i++) {
        means.set(i, mean(_samples[i]));
        stnds.set(i, stnd(_samples[i]));
    }
    for(int i = 1; i < _k; i++) {
        for(int j = i + 1; j < _k; j++) {
            double p_ab = 0.0;
            for(int k = 0; k < _n; k++)
                p_ab += ((_samples[i].get(k) - means.get(i)) * (_samples[j].get(k) - means.get(j))) / (stnds.get(i) * stnds.get(j));
            p_ab *= 1.0 / ((double)_n - 1.0);
            if(abs(p_ab) >= 0.75) {
                deleted.set(i, 1.0);
                i++; selected++;
            }
        }
    }
    selected = _k - selected;
    ARRAY *new_samples = new ARRAY[selected];
    cout << "Caracteristicas eliminadas: ";
    deleted.writeln();
    int j = 0;
    for(int i = 0; i < _k; i++) {
        if(deleted.get(i) != 1.0) {
            new_samples[j] = ARRAY(1, _n);
            for(int k = 0; k < _n; k++) new_samples[j].set(k, _samples[i].get(k));
            j++;
        }
    }
    _k = selected;
    return new_samples;
}
void PCA(ARRAY *&_samples, int _n, int _k) {
    ARRAY eigenv = eigenvalues();
    ARRAY *eigenx = eigenvector();
    ARRAY *new_samples = new ARRAY[_n];

    int k = 4;
    for(int i = 0; i < _n; i++) {
        new_samples[i] = ARRAY(1, k + 1);
        new_samples[i].set(0, _samples[i].get(0));
        for(int j = 0; j < k; j++) {
            ARRAY vector = ARRAY(1, _samples[i].N() - 1);
            for(int ii = 1; ii < _samples[i].N(); ii++) vector.set(ii - 1, _samples[i].get(ii));
            new_samples[i].set(j + 1, mult(vector, eigenx[j]).get(0));
        }
    }
    for(int i = 0; i < _n; i++) {
        _samples[i] = ARRAY(1, k + 1);
        for(int j = 0; j < k + 1; j++) _samples[i].set(j, new_samples[i].get(j));
    }
}
*/
/* Función que hace el preprocesado de las muestras de entrada. */
/*
void preprocess(ARRAY *&_samples, int _n, int &_k) {
    ARRAY *new_samples = new ARRAY[_k];
    for (int i = 0; i < _k; i++) {
        new_samples[i] = ARRAY(1, _n);
        for(int j = 0; j < _n; j++) new_samples[i].set(j, _samples[j].get(i));
        /* Ignorar el primer vector, es el de la clase.
        if(i != 0) replace(new_samples[i]);
    }
    new_samples = reduce(new_samples, _n, _k);
    for(int i = 1; i < _k; i++) normalize(new_samples[i]);
    /* Regresar la lista a su estructura normal.
    _samples = new ARRAY[_n];
    for (int i = 0; i < _n; i++) {
        _samples[i] = ARRAY(1, _k);
        for(int j = 0; j < _k; j++) _samples[i].set(j, new_samples[j].get(i));
    }
    shuffle(_samples, _n);
    PCA(_samples, _n, _k);
}
*/