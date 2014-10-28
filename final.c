/*
 * final.c
 *
 *  Created on: Oct 27, 2014
 *      Author: nkatre
 */

/*  1 --basic
	2 input.bin
	3 sort.bin
*/

#include<stddef.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <limits.h>

#pragma pack(1)
typedef struct{
	int *integers;
	int int_size;    // size of integers in this struct
	int int_start;
	char filename[40];
	int available;
}inputBufferStruct;

typedef struct{
	char filename[40];
	int offset;
	int left;
	int total;
	int available;
}sub_fo;

typedef struct{
	char filename[40];
	int offset;
	int left;
	int total;
	int available;
}super_fo;



inputBufferStruct *ptr = NULL;
sub_fo *small_fo=NULL;
super_fo *big_fo = NULL;

int totalSortedFiles=0;
int inputBuffer[1000]={0};
int max_inputBufferSize=1000;  //change
int outputBuffer[1000]={0};
int max_outputBufferSize=1000;  //change
int act_outputBufferSize=0;
int act_inputBufferSize=0;
int mergeType=0;// mergeType


int totalBytes=0;
int totalInts = 0;

sub_fo* garbageCollector(sub_fo *small_fo, inputBufferStruct *ptr,int size, int mergeType);
inputBufferStruct* resizeArray(inputBufferStruct *ptr, int counter,int size, char file[]);
inputBufferStruct* readValues(inputBufferStruct *ptr, int counter, int size);
void fileInput(int start, int end, int maxPossible, sub_fo *small_fo);
void printPtr(inputBufferStruct *ptr, int total);

sub_fo* register_sub_fo(sub_fo *small_fo, int counter, char file[], int offset, int total, int left){
	if(small_fo==NULL){
		small_fo = (sub_fo *)malloc(sizeof(sub_fo));
		small_fo[counter].available = 1;
		strncpy(small_fo[counter].filename,file,strlen(file));
		small_fo[counter].filename[strlen(file)] = '\0';
		small_fo[counter].offset = offset;
		small_fo[counter].total = total;
		small_fo[counter].left = left;
	}
	else if(small_fo!=NULL){
		        small_fo = (sub_fo *)realloc(small_fo,(counter+1)*sizeof(sub_fo));
				small_fo[counter].available = 1;
				strncpy(small_fo[counter].filename,file,strlen(file));
			    small_fo[counter].filename[strlen(file)] = '\0';
				small_fo[counter].offset = offset;
				small_fo[counter].total = total;
				small_fo[counter].left = left;
	}
	return small_fo;
}

/*

super_fo* register_super_fo(super_fo *big_fo, int counter, char file[], int offset, int total, int left){
	if(big_fo==NULL){
		big_fo = (super_fo *)malloc(sizeof(super_fo));
		big_fo[counter].available = 1;
		strncpy(big_fo[counter].filename,file,strlen(file));
		big_fo[counter].filename[strlen(file)] = '\0';
		big_fo[counter].offset = offset;
		big_fo[counter].total = total;
		big_fo[counter].left = left;
	}
	else if(big_fo!=NULL){
		        big_fo = (super_fo *)realloc(big_fo,(counter+1)*sizeof(super_fo));
		        big_fo[counter].available = 1;
		        strncpy(big_fo[counter].filename,file,strlen(file));
		        big_fo[counter].filename[strlen(file)] = '\0';
		        big_fo[counter].offset = offset;
		        big_fo[counter].total = total;
		        big_fo[counter].left = left;
	}
	return big_fo;
}
*/

//void fileInput()

int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}
void print(sub_fo *small_fo){
	int i=0;
	for(i=0;i<totalSortedFiles;i++){
	printf("FileName : %s\n",small_fo[i].filename);
	printf("Offset : %i\n",small_fo[i].offset);
	printf("Left : %i\n",small_fo[i].left);
	printf("Total : %i\n",small_fo[i].total);
	printf("Available : %i\n",small_fo[i].available);
	}
}

void createSortedRuns(int start, int end, char file[]){

	FILE *givenFile = NULL;
	givenFile=fopen(file,"r+b");
	rewind(givenFile);
	int i=0;

	char *temp = (char *)malloc(sizeof(char));

	for(i=start; i<end; i++){
		fseek(givenFile,0,SEEK_CUR);
		fread(&inputBuffer[act_inputBufferSize],sizeof(int),1,givenFile);
		act_inputBufferSize++;

		if((act_inputBufferSize==max_inputBufferSize) ||(i==(end-1))){

			//sort
			qsort(inputBuffer,act_inputBufferSize,sizeof(int),compare);

			// write to file

			strncpy(temp,file,strlen(file));
			temp[strlen(file)] = '\0';
			//int k=0;
			char counter[4]={0};
			/*for(k=0;k<4;k++){
				counter[k]=0;
			}*/

			snprintf(counter,5, ".%03d", totalSortedFiles);
			//sprintf(counter,".%03d",totalSortedFiles);
			counter[strlen(counter)] = '\0';
			strncat(temp,counter,4);

			FILE *int_File = NULL;
			int_File=fopen(temp,"w+b");
			rewind(int_File);
			fwrite(&inputBuffer,sizeof(int),act_inputBufferSize,int_File);

			// register the sub_fo information
			small_fo=register_sub_fo(small_fo,totalSortedFiles,temp, 0 ,act_inputBufferSize,act_inputBufferSize);


			// increase the totalSortedFiles
			totalSortedFiles+=1;


			// Reinitialize
						int j=0;
						for(j=0;j<max_inputBufferSize;j++){
							inputBuffer[j]=0;
						}
						act_inputBufferSize=0;

						int len = strlen(temp);
					    temp[len-4] = '\0';
		   // Garbage
						fclose(int_File);
						int_File = NULL;



		}
	} //for_end
	fclose(givenFile);
	givenFile=NULL;

	free(temp);
	temp=NULL;


} // func_end



int main(int argc, char *argv[]){

    if(strcmp("--basic",argv[1])==0)
	mergeType = 1;
    if(strcmp("--multistep",argv[1])==0)
	mergeType =2;
	if(strcmp("--replacement",argv[1])==0)
	mergeType =3;


	if(mergeType==1){
    FILE *inputReader = NULL;
    inputReader=fopen(argv[2],"r+b");
    rewind(inputReader);

    // get total integers
    fseek(inputReader,0,SEEK_END);
    totalBytes = ftell(inputReader);
    totalInts = totalBytes/sizeof(int);

    fclose(inputReader);
    inputReader=NULL;

    createSortedRuns(0,totalInts,argv[2]);
    print(small_fo);


    // get max possible input from each file
    int maxPossible = max_inputBufferSize/totalSortedFiles;
    fileInput(0,totalSortedFiles,maxPossible,small_fo);

    printPtr(ptr,totalSortedFiles);

    small_fo = garbageCollector(small_fo,ptr,(totalSortedFiles), mergeType);

	}
	return 0;
}

sub_fo* garbageCollector(sub_fo *small_fo, inputBufferStruct *ptr,int size, int mergeType){

	if(mergeType == 1){   // --basic
    free(small_fo);
    small_fo=NULL;
    free(ptr);
    ptr=NULL;
	}
	return small_fo;
}

inputBufferStruct* resizeArray(inputBufferStruct *ptr, int counter,int size, char file[]){
	if(ptr==NULL){
		ptr = (inputBufferStruct *)malloc(sizeof(inputBufferStruct));
		ptr[counter].int_size = size;
		ptr[counter].int_start = 0;
		ptr[counter].integers = (int*)(malloc(sizeof(int)*size));
		strncpy(ptr[counter].filename,file,strlen(file));
		ptr[counter].filename[strlen(file)] = '\0';
		ptr[counter].available = 1;
	}
	else if(ptr!=NULL){
		ptr = (inputBufferStruct *)realloc(ptr,(counter+1)*sizeof(inputBufferStruct));
		ptr[counter].int_size = size;
		ptr[counter].int_start = 0;
		ptr[counter].integers = (int*)(malloc(sizeof(int)*size));
		strncpy(ptr[counter].filename,file,strlen(file));
		ptr[counter].filename[strlen(file)] = '\0';
		ptr[counter].available = 1;
	}
	return ptr;
}
inputBufferStruct* readValues(inputBufferStruct *ptr, int counter, int size){
	if(ptr == NULL){
		printf("Cannot read the file to an empty structure \n");
	}
	else{
		FILE *tempInputFile = NULL;
		tempInputFile=fopen(ptr[counter].filename,"r+b");
		rewind(tempInputFile);
		int i=0;
		for(i=0;i<size;i++){
			fseek(tempInputFile,0,SEEK_CUR);
			fread(&ptr[counter].integers[i],sizeof(int),1,tempInputFile);
		}
		//garbage collection
		fclose(tempInputFile);
		tempInputFile=NULL;
	}
	return ptr;
}
void fileInput(int start, int end, int maxPossible, sub_fo *small_fo){   // we know that the total size of small_fo = totalSortedFiles = ptr size
	int i=0;
	for(i=start;i<end;i++){
		if(maxPossible<=small_fo[i].left){
			ptr=resizeArray(ptr,i,maxPossible,small_fo[i].filename);
			ptr = readValues(ptr,i,maxPossible);
			small_fo[i].offset += maxPossible;
			small_fo[i].left -= maxPossible;
		}
		else if(maxPossible>small_fo[i].left){
			ptr = resizeArray(ptr,i,small_fo[i].left,small_fo[i].filename);
			ptr = readValues(ptr,i,small_fo[i].left);
			small_fo[i].offset = small_fo[i].total;
			small_fo[i].left = 0;
		}
		if(small_fo[i].left==0){             // TODO : Should I remove this condition
			small_fo[i].available=0;
		}
	}
}

void printPtr(inputBufferStruct *ptr, int total){
	int i=0,j=0;
	for(i=0;i<total;i++){
		printf("Pointer number: %d\n",i);
		printf("Size of pointer is : %i\n",ptr[i].int_size);
		printf("Starting Index: %i\n",(ptr[i].int_start));
		printf("Associated file name is: %s\n",ptr[i].filename);
		for(j=0;j<ptr[i].int_size;j++)
		printf("Elements of the integer are: %i\n",ptr[i].integers[j]);
	}

}
