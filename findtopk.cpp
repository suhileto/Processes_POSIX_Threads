
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

void swap(int *xp, int *yp)
{
	int temp = *xp;
	*xp = *yp;
	*yp = temp;
}
//  sortlanacak array ve arrayin uzunlugu
void bubbleSort(int arr[], int n)
{
	int i, j;
	for (i = 0; i < n - 1; i++)
		for (j = 0; j < n - i - 1; j++)
			if (arr[j] > arr[j + 1])
				swap(&arr[j], &arr[j + 1]);
}

//array yazdir
void printArray(int A[], int size)
{
	int i;
	for (i = 0; i < size; i++)
		printf("%d ", A[i]);
	printf("\n");
}
//basamak sayisi don
int numberOfDigits(int number)
{
	int count = 0;
	while (number != 0)
	{
		number /= 10;
		++count;
	}
	return count;
}

//child program
void child(char *input, char *output, const int k)
{
	//dosyayi ac
	int rfd = open(input, O_RDONLY, 0);
	int *temp = (int *)malloc(1000000 * sizeof(int));
	int n_read;
	int cnt = 0, nsofar = 0;
	char num[1];
	int ctrl = 1;
	//char seklindeki karakterli okuyup inte cevir
	while ((n_read = read(rfd, num, 1)) > 0)
	{
		if (num[0] == '\n' || num[0] == '\t' || num[0] == ' ')
		{
			if (ctrl == 1)
			{
				temp[cnt] = nsofar;
				cnt++;
				nsofar = 0;
			}
			ctrl = 0;
		}
		else
		{
			nsofar *= 10;
			nsofar += atoi(num);
			ctrl = 1;
		}
	};
	int *arr = (int *)malloc(cnt * sizeof(int));
	for (int j = 0; j < cnt; j++)
		arr[j] = temp[j];
	free(temp);

	close(rfd);
	//inte cevrilmis karakterleri sortla
	bubbleSort(arr, cnt);
	//gecici dosya olustur ve duzenlenmis arrayi dosyaya yaz
	int ofd = open(output, O_RDWR | O_CREAT, 0777);

	for (int l = 0; l < k; l++)
	{
		int slct = arr[cnt - k + l];
		int nod = numberOfDigits(slct);
		char temp_array[nod + 1];
		sprintf(temp_array, "%d", slct);
		temp_array[nod] = ' ';

		write(ofd, temp_array, nod + 1);
	}

	close(ofd);
	free(arr);
}

int main(int argc, char *argv[])
{
	struct timeval startTime, endTime;
	long elapsedTime;
	gettimeofday(&startTime, NULL); //4. partta istenilen olcumler icin

	int k, n;

	k = atoi(argv[1]); // argv aldik
	n = atoi(argv[2]);

	int *temp = (int *)malloc(n * k * sizeof(int));
	for (int i = 0; i < n; i++) //her dosya icin process actik
	{
		char *infile = argv[(3 + i)];
		char s[] = "tempX.txt";
		s[4] = (i + 1) + '0';
		//puts(s);
		int pid = fork();
		if (pid == 0) //yeni bir processe
		{
			child(infile, s, k);
			_exit(0);
		}
		wait(NULL);
		//ana process
		int rfd = open(s, O_RDONLY, 0);

		int n_read;
		int nsofar = 0;
		char num[1];
		int cnt = 0;
		while ((n_read = read(rfd, num, 1)) > 0)
		{
			if (num[0] == '\n' || num[0] == '\t' || num[0] == ' ')
			{
				temp[cnt + (i * k)] = nsofar;
				cnt++;
				nsofar = 0;
			}
			else
			{
				nsofar *= 10;
				nsofar += atoi(num);
			}
		};

		int delStatus = remove(s);
	}
	bubbleSort(temp, (n * k));
	int ofd = open(argv[3 + n], O_RDWR | O_CREAT, 0777);

	int slct = temp[(n * k) - (k - 1) - 1];
	int nod = numberOfDigits(slct);
	char temp_array[nod];
	sprintf(temp_array, "%d", slct);

	write(ofd, temp_array, nod);
	write(ofd, "\n", strlen("\n"));

	close(ofd);
	free(temp);
	gettimeofday(&endTime, NULL);
	elapsedTime = (endTime.tv_sec - startTime.tv_sec) * 1000000 +
				  endTime.tv_usec - startTime.tv_usec;
	printf("findtopk in ms: %ld\n", elapsedTime);
	return (0);
}
