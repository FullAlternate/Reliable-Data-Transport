/// @file
/// <summary>
/// Contains the code implementation for the transport layer abstraction in the OSI-stack.<br/>
/// <strong>Students are required to implement the provided stubs within this file according to their own understanding of the transport layer.</strong>
/// </summary>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "transport_layer.h"
#include "transport_package_impl.h"
#include "application_layer_impl.h"

/// <summary>Sets a tick timer object for the specified transport layer.</summary>
/// <param name="tp_layer">A reference to the transport layer instance that sets the timer.</param>
/// <param name="timer">A reference to the timer object to set.</param>
/// <param name="ticks">The number of ticks to set the timer for.</param>
/// <remarks>
/// This serves as a simple convenience function that provides easy access to the more general <see cref="timer_set"/> function.
/// <para>This function will set the timer object with <see cref="transport_layer_onLayerTimeout"/> as the callback function, and
/// <paramref name="tp_layer"/> as the context argument. This will <see cref="transport_layer_onLayerTimeout"/> to be invoked correctly
/// with the right argument value when the timer expires.</para>
/// </remarks>
static void transport_layer_timer_set(transport_layer_t* tp_layer, tick_timer_t* timer, int ticks);

/// <summary>Data structure implementing the <see cref="transport_layer_t"/> type.</summary>
/// <remarks>
/// <strong>Students should modify/add fields declared in this structure according to their own understanding of the transport layer.</strong>
/// </remarks>
struct transport_layer
{
	/// <summary>A reference to the OSI-stack the transport layer belong to.</summary>
	osi_stack_t* osi_stack;
	tick_timer_t* timeout;

	transport_package_t* window[4];

	int current;
	int fail_count;
};

// STUDENTS BEGIN:
// Implementation for the transport layer

transport_layer_t* transport_layer_create(osi_stack_t* osi_stack)
{
	// Remember to assign the osi_stack parameter to the new transport layer object you create in this function.
	printf("\n\ntransport_layer_create - adress osi_stack: %p\n", osi_stack);
	transport_layer_t *transport_layer = malloc(sizeof(transport_layer_t));
	printf("transport_layer_create - adress tp_layer: %p\n\n\n\n", osi_stack);

	transport_layer->osi_stack = osi_stack;
	transport_layer->current = 0;
	transport_layer->fail_count = 5;

	//transport_layer->window = calloc(4, sizeof(transport_package_t));

	for(int i = 0; i <= 3; i++){
		transport_layer->window[i] = malloc(sizeof(transport_package_t));
		transport_layer->window[i]->data = NULL;
		transport_layer->window[i]->size = 0;
		transport_layer->window[i]->checksum = 0;
	}
	



	transport_layer->timeout = malloc(sizeof(tick_timer_t));
	transport_layer_timer_set(transport_layer, transport_layer->timeout, 10000);
	
	if(transport_layer == NULL){
		//errno("Out of memory");
		return 0;
	}

	if(transport_layer->osi_stack == NULL){
		return 0;	
	}

	return transport_layer;

}

void transport_layer_destroy(transport_layer_t* tp_layer)
{
	if(tp_layer){
		free(tp_layer);
	}	
}

void transport_layer_onAppSend(transport_layer_t* tp_layer, void* data, size_t size)
{	

	transport_package_t *pack = malloc(sizeof(transport_package_t));
	pack = transport_pkg_create(data, size);
	
	pack->checksum = checksum(pack->data, pack->size);
	tp_layer->window[tp_layer->current] = pack;

	printf("\n\ntransport_layer_app - adress tp_layer: %p\n", tp_layer);
	printf("transport_layer_app - adress data: %p\n", data);
	printf("transport_layer_app - adress tp_layer->osi_stack: %p\n", tp_layer->osi_stack);
	printf("transport_layer_app - ORIGINAL_SIZE: %d\n", (int)pack->size);
	printf("transport_layer_app - checksum: %d\n", tp_layer->window[tp_layer->current]->checksum);
	printf("transport_layer_app - current: %d\n\n\n\n\n", tp_layer->current);

	tp_layer->current++;
	tp_layer->current = tp_layer->current % 4;	

	osi_tp2nw(tp_layer->osi_stack, pack);

	
	
}

void transport_layer_onNwReceive(transport_layer_t* tp_layer, transport_package_t* tp_pkg)
{
	transport_package_t *pkg_cpy = transport_pkg_copy(tp_pkg);



	int32_t localchecksum = checksum(pkg_cpy->data, pkg_cpy->size);
	tp_layer->window[tp_layer->current] = pkg_cpy;

	printf("\n\ntransport_layer_net - adress tp_layer: %p\n", tp_layer);
	printf("transport_layer_net - adress tp_pkg: %p\n", tp_pkg);
	printf("transport_layer_net - adress tp_layer->osi_stack: %p\n", tp_layer->osi_stack);
	printf("transport_layer_net - current: %d\n", tp_layer->current);
	printf("transport_layer_net - BEFORE CHECKSUM PRINTF\n");
	printf("transport_layer_net - checksum local: %d\n", localchecksum);
	printf("transport_layer_net - checksum window: %d\n", tp_layer->window[tp_layer->current]->checksum);
	printf("transport_layer_net - AFTER CHECKSUM PRINTF\n\n\n\n\n");
	

	if(tp_layer->fail_count == 5){
		while(tp_layer->window[tp_layer->current]->checksum != localchecksum){
			timer_tickall();
		}

		osi_tp2app(tp_layer->osi_stack, tp_layer->window[tp_layer->current]->data, tp_layer->window[tp_layer->current]->size);
	}

	if(tp_layer->current == (tp_layer->fail_count - 1) % 4 && tp_layer->fail_count != 5){
		resend_window(tp_layer);
	}

	tp_layer->current = tp_layer->current + 1;
	tp_layer->current = tp_layer->current % 4;
}	

void transport_layer_onLayerTimeout(transport_layer_t* tp_layer)
{
	printf("timeout - adress tp_layer: %p\n\n\n\n\n", tp_layer);

	tp_layer->fail_count = tp_layer->current;
	transport_layer_timer_set(tp_layer, tp_layer->timeout, 10000);
}

uint16_t checksum(void* data, size_t size){
	char* c_data = (char*)data;

	uint32_t acc = 0xffff;

	for(size_t i = 0; i+1 < size; i+=2){
		uint16_t word;
		memcpy(&word, c_data+i, 2);

		acc += ntohs(word);
		if(acc > 0xffff){
			acc -= 0xffff;
		}
	}

	if(size & 1){
		uint16_t word = 0;
		memcpy(&word, c_data+size-1, 1);

		acc += ntohs(word);
		if(acc > 0xffff){
			acc -= 0xffff;
		}
	}

	return htons(~acc);
}

void resend_window(transport_layer_t* tp_layer){
	printf("resend_window - adress tp_layer: %p\n\n\n\n", tp_layer);
	
	transport_package_t *pack = malloc(sizeof(transport_package_t));
	int fc = tp_layer->fail_count;

	for(int i = 0; i <= 3; i++){
		pack = tp_layer->window[fc];

		osi_tp2nw(tp_layer->osi_stack, pack);

		fc = (fc + 1) % 4;
	}

	tp_layer->fail_count = 5;
	free(pack);
}


// EXTRA PROBLEM:
// Establish connection by syncing the transmission (e.g. TCP SYN-packets)
// Shutdown connection when the application tears down the OSI stack

void transport_layer_init(transport_layer_t* tp_layer)
{

}

void transport_layer_onTeardown(transport_layer_t* tp_layer)
{

}

// The following code defines convenience functions for you to use for timers.
// Feel free to look through it, but do not worry if you cannot make heads or tails of it.

static void transport_layer_timer_set(transport_layer_t* tp_layer, tick_timer_t* timer, int ticks)
{
	timer_set(timer, ticks, (tick_timer_callback_t)transport_layer_onLayerTimeout, tp_layer);
}