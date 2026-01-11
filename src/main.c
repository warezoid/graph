/*

    graph: data from input file transform to svg chart

*/

#include <stdio.h>
#include <assert.h>
#include <stdint.h>

#define LINE_SIZE 51

typedef struct{
    uint32_t size;
    int32_t min;
    int32_t max;
} Dataset;



/* development zone */
void print_dataset(Dataset d){
    printf("size: %d, min: %d, max: %d\n", d.size, d.min, d.max);
}

void print_line(char *line){
    for(int i = 0; i < LINE_SIZE; i++){
        printf("%d ", line[i]);
    }

    printf("\n");
}


void int_parse(char *line, int32_t *num){
    int i = 0;
    while(line[i] != 0 && line[i] != 10){
        if(line[i] == '-'){
            printf("NEGATIVE NUMBER!\n");
            *num = 0;
            return;
        }

        if(line[i] >= 48 && line[i] <= 57){
            *num = *num * 10 + (line[i] - 48);
            i++;
        }
        else{
            printf("CONTAINS NON NUMBER CHAR!\n");
            *num = 0;
            return;
        }
    }
}


/* line */
void null_line(char *line){
    for(int i = 0; i < LINE_SIZE; i++){
        line[i] = 0;
    }
}

/* dataset */
void init_dataset(Dataset *d, FILE *f){
    char line[LINE_SIZE] = {0};
    while(fgets(line, sizeof(line), f)){
        d->size++;
        
        //print_line(line);
        int32_t num = 0;
        int_parse(line, &num);
        printf("int: %d | num: %s", num, line);

        null_line(line);
    }
}

int main(int argc, char **argv){
    FILE *f = fopen(argv[1], "rt");
    assert(f);

    Dataset d = {.size = 0, .min = INT32_MIN, .max = INT32_MAX};
    print_dataset(d);

    init_dataset(&d, f);
    print_dataset(d);
    


    return 0;
}

//created by warezoid with the love of freedom and numbers 🧡