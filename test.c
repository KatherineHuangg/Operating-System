#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<time.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>

typedef struct{
    int number;
    int start;
    int end;
    char *str;
    FILE *fin;
    FILE *fput;
}Thread;


void assign(int num_thread,int amount,FILE *fp,int col[]){
    int i=0,j,move = amount; //move 表到下一個thread之間的長度;
    char get[1];
    for(j=0;j<num_thread;j++){
        if(j==num_thread-1) move=0;
        else move = amount;
        fseek(fp,move,SEEK_CUR);
        while(1){
            get[0] = fgetc(fp);
            if(get[0]==EOF) {
                col[i] = move;
                printf("%d\n",move);
                break;
            }
            if((get[0] == '\n') && (j!=num_thread-1)){
                move++;
                col[i] = move;
                i++;
                printf("%d\n",move);
                break;
            }
            else move++;
        }
    }
}

void set(Thread thread[],int col[],int num_thread){
    int i = 0,begin = 0,tail = 0;

    for(i=0;i<num_thread;i++){
        thread[i].number = i;
        tail += col[i];
        thread[i].start = begin;//從1開始
        thread[i].end = tail-1;
        begin += col[i];
        printf("%d %d \n",thread[i].start,thread[i].end);

        thread[i].fput = fopen("output.json","a");
        thread[i].fin = fopen("input1.csv","r");
    }
}

void* apart(void* arg){
    Thread *thr = (Thread*) arg;
    thr->str = (char*)malloc(sizeof(char)*240);
    int index = thr->number;
    int position = thr->end;


    fseek(thr->fin,thr->start,SEEK_SET);
    while(position != ftell(thr->fin)){
        printf("!%ld\n",ftell(thr->fin));
        if(fgets(thr->str,240,thr->fin)!=NULL){

        }
        else break;
        printf("hi\n");
    }
    printf("out\n");
    pthread_exit(0);
}


int main(int argc,char* argv[]){
    ///看input.csv有幾位元
    int fd = open("input1.csv", O_RDONLY);
    int num_thread = atoi(argv[1]);

    //fput = fopen("output.json","a");
    struct stat s;
    fstat(fd, &s);
    printf("%ld\n", s.st_size);

    int amount = s.st_size/num_thread; //多一個thread大約可得的資料數量//220
    close(fd);
    int j;
    FILE *fput[num_thread],*fin[num_thread],*fp;
    
    //一些初始化
    int i,rc,col[num_thread];//紀錄一個thread要讀幾個位元
    for(i=0;i<num_thread;i++) col[i] = 0;
    fp = fopen("input1.csv","r");
    if(fp == NULL){
        printf("fail to open\n");
        exit(0);
    }
    //char *iput = (char*)malloc(sizeof(char)*240);
    pthread_t tid[num_thread];
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    Thread thread[num_thread];//struct

    ///分每個thread要讀幾個位元
    assign(num_thread,amount,fp,col);
    
    ///set每個thread所需的資料
    set(thread,col,num_thread);
    

    ///start
    for(i=0;i<num_thread;i++){
        printf("%d ",i);
        rc = pthread_create(&tid[i],&attr,apart,&thread[i]);
        if(rc){
            printf("error return code from thread%d is %d\n",i,rc);
            exit(-1);
        }
    }
    printf("before join\n");
    //pthread_join(tid[num_thread],NULL);
    //fclose(fin[num_thread]);

    //free(iput);
    //pthread_exit(0);
    for(i=0;i<num_thread;i++){
        fclose(thread[i].fput);
        fclose(thread[i].fin);
    }
    return 0;
}
