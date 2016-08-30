/* This is a very simple hello_world program.
 * It aims to demonstrate the co-existence of two processes:
 * One of them prints a hello world message and the other blinks the LEDs
 *
 * It is largely based on hello_world in $(CONTIKI)/examples/sensinode
 *
 * Author: George Oikonomou - <oikonomou@users.sourceforge.net>
 * Aletrado para a disciplina de Rede de Sensores Sem Fio
 */

#include "contiki.h"
#include "dev/leds.h"

#include <stdio.h> /* For printf() */
/*---------------------------------------------------------------------------*/
#define LED_PING_EVENT (44)
#define LED_PONG_EVENT (45)
/*---------------------------------------------------------------------------*/
static struct etimer et_hello;
static struct etimer et_blink;
static struct etimer et_proc3;
static struct etimer et_pong;
static uint16_t count;
static uint8_t blinks;
static uint8_t liga;
/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
PROCESS(proc3_process, "Proc3");
PROCESS(blink_process, "LED blink process");
PROCESS(pong_process, "Ping Pong");
AUTOSTART_PROCESSES(&blink_process, &hello_world_process, &proc3_process, &pong_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
{
  PROCESS_BEGIN();

  etimer_set(&et_hello, CLOCK_SECOND * 4);
  count = 0;

  while(1) {
    PROCESS_WAIT_EVENT();

    if(ev == PROCESS_EVENT_TIMER) {
      printf("Hello World : pingig!\n", count);
      process_post (& pong_process ,LED_PING_EVENT , (void*) (& hello_world_process ));
      etimer_reset(&et_hello);
    }
    if (ev == LED_PONG_EVENT){
    	printf("Hello World :LED Pong!\n\n");
    }
      }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(blink_process, ev, data)
{
  PROCESS_BEGIN();

  while(1) {
    etimer_set(&et_blink, 2*CLOCK_SECOND);

    PROCESS_WAIT_EVENT();

    if(ev == LED_PONG_EVENT){
    leds_toggle(LEDS_YELLOW);
    printf("Blink : LED Pong!\n\n");
    }
    if(ev == PROCESS_EVENT_TIMER){
    	printf("Blink : pinging!\n");
    	process_post (& pong_process ,LED_PING_EVENT , (void*) (& blink_process ));
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(proc3_process, ev, data)
{
  PROCESS_BEGIN();
  liga = 0;
  etimer_set(&et_proc3, CLOCK_SECOND * 5);

  while(1) {
    PROCESS_WAIT_EVENT();

    if(ev == LED_PONG_EVENT) {
    	if(liga == 0)
    	{
    		leds_on(LEDS_RED);
    		liga = 1;
    	}
    	else
    	{
    		leds_off(LEDS_RED);
    		liga = 0;
    	}
        etimer_reset(&et_proc3);
    	printf("Proc3 : LED Pong!\n\n");
    }
    if(ev == PROCESS_EVENT_TIMER){
    	printf("Proc3 : pinging\n");
    	process_post (& pong_process ,LED_PING_EVENT , (void*) (& proc3_process ));
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(pong_process, ev, data)
{
  PROCESS_BEGIN();

  while(1) {
    PROCESS_WAIT_EVENT();
    if (ev == LED_PING_EVENT ){
    	leds_on(LEDS_BLUE);
        printf("Pong : LED Ping!  ponging...\n");
    	etimer_set(&et_pong, 0.2*CLOCK_SECOND);
    	process_post ((struct process*)data ,LED_PONG_EVENT , NULL );
    }
    else if (ev == PROCESS_EVENT_TIMER){
    	leds_off(LEDS_BLUE);
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
