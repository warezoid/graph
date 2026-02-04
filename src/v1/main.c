#include <stdio.h>

#include <stdlib.h>

#define MAX_INT 2147483647
#define MIN_INT -2147483648

#define BUF_SIZE 21



int int_parse(char *line){
    int sign = 1;
    int num = 0;

    for(int i = 0; line[i] != 0 && line[i] != '\n'; i++){
        if(line[i] >= '0' && line[i] <= '9'){
            num = (num * 10) + (line[i] - '0');
            continue;
        }

        if(line[i] == '-' && sign == 1){
            sign = -1;
            continue;
        }

        printf("int_parse() error: contains invalid chars!\n");
        exit(1);
    }

    return num * sign;
}

typedef struct{
    unsigned int size;
    int min;
    int max;
} Props;

Props init_props(FILE *f){
    Props res = {
        .size = 0,
        .min = MAX_INT,
        .max = MIN_INT
    };

    char buf[BUF_SIZE] = {0};
    int num = 0;
    while(fgets(buf, sizeof(buf), f)){
        res.size++;
        num = int_parse(buf);
        if(num > res.max) res.max = num;
        if(num < res.min) res.min = num;
    }

    rewind(f);
    return res;
}

int main(){
    FILE *f_in = fopen("data.txt", "rt");
    if(f_in == NULL) return 1;

    FILE *f_out = fopen("out.svg", "wt");
    if(f_out == NULL) return 1;

    Props p = init_props(f_in);
    printf("%d %d %d\n", p.size, p.min, p.max);

    return 0;
}