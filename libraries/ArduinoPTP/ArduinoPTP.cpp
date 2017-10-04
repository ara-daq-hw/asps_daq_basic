#include <ArduinoPTP.h>

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
	packetSize = _general.parsePacket();
	if (packetSize) {
		// Do something to handle general packets.
	}
	packetSize = _event.parsePacket();
	if  (packetSize) {
		// Do something to handle event packets
	}
}
