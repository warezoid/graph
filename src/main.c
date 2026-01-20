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
    uint32_t range;
    uint32_t hoe;
    int32_t min;
    int32_t max;
} Dataset;

typedef struct{
    const uint32_t width;
    const uint32_t height;
    const uint32_t padding;
    const uint32_t line_width;
    uint8_t x_label;
} Output;



/* development zone */
void print_dataset(Dataset d){
    printf("size: %d, min: %d, max: %d, range: %d | hoe: %d\n", d.size, d.min, d.max, d.range, d.hoe);
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

/*
    init_dataset:
        - get dataset and file
        - init dataset
        - read till end of file
            - int_parse() line
            - increment size
            - set min and max
            - clear line
        - set range and hoe
        - reset file pointer and return
*/
void init_dataset(Dataset *d, FILE *f){
    d->size = 0;
    d->range = 0;
    d->min = INT32_MAX;
    d->max = INT32_MIN;

    int num;
    char line[LINE_SIZE] = {0};
    
    while(fgets(line, sizeof(line), f)){
        int_parse(line, &num);
        d->size++;
        
        if(num > d->max){
            d->max = num;
        }
        
        if(num < d->min){
            d->min = num;
        }
    }

    uint32_t min_abs = abs(d->min);
    uint32_t max_abs = abs(d->max);
    d->range = min_abs + max_abs;
    (max_abs >= min_abs) ? (d->hoe = max_abs) : (d->hoe = min_abs);
    rewind(f);
}

/*
    gen_grid:
        - get output file and dataset
        - create svg chart grid
            - x and y axis
            - x and y labels and its markers
        - return
*/
typedef struct{
    uint32_t x1;
    uint32_t x2;
    uint32_t y1;
    uint32_t y2;
    uint32_t line_width;
} Line;

void gen_line(Line *l, FILE *f){
    fprintf(
        f,
        "\t<line x1=\"%d\" x2=\"%d\" y1=\"%d\" y2=\"%d\" stroke-width=\"%d\" stroke=\"#000000\"></line>\n",
        l->x1,
        l->x2,
        l->y1,
        l->y2,
        l->line_width
    );
}

//hoe = highest order element
void get_order(int *order, int32_t *hoe){
    while(*hoe % *order != *hoe){
        *order *= 10;
    }

    *order /= 10;
}

void get_anchor_val(int *anchor_val, int *order){
    while(*anchor_val % *order != 0){
        *anchor_val += 1;
    }
}

void gen_grid(Dataset *d, Output *o, FILE *f){
    fprintf(f, "<svg width=\"%d\" height=\"%d\">\n", o->width, o->height);

    //vw
    /*
    int y_mid = (o->height - o->line_width) / 2;
    */
    int x_len = o->width - o->padding * 2;
    int y_len = o->height - o->padding * 2;
    
    //line
    Line y_axis = {
        .x1 = o->padding,
        .x2 = o->padding,
        .y1 = o->padding,
        .y2 = o->padding + y_len,
        .line_width = o->line_width,
    };
    gen_line(&y_axis, f);



    int order = 10;
    get_order(&order, &(d->hoe));
    
    int anchor_max = d->hoe;
    get_anchor_val(&anchor_max, &order);

    const int x = 10;
    //log
    print_dataset(*d);
    printf("newmax+: %d | step: %d | order: %d\n", anchor_max, anchor_max / x, order);
    printf("xlen: %d | ylen: %d\n", x_len, y_len);

    fprintf(f, "</svg>\n");
}

/*
    - solve the anchor points
        - never put max and min as anchor points
        - always keep the same parts of x labels -> must change if range is too small
        - if is possible (not max && min > 0 || max && min < 0) -> print 0 label
    - i have to recalc data -> to fit them on y axis
    - scale data to fit them on x axis

    - ...

*/
void gen_chart(Dataset *d, Output *o, FILE *f){
    int val = 100;
    const int full_height = 1000;

    /*
        - this calc the y-position of point in graph -> also this can calc 0 line
        - i need to think about standard for anchor points (min and max)
        - for normal graph (min - | max +) use range = abs(d->min) + abs(d->max)
            - else use abs(d->max) - abs(d->min) -> + works fine | - works wine also ?? -> CHECK
        - ...
    */
    const int range = abs(d->max) - abs(d->min);    //use anchor values (top and bottom x lines) abs(d->min) + abs(d->max)
    const double a = (abs(d->max) - val) / (double)range;
    const int res = (int)(a * full_height);

    printf("res: %d\n", res);
}


int main(int argc, char **argv){
    FILE *f_in = fopen(argv[1], "rt");
    assert(f_in);

    Dataset d;
    init_dataset(&d, f_in);
    
    FILE *f_out = fopen("out.svg", "wt");
    assert(f_out);

    Output o = {
        .width = 1920,
        .height = 1080,
        .padding = 100,
        .line_width = 8,
        .x_label = 6,
    };

    gen_grid(&d, &o, f_out);

    gen_chart(&d, &o, f_in);

    return 0;
}

//created by warezoid with the love of freedom and numbers 🧡