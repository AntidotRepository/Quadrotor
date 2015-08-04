#include "ch.h"
#include "hal.h"
#include "XBee.h"

Mailbox mb_XBee;
msg_t mb_XBee_buf[MB_XBEE_MSG_SIZE];

int main(void)
{
	msg_t msg;
	DATA_COMM data_comm;
	/* OS init */
	halInit();
	chSysInit();
	
	/* Create one more task */
	
	// Delay creation of comm task to let time to sensors being initialized and not send shit over comm
	chThdSleepMilliseconds(2000);
	chMBInit(&mb_XBee, mb_XBee_buf, MB_XBEE_MSG_SIZE);
	chThdCreateStatic(waComSnd, sizeof(waComSnd), NORMALPRIO, ThreadComSnd, NULL);
	
	/* Main task (always present and have priority NORMALPRIO) */
	while(TRUE)
	{
		data_comm.altitude = 1.23;
		msg = (msg_t)&data_comm;
		chMBPost(&mb_XBee, msg, TIME_IMMEDIATE);
		
		chThdSleepMilliseconds(100);
	}
}

