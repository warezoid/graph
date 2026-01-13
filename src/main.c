/*

    graph: data from input file transform to svg chart

*/

#include <stdio.h>
#include <stdlib.h>
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

/*
    int_parse:
        - get line string and num pointer
        - convert to int till 0 or 10
            - if not a number (48 - 57) return exit(1) and print error mesagge
            - check for minus
        - set minus
        - return
*/
void int_parse(char *line, int *num){
    int sign = 1;
    *num = 0;

    for(int i = 0; line[i] != 0 && line[i] != '\n'; i++){
        if(line[i] >= '0' && line[i] <= '9'){
            *num = (*num * 10) + (line[i] - '0');
            continue;
        }

        if(line[i] == '-' && sign == 1){
            sign = -1;
            continue;
        }

        printf("int_parse() error: contains invalid chars!\n");
        exit(1);
    }

    *num *= sign;
}


/* line */
void null_line(char *line){
    for(int i = 0; i < LINE_SIZE; i++){
        line[i] = 0;
    }
}

/* dataset 
    - get init dataset and file
    - read till end of file
        - int_parse() line
        - increment size
        - set min and max
        - clear line
    - reset file pointer and return
*/
void init_dataset(Dataset *d, FILE *f){
    int num;
    char line[LINE_SIZE] = {0};
    
    while(fgets(line, sizeof(line), f)){
        int_parse(line, &num);
        
        d->size++;

        if(num > d->max){
            d->max = num;
        }
        else if(num < d->min){
            d->min = num;
        }
    }

    rewind(f);
}

int main(int argc, char **argv){
    FILE *f = fopen(argv[1], "rt");
    assert(f);

    Dataset d = {.size = 0, .min = INT32_MAX, .max = INT32_MIN};
    init_dataset(&d, f);
    
    print_dataset(d);

    return 0;
}

//created by warezoid with the love of freedom and numbers 🧡