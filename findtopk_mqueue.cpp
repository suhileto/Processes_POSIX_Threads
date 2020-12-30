/*
	Bahadir Kacar 171418001
	Elif Genc 170418038
	isletim sistemleri proje gorevi
*/

//Dosyanın path ini belirlemek için
#include <limits.h>
//Key oluşturmak için
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
//String fonkisyonları için
#include <string.h>
//Dosyaları okumak için (RDONLY)
#include <fcntl.h>
//Hata mesajları için
#include <errno.h>
#include <sys/syscall.h>
//Posix ve messages için gerekli kütüphaneler
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>

//Kullanılacak fonksiyonların tanımları

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

void dosya_islemleri(char *input, char *output, const int k)
{
	//dosyayi ac
	int read_file = open(input, O_RDONLY, 0);
	if (read_file < 0)
	{
		printf("Open failed..1");
		exit(2);
	};
	//dosyayi oku
	int *temp = (int *)malloc(1000000 * sizeof(int));
	int n_read;
	int cnt = 0, nsofar = 0;
	char num[1];
	int ctrl = 1;
	int nodcnt = 0;
	//Dosyanın içindeki sayıları okuma 
	while ((n_read = read(read_file, num, 1)) > 0)
	{
		if (num[0] == '\n' || num[0] == '\t' || num[0] == ' ')
		{
			if (ctrl == 1)
			{
				temp[cnt] = nsofar;
				cnt++;
				nodcnt += basamak_sayisi(nsofar);
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
	close(read_file);
	bubbleSort(arr, cnt);
	//Okunan sayılar char dan int e çevriliyor.
	char temp_array[nodcnt + cnt];
	int nod = basamak_sayisi(arr[cnt - k]);
	int p = 0;
	for (int l = 0; l < k; l++)
	{
		int slct = arr[cnt - k + l];
		p += sprintf(&temp_array[p], "%d ", slct);
		nod += basamak_sayisi(slct);
	}

	strcpy(output, temp_array);

	free(arr);
}
struct mesg_buffer
{
	long mesg_type;
	char mesg_text[6000];
} message;

//Main fonksiyonumuz .Kendisine gönderilen argümanları alıyor
int main(int argc, char *argv[])
{
	//Projenin olduğu dosyanın yolunu buluyor
	char cwd[PATH_MAX];
	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		printf("Current working dir: %s\n", cwd);
	}
	else
	{
		perror("getcwd() error");
		return 1;
	}

	int k, n;

	k = atoi(argv[1]);
	n = atoi(argv[2]);
	if (k < 1 || k > 1000)
	{
		printf("k ,1 ile 1000 arasında olmalı !\n");
		exit(1);
	}
	if (n < 1 || n > 5)
	{
		printf("N ,1 ile 5 arasında olmalı !\n");
		exit(1);
	}

	int *temp = (int *)malloc(n * k * sizeof(int));
	for (int i = 0; i < n; i++)
	{
		//Parametre olarak verdiğimiz dosyalarımız
		char *infile = argv[(3 + i)];
		//Child oluşturma işlemi
		/*Negative Value: creation of a child process was unsuccessful.
		Zero: Returned to the newly created child process.
		Positive value: Returned to parent or caller. The value contains process ID of newly created child process.*/
		int pid = fork();
		if (pid == 0)
		{
			char text[6000] = "0";//
			dosya_islemleri(infile, text, k);
			//Key atama
			key_t key;
			int msgid;

			
			//Mesaj oluşturma
			msgid = msgget(key, 0666 | IPC_CREAT);
			message.mesg_type = 1;

			strcpy(message.mesg_text, text);

			msgsnd(msgid, &message,
				   sizeof(message), 0);

			_exit(0);
		}
		wait(NULL);
	}
	for (int l = 0; l < n; l++)
	{
		//Mesaj okuma 
		int msgid;
		key_t key;
		key = ftok(cwd, l);
		msgid = msgget(key, 0666 | IPC_CREAT);
		msgrcv(msgid, &message,
			   sizeof(message), 1, 0);

		int cnt = 0, nsofar = 0;
		for (int i = 0; message.mesg_text[i] != '\0'; i++)
		{
			char num[1];
			num[0] = message.mesg_text[i];
			if (num[0] == '\n' || num[0] == '\t' || num[0] == ' ')
			{
				temp[cnt + (l * k)] = nsofar;
				cnt++;
				nsofar = 0;
			}
			else
			{
				nsofar *= 10;
				nsofar += atoi(num);
			}
		}
	}
	bubbleSort(temp, (n * k) - 1);
	int ofd = open(argv[3 + n], O_RDWR | O_CREAT, 0777);
	if (ofd < 0)
	
	int slct = temp[(n * k) - (k - 1) - 1];
	int nod = basamak_sayisi(slct);
	char temp_array[nod];
	sprintf(temp_array, "%d", slct);
	//dosyaya yaz
	write(ofd, temp_array, nod);
	write(ofd, "\n", strlen("\n"));
	//dosyayi kapat
	close(ofd);
	free(temp);

	return (0);
}
