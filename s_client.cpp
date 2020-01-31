//
// Created by asus on 2019/11/3.
//

#include <Winsock2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define toascii(c) (((unsigned char)(c)) & 0x7f)

struct send_buffer
{
    char message[300];
}sendBuffer[10];

char *appLayer(char[]);   //add header for application layer
char *transLayer(char[]); //add header for transport layer
char *netLayer(char[], int);   //add header for net layer
char *linkLayer(char[]);  //add header for link layer
void phyLayer(void);      //translate it to ASCII

char *get_dstmacaddress(char[]);
char *get_srcmacaddress(char[]);
char *bit_fill(char[]);
unsigned char crc_check(char[]);
char *get_crc(char[], unsigned char);

char message[1000];
int send_times = 0;
struct send_buffer judge_length_of_message(char inputstring[], struct send_buffer sendBuffer[])
{
    char head2[12] = "application";
    char head[100] = "";
    char *ret;
    char *p;
	p = strtok(inputstring, "#");
	char head1[100] = "";
	strcpy(head1, p);
	p = strtok(NULL, "#");
	p = strtok(NULL, "#");
	char data[100];
	strcpy(data, p);
	strcpy(head, head1);
	strcat(head, "#");
	strcat(head, head2);
	strcat(head, "#");
    int len = strlen(data);
    ret = data;
    for (; len > 0; len -= 10)
    {
        char temp[101] = "";
        strcpy(sendBuffer[send_times].message, head);
        if (len > 10)
        {
            strncpy(temp, ret, 10);
            ret = ret + 10;    
		}
        else
        {
            strncpy(temp, ret, len);
        }
        strcat(sendBuffer[send_times++].message, temp);
    }
}

void wait_time()
{
	int ppp = 1000000000;
	while(ppp)
	{
		ppp--;
	};
}

int main()
{
    WSADATA wsaData;
    SOCKET sockClient;                    //client Socket
    SOCKADDR_IN addrServer;               //server address
    WSAStartup(MAKEWORD(2, 2), &wsaData); //create client socket

    while (1)
    {
        sockClient = socket(AF_INET, SOCK_STREAM, 0);             //define connect address socket
        addrServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); //aim IP(127.0.0.1 is this computer itself)
        addrServer.sin_family = AF_INET;
        addrServer.sin_port = htons(8888); //connect port 6000
        //connect to server
        connect(sockClient, (SOCKADDR *)&addrServer, sizeof(SOCKADDR)); //send data
        char data[100] = "";
        printf("please enter the data\n");
        int i = 0;
        do
        {
            char ch;
            ch = getchar();
            if (ch != '\n')
            {
                data[i] = ch;
                i++;
            }
            else
                break;
        } while (i < 100);
        int data_len = i;
		char message[1000] = " ";	
        for (int j = 0; j < data_len; j++)
        {
            message[j] = data[j];
        }
        printf("\n发送方的信息：\n");
        printf("ip-address: 127.0.0.1\n");
        printf("port: 8888\n");
        printf("\n");
        printf("接受方的信息: \n");
        printf("ip-address: 127.0.0.1\n");
        printf("port: 8888\n");
        printf("\n");
        
        printf("发送的数据为:");
        puts(message);
        printf("\n");
        appLayer(message);
		wait_time();
        transLayer(message);
		wait_time();
		send_times = 0;
		judge_length_of_message(message, sendBuffer);
        for (int i = 0; i < send_times; i++)
        {
        	if(send_times>1)
        	{
        		printf("\n");
        		wait_time();
        		printf("-------------分片%d:-------------\n", i+1);
        	}
        	char inputstring[500];
        	strcpy(inputstring, sendBuffer[i].message);
            netLayer(inputstring, send_times-i);
			wait_time();
            linkLayer(inputstring);
			wait_time();
            phyLayer();
            send(sockClient, inputstring, strlen(inputstring) + 1, 0);
			wait_time();
        }
    closesocket(sockClient); //close socket
    }
    
    WSACleanup();
    
    return 0;
}

char *appLayer(char inputString[])
{
	printf("=======================================application_layer=======================================\n");
    char msg[200] = {"application"};
    strcat(msg, "#");
	int data_len = strlen(inputString);
    char input_temp[500] = "";
    memset(input_temp, 0, sizeof(input_temp));
    char ascii_temp[10] = "";
    printf("\n首先进行ascii码转化\n");
    printf("\n转换前:");
    puts(inputString);
    for(int i=0;i<data_len;i++)
    {
    	memset(ascii_temp, 0, sizeof(ascii_temp));
    	int ascii_value;
    	ascii_value = int(inputString[i]);
    	int p = 7;
    	int index=0;
    	while(p>=0)
    	{
    		ascii_value = ascii_value - pow(2, p);
    		if(ascii_value>0)
    			ascii_temp[index++] = '1';
    		else
    		{
    			ascii_temp[index++] = '0';
    			ascii_value = ascii_value + pow(2, p);
    		}
    		p--;
    	}
    	strcat(input_temp, ascii_temp);
    }
    strcpy(inputString, input_temp);
    printf("\n转换后:");
    puts(inputString);
    printf("\n"); 
    strcat(msg, inputString);
    strcpy(inputString, msg);
    printf("Application layer: ");
    puts(inputString);
	printf("\n");
    return inputString;
}

char *transLayer(char inputString[])
{
	printf("=======================================transport_layer=========================================\n");
	printf("\n");
	printf("开始添加TCP头部信息\n");
    char TCPheader[500] = "";

    char SourcePort[17] = "0000000011111111"; //0-15bit Source Port

    char DesPort[17] = "0010101101100111"; //16-31bit Destination Port 11111

    char SequenceNum[33] = "00000000000000000000000000001011"; //32bits Sequence Number

    char AckNum[33] = "00000000000000000000000011111011"; //32bits Acknowledgement Number

    char HeaderLength[5] = "0101"; //4bits Header Length

    char Reserved[7] = "000000"; //6bits Reserved

    char Flag[7] = "000000"; //URG;ACK;PSH;RST;SYN;FIN

    char Windowsize[17] = "0000000000000111"; //16bits Window Size

    char CheckSum[17] = "0101010101010010"; //16bits Checksum

    char UrgentP[17] = "0000000000001111"; //16bits UrgentPointer
    strcat(TCPheader, SourcePort);
    strcat(TCPheader, DesPort);
    strcat(TCPheader, SequenceNum);
    strcat(TCPheader, AckNum);
    strcat(TCPheader, HeaderLength);
    strcat(TCPheader, Reserved);
    strcat(TCPheader, Flag);
    strcat(TCPheader, Windowsize);
    strcat(TCPheader, CheckSum);
    strcat(TCPheader, UrgentP);
    strcat(TCPheader, "#");
	printf("\n添加完毕\n");
    char TCP_message[600] = "";
    strcat(TCP_message, TCPheader);
    strcat(TCP_message, inputString);

    strcpy(inputString, TCP_message);
    printf("\nTransport layer: ");
    puts(inputString);
	printf("\n");
    return inputString;
}

char *netLayer(char inputstring[], int send_time)
{
	printf("=======================================network_layer===========================================\n");
	printf("\n");
    char NetHeader[300] = "";
    char VER[5] = "0100";                                               //4bits
    char HLEN[5] = "1111";                                              //4bits
    char Service[9] = "00000000";                                       //8bits
    char totalLength[17] = "";                                          // 16bits  后面获取
    char Identification[17] = "0000000000000000";                       //16bits
    char Flag[4] = "";                                                  //3bits   后面获取
    char FragmentationOffset[14] = "0000000000000";                     //13bits
    char TTL[9] = "11111111";                                           //8bits,time to live
    char Protocol[9] = "00000000";                                      //8bits
    char HeaderChecksum[17] = "";                                       //16bits   后面获取
    char SourIPAddress[33] = "00000000000000000000000000000000";        //32bits
    char DestinationIPAddress[33] = "00000000000000000000000000000000"; //32bits
    char Option[2] = "";                                                //empty option

    int data_len = 20; //初始长度
    char *ret;
    ret = strchr(inputstring, '#') + 1;
    for (ret;; ret++, data_len++)
        if (*ret == '\0')
            break;
    // 获取标志位信息
    //printf("data_len:%d", data_len);
    if (send_time == 1)
        strcpy(Flag, "000");
    else
        strcpy(Flag, "010");

    //printf("Flag:%s\n", Flag);

    //获取总长度信息
    char temp[30] = "";
    int turns = 0;
    for (int i = 0; data_len > 0; turns++)
    {
        temp[i] = data_len % 2 + 48; // 转化为ASCII码
        i = i + 1;
        data_len = data_len / 2;
    }
    for (int i = 0; i < turns; i++)
        totalLength[i] = temp[turns - i - 1];
    // 获取首部校验和
    printf("\n开始首部校验\n");
    char temp_head[200];
    strcat(temp_head, VER);
    strcat(temp_head, HLEN);
    strcat(temp_head, Service);
    strcat(temp_head, totalLength);
    strcat(temp_head, Identification);
    strcat(temp_head, Flag);
    strcat(temp_head, FragmentationOffset);
    strcat(temp_head, TTL);
    strcat(temp_head, Protocol);
    strcat(temp_head, SourIPAddress);
    strcat(temp_head, DestinationIPAddress);
    strcat(temp_head, Option);
    int count = 0;
    for (int i = 0; i < 9; ++i)
    {
        int pre_16_bit[16] = {0};
        for (int j = 0; j < 16; ++j)
        {
            pre_16_bit[j] = temp_head[i * 16 + j] - 48;
            count = count + pow(2 * pre_16_bit[j], 15 - j);
        }
    }
    count = count - 1; // 取反加一，换回来就是整数减一
    turns = 0;
    char temp1[50] = "";
    char temp2[50] = "";
    for (int i = 0; count > 0; turns++)
    {
        temp1[i] = count % 2 + 48; // 转化为ASCII码
        i = i + 1;
        count = count / 2;
    }
    for (int i = 0; i < turns; i++)
        temp2[i] = temp1[turns - i - 1];
    //printf("%s\n", temp2);
    if (turns >= 16)
        for (int i = 0; i < 16; ++i)
        {
            HeaderChecksum[i] = temp2[turns - 16 + i];
        }
    else
    {
        for (int i = 0; i < 16 - turns; i++)
            HeaderChecksum[i] = 48;
        for (int i = 0; i < turns; i++)
            HeaderChecksum[16 - turns + i - 1] = temp2[i];
    }
    printf("\n获取首部校验和\n");
    // 获取 NetHeader
    strcat(NetHeader, VER);
    strcat(NetHeader, HLEN);
    strcat(NetHeader, Service);
    strcat(NetHeader, totalLength);
    strcat(NetHeader, Identification);
    strcat(NetHeader, Flag);
    strcat(NetHeader, FragmentationOffset);
    strcat(NetHeader, TTL);
    strcat(NetHeader, Protocol);
    strcat(NetHeader, HeaderChecksum);
    strcat(NetHeader, SourIPAddress);
    strcat(NetHeader, DestinationIPAddress);
    strcat(NetHeader, Option);
    // 合并inputString
    char temp_input[300] = "";
    strcat(temp_input, NetHeader);
    strcat(temp_input, "+");
    strcat(temp_input, inputstring);
    strcpy(inputstring, temp_input);
    printf("\nNet layer:");
    puts(inputstring);
	printf("\n");
    return inputstring;
}

char *linkLayer(char inputString[])
{
	printf("=======================================linker_layer============================================\n");
	printf("\n");
    char frame[800] = "";

    char Frame_sync[9] = "01010101"; //frame synchronization code
    strcat(frame, Frame_sync);

    char Frame_soh[9] = "11111111"; //frame start code
    strcat(frame, Frame_soh);

    char Frame_dst[49] = "";
    get_dstmacaddress(Frame_dst);
    strcat(frame, Frame_dst);

    char Frame_src[49] = "";
    get_srcmacaddress(Frame_src);
    strcat(frame, Frame_src);

    strcat(frame, "+");
    bit_fill(inputString);
    strcat(frame, inputString);

    unsigned char crc_code_un;
    char crc_code[9] = "";
    crc_code_un = crc_check(inputString);
    get_crc(crc_code, crc_code_un);
    strcat(frame, "=");
    strcat(frame, crc_code);

    strcpy(inputString, frame);

    printf("Link layer: ");

    puts(inputString);
    printf("\n循环校验码为 %s\n", crc_code);
	printf("\n");
    return inputString;
}

char *get_dstmacaddress(char Frame_dst[])
{
    char mac_dst[49] = "101010101010101010101010101010101010101010101010";
    strcpy(Frame_dst, mac_dst);
    return Frame_dst;
}

char *get_srcmacaddress(char Frame_src[])
{
    char mac_dst[49] = "111010101010101010101010001110101110101010101110";
    strcpy(Frame_src, mac_dst);
    return Frame_src;
}

char *bit_fill(char inputstring[])
{
	printf("\n开始字节填充...\n");
    char temp[700] = "";
    int pos_input = 0, pos_temp = 0;
    int count = 0;
    int flag = 0;
    while (inputstring[pos_input] != '\0')
    {
        if (inputstring[pos_input] == '1')
        {
            count++;
            if (count > 7)
            {
                flag = 1;
                count = 0;
            }
            if (flag == 1)
            {
                temp[pos_temp++] = '0';
                flag = 0;
            }
            else
            {
                temp[pos_temp++] = inputstring[pos_input++];
            }
        }
        else
        {
            count = 0;
            temp[pos_temp++] = inputstring[pos_input++];
        }
    }
    strcpy(inputstring, temp);
	printf("\n结束字节填充...\n");
	printf("\n");
    return inputstring;
}

unsigned char crc_check(char inputstring[])
{
    unsigned char reg_crc;
    unsigned char poly;
    int s_crcchk;
    s_crcchk = 0;
    reg_crc = 0xff;
    poly = 0x0f;
    int i = 0;
    int len = strlen(inputstring);
    while (len--)
    {
        unsigned char c = inputstring[i++];
        reg_crc = c;
        for (s_crcchk = 0; s_crcchk < 8; s_crcchk++)
        {
            if (reg_crc & 0x01)
            {
                reg_crc >>= 1;
                reg_crc = reg_crc ^ poly;
            }
            else
            {
                reg_crc = reg_crc >> 1;
            }
        }
    }
    return reg_crc;
}

char *get_crc(char crc_code[], unsigned char crc_code_un)
{
    int crc_ascii = 0;
    crc_ascii = toascii(crc_code_un);
    int exp = 7;
    int index = 0;
    while (crc_ascii >= 0 && exp >= 0)
    {
        if (crc_ascii >= pow(2, exp))
        {
            crc_code[index++] = '1';
            crc_ascii -= pow(2, exp);
            exp--;
        }
        else
        {
            crc_code[index++] = '0';
            exp--;
        }
    }
    return crc_code;
}

void phyLayer(void)
{
	printf("=======================================phyical_layer===========================================\n");
	printf("\n");
    printf("Phyical layer: ");
    printf("00000101011010101101010101010101101010101011011111"
           "01010101010101010101010101111111110000111111101111"
           "01010101010101010101010111111111010101010101010111"
           "01010101010100000101011010101101010101010101101011"
           "10101011011111010101010101010101010101011111111100"
           "01111111010101010101010101010101011111111101100111"
           "01010101010101010101010101000010101010110010110101\n");
    printf("\n");
    printf("结束传送\n");
    printf("\n");
}
