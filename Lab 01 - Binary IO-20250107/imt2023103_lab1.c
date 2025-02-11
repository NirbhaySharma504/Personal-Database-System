# include <stdio.h> 

struct student {
  int rollnum;
  char name[30];
  int age;
};

int save_num_text( char *filename ) {
  FILE *fp;
  fp=fopen(filename,"w");
  if(fp == NULL){
    printf("Error opening file=\n");
    return 1;
  }
  int sequence[20] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
  for(int i=0;i<20;i++){
    fprintf(fp,"%d\n", sequence[i]);
  }
  fclose(fp);
  return 0;
//write your code
}

int read_num_text( char *filename ) {
  FILE *fp;
  int number;
  fp=fopen(filename,"r");
  if(fp == NULL){
    printf("Error opening file=\n");
    return 1;
  }
  while (fscanf(fp, "%d", &number) == 1) {
        printf("%d\n", number);
    }
  fclose(fp);
  return 0;
  //write your code
}

int save_struct_text( char *filename ) {
  FILE *fp;
  int number;
  fp=fopen(filename,"w");
  if(fp == NULL){
    printf("Error opening file=\n");
    return 1;
  }
  struct student s1[5] = {{001,"S1",18},{002,"S2",19},{003,"S3",18},{004,"S4",20},{005,"S5",19}};
  for(int i=0;i<5;i++){
    fprintf(fp,"%d ",s1[i].rollnum);
    fprintf(fp,"%s ",s1[i].name);
    fprintf(fp,"%d\n",s1[i].age);
  }
  fclose(fp);
  return 0;
  //write your code
}

int read_struct_text( char *filename ) {
  FILE *fp;
  int number;
  fp=fopen(filename,"r");
  if(fp == NULL){
    printf("Error opening file=\n");
    return 1;
  }
  char buffer[38];
  int buflen=38;
  while(fgets(buffer, buflen, fp)){
    printf("%s", buffer);
  }
  fclose(fp);
  return 0;
//write your code
} 


int save_num_binary( char *filename ) {
  FILE *fp;
  fp=fopen(filename,"wb");
  if(fp == NULL){
    printf("Error opening file\n");
    return 1;
  }
  int sequence[20] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
  for(int i=0;i<20;i++){
    fwrite(&sequence[i], sizeof(int), 1, fp);
  }
  fclose(fp);
  return 0;
  // Save integer data into binary file
  
//write your code
}

int read_num_binary( char *filename ) {
  FILE *fp;
  fp=fopen(filename,"rb");
  if(fp == NULL){
    printf("Error opening file\n");
    return 1;
  }
  int number;
  while(fread(&number,sizeof(int), 1, fp)){
    printf("%d\n", number);
  }
  fclose(fp);
  return 0;
 //write your code
}

int save_struct_binary( char *filename ) {
  FILE *fp;
  int number;
  fp=fopen(filename,"wb");
  if(fp == NULL){
    printf("Error opening file=\n");
    return 1;
  }
  struct student s1[5] = {{001,"S1",18},{002,"S2",19},{003,"S3",18},{004,"S4",20},{005,"S5",19}};
  for(int i=0;i<5;i++){
    fwrite(&s1[i],38,1,fp);
  }
  fclose(fp);
  return 0;
  //write your code
}

int read_struct_binary( char *filename ) {
  FILE *fp;
  fp=fopen(filename,"rb");
  if(fp == NULL){
    printf("Error opening file\n");
    return 1;
  }
  struct student s;
  for(int i=0;i<5;i++){
    fread(&s,38, 1, fp);
    printf("%d ",s.rollnum);
    printf("%s ",s.name);
    printf("%d\n",s.age);
  }
  fclose(fp);
  return 0;
 //write your code
}

