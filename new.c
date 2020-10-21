///小buffer.json合成output.json(未完成)
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
    int number;//第幾個thread
    int start;
    int end;
    char *str;
    FILE *fin;
    FILE *fput;
}Thread;

int *sum;//自己的thread要讀的行數(\n數量)


char* fName(char* fileName,int index){
    char number[5]={};
    sprintf(number,"%d",index); //number=1
    strcpy(fileName,"buffer"); 
    strcat(fileName,number); //fileName = buffer1
    strcat(fileName,".json"); //fileName = buffer1.txt
    return fileName;
}

void calculate(FILE *fp,int sum[],Thread thread[],int num_thread,int col[]){
    char c;
    char *string = (char*)malloc(sizeof(char)*240);
    int i,have_read = 0,need_read = 0,length;

    fseek(fp,0,SEEK_SET);
    for(i=0;i<num_thread;i++){
        //printf("%d ",thread[i].end);
        need_read += thread[i].end+1;
        while(fgets(string,240,fp)!=NULL){
            length = strlen(string);
            //printf("str:%d ",length);
            have_read+=length;
            sum[i]++;
            if(have_read == need_read) break;
        }
        //printf("%d ",sum[i]);
    }
    printf(" out cal\n");
    free(string);
}

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
                //printf("%d\n",col[i]);
                break;
            }
            if((get[0] == '\n') && (j!=num_thread-1)){
                move++;
                col[i] = move;
                i++;
                //printf("%d\n",col[i]);
                break;
            }
            else move++;
        }
    }
    printf("out assign\n");
}

void set(Thread thread[],int col[],int num_thread){
    int i = 0,begin = 0,tail = 0;
    char fileName[20]={};
    for(i=0;i<num_thread;i++){
        thread[i].number = i;
        tail += col[i];
        thread[i].start = begin;//從1開始
        thread[i].end = tail-1;
        begin += col[i];
        printf("%d %d \n",thread[i].start,thread[i].end);

        //thread[i].fput = fopen("output.json","a");
        thread[i].fin = fopen("input1.csv","r");
        thread[i].fput = fopen(fName(fileName,thread[i].number),"w");
    }
    printf("out set\n");
}

void* apart(void* arg){
    Thread *thr = (Thread*) arg;
    thr->str = (char*)malloc(sizeof(char)*240);
    int i,index=0;
    //int index = thr->number;//是第幾個thread
    int position = thr->end;

    const char* delim = "||\r|\n";
    char *saveptr = NULL;
    char *num = NULL;
    int count = 1,length;

    fseek(thr->fin,thr->start,SEEK_SET);

    if(thr->number == 0) fseek(thr->fput,0,SEEK_SET);
    else{
        for(i=0;i<(thr->number);i++) index += sum[i];
        printf("index:%d ",index);
        fseek(thr->fput,279*index + thr->start,SEEK_SET);
    }

    while(position+1 != ftell(thr->fin)){
        //printf("%d %d %d ",thr->start,thr->end,position+1);
        printf("ftell:%ld\n",ftell(thr->fin));
        if(fgets(thr->str,240,thr->fin)!=NULL){
            length = strlen(thr->str);
            num = strtok_r(thr->str,delim,&saveptr);
        
            printf("sum:%d ",279*index+thr->start);
            do{
                if(count==1) fprintf(thr->fput,"  {\n    \"col_%d\":%s,\n",count,num);
                else if(count!=20) fprintf(thr->fput,"    \"col_%d\":%s,\n",count,num);
                else fprintf(thr->fput,"    \"col_%d\":%s\n",count,num);
                num = strtok_r(NULL,delim,&saveptr);
                count++;
            }while(num);
            count = 1;
            fprintf(thr->fput,"  },\n");
        }
        else break;
    }
    
    //printf("out\n");
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
    FILE *fp;
    
    //一些初始化
    int i,rc,col[num_thread];//紀錄一個thread要讀幾個位元
    for(i=0;i<num_thread;i++) col[i] = 0;
    fp = fopen("input1.csv","r");
    if(fp == NULL){
        printf("fail to open\n");
        exit(0);
    }
    pthread_t tid[num_thread];
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    Thread thread[num_thread];//struct

    ///分每個thread要讀幾個位元
    assign(num_thread,amount,fp,col);
    
    ///set每個thread所需的資料
    set(thread,col,num_thread);
    
    ///紀錄自己thread要讀的行數(\n數量)
    sum = (int*)malloc(sizeof(int)*num_thread);
    for(i=0;i<num_thread;i++) sum[i] = 0;
    calculate(fp,sum,thread,num_thread,col);

    
    ///start
    for(i=0;i<num_thread;i++){
        rc = pthread_create(&tid[i],&attr,apart,&thread[i]);
        if(rc){
            printf("error return code from thread%d is %d\n",i,rc);
            exit(-1);
        }
    }
    
    for(i=0;i<num_thread;i++){
        pthread_join(tid[i],NULL);
    }
    
    pthread_exit(0);
    free(sum);
    return 0;
}


/*
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
        if(index!=20) fprintf(fput,"    \"col_%d\":%s,\n",index,number);
        //printf("    \"col_%d\":%s,\n",index,number);
        else fprintf(fput,"    \"col_%d\":%s\n",index,number);
        //printf("    \"col_%d\":%s\n",index,number);
        number = strtok_r(NULL,delim,&saveptr);
        index++;
    }while(number);
    fprintf(fput,"  }");
    //printf("  }");
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

    FILE *fin = fopen("input.csv","r");
    
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
            fprintf(fput,"[\n  {\n");
            //printf("[\n  {\n");
            count++;
        }
        else fprintf(fput,",\n  {\n");
        pthread_join(tid[index-1],NULL); 
        if(index == num_thread) index = 1;
        else index++;
    }
    fprintf(fput,"\n]\n");
    //printf("\n]\n");
    fclose(fin);
    free(iput);
    

    end = clock();
    Cpu_time_used = ((double)(end-start))/CLOCKS_PER_SEC;
    printf("Time:%f\n",Cpu_time_used);
    printf("pid=%d\n", getpid());
    pthread_exit(0);
    return 0;
}

*/