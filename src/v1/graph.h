#ifndef GRAPH_H
#define GRAPH_H



#include <stdio.h>
#include <stdlib.h>



#define MAX_INT 2147483647
#define MIN_INT -2147483648

#define BUF_SIZE 21

#define OUT_WIDTH 1920
#define OUT_HEIGHT 1080
#define OUT_PADDING 100
#define OUT_STROKE_WIDTH 8
#define OUT_FONT_SIZE 35

#define PROPS_SIZE_MAX 1000



typedef struct{
    unsigned int size;
    int min;
    int max;
} Props;

typedef struct{
    unsigned int x1;
    unsigned int x2;
    unsigned int y1;
    unsigned int y2;
} LineCords;



static int int_ceil(double num);
static int int_parse(char *line);
static inline Props init_props(FILE *f);
static void gen_line(LineCords *l, FILE *f, int is_graph);
static void gen_dashed_line(LineCords *l, FILE *f);
static inline void gen_grid(FILE *f);
static inline void gen_constant(FILE *fo);
static inline void gen_polyline(Props *p, FILE *fi, FILE *fo);
static inline void gen_chart(Props *p, FILE *fi, FILE *fo);
void graphs(char *input_file);



#if defined(GRAPH_IMPLEMENTATION)


static int int_ceil(double num){
    return (num - (int)num) != 0 ? (int)num + 1 : (int)num;
}
static int int_parse(char *line){
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

static inline Props init_props(FILE *f){
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

static void gen_line(LineCords *l, FILE *f, int is_graph){
    fprintf(f, "\t<line x1=\"%d\" x2=\"%d\" y1=\"%d\" y2=\"%d\" stroke-width=\"%d\" stroke=\"#%s\"></line>\n",
        l->x1,
        l->x2,
        l->y1,
        l->y2,
        OUT_STROKE_WIDTH,
        is_graph ? "9900ff" : "000000"
    );
}
static void gen_dashed_line(LineCords *l, FILE *f){
    fprintf(f, "\t<line x1=\"%d\" x2=\"%d\" y1=\"%d\" y2=\"%d\" stroke-width=\"%d\" stroke-dasharray=\"20, 20\" stroke=\"#000000\"></line>\n",
        l->x1,
        l->x2,
        l->y1,
        l->y2,
        OUT_STROKE_WIDTH
    );
}
static inline void gen_grid(FILE *f){
    LineCords lc = {
        .x1 = OUT_PADDING,
        .x2 = OUT_PADDING,
        .y1 = OUT_PADDING,
        .y2 = OUT_HEIGHT - OUT_PADDING
    };
    gen_line(&lc, f, 0);

    lc.x1 = OUT_PADDING - OUT_STROKE_WIDTH / 2;
    lc.x2 = OUT_WIDTH - OUT_PADDING;
    lc.y2 = OUT_PADDING;
    gen_dashed_line(&lc, f);

    lc.y1 = OUT_HEIGHT - OUT_PADDING;
    lc.y2 = lc.y1;
    gen_dashed_line(&lc, f);
}

static inline void gen_constant(FILE *fo){
    fprintf(fo, "\n");

    LineCords g = {
        .x1 = OUT_PADDING,
        .x2 = OUT_WIDTH - OUT_PADDING,
        .y1 = OUT_HEIGHT >> 1,
        .y2 = 0
    };
    g.y2 = g.y1;

    gen_line(&g, fo, 1);
}
static inline void gen_polyline(Props *p, FILE *fi, FILE *fo){
    fprintf(fo, "\n\t<polyline\n\t\tpoints=\"\n");

    unsigned int scl_size = 1;
    if(p->size > PROPS_SIZE_MAX){
        double tmp = (double)(p->size) / (double)(PROPS_SIZE_MAX);
        scl_size = int_ceil(tmp);
        p->size = int_ceil((double)p->size / (double)scl_size);
    }

    const double pts_spc = (double)(OUT_WIDTH - OUT_PADDING * 2) / (double)(p->size - 1);
    const double y_hgt = (double)(OUT_HEIGHT - OUT_PADDING * 2);
    double prc = 0;

    char buf[BUF_SIZE] = {};
    double num = 0;

    unsigned int i = 0;
    unsigned int j = 0;
    while(fgets(buf, sizeof(buf), fi)){
        j++;

        num += (double)int_parse(buf);

        if(j >= scl_size){
            num /= (double)scl_size;
            prc = 1.0 - (double)(num - p->min) / (double)(p->max - p->min);

            fprintf(fo, "\t\t\t%f, %f\n",
                OUT_PADDING + i * pts_spc,
                prc * y_hgt + OUT_PADDING
            );

            num = 0;
            i++;
            j = 0;
        }
    }

    fprintf(fo, "\t\t\"\n\t\tstroke-width=\"%d\"\n\t\tstroke=\"#9900ff\"\n\t\tfill=\"none\"\n\t/>\n",
        OUT_STROKE_WIDTH
    );
}

static inline void gen_chart(Props *p, FILE *fi, FILE *fo){
    fprintf(fo, "<svg width=\"%d\" height=\"%d\">\n", OUT_WIDTH, OUT_HEIGHT);

    gen_grid(fo);
    
    fprintf(fo, "\n\t<text x=\"%d\" y=\"%d\" fill=\"#000000\" style=\"font: bold %dpx sans-serif\">[min: %d | max: %d | size: %d]</text>\n",
        OUT_PADDING,
        OUT_PADDING - (OUT_FONT_SIZE / 2),
        OUT_FONT_SIZE,
        p->min,
        p->max,
        p->size
    );

    (p->max == p->min) ? gen_constant(fo) : gen_polyline(p, fi, fo);
    
    fprintf(fo, "</svg>\n");
}

void graph(char *input_file){
    FILE *f_in = fopen(input_file, "rt");
    if(f_in == NULL) exit(1);

    Props p = init_props(f_in);
    if(p.size < 2) exit(1);

    FILE *f_out = fopen("graph.svg", "wt");
    if(f_out == NULL) exit(1);

    gen_chart(&p, f_in, f_out);

    printf("%d\n", 10 >> 1);

    fclose(f_in);
    fclose(f_out);
}


#endif //GRAPHS_IMPLEMENTATION
#endif //GRAPHS_H