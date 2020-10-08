#include<stdio.h>
#include<stdlib.h>
#include<time.h>

int main(){
    srand((unsigned int)time(NULL));
    int number;
    int i,sign;
    FILE *fout = fopen("input1.txt","w");
    if(!(fout)){
        printf("open fail\n");
        exit(0);
    }
    for(i=0;i<10000000;i++){
        sign = rand()%2 ==1 ? 1: -1;
        if(sign == -1) number = sign * ((rand()+1)*(rand()+1)*(rand()%2 + 1)) + 100;
        else number = sign * (rand()+1) * (rand()+1) * (rand()%2 + 1) - 100;
        fprintf(fout,"%d\n",number);
    }
    fclose(fout);
    
    return 0;
}