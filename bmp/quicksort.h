#include <iostream>
#include <stdio.h>

/* Algoritmo de ordenación Quick Sort */
using namespace std;

int partition(unsigned char* _A, unsigned char _p, unsigned char _r) {
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

void quick_sort(unsigned char* _A, unsigned char _p, unsigned char _r) {
    if(_p < _r) {
        int q = partition(_A, _p, _r);
        quick_sort(_A, _p, q - 1);
        quick_sort(_A, q + 1, _r);
    }
}

void quick_sort(unsigned char* _A, unsigned char _n) {
    quick_sort(_A, 1, _n - 1);
}