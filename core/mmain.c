#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "lan.h"
#include "ntp.h"
#include "hd44780.h"


#define NTP_SERVER	inet_addr(62,117,76,142)
#define TIMEZONE	7

static volatile uint16_t ms_count;
static volatile uint32_t second_count;
static volatile uint32_t ntp_next_update;
static volatile uint32_t time_offset;

ISR(TIMER0_COMP_vect)
{
	if(++ms_count == 1000)
	{
		++second_count;
		ms_count = 0;
	}
}

void udp_packet(eth_frame_t *frame, uint16_t len)
{
	ip_packet_t *ip = (void*)(frame->data);
	udp_packet_t *udp = (void*)(ip->data);
	uint32_t timestamp;

	if(udp->to_port == NTP_LOCAL_PORT)
	{
		if((timestamp = ntp_parse_reply(udp->data, len)))
		{
			time_offset = timestamp - second_count;
			ntp_next_update = second_count + 12UL*60*60;
		}
	}
}

int main()
{
	uint32_t display_next_update = 0;
	uint32_t loctime;
	uint8_t s, m, h;
	char buf[5];

	// Init LAN
	lan_init();

	// Init LCD
	_delay_ms(30);
	hd44780_init();
	hd44780_mode(1,1,0);
	hd44780_clear();

	// init timer, freq = 1 kHz @ CLK = 16 MHz
	TCCR0 = (1<<WGM01)|(1<<CS01)|(1<<CS00);
	OCR0 = 250;
	TIMSK |= (1<<OCIE0);
	sei();

	while(1)
	{
		lan_poll();
		
		// Time to send NTP request?
		if(second_count >= ntp_next_update)
		{
			if(!ntp_request(NTP_SERVER))
				ntp_next_update = second_count+2;
			else
				ntp_next_update = second_count+60;
		}

		// Time to refresh display?
		if((time_offset) && (second_count >= display_next_update))
		{
			display_next_update = second_count+1;
			
			loctime = time_offset+second_count + 60UL*60*TIMEZONE;
			s = loctime % 60;
			m = (loctime/60)%60;
			h = (loctime/3600)%24;

			hd44780_clear();
			itoa(h,buf,10);
			hd44780_puts(buf);
			hd44780_puts(":");
			itoa(m,buf,10);
			hd44780_puts(buf);
			hd44780_puts(":");
			itoa(s,buf,10);
			hd44780_puts(buf);

		}
	}

	return 0;
}
