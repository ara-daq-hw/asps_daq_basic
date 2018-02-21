#include <ArduinoPTP.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

extern bool feedWatchdog;

char buffer[100];
//uint8_t buffer_out[100];
//char buffer_event[100];
int v0 = 0; //0 if debugging mode is off.
int v4=1;//0 if debugging is off.
int globalCountgeneral = 0; //keep track of number of general packets.
int globalCountevent = 0; //keep track of number of event packets.
int k=0; 
int i=0;
int sequence_id_delay=0;
//char timestamp[6];
//IPAddress ip(192,168,1,177);
IPAddress remoteIP;
uint16_t remotePort;

typedef struct _PTPMsg {
		uint16_t sourcePortID;
		uint8_t version;
		uint8_t domain;
		//long long clockID;
		uint8_t clockID[8];
		unsigned int transportSpecific; 
		unsigned int type; 
		uint16_t length;
		uint16_t flags;
		long long correction;
		char correction1[8];
		uint16_t sequenceID;
		uint8_t control;
		uint8_t log_period; 
} PTPMsg_t;

typedef struct _TimeStamp {
	long long seconds; 
	uint32_t nanoseconds; 
	
} TimeStamp_t;

typedef struct _ClockQuality {
	uint8_t clockClass;
	uint8_t clockAccuracy;
	uint16_t clockVariance;
} ClockQuality_t;

typedef struct _AnnounceMsg{
	struct _TimeStamp originTimestamp;
	uint8_t clockID[8];
	uint16_t currentUtcOffset;
	uint8_t grandmasterPriority1;
	uint8_t grandmasterPriority2;
	uint16_t stepsRemoved;
	struct _ClockQuality masterClockQuality;
	uint8_t TimeSource;
	
} AnnounceMsg_t;

typedef struct _SyncMsg {
	struct _TimeStamp originTimestamp;
} SyncMsg_t;

typedef struct _FollowupMsg {
	struct _TimeStamp preciseOriginTimestamp;
} FollowupMsg_t;

typedef struct _DelayReq {
	struct _TimeStamp originTimestampDelay;
} DelayReqMsg_t;

uint8_t ArduinoPTP::begin() {
	if (!_general.begin(320)) return 1;
	if (!_event.begin(319)) return 1;
	return 0;
}
void ArduinoPTP::stop() {
	_general.stop();
	_event.stop();
}

PTPMsg_t PTPMsg;
FollowupMsg_t FollowupMsg;
AnnounceMsg_t AnnounceMsg;
SyncMsg_t SyncMsg;

void ArduinoPTP::handle() {
	feedWatchdog=true;
	int packetSize;
	int packetByte;
	packetSize = _general.parsePacket();
	
	//Serial.println("packet size:%d", packetSize);
	if (packetSize) {
		// Do something to handle general packets.
		globalCountgeneral++;
		//if(globalCountgeneral%10==0)
		
			memset(buffer,0,sizeof(buffer));
			/*if (v4)
			{
				Serial.println("");
			}*/
			/*if (v3)
			{
			Serial.print(globalCountgeneral);
			}*/
			if (packetSize==44)
			packetByte = _general.read(buffer,44);
		    else if (packetSize==64)
		    packetByte = _general.read(buffer,64);
		    else if (packetSize==54)
			packetByte=_general.read(buffer,54);
			/*if (v1)
			{
			Serial.println(packetByte);
			}*/
			PTPMsg.version=buffer[1];
			PTPMsg.domain=buffer[4];
			PTPMsg.sourcePortID=(buffer[28] << 8) | buffer[29];
			PTPMsg.transportSpecific= (buffer[0]>>4)& 0xF;
			PTPMsg.type=(buffer[0])& 0xF;
			PTPMsg.length=(buffer[2] << 8) | buffer[3];
			PTPMsg.flags=((buffer[6] << 8) | buffer[7]);
			PTPMsg.sequenceID=(buffer[30] << 8) | buffer[31];
			PTPMsg.control=buffer[32];
			PTPMsg.log_period=buffer[33];
			
			//if (PTPMsg.control==3)
			//{
			//	Serial.println(PTPMsg.type);
			//}
			/*i=0;
            for (k=8;k<16;k++)
			{
				PTPMsg.correction[i]=buffer[k];
				i++;
			}*/
			i=0;
			for(k=20;k<28;k++)
			{
				PTPMsg.clockID[i]=buffer[k];
				i++;
			}
			PTPMsg.correction=(buffer[8] << 56) | (buffer[9] << 48) | (buffer[10] << 40) | (buffer[11] << 32) | (buffer[12] << 24) | (buffer[13] << 16) | (buffer[14] << 8) | (buffer[15]);
			//PTPMsg.clockID =(buffer[20] << 56) | (buffer[21] << 48) | (buffer[22] << 40) | (buffer[23] << 32) | (buffer[24] << 24) | (buffer[25] << 16) | (buffer[26] << 8) | (buffer[27]);
			itoa(PTPMsg.correction,PTPMsg.correction1,10);
			//itoa(PTPMsg.clockID,PTPMsg.clockID1,10);
			
			
			/*if (v1)
			{
			for (int i=0;i<44;i++)
			{
				
				
				Serial.print(buffer[i],HEX);
				Serial.print(" ");
			}
			}
			if (v2)
			{
				Serial.print("Printing Version info general:");
				Serial.print(PTPMsg.version);
				Serial.println("");
				Serial.print("Printing Domain info general:");
				Serial.print(PTPMsg.domain);
				Serial.println("");
				Serial.print("Printing source port ID general:");
				Serial.print(PTPMsg.sourcePortID);
				Serial.println("");
				Serial.print("Printing transport specific general:");
				Serial.print(PTPMsg.transportSpecific,HEX);
				Serial.println("");
				Serial.print("Printing message type general:");
				Serial.print(PTPMsg.type,HEX);
				Serial.println("");
				Serial.print("Printing length info general:");
				Serial.print(PTPMsg.length);
				Serial.println("");
				Serial.print("Printing flags info general:");
				Serial.print(PTPMsg.flags,HEX);
				Serial.println("");
				Serial.print("Printing sequence ID info general:");
				Serial.print(PTPMsg.sequenceID);
				Serial.println("");
				Serial.print("Printing control info general:");
				Serial.print(PTPMsg.control);
				Serial.println("");
				Serial.print("Printing log period info general:");
				Serial.print(PTPMsg.log_period);
				Serial.println("");
				Serial.print("Printing clock info general:");
				for(k=0;k<8;k++)
				{
					Serial.print(PTPMsg.clockID[k],HEX);
				}
				Serial.println("");
				//Serial.println("%02x02x02x02x02x02x02x02x",PTPMsg.clockID1);
				//Serial.print("Size of clockid:");
				//Serial.println(sizeof(PTPMsg.clockID));
				Serial.print("Printing correction info general:");
				Serial.println(PTPMsg.correction1);
				/*for(k=0;k<8;k++)
				{
					Serial.print(PTPMsg.correction[k]);
				}
				Serial.println("");
				Serial.print("Size of correction:");
				
				Serial.println(sizeof(PTPMsg.correction));
				
				
			}*/
			if (packetSize==44 && PTPMsg.control == 2)
			{
				//long bigNumber = (buffer[34] << 40) | (buffer[35] << 32) | (buffer[36] << 24) | (buffer[37] << 16) | (buffer[38] << 8) | (buffer[39]);
				//FollowupMsg.preciseOriginTimestamp.seconds = bigNumber.toString();
			    FollowupMsg.preciseOriginTimestamp.seconds = (buffer[34] << 40) | (buffer[35] << 32) | (buffer[36] << 24) | (buffer[37] << 16) | (buffer[38] << 8) | (buffer[39]);
				FollowupMsg.preciseOriginTimestamp.nanoseconds=(buffer[40] << 24) | (buffer[41] << 16) |(buffer[42] << 8) | buffer[43];
				//itoa(FollowupMsg.preciseOriginTimestamp.seconds,timestamp,10);
				
				/*if(v2)
			{
				//Serial.print(packetSize);
				Serial.print("Printing follow-up timestamp (seconds):");
				Serial.println(timestamp);
				Serial.print("Size of seconds timestamp:");
				Serial.println(sizeof(FollowupMsg.preciseOriginTimestamp.seconds));
				Serial.print("Printing follow-up timestamp (nanoseconds)");
				Serial.println(FollowupMsg.preciseOriginTimestamp.nanoseconds);
			}*/
			}
			else if (packetSize==64 && PTPMsg.control == 5)
			{
				AnnounceMsg.originTimestamp.seconds = (buffer[34] << 40) | (buffer[35] << 32) | (buffer[36] << 24) | (buffer[37] << 16) | (buffer[38] << 8) | (buffer[39]);
				AnnounceMsg.originTimestamp.nanoseconds = (buffer[40] << 24) | (buffer[41] << 16) |(buffer[42] << 8) | buffer[43];
				//itoa(AnnounceMsg.originTimestamp.seconds,timestamp,10);
				AnnounceMsg.currentUtcOffset = (buffer[44] << 8)|(buffer[45]);
				AnnounceMsg.grandmasterPriority1=buffer[47];
				AnnounceMsg.grandmasterPriority2=buffer[52];
				AnnounceMsg.stepsRemoved=(buffer[61] << 8)|buffer[62];
				AnnounceMsg.TimeSource=buffer[63];
				AnnounceMsg.masterClockQuality.clockClass=buffer[48];
				AnnounceMsg.masterClockQuality.clockAccuracy=buffer[49];
				AnnounceMsg.masterClockQuality.clockVariance=(buffer[50] << 8)|buffer[51];
				k=0;
				for(i=53;i<61;i++)
				{
					AnnounceMsg.clockID[k]=buffer[i];
					k++;
				}
				/*if(v1)
				{
					Serial.print("Announce timestamp seconds:");
					Serial.println(timestamp);
					Serial.print("Announce timestamp nanoseconds:");
					Serial.println(AnnounceMsg.originTimestamp.nanoseconds);
					Serial.print("Announce timestamp utc offset:");
					Serial.println(AnnounceMsg.currentUtcOffset);
					Serial.print("Announce timestamp priority 1:");
					Serial.println(AnnounceMsg.grandmasterPriority1);
					Serial.print("Announce timestamp priority 2:");
					Serial.println(AnnounceMsg.grandmasterPriority2);
					Serial.print("Announce timestamp steps removed:");
					Serial.println(AnnounceMsg.stepsRemoved);
					Serial.print("Announce timestamp time source:");
					Serial.println(AnnounceMsg.TimeSource,HEX);
					Serial.print("Announce timestamp clock class:");
					Serial.println(AnnounceMsg.masterClockQuality.clockClass);
					Serial.print("Announce timestamp clock accuracy:");
					Serial.println(AnnounceMsg.masterClockQuality.clockAccuracy,HEX);
					Serial.print("Announce clock id");
					for(i=0;i<8;i++)
					{
						Serial.print(AnnounceMsg.clockID[i],HEX);
					}
				Serial.println("");
				}*/
				
			}
			
			
			
			/*if (v0)
		{
		Serial.println("It is working!");
		}*/
			
			
			
		
		


	}

	packetSize = _event.parsePacket();
	
	if  (packetSize) {
		// Do something to handle event packets
		//remoteIP=_event.remoteIP();
	    
		/*if (v0)
		{
		Serial.println("It is working");
		}*/
		globalCountevent++;
		//if(globalCountevent%10==0)
		
			memset(buffer,0,sizeof(buffer));
	
			/*if (v3)
			{
				Serial.println("");
			}
			if (v1)
			{
			Serial.println(globalCountevent);
			}*/
			if(packetSize==44)
			{
			packetByte = _event.read(buffer,44);
			}
		
			/*if (v1)
			{
			Serial.println(packetByte);
			}*/
			PTPMsg.version=buffer[1];
			PTPMsg.domain=buffer[4];
			PTPMsg.sourcePortID=(buffer[28] << 8) | buffer[29];
			PTPMsg.transportSpecific= (buffer[0]>>4)& 0xF;
			PTPMsg.type=(buffer[0])& 0xF;
			PTPMsg.length=(buffer[2] << 8) | buffer[3];
			PTPMsg.flags=((buffer[6] << 8) | buffer[7]);
			PTPMsg.sequenceID=(buffer[30] << 8) | buffer[31];
			PTPMsg.control=buffer[32];
			PTPMsg.log_period=buffer[33];
			
			
			
			//i=0;
           /* for (k=8;k<16;k++)
			{
				PTPMsg.correction[i]=buffer[k];
				i++;
			}*/
			i=0;
			for(k=20;k<28;k++)
			{
				PTPMsg.clockID[i]=buffer[k];
				i++;
			}
			PTPMsg.correction=(buffer[8] << 56) | (buffer[9] << 48) | (buffer[10] << 40) | (buffer[11] << 32) | (buffer[12] << 24) | (buffer[13] << 16) | (buffer[14] << 8) | (buffer[15]);
			//PTPMsg.clockID =(buffer[20] << 56) | (buffer[21] << 48) | (buffer[22] << 40) | (buffer[23] << 32) | (buffer[24] << 24) | (buffer[25] << 16) | (buffer[26] << 8) | (buffer[27]);
			itoa(PTPMsg.correction,PTPMsg.correction1,10);
			/*if (v1)
			{
				Serial.print("Printing Version info event:");
				Serial.print(PTPMsg.version);
				Serial.println("");
				Serial.print("Printing Domain info event:");
				Serial.print(PTPMsg.domain);
				Serial.println("");
				Serial.print("Printing source port ID event:");
				Serial.print(PTPMsg.sourcePortID);
				Serial.println("");
				Serial.print("Printing transport specific event:");
				Serial.print(PTPMsg.transportSpecific,HEX);
				Serial.println("");
				Serial.print("Printing message type event:");
				Serial.print(PTPMsg.type,HEX);
				Serial.println("");
				Serial.print("Printing length info event:");
				Serial.print(PTPMsg.length);
				Serial.println("");
				Serial.print("Printing flags info event:");
				Serial.print(PTPMsg.flags,HEX);
				Serial.println("");
				Serial.print("Printing sequence ID info event:");
				Serial.print(PTPMsg.sequenceID);
				Serial.println("");
				Serial.print("Printing control info event:");
				Serial.print(PTPMsg.control);
				Serial.println("");
				Serial.print("Printing log period info event:");
				Serial.print(PTPMsg.log_period);
				Serial.println("");
				Serial.print("Printing clock info event:");
				for(k=0;k<8;k++)
				{
					Serial.print(PTPMsg.clockID[k],HEX);
				}
				Serial.println("");
				//Serial.print("Size of clockid:");
				//Serial.println(sizeof(PTPMsg.clockID));
				Serial.print("Printing correction info event:");
				Serial.println(PTPMsg.correction1);
				for(k=0;k<8;k++)
				{
					Serial.print(PTPMsg.correction[k]);
				}
				Serial.println("");
				//Serial.print("Size of correction:");
				//Serial.println(sizeof(PTPMsg.correction));
				
				
			} */
			if (PTPMsg.control ==0)
			{
			SyncMsg.originTimestamp.seconds=(buffer[34] << 40) | (buffer[35] << 32) | (buffer[36] << 24) | (buffer[37] << 16) | (buffer[38] << 8) | (buffer[39]);
			SyncMsg.originTimestamp.nanoseconds=(buffer[40] << 24) | (buffer[41] << 16) |(buffer[42] << 8) | buffer[43];
			//itoa(SyncMsg.originTimestamp.seconds,timestamp,10);
			
			/*if(v2)
			{
				//Serial.print(packetSize);
				Serial.print("Printing sync timestamp (seconds):");
				Serial.println(timestamp);
				//Serial.print("Size of seconds timestamp:");
				//Serial.println(sizeof(SyncMsg.originTimestamp.seconds));
				Serial.print("Printing sync timestamp (nanoseconds)");
				Serial.println(SyncMsg.originTimestamp.nanoseconds);
			}*/
			}
			/*if (v1)
			{
			for (int i=0;i<44;i++)
			{
				
				
				Serial.print(buffer[i],HEX);
				Serial.print(" ");
			}
			}*/
			/*if (v2)
			{
				Serial.print("Printing Version info event:");
				Serial.print(PTPMsg.version);
				Serial.println("");
				Serial.print("Printing Domain info event:");
				Serial.print(PTPMsg.domain);
				Serial.println("");
				Serial.print("Printing source port ID event:");
				Serial.print(PTPMsg.sourcePortID);
				Serial.println("");
				Serial.print("Printing clock info event:");
				Serial.println("");
			}*/
			
			
				/*for(int j=34;j<40;j++)
				{
				SyncMsg.originTimestamp.secondsField[i]=buffer[j];
				i++;
				}
				i=0;
			    for(int j=40;j<44;j++)
				{
				SyncMsg.originTimestamp.nanosecondsField[i]=buffer[j];
				i++;
				}
				if(v4)
			{
				Serial.print(packetSize);
				Serial.print("Printing sync timestamp:");
				for(int j=0;j<6;j++)
					Serial.print(SyncMsg.originTimestamp.secondsField[j]);
				    
			}*/
	packMsg();	


	

}
}
int begindelayPacket;
int senddelayPacket;
byte s;

void ArduinoPTP::packMsg() {
	//remoteIP=_event.remoteIP();
	remotePort=_event.remotePort();
	remoteIP=IPAddress(128,146,39,115);
	//Serial.println(remotePort);
//	remotePort=0xC0BF;

	//if (globalCountevent%10==0)
	//{
	//Serial.print("Remote IP:");
	//Serial.println(remoteIP);
	begindelayPacket=_event.beginPacket(remoteIP,remotePort); 
	//Serial.println(begindelayPacket);
	//buffer_out[0]=PTPMsg.version;
	//Serial.println(buffer_out[0]);
	
	sequence_id_delay++;
	//const uint8_t transportSpecific = 11;
	const uint8_t type = 0x1;
	//const uint8_t reserved =11;
	//const uint8_t version=11;
	//const uint16_t length=0xABCD;//length of packet
	//const uint8_t domain=255;//domain of clocks
	const uint8_t reserved1=255;
	const uint16_t flags = 0x0000;
	//const uint16_t seqid=0xEFAB;//sequence ID
	const uint8_t transport_type= ((PTPMsg.transportSpecific & 0xF) <<4)|((type & 0xF) <<0); //transportSpecific & type byte
	//const uint8_t reserved_version=((reserved & 0xFF) <<4)|((version & 0xFF) <<0); // version number byte
	const uint8_t control_delay=0x01;//type of message
	const uint8_t log_delay=127;
	
	const uint32_t reserved2=0x01234567;
	//const uint32_t sourceportid_high1=0x01234567;
	//const uint32_t sourceportid_high2=0x89ABCDEF;
	//const uint16_t sourceportid_low=0xABCD;
	const uint16_t sourceportid_delay=0x0002;
	const uint64_t timestamp_delay_sec=0x00005a86f62b;
	const uint32_t timestamp_delay_nano=0x0ceb8b24;
	
    
	
	
	
	buffer_out[0] = transport_type;//transport_type;
	buffer_out[1]=PTPMsg.version;//reserved_version;
	buffer_out[2]=((PTPMsg.length>>8) & 0xFF);
	buffer_out[3]=((PTPMsg.length>>0) & 0xFF);
	buffer_out[4]=PTPMsg.domain;
	buffer_out[5]=reserved1;
	buffer_out[6]=((flags>>8) & 0xFF);
	buffer_out[7]=((flags>>0) & 0xFF);
	buffer_out[8]=((PTPMsg.correction>>56) & 0xFF);
	buffer_out[9]=((PTPMsg.correction>>48) & 0xFF);
	buffer_out[10]=((PTPMsg.correction>>40) & 0xFF);
	buffer_out[11]=((PTPMsg.correction>>32) & 0xFF);
	buffer_out[12]=((PTPMsg.correction>>24) & 0xFF);
	buffer_out[13]=((PTPMsg.correction>>16) & 0xFF);
	buffer_out[14]=((PTPMsg.correction>>8) & 0xFF);
	buffer_out[15]=((PTPMsg.correction>>0) & 0xFF);
	buffer_out[16]=((reserved2>>24) & 0xFF);
	buffer_out[17]=((reserved2>>16) & 0xFF);
	buffer_out[18]=((reserved2>>8) & 0xFF);
	buffer_out[19]=((reserved2>>0) & 0xFF);
	k=20;
	for(i=0;i<8;i++)
	{
	buffer_out[k]=PTPMsg.clockID[i];
	k++;
	}
	
	
	buffer_out[28]=((sourceportid_delay >> 8) & 0xFF);
	buffer_out[29]=((sourceportid_delay >> 0) & 0xFF);
	buffer_out[30]=((sequence_id_delay >> 8) & 0xFF);
	buffer_out[31]=((sequence_id_delay >> 0) & 0xFF);
	buffer_out[32]=control_delay;
	buffer_out[33]=log_delay;
	buffer_out[34]=((timestamp_delay_sec >> 40) & 0xFF);
	buffer_out[35]=((timestamp_delay_sec >> 32) & 0xFF);
	buffer_out[36]=((timestamp_delay_sec >> 24) & 0xFF);
	buffer_out[37]=((timestamp_delay_sec >> 16) & 0xFF);
	buffer_out[38]=((timestamp_delay_sec >> 8) & 0xFF);
	buffer_out[39]=((timestamp_delay_sec >> 0) & 0xFF);
	buffer_out[40]=((timestamp_delay_nano >> 24) & 0xFF);
	buffer_out[41]=((timestamp_delay_nano >> 16) & 0xFF);
	buffer_out[42]=((timestamp_delay_nano >> 8) & 0xFF);
	buffer_out[43]=((timestamp_delay_nano >> 0) & 0xFF);
	
	
	
	//buffer_out[10]=control;
	//buffer_out[11]=log;
	//	uint8_t control=255;
	//	uint8_t log=0xAB;
	
	
	//Serial.print(((transportSpecific & 0xFF) <<4)|((type & 0xFF) <<0));
	//Serial.println(s);
	
	//Serial.println(s);
	
	//Serial.println(s);
	/*s=_event.write((length>>8) & 0xFF); //send length of msg
	//Serial.println(s);
	s=_event.write((length>>0) & 0xFF); //send domain
	//Serial.println(s);
    s=_event.write(domain);//send control type
	//Serial.println(s); 
	_event.write(reserved1);
	s=_event.write((flags>>8) & 0xFF); //send length of msg
	//Serial.println(s);
	s=_event.write((flags>>0) & 0xFF);

	const uint32_t correction_high = 0x01234567;
    const uint32_t correction_low=0x89ABCDEF;
	const uint32_t seconds_high=0x01234567;
	const uint16_t seconds_low=0xABCD;
	const uint32_t nanoseconds=0x01234567;
	
	s = _event.write((correction_high >> 24) & 0xFF);
	_event.write((correction_high >> 16) & 0xFF);
	_event.write((correction_high >> 8) & 0xFF);
	_event.write((correction_high >> 0) & 0xFF);
	_event.write((correction_low >> 24) & 0xFF);
	_event.write((correction_low >> 16) & 0xFF);
	_event.write((correction_low >> 8) & 0xFF);
	_event.write((correction_low >> 0) & 0xFF);
	
	_event.write((reserved2 >> 24) & 0xFF);
	_event.write((reserved2 >> 16) & 0xFF);
	_event.write((reserved2 >> 8) & 0xFF);
	_event.write((reserved2 >> 0) & 0xFF);
	
	_event.write((sourceportid_high1 >> 24) & 0xFF);
	_event.write((sourceportid_high1 >> 16) & 0xFF);
	_event.write((sourceportid_high1 >> 8) & 0xFF);
	_event.write((sourceportid_high1 >> 0) & 0xFF);
	_event.write((sourceportid_high2 >> 24) & 0xFF);
	_event.write((sourceportid_high2 >> 16) & 0xFF);
	_event.write((sourceportid_high2 >> 8) & 0xFF);
	_event.write((sourceportid_high2 >> 0) & 0xFF);
	_event.write((sourceportid_low >> 8) & 0xFF);
	_event.write((sourceportid_low >> 0) & 0xFF);
	
	_event.write((seqid >> 8) & 0xFF);
	_event.write((seqid >> 0) & 0xFF);*/
	
	s=_event.write(buffer_out, 44);
//	Serial.println(s);
//  s=_event.write(log);
//	Serial.println(s);
	
	/*_event.write((seconds_high >> 24) & 0xFF);
	_event.write((seconds_high >> 16) & 0xFF);
	_event.write((seconds_high >> 8) & 0xFF);
	_event.write((seconds_high >> 0) & 0xFF);
	_event.write((seconds_low >> 8) & 0xFF);
	_event.write((seconds_low >> 0) & 0xFF);
	
	_event.write((nanoseconds >> 24) & 0xFF);
	_event.write((nanoseconds >> 16) & 0xFF);
	_event.write((nanoseconds >> 8) & 0xFF);
	_event.write((nanoseconds >> 0) & 0xFF);*/
	
	
	
	
	
	
	
	
	
	
	
	
	senddelayPacket=_event.endPacket();
	//Serial.println(senddelayPacket);
	//}
	
}



