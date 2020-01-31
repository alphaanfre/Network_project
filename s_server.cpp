//
// Created by asus on 2019/11/3.
//

#include <Winsock2.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

//bool is_continue(char[]);
char *appLayer(char[]);   //remove header for application layer
char *transLayer(char[]); //remove header for transport layer
char *netLayer(char[]);   //remove header for net layer
char *linkLayer(char[]);  //remove header for link layer
void phyLayer(char[]);    //show phyical layer

char *de_bitfill(char[]);
bool is_continue(char inputstring[])
{
    int index;
    index = 31 + 16 + 2;
    if (inputstring[index] == '1')
        return true;
    else
        return false;
}

int main()
{
    WSADATA wsaData;
    SOCKET sockServer;
    SOCKADDR_IN addrServer;
    SOCKET sockClient;
    SOCKADDR_IN addrClient;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    sockServer = socket(AF_INET, SOCK_STREAM, 0);
    addrServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); //INADDR_ANY means any IP
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(8888);                           //bind port 6000
    bind(sockServer, (SOCKADDR *)&addrServer, sizeof(SOCKADDR)); //Listen
    while (1)
    {
    	printf("\n!!!开始接收\n");
        listen(sockServer, 5); //5 wait connect number
        printf("server launched:\nlisting...\n");
        int len = sizeof(SOCKADDR);

        char recvBuf[20000]; //recive string

        sockClient = accept(sockServer, (SOCKADDR *)&addrClient, &len); //block process??until client connect
        if (sockClient == INVALID_SOCKET)
        {
            printf("accept error !");
            continue;
        }

        int count = 0;
		char recvBuf_sum[5000] = "";
		int i=0;        		
        while(recv(sockClient, recvBuf, 20000, 0)) //accept client data
        {
        	if(i == 0)
        	{
				printf("\n接受方的信息: \n");
        		printf("ip-address: 127.0.0.1\n");
        		printf("port: 8888\n");
        		printf("\n");
        		printf("\n发送方的信息：\n");
        		printf("ip-address: 127.0.0.1\n");
        		printf("port: 8888\n");
       	 		printf("\n");	   
			}
			printf("\n");
			printf("!!!===============收到第%d个数据===============!!!\n", i+1);
			i++;
        	phyLayer(recvBuf);
        	linkLayer(recvBuf);
        	netLayer(recvBuf);
           	count++;
           	if (count == 1)
               strcpy(recvBuf_sum, recvBuf);
			else
            {
                char *temp_p;
                char recvBuf_temp[5000];
                strcpy(recvBuf_temp, recvBuf_sum);
                
               	temp_p = strtok(recvBuf, "#");
              	temp_p = strtok(NULL, "#");
                temp_p = strtok(NULL, "#");
				//printf("%d ....%s\n", count, recvBuf_temp);
				//printf("%d =====%s\n", temp_p);
                strcat(recvBuf_temp, temp_p);
                strcpy(recvBuf_sum, recvBuf_temp);
            }
		}  
		if(i > 1)
		{	
			printf("\n合并后的数据为\n");
			puts(recvBuf_sum);
		}
		transLayer(recvBuf_sum);
        appLayer(recvBuf_sum);
		printf("\n");      	
		printf("收到的数据为: ");
        puts(recvBuf_sum);
        printf("\n");
        closesocket(sockClient); //close socket
    }

    WSACleanup();

    system("pause");
    return 0;
}

char *appLayer(char inputstring[])
{
	printf("\n=======================================application_layer=======================================\n");
    printf("\nappLayer: %s\n", inputstring);
    char *ret;
    ret = strchr(inputstring, '#') + 1;
    strcpy(inputstring, ret);
    int ascii_value = 0;
    int count = 0;
    char input_temp[200] = "";
    int index = 0;
    for(int i=0;i<strlen(inputstring);i++)
    {
    	ascii_value += int(inputstring[i] - '0') * pow(2, 7-count);
    	count++;
    	if(count == 8)
    	{
    		count = 0;
    		input_temp[index++] = ascii_value + 1;
    		ascii_value = 0;
    	}
    }
    strcpy(inputstring, input_temp);
    return inputstring;
}

char *transLayer(char inputstring[])
{
	printf("\n=======================================transport_layer=========================================\n");
    printf("\ntranLayer: %s\n", inputstring);
    char *ret;
    ret = strchr(inputstring, '#') + 1;
	strcpy(inputstring, ret);
	puts(inputstring);
    return inputstring;
}

char *netLayer(char inputstring[])
{
	printf("\n=======================================network_layer===========================================\n");
    printf("\nnetLayer: %s\n", inputstring);
    char *ret;
    ret = strchr(inputstring, '+') + 1;
    strcpy(inputstring, ret);
    return inputstring;
}

char *linkLayer(char inputstring[])
{
	printf("\n=======================================linker_layer============================================\n");
    printf("\nlinkLayer: %s\n", inputstring);
    char *ret;
    ret = strchr(inputstring, '+') + 1;
    strcpy(inputstring, ret);
    printf("\n还原被字节填充的数据\n");
    de_bitfill(inputstring);
    printf("\n还原成功\n");
    char *p;
    p = strtok(inputstring, "=");
    strcpy(inputstring, p);

    return inputstring;
}

char *de_bitfill(char inputstring[])
{
    int index, i_temp;
    char temp_input[700] = "";
    int count = 0;
    index = 0;
    i_temp = 0;
    while (inputstring[index] != '\0')
    {
        if (inputstring[index] == '1')
        {
            count++;
            if (count > 7)
            {
                temp_input[i_temp++] = inputstring[index++];
                index++;
                count = 0;
            }
            else
            {
                temp_input[i_temp++] = inputstring[index++];
            }
        }
        else
        {
            temp_input[i_temp++] = inputstring[index++];
            count = 0;
        }
    }
    strcpy(inputstring, temp_input);
    return inputstring;
}

void phyLayer(char inputstring[])
{
	printf("\n=======================================phyical_layer===========================================\n");
    printf("\nPhyical layer: ");
    printf("00000101011010101101010101010101101010101011011111"
           "01010101010101010101010101111111110000111111101111"
           "01010101010101010101010111111111010101010101010111"
           "01010101010100000101011010101101010101010101101011"
           "10101011011111010101010101010101010101011111111100"
           "01111111010101010101010101010101011111111101100111"
           "01010101010101010101010101000010101010110010110101");
    printf("\n");
}
