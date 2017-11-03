#include <ArduinoPTP.h>
char buffer[100];
//char buffer_event[100];
int v0 = 0; //0 if debugging mode is off.
int v1=0;//0 if debugging is off.
int v2=0;//0 if debugging is off.
int v3=1;
int globalCountgeneral = 0; //keep track of number of general packets.
int globalCountevent = 0; //keep track of number of event packets.


struct _PTPMsg {
		uint16_t sourcePortID;
		uint8_t version;
		uint8_t domain;
		uint8_t clockID[8];
		unsigned int transportSpecific; 
		unsigned int type; 
		uint16_t length;
		uint16_t flags;
		uint8_t correction[8];
		uint16_t sequenceID;
		uint8_t control;
		uint8_t log_period;
} PTPMsg;
uint8_t ArduinoPTP::begin() {
	if (!_general.begin(320)) return 1;
	if (!_event.begin(319)) return 1;
	return 0;
}
void ArduinoPTP::stop() {
	_general.stop();
	_event.stop();
}
void ArduinoPTP::handle() {
	int packetSize;
	int packetByte;
	struct _PTPMsg PTPMsg;
	packetSize = _general.parsePacket();
	//Serial.println("packet size:%d", packetSize);
	if (packetSize) {
		// Do something to handle general packets.
		globalCountgeneral++;
		if(globalCountgeneral%10==0)
		{
			if (v1)
			{
			Serial.println(globalCountgeneral);
			}
			packetByte = _general.read(buffer,44);
			if (v1)
			{
			Serial.println(packetByte);
			}
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
			int k=0;
			for (int j=8;j<16;j++)
			{
				
				PTPMsg.correction[k]=(buffer[j]);
				k++;
			}
			k=0;
			for (int j=20;j<28;j++)
			{
				
				PTPMsg.clockID[k]=(buffer[j]);
				k++;
			}
			
			
			if (v1)
			{
			for (int i=0;i<44;i++)
			{
				
				
				Serial.print(buffer[i],HEX);
				Serial.print(" ");
			}
			}
			if (v3)
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
				for (int y=0;y<8;y++)
				{
					
					Serial.print(PTPMsg.clockID[y],HEX);
				}
				Serial.println("");
				Serial.print("Printing correction info general:");
				for (int y=0;y<8;y++)
				{
					
					Serial.print(PTPMsg.correction[y]);
				}
				Serial.println("");
				Serial.println("");
			}
			
			
			if (v0)
		{
		Serial.println("It is working!");
		}
			
			memset(buffer,0,sizeof(buffer));
			
		
		
}

	}

	packetSize = _event.parsePacket();
	if  (packetSize) {
		// Do something to handle event packets
		if (v0)
		{
		Serial.println("It is working");
		}
		globalCountevent++;
		if(globalCountevent%10==0)
		{
			if (v1)
			{
			Serial.println(globalCountevent);
			}
			packetByte = _event.read(buffer,44);
			if (v1)
			{
			Serial.println(packetByte);
			}
			PTPMsg.version=buffer[1];
			PTPMsg.domain=buffer[4];
			PTPMsg.sourcePortID=(buffer[28] << 8) | buffer[29];
			int k=0;
			for (int j=20;j<28;j++)
			{
				
				PTPMsg.clockID[k]=buffer[j];
				k++;
			}
			
			
			if (v1)
			{
			for (int i=0;i<44;i++)
			{
				
				
				Serial.print(buffer[i],HEX);
				Serial.print(" ");
			}
			}
			if (v2)
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
				for (int y=0;y<8;y++)
				{
					
					Serial.print(PTPMsg.clockID[y],HEX);
				}
				Serial.println("");
			}
			
		memset(buffer,0,sizeof(buffer));
	}
}
}
