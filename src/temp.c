#include <stdio.h>


int get_order(int num){
    if(num < 0) num *= -1;
    
    int res = 10;
    while(num % res != num){
        res *= 10;
    }

    return (res /= 10);
}

int get_anchor(int num){
    int sign = 1;
    if(num < 0){
        sign = -1;
        num *= -1;
    }

    int res = num;
    int order = get_order(num);
    int order_half = order >> 1;
    
    int a = num % order;

    if(a != 0){
        if(a >= order_half){
            while(res % order != 0){
                res++;
            }
        }
        else{
            while(res % order_half != 0){
                res++;
            }
        }

        return (res * sign);
    }
    
    res = res + order;
    return (res * sign);
}



int main(){
    int arr[10] = {1, -15, 361, 50, -200001, 12, 88, 999, 1001, -50};
    for(int i = 0; i < 10; i++){
        printf("el: %d | ord: %d | anch: %d\n", arr[i], get_order(arr[i]), get_anchor(arr[i]));
    }


    return 0;
}