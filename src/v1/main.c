#include <stdio.h>

#include <stdlib.h>

//program constants
#define MAX_INT 2147483647
#define MIN_INT -2147483648

//string constants
#define BUF_SIZE 21

//output constants
#define OUT_WIDTH 1920
#define OUT_HEIGHT 1080
#define OUT_PADDING 100
#define OUT_STROKE_WIDTH 8



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



typedef struct{
    unsigned int x1;
    unsigned int x2;
    unsigned int y1;
    unsigned int y2;
} LineCords;

void gen_line(LineCords *l, FILE *f){
    fprintf(f, "\t<line x1=\"%d\" x2=\"%d\" y1=\"%d\" y2=\"%d\" stroke-width=\"%d\" stroke=\"#000000\"></line>\n",
        l->x1,
        l->x2,
        l->y1,
        l->y2,
        OUT_STROKE_WIDTH
    );
}

void gen_dashed_line(LineCords *l, FILE *f){
    fprintf(f, "\t<line x1=\"%d\" x2=\"%d\" y1=\"%d\" y2=\"%d\" stroke-width=\"%d\" stroke-dasharray=\"20, 20\" stroke=\"#000000\"></line>\n",
        l->x1,
        l->x2,
        l->y1,
        l->y2,
        OUT_STROKE_WIDTH
    );
}

void gen_grid(FILE *f){
    //y axis
    LineCords lc = {
        .x1 = OUT_PADDING,
        .x2 = OUT_PADDING,
        .y1 = OUT_PADDING,
        .y2 = OUT_HEIGHT - OUT_PADDING
    };
    gen_line(&lc, f);

    //max axis
    lc.x1 = OUT_PADDING - OUT_STROKE_WIDTH / 2;
    lc.x2 = OUT_WIDTH - OUT_PADDING;
    lc.y2 = OUT_PADDING;
    gen_dashed_line(&lc, f);

    //min axis 
    lc.y1 = OUT_HEIGHT - OUT_PADDING;
    lc.y2 = lc.y1;
    gen_dashed_line(&lc, f);
}

/*
    x axis -> scaling
    y axis -> ploting
*/
void gen_polyline(Props *p, FILE *fi, FILE *fo){
    fprintf(fo, "\n\t<polyline\n\t\tpoints=\"\n");

    const double pts_spc = (double)(OUT_WIDTH - OUT_PADDING * 2) / (double)(p->size - 1);

    char buf[BUF_SIZE] = {};
    int num = 0;

    unsigned int i = 0;
    while(fgets(buf, sizeof(buf), fi)){
        num = int_parse(buf);
        fprintf(fo, "\t\t\t%f, %f\n",
            OUT_PADDING + i * pts_spc,
            (double)OUT_HEIGHT / 2.0
        );
        i++;
    }

    fprintf(fo, "\t\t\"\n\t\tstroke-width=\"%d\"\n\t\tstroke=\"#9900ff\"\n\t/>\n",
        OUT_STROKE_WIDTH
    );
}

void gen_chart(Props *p, FILE *fi, FILE *fo){
    fprintf(fo, "<svg width=\"%d\" height=\"%d\">\n", OUT_WIDTH, OUT_HEIGHT);

    gen_grid(fo);

    gen_polyline(p, fi, fo);

    fprintf(fo, "</svg>\n");
}



int main(){
    FILE *f_in = fopen("data.txt", "rt");
    if(f_in == NULL) return 1;

    Props p = init_props(f_in);
    if(p.size < 2) return 1;

    FILE *f_out = fopen("out.svg", "wt");
    if(f_out == NULL) return 1;

    gen_chart(&p, f_in, f_out);

    return 0;
}