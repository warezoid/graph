/*

2026 warezoid.
this code will fille data.txt with data

*/

#include <stdio.h>
#include <stdlib.h>

int main(){
    FILE *f = fopen("data.txt", "wt");
    const int N = 2000;
    int x = 1;

    for(int i = 0; i < N; i++){
        fprintf(f, "%d\n", rand() % 100);

        /*
        if(x){
            fprintf(f, "1\n");
            x = 0;
        }
        else{
            fprintf(f, "-1\n");
            x = 1;
        }
        */
    }

    return 0;
}