#include<stdio.h>
#include<stdlib.h>

int main()
{
  int intvar = 5;
  char strvar[] = "Hello, world";
  char buffer[20];
  int buflen = 20;
  
  FILE *fp;
  printf("//// TEXT I/O Example WRITE Start ////////////\n");
  // Open text file for write
  fp=fopen("demo.txt","w");
  if(fp == NULL){
    printf("Error opening file=\n");
    exit(1);
  }
  
  // Save text data into text file
  fprintf(fp,"%d\n", intvar);
  fprintf(fp,"%s\n", strvar);
  
  // Close the file
  fclose(fp);
//////////////////////////////////////////////////////////////////////////////////
  printf("//// TEXT I/O Example READ Start ////////////\n");
  // Open text file for read
  fp=fopen("demo.txt","r");
  if(fp == NULL){
    printf("Error opening file=\n");
    exit(1);
  }
  
  // Read text data from text file in a loop
  while(fgets(buffer, buflen, fp)){
    printf("Line from file: %s\n", buffer);
  }
  // Go back to the beginning of the file
  rewind(fp);
  // Read text number from text file
  // Notice the & before the int variable name
  fscanf(fp, "%d", &intvar);
  printf("Integer from file: %d\n", intvar);
  
  // Close the file
  fclose(fp);

//////////////////////////////////////////////////////////////////////////////////
  printf("//// BINARY I/O Example WRITE Start ////////////\n");
  // Open binary file for write
  fp=fopen("demo.bin","wb");
  if(fp == NULL){
    printf("Error opening file\n");
    exit(1);
  }
  
  // Save integer data into binary file
  fwrite(&intvar, sizeof(int), 1, fp);
  
  // Close the file
  fclose(fp);

//////////////////////////////////////////////////////////////////////////////////
  printf("//// BINARY I/O Example READ Start ////////////\n");
  // Open binary file for read
  fp=fopen("demo.bin","rb");
  if(fp == NULL){
    printf("Error opening file=\n");
    exit(1);
  }
  
  // Read int data from binary file in a loop
  int bytesread;
  while((bytesread=fread(&intvar,sizeof(int), 1, fp)) > 0){
    printf("Integer from file (loop): %d\n", intvar);
  }
  // Go back to the beginning of the file
  rewind(fp);
  // Read integer from binary file
  // Notice the & before the int variable name
  fread(&intvar,sizeof(int), 1, fp);
  printf("Integer from file: %d\n", intvar);
  
  // Close the file
  fclose(fp);
  return 1;
}
  
  
