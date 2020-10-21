//單執行序
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#define input_num 5100000
FILE *fput;


int* apart(char* iput,int* num_arr){
    int count = 0,i,index=1;
    const char* delim = "||\r|\n";
    char *saveptr = NULL;
    char *number = NULL;
    number = strtok(iput,delim);
    printf("  {\n");
    do{
        if(index!=20) printf("    col_%d:%s,\n",index,number);
        else printf("    col_%d:%s\n",index,number);
        //printf("%s ",number);
        number = strtok(NULL,delim);
        index++;
    }while(number);
    printf("  },\n");
}    
int main(int argc,char* argv[]){
    char* iput = (char*)malloc(sizeof(char)*240);
    int* num_arr = (int*)malloc(sizeof(int)*20);
    int i,index = 1;
    int num_thread = atoi(argv[1]);
    pthread_t tid[num_thread];
    FILE *fin = fopen("input1.csv","r");
    if(fin == NULL){
        printf("fail to open\n");
        exit(0);
    }
    while(fgets(iput,240,fin)!=NULL){
        if(index!=num_thread) {
            apart(iput,num_arr);//for thread
            index++;
        }
        else if(index==num_thread){
            apart(iput,num_arr);//for thread
            index++;
        }
    }
    fclose(fin);
    free(iput);
    free(num_arr);
    return 0;
}
