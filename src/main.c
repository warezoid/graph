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
    int32_t min;    //can be deleted (mby usecase for some limit functions)
    int32_t max;    //can be deleted (mby usecase for some limit functions)
    int32_t min_anchor;
    int32_t max_anchor;
} Dataset;

typedef struct{
    FILE *input_file;
    FILE *output_file;
    const uint32_t width;
    const uint32_t height;
    const uint32_t padding;
    const uint32_t line_width;
    const uint32_t x_axis_len;
    const uint32_t y_axis_len;
    const uint8_t x_label_count;
    const uint8_t y_label_count;
} Output;



/* development zone */
void print_dataset(Dataset d){
    printf("size: %d, min: %d (%d), max: %d (%d), range: %d\n", d.size, d.min, d.min_anchor, d.max, d.max_anchor, d.range);
}
void print_line(char *line){
    for(int i = 0; i < LINE_SIZE; i++){
        printf("%d ", line[i]);
    }

    printf("\n");
}
void null_line(char *line){
    for(int i = 0; i < LINE_SIZE; i++){
        line[i] = 0;
    }
}



int get_order(int32_t num){
    if(num < 0) num *= -1;
    
    int res = 10;
    while(num % res != num){
        res *= 10;
    }

    return (res /= 10);
}

int get_anchor(int32_t num){
    int sign = 1;
    if(num < 0){
        sign = -1;
        num *= -1;
    }

    int ord = get_order(num);   //order
    int rem = num % ord;    //remaining

    if(rem != 0){
        int ord_hf = ord >> 1;  //half order

        if(rem < ord_hf) ord = ord_hf;

        while(num % ord != 0) num++;
        
        return (num * sign);
    }

    return ((num + ord) * sign);
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
    d->min_anchor = INT32_MAX;
    d->max_anchor = INT32_MIN;

    int num = 0;
    char line[LINE_SIZE] = {0};
    
    while(fgets(line, sizeof(line), f)){
        num = int_parse(line);
        d->size++;
        
        if(num > d->max){
            d->max = num;
        }
        
        if(num < d->min){
            d->min = num;
        }
    }

    d->min_anchor = get_anchor(d->min);
    d->max_anchor = get_anchor(d->max);
    d->range = abs(d->min_anchor) + abs(d->max_anchor);
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

/*
void gen_grid(Dataset *d, Output *o, FILE *f){
    //line
    int x_len = o->width - o->padding * 2;

    Line y_axis = {
        .x1 = o->padding,
        .x2 = o->padding,
        .y1 = o->padding,
        .y2 = o->padding + y_len,
        .line_width = o->line_width,
    };
    gen_line(&y_axis, f);

    //log
    print_dataset(*d);
}
*/

void gen_y_grid(Output *o){
    Line y_axis = {
        .x1 = o->padding,
        .x2 = o->padding,
        .y1 = o->padding,
        .y2 = o->padding + o->y_axis_len,
        .line_width = o->line_width,
    };
    gen_line(&y_axis, o->output_file);
}

void gen_x_grid(Dataset *d, Output *o){
    Line x_axis = {
        .x1 = o->padding,
        .x2 = o->padding + o->x_axis_len,
        .y1 = 0,
        .y2 = 0,
        .line_width = o->line_width / 2,
    };

    int spc = (o->y_axis_len + o->padding) / o->y_label_count;
    for(int i = 0; i < o->y_label_count; i++){
        x_axis.y1 = o->padding + (spc * i);
        x_axis.y2 = x_axis.y1;
        gen_line(&x_axis, o->output_file);
    }
}


/*
    - solve the anchor points
        - never put max and min as anchor points
        - always keep the same parts of x labels -> must change if range is too small
        - if is possible (not max && min > 0 || max && min < 0) -> print 0 label
    - i have to recalc data -> to fit them on y axis
    - scale data to fit them on x axis
*/
void gen_chart(Dataset *d, Output *o){
    fprintf(o->output_file, "<svg width=\"%d\" height=\"%d\">\n", o->width, o->height);

    gen_y_grid(o);

    gen_x_grid(d, o);

    /*
        - this calc the y-position of point in graph -> also this can calc 0 line
        - i need to think about standard for anchor points (min and max)
        - for normal graph (min - | max +) use range = abs(d->min) + abs(d->max)
            - else use abs(d->max) - abs(d->min) -> + works fine | - works wine also ?? -> CHECK
        - ...

    int val = 100;
    const int full_height = 1000;
    const int range = abs(d->max) - abs(d->min);    //use anchor values (top and bottom x lines) abs(d->min) + abs(d->max)
    const double a = (abs(d->max) - val) / (double)range;
    const int res = (int)(a * full_height);
    printf("res: %d\n", res);
    */

    fprintf(o->output_file, "</svg>\n");
}


int main(int argc, char **argv){
    //open input file
    FILE *f_in = fopen(argv[1], "rt");
    assert(f_in);

    //init dataset
    Dataset d;
    init_dataset(&d, f_in);
    
    //open output file
    FILE *f_out = fopen("out.svg", "wt");
    assert(f_out);
    
    //init output
    Output o = {
        .input_file = f_in,
        .output_file = f_out,
        .width = 1920,
        .height = 1080,
        .padding = 100,
        .line_width = 8,
        .x_axis_len = o.width - o.padding * 2,
        .y_axis_len = o.height - o.padding * 2,
        .x_label_count = 0,
        .y_label_count = 10,
    };

    //generate chart
    gen_chart(&d, &o);

    return 0;
}

//created by warezoid with the love of freedom and numbers 🧡