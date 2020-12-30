/*

	Bahadir Kacar 171418001
	Elif Genc 170418038
	isletim sistemleri proje gorevi

*/
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <sys/ipc.h>
#include <pthread.h>
#include <unistd.h>
void swap(int *xp, int *yp)
{
	int temp = *xp;
	*xp = *yp;
	*yp = temp;
}
// Sıralanacak dizi ve dizinin uzunluğu
void bubbleSort(int arr[], int n)
{
	int i, j;
	for (i = 0; i < n - 1; i++)
		for (j = 0; j < n - i - 1; j++)
			if (arr[j] > arr[j + 1])
				swap(&arr[j], &arr[j + 1]);
}
//Fonksiyona gönderilen sayıların kaç basamaklı olduğunu bulur
int basamak_sayisi(int number)
{
	int count = 0;
	while (number != 0)
	{
		number /= 10;
		++count;
	}
	return count;
}
//Fonksiyona gönderilen sayıların kaç basamaklı olduğunu bulur

int basamak_sayisi(int number)
{
	int count=0;
	while(number != 0)
	{
		number /= 10;
		++count;	
	}
	return count;
}
typedef struct{
char * input;
int arr[2]; 
}params;
int values[5000];

void* dosya_islemleri(void * args)
{
	//Dosya okuma modunda açılır
	params * pr= args;
	int dosya=open(pr->input, O_RDONLY, 0);
	if (dosya < 0){
   		printf("Open failed..1");
		exit(2);
	};
	//Dosyayı okur ve arr içine atar
	int * temp = (int *)malloc(1000000*sizeof(int));
	int n_read;
	int cnt=0, gelinen_yer =0;
	char num[1];	
	while( (n_read = read(dosya,num,1) ) > 0)
	{		
		if(num[0] == '\n' || num[0] == '\t' || num[0] == ' '){
			temp[cnt] = gelinen_yer;
			cnt++; gelinen_yer=0;		
		}
		else{
			gelinen_yer *= 10;		
			gelinen_yer += atoi(num);
		}	
	};
	int * arr = (int *)malloc(cnt*sizeof(int));
	for(int j = 0; j<cnt;j++)
		arr[j]=temp[j];
	free(temp);
	close(dosya);
	//Okunan bitin sayılar sıralanır 
	bubbleSort(arr, cnt); 
	int k=pr->arr[0];
	//Thread kullanımı
	int threadNum = pr->arr[1];
	for(int l=0; l<k;l++)
	{
		values[(threadNum*k)+l]= arr[cnt-k+l];
	}
					
	free(arr);
	free(pr);
	pthread_exit(NULL);	
}


int main(int argc, char *argv[]) {
	//Test amaçlı ms yi ölçmek için 
	struct timeval startTime, endTime;    	
	long elapsedTime;
	gettimeopen_fileay(&startTime, NULL);
	
	int k, n;

	k = atoi(argv[1]);
	n = atoi(argv[2]);
	
	pthread_t tid1,tid2,tid3,tid4,tid5;
	pthread_t x[5] = {tid1,tid2,tid3,tid4,tid5};
	for(int i=0;i<n;i++){
		char* infile = argv[(3+i)];
		params * pr = malloc(sizeof(*pr));	
		pr->input = infile;
		pr->arr[0]= k;
		pr->arr[1] = i;
		//Thread Create
		int ret = pthread_create(&(x[i]),NULL,dosya_islemleri,pr);
		if(ret != 0){
			printf("thread create failed\n");
			exit(1);
		}
				
	}
	for(int l =0; l<n;l++)	
		pthread_join(x[l],NULL);
	bubbleSort(values, k*n);
	int open_file=open(argv[3+n],O_RDWR|O_CREAT, 0777);
	
	
		//istenilen k. en büyük değeri bulma işlemi
		int slct = values[(n*k)-(k-1)-1];
		int nod = basamak_sayisi(slct);
		char temp_array[nod];	
		sprintf(temp_array,"%d", slct);	
		write(open_file,temp_array,nod);
		write(open_file, "\n", strlen("\n"));
	
	close(open_file);	
	gettimeopen_fileay(&endTime, NULL);
	elapsedTime = (endTime.tv_sec - startTime.tv_sec) * 1000000 +
	endTime.tv_usec - startTime.tv_usec;
	printf("findtopk_thread in ms: %ld\n", elapsedTime);
	return(0);
}

