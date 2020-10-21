//1行input一個thread
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<time.h>
#include <unistd.h>
#define input_num 5100000
FILE *fput;


clock_t start,end;
double Cpu_time_used;

void* apart(void* iput){//the thread will execute in this function
    char *str = (char*) iput;

    int count = 0,i,index=1;
    const char* delim = "||\r|\n";
    char *saveptr = NULL;
    char *number = NULL;
    number = strtok_r(str,delim,&saveptr);
    do{
        if(index!=20) fprintf(fput,"    \"col_%d\":%s,\n",index,number);//printf("    \"col_%d\":%s,\n",index,number);
        else fprintf(fput,"    \"col_%d\":%s\n",index,number);//printf("    \"col_%d\":%s\n",index,number);
        number = strtok_r(NULL,delim,&saveptr);
        index++;
    }while(number);
    fprintf(fput,"  }");//printf("  }");
    pthread_exit(0);
}    


int main(int argc,char* argv[]){
    char* iput = (char*)malloc(sizeof(char)*240);
    int i,index = 1;
    int num_thread = atoi(argv[1]);
    fput = fopen("output.json","a");
    int rc;
    pthread_t tid[num_thread];
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    FILE *fin = fopen("input1.csv","r");
    
    if(fin == NULL){
        printf("fail to open\n");
        exit(0);
    }
    start = clock();
    int count = 0;
    while(fgets(iput,240,fin)!=NULL){
        rc = pthread_create(&tid[index-1],&attr,apart,iput);
        if(rc){
            printf("error:return code from pthread_create is %d\n",rc);
            exit(-1);
        }
        if(count==0) {
            fprintf(fput,"[\n  {\n");//printf("[\n  {\n");
            count++;
        }
        else fprintf(fput,",\n  {\n");
        pthread_join(tid[index-1],NULL); 
        if(index == num_thread) index = 1;
        else index++;
    }
    fprintf(fput,"\n]\n");//printf("\n]\n");
    fclose(fin);
    free(iput);
    

    end = clock();
    Cpu_time_used = ((double)(end-start))/CLOCKS_PER_SEC;
    printf("Time:%f\n",Cpu_time_used);
    printf("pid=%d\n", getpid());
    pthread_exit(0);
    return 0;
}