#include "XBee.h"

extern Mailbox mb_XBee;
extern msg_t mb_XBee_buf[MB_XBEE_MSG_SIZE];

SerialConfig uartCfg=
{
	BAUDRATE,
};

msg_t ThreadComSnd( void *arg )
{
	DATA_COMM *dataComm = NULL;
	msg_t msg;
	systime_t time = chTimeNow();
	int refreshRate = 1000/XBEE_REFRESH_RATE;
	
	initXBee();
	
	while(TRUE)
	{	
		time += MS2ST(refreshRate);
		chMBFetch(&mb_XBee, &msg, TIME_IMMEDIATE);
		dataComm = (DATA_COMM*)msg;
		sendData(dataComm);
		
		chThdSleepUntil(time);
	}
}

void initXBee()
{
	// SD2
	palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
	palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));
	
	sdStart(&SD2, &uartCfg);
}

void sendData( DATA_COMM *data )
{
	char bufSend[10] = {0};
	sprintf(bufSend, "%lf;", data->tangage);
		
	//char bufSend[] = {'T','0','1','5','R','0','4','6','L','0','1','3','A','0','1','2','4','5','B','0','9','5','S','0','5','1'};

	//sprintf(bufSend, "T%03dR%03dL%03dA%05dB%03dS%03d", data->tangage, data->roulis, data->lacet, data->altitude, data->battery, data->signal);
	sdWrite(&SD2, (uint8_t*)bufSend, strlen(bufSend));
}

#warning Function rcvData not tested
DATA_COMM rcvData()
{	
	int i = 0;
	int j = 0;
	DATA_COMM data;
	char bufRead[] = {'T','0','1','5','R','0','4','6','L','0','1','3','A','0','1','2','4','5','B','0','9','5','S','0','5','1','\0'};
	char bufFinalString[] = {'T','0','1','5','R','0','4','6','L','0','1','3','A','0','1','2','4','5','B','0','9','5','S','0','5','1','\0'};
	
	do
	{
		sdReadTimeout(&SD2, (uint8_t*)bufRead, (size_t)strlen(bufRead), (systime_t)TIME_INFINITE); 
		i = 0;
		while(bufRead[i] != 'T')
		{
			i++;
		}
		
		for ( j = 0; j<strlen(bufRead); j++)
		{
			bufFinalString[j] = bufRead[(j+i)%strlen(bufRead)];
		}
	}
	while(sscanf(bufFinalString, "T%dR%dL%dA%dB%dS%d", &data.tangage, &data.roulis, &data.lacet, &data.altitude, &data.battery, &data.signal)!=6);

	return data;
}
