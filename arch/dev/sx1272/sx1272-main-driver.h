#ifndef SX1272_MAIN_DRIVER_H_
#define SX1272_MAIN_DRIVER_H_

int init_sx1272();/*Full initialization process of the driver*/
int rx_mode();/*Turns on the radio and listens*/
int off_sx1272();/*Turns off the radio*/
int set_channel(uint8_t channel);/*Sets the channel*/
int receiving_packet_sx1272();/*Checks if reception is ongoing*/
rtimer_clock_t time_of_last_packet_reception();/*The rtimer time when we last received a packet*/
int pending_packet();
int read_packet(void *buf, unsigned short buf_len);

int prepare(const void *buffer, unsigned short size);
int transmit(unsigned short size);
int send(const void *buffer, unsigned short size);

int get_current_rssi();
int get_last_packet_rssi();
int get_last_snr();

#endif