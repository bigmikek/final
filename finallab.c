#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h> 
#include <pthread.h>

#define _BSD_SOURCE

#define RADIO_TUNER_FAKE_ADC_PINC_OFFSET 0
#define RADIO_TUNER_TUNER_PINC_OFFSET 1
#define RADIO_TUNER_CONTROL_REG_OFFSET 2
#define RADIO_TUNER_TIMER_REG_OFFSET 3
#define RADIO_TUNER_FIFO_COUNTS 4
#define RADIO_TUNER_FIFO_DATA 5
#define RADIO_PERIPH_ADDRESS 0x43c00000

int readval;
int samples;
int values;
int sent_bytes;
int data[256];
short int mycntr;
int adcset;
int tuneset;
int tune2;
char addr[20];
char* packet;
pthread_t thread_id;


int handle;

volatile unsigned int * get_a_pointer(unsigned int phys_addr)
{

	int mem_fd = open("/dev/mem", O_RDWR | O_SYNC); 
	void *map_base = mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, phys_addr); 
	volatile unsigned int *radio_base = (volatile unsigned int *)map_base; 
	return (radio_base);
}

void radioTuner_tuneRadio(volatile unsigned int *ptrToRadio, float tune_frequency)
{
	float pinc = (-1.0*tune_frequency)*(float)(1<<27)/125.0e6;
	*(ptrToRadio+RADIO_TUNER_TUNER_PINC_OFFSET)=(int)pinc;
}

void radioTuner_setAdcFreq(volatile unsigned int* ptrToRadio, float freq)
{
	float pinc = freq*(float)(1<<27)/125.0e6;
	*(ptrToRadio+RADIO_TUNER_FAKE_ADC_PINC_OFFSET) = (int)pinc;
}



int init_socket()
{
        handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

        if (handle <= 0)
        {
                printf("failed to create socket\n");
                return 1;
        }

        printf("sockets successfully initialized\n");

        return 0;
}
void *UDPtime (void *vargp)
{
    volatile unsigned int *my_periph = get_a_pointer(RADIO_PERIPH_ADDRESS);
    unsigned short port = 30000;
    
    printf("\n\rThread Established in UDPtime!\n\r");
    init_socket();

        struct sockaddr_in address;

    memset((void * ) &address, 0, sizeof(address));

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = inet_addr(addr); // this is address of host which I want to send the socket
        address.sin_port = htons(port);
        //printf("\n\raddress set!\n\r");


    
        printf("\n\rbeginning network transmission, standby\n\r");
            
            while(1)
            {
            readval = *(my_periph+RADIO_TUNER_FIFO_COUNTS);
            if (readval > 256)
            {
                //printf("\n\rread > 256\n\r");
                mycntr = readval;
                //printf("\n\rreadval set to data\n\r");
            for (int i = 0; i < 256; i++)
                {
                    data[i] = *(my_periph+RADIO_TUNER_FIFO_DATA);
                }
            //printf("\n\rdata array filled\n\r");
            char sendbuffer[1026];
            memcpy(sendbuffer, &mycntr, 2);
            memcpy(sendbuffer + 2, &data, 1024);
            //printf("\n\r%x\n\r", data[512]);
            sent_bytes = sendto(handle, sendbuffer, 1026, 0, (const struct sockaddr*) &address, sizeof(struct sockaddr_in));
            }
        
        
        }
    
}

void tcity()
{
	volatile unsigned int *my_periph = get_a_pointer(RADIO_PERIPH_ADDRESS);
	printf("\n\rPlease enter desired tuning frequency\n\r");
    scanf("%d", &tuneset);
    radioTuner_tuneRadio(my_periph,tuneset);  
    printf("\n\rfrequency tuned to %d Hz\n\r", tuneset);
}

void ucity()
{
	volatile unsigned int *my_periph = get_a_pointer(RADIO_PERIPH_ADDRESS);
	adcset = adcset + 100;
        radioTuner_setAdcFreq(my_periph,adcset);
        printf("\n\rNew Frequency is %d\n\r", adcset);
}

void Ucity()
{
	volatile unsigned int *my_periph = get_a_pointer(RADIO_PERIPH_ADDRESS);
	adcset = adcset + 1000;
        radioTuner_setAdcFreq(my_periph,adcset);
        printf("\n\rNew Frequency is %d\n\r", adcset);
}

void dcity()
{
	volatile unsigned int *my_periph = get_a_pointer(RADIO_PERIPH_ADDRESS);
	adcset = adcset - 100;
        radioTuner_setAdcFreq(my_periph,adcset);
        printf("\n\rNew Frequency is %d\n\r", adcset);
}
void Dcity()
{
	volatile unsigned int *my_periph = get_a_pointer(RADIO_PERIPH_ADDRESS);
	adcset = adcset - 1000;
        radioTuner_setAdcFreq(my_periph,adcset);
        printf("\n\rNew Frequency is %d\n\r", adcset);
}
void fcity()
{
    volatile unsigned int *my_periph = get_a_pointer(RADIO_PERIPH_ADDRESS);
	printf("\n\rPlease enter desired listening frequency\n\r");
    scanf("%d", &adcset);
    radioTuner_setAdcFreq(my_periph,adcset);
    printf("\n\rNew Frequency is %d\n\r", adcset);
}

void radio_customization()
{
    pthread_create(&thread_id, NULL, UDPtime, NULL);
    printf("\n\rThread Created!\n\r");
    char sel[20];
    while(1)
    {
    printf("\n\rBeginning Customization!\n\r");
    printf("\n\rPress t to change the tune frequency\n\rPress U to change freq by 1kHz\n\rPress u to change freq by 100Hz\n\rPress d to change freq by -100 Hz\n\rPress D to change freq by -1khZ\n\rPress f to change the frequency\n\r");
    scanf("%s", sel);
    const char *inputsel[]={"t","u","d","U","D", "f"};
    if (strcmp(sel,inputsel[0]) == 0)
    {
        
        tcity();
    }
    else if (strcmp(sel,inputsel[3]) == 0)
    {
        
        Ucity();
    }
    else if (strcmp(sel,inputsel[1]) == 0)
    {
        
        ucity();
    }
    else if (strcmp(sel,inputsel[2]) == 0)
    {
        
        dcity();
    }
    else if (strcmp(sel,inputsel[4]) == 0)
    {
        
        Dcity();
    }
    else if (strcmp(sel,inputsel[5]) == 0)
    {
        
        fcity();
    }
    else
    {
        printf("\n\rinvalid entry, please try again\n\r");
    }
    }
    
    

}



int main()
{

     
// first, get a pointer to the peripheral base address using /dev/mem and the function mmap
    
    printf("\r\n\r\n\r\nLab 6 Michael Krzystowczyk - SDR Milestone Demonstration\n\r");
    volatile unsigned int *my_periph = get_a_pointer(RADIO_PERIPH_ADDRESS);	
    printf("\n\rPlease enter desired listening frequency\n\r");
    scanf("%d", &adcset);
    radioTuner_setAdcFreq(my_periph,adcset);
    printf("\n\radc freq set!\n\r");
    printf("\n\rPlease enter desired tuning frequency\n\r");
    scanf("%d", &tuneset);
    radioTuner_tuneRadio(my_periph,tuneset);
    printf("\n\rradio tuned!\n\r");
    printf("\n\rplease enter destination ip address\n\r");
    scanf("%19s", addr);
    printf("\n\rNetwork traffic started\n\r");
    //UDPtime(my_periph);
    radio_customization();
    
    return 0;
}
