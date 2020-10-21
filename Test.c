
int main(){
    system("cat buffer* > output.json");
    return 0;
}


/*int main (int argc ,char*argv[]){
   int i;
   //pthread_t tid[2];
   //pthread_attr_t attr;
   //pthread_attr_init(&attr);
   //FILE *fin1 = fopen("file,txt","a");
   FILE *fput1=fopen("output.txt","a");
   FILE *fput2=fopen("output.txt","a");
   
   fseek(fput1,100,SEEK_SET);
   fprintf(fput1,"hi\n");
   fseek(fput2,0,SEEK_SET);
   fprintf(fput2,"yo\n");

   //for(i=0;i<2;i++){
   //   pthread_join(tid[i],NULL);
   //}
  // pthread_exit(0);
   fclose(fput1);
   fclose(fput2);
   return(0);
}*/