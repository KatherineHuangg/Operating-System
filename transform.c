//after multithread時間會越來越長是因為space是用string存，看起來是連在一起的區塊但其實是只有第一個位置被紀錄，其他位置分散再記憶體的不同地方，所以thread越多就越分散
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<time.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>

//clock_t start,end;
double Cpu_time_used;
struct timespec start;
struct timespec end;

typedef struct{
    int total_thread;
    int number;//第幾個thread
    int start;
    int end;
    char *str;
    FILE *fin;
    struct timespec head;
    struct timespec tail;
}Thread;

int *sum;//自己的thread要讀的行數(\n數量)
char **space;

int calculate(FILE *fp,int sum[],Thread thread[],int num_thread,int col[]){
    char c;
    char *string = (char*)malloc(sizeof(char)*240);
    int i,have_read = 0,need_read = 0,length;

    fseek(fp,0,SEEK_SET);
    for(i=0;i<num_thread;i++){
        need_read = thread[i].end+1;
        while(fgets(string,240,fp)!=NULL){
            length = strlen(string);
            have_read+=length;
            sum[i]++;
            if(have_read == need_read) break;
        }
    }
    free(string);
    int result=0;
    for(i=0;i<num_thread;i++) result+=sum[i];
    return result;
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
                break;
            }
            if((get[0] == '\n') && (j!=num_thread-1)){
                move++;
                col[i] = move;
                i++;
                break;
            }
            else move++;
        }
    }
}

void set(Thread thread[],int col[],int num_thread){
    int i = 0,begin = 0,tail = 0;
    for(i=0;i<num_thread;i++){
        thread[i].total_thread = num_thread;
        thread[i].number = i;
        tail += col[i];
        thread[i].start = begin;//從1開始
        thread[i].end = tail-1;
        begin += col[i];
        thread[i].fin = fopen("input.csv","r");
    }  
}

void* apart(void* arg){
    Thread *thr = (Thread*) arg;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID,&(thr->head));
    thr->str = (char*)malloc(sizeof(char)*240);
    int i,index=0,position = thr->end;

    const char* delim = "||\r|\n";
    char *saveptr = NULL;
    char *num = NULL;
    int count = 1,length;
    char cou[5]={};

    fseek(thr->fin,thr->start,SEEK_SET);

    if(thr->number == 0) index=0;//從space[index]開始印
    else{
        for(i=0;i<(thr->number);i++) index += sum[i];
        index = index*20;//從space[index]開始印
    }
    if(sum[thr->number]!=0){
        while(position+1 != ftell(thr->fin)){
            if(fgets(thr->str,240,thr->fin)!= NULL){
                num = strtok_r(thr->str,delim,&saveptr);
                do{
                    if(count==1) sprintf(space[index],"  {\n    \"col_%d\":%s,\n",count,num);
                    else if(count!=20) sprintf(space[index],"    \"col_%d\":%s,\n",count,num);
                    else sprintf(space[index],"    \"col_%d\":%s\n  },\n",count,num);
                    num = strtok_r(NULL,delim,&saveptr);
                    count++;
                    index++;
                }while(num);
                count=1;
            }
            else break;
        }
    }   
    clock_gettime(CLOCK_THREAD_CPUTIME_ID,&(thr->tail));
    printf("In thread%d: %f\n",(thr->number)+1,(thr->tail.tv_sec-thr->head.tv_sec)+(thr->tail.tv_nsec-thr->head.tv_nsec)/1000000000.0);
    pthread_exit(0);
}

int main(int argc,char* argv[]){
    clock_gettime(CLOCK_MONOTONIC,&start);

    ///看input.csv有幾位元
    int fd = open("input.csv", O_RDONLY);
    int num_thread = atoi(argv[1]);

    struct stat s;
    fstat(fd, &s);
    //printf("%ld\n", s.st_size);

    int amount = s.st_size/num_thread; //多一個thread大約可得的資料數量//220
    close(fd);
    int j;
    FILE *fp;
    
    //一些初始化
    int i,rc,total,col[num_thread];//紀錄一個thread要讀幾個位元
    for(i=0;i<num_thread;i++) col[i] = 0;
    fp = fopen("input.csv","r");
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
    
    ///紀錄自己thread要讀的行數(\n數量),total=總input行數
    sum = (int*)malloc(sizeof(int)*num_thread);
    for(i=0;i<num_thread;i++) sum[i] = 0;
    total = calculate(fp,sum,thread,num_thread,col);

    ///thread要output到這
    space = (char **)malloc(20 * total * sizeof(char*));
    for(i=0;i<(20*total);i++) space[i] = (char*)malloc(35*sizeof(char));
    for(i=0;i<(20*total);i++) memset(space[i], '\0', sizeof(char)*35 );//initialize
    
    clock_gettime(CLOCK_MONOTONIC,&end);
    printf("before multi-thread: %f\n",(end.tv_sec-start.tv_sec)+(end.tv_nsec-start.tv_nsec)/1000000000.0);

    ///start
    for(i=0;i<num_thread;i++){
        clock_gettime(CLOCK_MONOTONIC,&start);
        rc = pthread_create(&tid[i],&attr,apart,&thread[i]);
        if(rc){
            printf("error return code from thread%d is %d\n",i,rc);
            exit(-1);
        }
    }
    for(i=0;i<num_thread;i++) pthread_join(tid[i],NULL);

    clock_gettime(CLOCK_MONOTONIC,&start);

    ///output到output.json
    FILE *fput = fopen("output.json","w");
    fprintf(fput,"[\n");
    for(i=34;i>=0;i--){
        if(space[20*total -1][i] == ','){
            space[20*total - 1][i] = '\n';
            space[20*total - 1][i+1] = '\0';
        }
    }
    for(i=0 ; i<(20*total) ; i++) {
        if(fputs == NULL) {
            printf("error: %d \n",i);
            exit(0);
        }
        fputs(space[i],fput);
    }
    fprintf(fput,"]");    

    clock_gettime(CLOCK_MONOTONIC,&end);
    printf("after multi-thread: %f\n",(end.tv_sec-start.tv_sec)+(end.tv_nsec-start.tv_nsec)/1000000000.0);

    printf("pid=%d\n", getpid());

    free(sum);
    free(fp);
    for(i=0;i<num_thread;i++) fclose(thread[i].fin);
    for(i=0;i<20*total;i++) free((char *)space[i]);
    free((char *)space);
    
    pthread_exit(0);
    free(fput);

    return 0;
}