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
    //printf(" out cal\n");
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
        for(i=0;i<thr->number-1;i++) index+=sum[i];
        printf("index:%d ",index);
        fseek(thr->fput,279*sum[index]+thr->start,SEEK_SET);
    }

    while(position+1 != ftell(thr->fin)){
        //printf("%d %d %d ",thr->start,thr->end,position+1);
        //printf("%ld\n",ftell(thr->fin));
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
    
    ///紀錄自己thread要讀的行數(\n數量)
    sum = (int*)malloc(sizeof(int)*num_thread);
    for(i=0;i<num_thread;i++) sum[i] = 0;
    calculate(fp,sum,thread,num_thread,col);

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
    for(i=0;i<num_thread;i++){
        pthread_join(tid[i],NULL);
    }
    
    
    //fprintf(fput,"\n]\n");
    //free(iput);
    free(sum);
    pthread_exit(0);
    for(i=0;i<num_thread;i++){
        fclose(thread[i].fput);
        fclose(thread[i].fin);
    }
    return 0;
}