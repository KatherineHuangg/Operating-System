#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>
#include<time.h>
#define swap(a,b) int t=a;a=b;b=t;
#define var 400000000

clock_t start,end;
double Cpu_time_used;

char* fName(char* fileName,int index){
    char number[20]={};
        sprintf(number,"%d",index); //number=1
        strcpy(fileName,"buffer"); 
        strcat(fileName,number); //fileName = buffer1
        strcat(fileName,".txt"); //fileName = buffer1.txt
    return fileName;
}

void quickSort(int *buff,int left,int right){
    int i,j,pivot,m;
    if(left<right){
        i=left;
        pivot = buff[left];
        for(m=left+1; m<=right; m++) {
            if(buff[m]<pivot){
                i++;
                swap(buff[m],buff[i]);
            }
        }
        j=i;
        swap(buff[left],buff[j]);
        quickSort(buff,left,j-1);
        quickSort(buff,j+1,right);
    }
}

void reName(char* fileName,int count,int buffCount){
    char change[20];//new one
    if(count == -1){
        strcpy(change,"output.txt");
        rename(fName(fileName,buffCount),change);
    }
    else{
        strcpy(change,fName(fileName,buffCount));
        rename(fName(fileName,count),change);
    }
}

int isEof(int num){
    if(num==1) return 1;
    else return 0; //== EOF
}


void mergeSort(char *fileName,int count,int index,int buffCount,int *reg,int calculate){
    FILE *fmerge[3];
    int number0,number1,num=0,k,i=0,j=0,m;
    int a,b,merge;
    
    merge = index/2;//2個合併會有幾次

    while(i!=merge){ //開始merge
        buffCount++;
        fmerge[0]= fopen(fName(fileName,count+1),"r");//count+1 = 1
        fmerge[1] = fopen(fName(fileName,count+2),"r");//count+2 = 2
        fmerge[2] = fopen(fName(fileName,buffCount),"a");//combine 2 file to 1 file

        if((a=isEof(fscanf(fmerge[0],"%d\n",&number0))) && (b=isEof(fscanf(fmerge[1],"%d\n",&number1)))){
            do{
                if(number0 <= number1){
                    reg[j] = number0;
                    a = isEof(fscanf(fmerge[0],"%d\n",&number0));
                }
                else{
                    reg[j] = number1;
                    b = isEof(fscanf(fmerge[1],"%d\n",&number1));
                }
                if(j == var-1){
                    for(k=0;k<var;k++){
                        fprintf(fmerge[2],"%d\n",reg[k]);
                        reg[k] = 0;//clear the buff[] for the next file
                    }
                    j=0;
                }
                else j++;
            }while(a && b);//都還讀的到資料
        }
        if(a == 1){
            do{
                reg[j] = number0;
                if(j == var-1){
                    for(k=0;k<var;k++){
                        fprintf(fmerge[2],"%d\n",reg[k]);
                        reg[k] = 0;
                    }
                    j=0;
                }
                else j++;
            }while(fscanf(fmerge[0],"%d",&number0) != EOF);
        }
        else if(b == 1){
            do{
                reg[j] = number1;
                if(j == var-1){
                    for(k=0;k<var;k++){
                        fprintf(fmerge[2],"%d\n",reg[k]);
                        reg[k] = 0;
                    }
                    j=0;
                }
                else j++;
            }while(fscanf(fmerge[1],"%d",&number1) != EOF);
        }
        //可能reg還沒滿
        if(reg[0]!=reg[1]){
            for(m=0;m<j;m++){
                fprintf(fmerge[2],"%d\n",reg[m]);
                reg[m] = 0;
            }
        }

        count += 2;
        i++;
        fclose(fmerge[2]);
    }
    if(merge!=index/2+index%2){//表示有一個要直接上移一層
        buffCount++;
        count++;
        reName(fileName,count,buffCount);
        
    }
    index = index/2+index%2;//merge完這一層有幾個file
    if(index == 1){
        reName(fileName,-1,buffCount);
        for(m=1;m<buffCount;m++) unlink(fName(fileName,m));
        return;
    }
    mergeSort(fileName,count,index,buffCount,reg,calculate);
}

int main(){
    FILE *fin = fopen("input.txt","r");
    FILE *fput; //put the data to the buffer
    if(!(fin)){
        printf("open fail\n");
        exit(0);
    }

    int number=0,index = 1,i = 0,k=0;
    int *buff = (int*)malloc(sizeof(int)*var);
    char fileName[20];
    strcpy(fileName,"buffer");

    start = clock();
///////////把input拆成排序好的小buffer/////////
    while(fscanf(fin,"%d\n",&number)!= EOF){
        if(i<=var-1){
            buff[i] = number;
            if(i == var-1){//put into the file
                quickSort(buff,0,var-1);
                fput = fopen(fName(fileName,index),"w");
                for(k=0;k<var;k++){
                    fprintf(fput,"%d\n",buff[k]);
                    buff[k] = 0;//clear the buff[] for the next file
                }
                i=0;
                index++;
                fclose(fput);
            }
            else i++;
        }
    }
    if(buff[0]!=buff[1]){    //會有一個buff沒有滿，要考慮進去
        int length = 0;
        for(i=0;i<var;i++){
            if(buff[i]==0) break;
            else length++;
        }
        quickSort(buff,0,length-1);
        fput = fopen(fName(fileName,index),"w");
        for(k=0;k<length;k++){
            fprintf(fput,"%d\n",buff[k]);
            buff[k] = 0;//clear the buff[] for the next file
        }
        fclose(fput);
    }
    else index--;//buff剛好滿
    fclose(fin);
    free(buff);
    
///////////開始merge//////////////
    int buffCount = index; //which means that what buff that this turn will use
    int *reg = (int*)malloc(sizeof(int)*var);
    
    mergeSort(fileName,0,index,buffCount,reg,index);

    free(reg);
    end = clock();
    Cpu_time_used = ((double)(end-start))/CLOCKS_PER_SEC;
    printf("Time:%f\n",Cpu_time_used);
    printf("pid=%d\n", getpid());
    return 0;
}
