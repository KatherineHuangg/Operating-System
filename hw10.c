#include<stdio.h>
#include<stdlib.h>
#include <time.h>
#define swap(a,b) int t=a;a=b;b=t;

clock_t start, end;
double cpu_time_used;


void leftori(int list[],int left,int right){
    int i,j,pivot;
    if(left<right){
        i=left;
        j=right+1;
        pivot=list[left];
        while(i<j){
            do{
                i++;
            }while(list[i]<pivot);
            do{
                j--;
            }while(list[j]>pivot);

            if(i < j){
                swap(list[i],list[j])
            }
        }
        swap(list[left], list[j])
        /*temp=list[i];
        list[i]=list[j];
        list[j]=temp;*/
        leftori(list,left,j-1);
        leftori(list,j+1,right);
    }

}

void leftlong(int list[],int left,int right){
    int i,j,pivot,temp;
    if(left<right){
        i=left;
        j=right+1;
        pivot=list[left];
        while(i<j){
            do{
                i++;
            }while(list[i]<pivot);
            do{
                j--;
            }while(list[j]>pivot);

            if(i < j){
                swap(list[i],list[j])
            }
        }
        swap(list[left], list[j])
        int a=j-1-left,b=right-j-1;
        if(a>=b){
            leftlong(list,left,j-1);
            leftlong(list,j+1,right);
        }
        else{
            leftlong(list,j+1,right);
            leftlong(list,left,j-1);
        }
    }
}

void leftshort(int list[],int left,int right){
    int i,j,pivot,temp;
    if(left<right){
        i=left;
        j=right+1;
        pivot=list[left];
        while(i<j){
            do{
                i++;
            }while(list[i]<pivot);
            do{
                j--;
            }while(list[j]>pivot);

            if(i < j){
                swap(list[i],list[j])
            }
        }
        swap(list[left], list[j])
        int a=j-1-left,b=right-j-1;
        if(a>=b){
            leftshort(list,j+1,right);
            leftshort(list,left,j-1);
        }
        else{
            leftshort(list,left,j-1);
            leftshort(list,j+1,right);
        }
    }
}

int Choosepivot(int left,int right,int middle){
    int array[3]={left,right,middle};
    int i,j,pivot;
    for(j=0;j<3;j++){
        for(i=0;i<3;i++){
            if(i!=j){
                if(i<j && array[i]>array[j]) {swap(array[i],array[j])}
                if(i>j && array[i]<array[j]) {swap(array[i],array[j])}
            }
        }
    }
    pivot=array[1];
    return pivot;
}

void medianori(int list[],int left,int right){
    int i,j,pivot;
    int middle=(left+right)/2;
    pivot=Choosepivot(list[left],list[right],list[middle]);

    if(left<right){
        i=left;
        j=right+1;

        if(list[right]==pivot) {swap(list[left],list[right])}
        if(list[middle]==pivot) {swap(list[left],list[middle])}
        while(i<j){
            do{
                i++;
            }while(list[i]<pivot);
            do{
                j--;
            }while(list[j]>pivot);

            if(i < j){
                swap(list[i],list[j])
            }
        }
        swap(list[left], list[j])
        medianori(list,left,j-1);
        medianori(list,j+1,right);
    }
}

void medianlong(int list[],int left,int right){
    int i,j,pivot,temp;
    int middle=(left+right)/2;
    pivot=Choosepivot(list[left],list[right],list[middle]);

    if(left<right){
        i=left;
        j=right+1;

        if(list[right]==pivot) {swap(list[left],list[right])}
        if(list[middle]==pivot) {swap(list[left],list[middle])}

        while(i<j){
            do{
                i++;
            }while(list[i]<pivot);
            do{
                j--;
            }while(list[j]>pivot);

            if(i < j){
                swap(list[i],list[j])
            }
        }
        swap(list[left], list[j])
        int a=j-1-left,b=right-j-1;
        if(a>=b){
            medianlong(list,left,j-1);
            medianlong(list,j+1,right);
        }
        else{
            medianlong(list,j+1,right);
            medianlong(list,left,j-1);
        }
    }
}

void medianshort(int list[],int left,int right){
    int i,j,pivot,temp;
    int middle=(left+right)/2;
    pivot=Choosepivot(list[left],list[right],list[middle]);

    if(left<right){
        i=left;
        j=right+1;

        if(list[right]==pivot) {swap(list[left],list[right])}
        if(list[middle]==pivot) {swap(list[left],list[middle])}

        while(i<j){
            do{
                i++;
            }while(list[i]<pivot);
            do{
                j--;
            }while(list[j]>pivot);

            if(i < j){
                swap(list[i],list[j])
            }
        }
        swap(list[left], list[j])
        int a=j-1-left,b=right-j-1;
        if(a>=b){
            medianshort(list,j+1,right);
            medianshort(list,left,j-1);
        }
        else{
            medianshort(list,left,j-1);
            medianshort(list,j+1,right);
        }
    }
}

int main(){
    int data,i;
    int* leftorilist=(int*)malloc(sizeof(int)*450001);
    int* leftlonglist=(int*)malloc(sizeof(int)*450001);
    int* leftshortlist=(int*)malloc(sizeof(int)*450001);
    int* medianorilist=(int*)malloc(sizeof(int)*450001);
    int* medianlonglist=(int*)malloc(sizeof(int)*450001);
    int* medianshortlist=(int*)malloc(sizeof(int)*450001);
    for(i=0;i<450000;i++){
        scanf("%d",&data);
            leftorilist[i]=data;
            leftlonglist[i]=data;
            leftshortlist[i]=data;
            medianorilist[i]=data;
            medianlonglist[i]=data;
            medianshortlist[i]=data;
    }
    int j;
    start = clock();
    leftori(leftorilist,0,i - 1);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("use leftmost and original execution time = %f\n", cpu_time_used);

    start = clock();
    leftlong(leftlonglist,0,i-1);
    end = clock();
    cpu_time_used = (double) (end - start) / CLOCKS_PER_SEC;
    printf("use leftmost and longer fist execution time = %f\n", cpu_time_used);

    start = clock();
    leftshort(leftshortlist,0,i-1);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("use leftmost and shorter fist execution time = %f\n", cpu_time_used);

    start = clock();
    medianori(medianorilist,0,i-1);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("use median of three and original execution time = %f\n", cpu_time_used);

    start = clock();
    medianlong(medianlonglist,0,i-1);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("use median of three and longer first execution time = %f\n", cpu_time_used);

    start = clock();
    medianshort(medianshortlist,0,i-1);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("use median of three and shorter first execution time = %f\n", cpu_time_used);

    printf("\n");
    return 0;
}
