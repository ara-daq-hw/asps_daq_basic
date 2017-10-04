/*
 *
 * PTP daemon class.
 * Note that to operate correctly, the microcontroller needs to receive
 * PTP packets, which requires joining IGMP group 224.0.1.129.
 */
 
 #include <EthernetUdp.h>
 
class ArduinoPTP {
public:
	ArduinoPTP() {}
	uint8_t begin();
	void stop();
	void handle();
private:
	EthernetUDP _general;
	EthernetUDP _event;
};
