#include <stdio.h>

#include <stdlib.h>


typedef struct{
    unsigned int size;
    int min;
    int max;
}d ;

void gen_chart(){}


int main(){
    FILE *f_in = fopen("data.txt", "rt");
    if(f_in == NULL) return 1;

    FILE *f_out = fopen("out.svg", "wt");
    if(f_out == NULL) return 1;

    gen_chart();

    return 0;
}