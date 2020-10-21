#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#define var 5100000
//5100000
int main(){
    int i,j,sign,number;
    srand((unsigned int)time(NULL));
    FILE *fout = fopen("input.csv","w");
    if(!fout){
        printf("open fail\n");
        exit(0);
    }
    for(i=0;i<var;i++){
        for(j=0;j<20;j++){
            sign = rand()%2 ==1 ? 1: -1;
            if(sign == -1) number = sign * ((rand()+1)*(rand()+1)*(rand()%2 + 1));
            else number = sign * (rand()+1) * (rand()+1) * (rand()%2 + 1) - 1;
            if(j!=19) fprintf(fout,"%d|",number);
            else fprintf(fout,"%d\n",number);
        }
    }

    return 0;
}