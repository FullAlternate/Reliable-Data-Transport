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
#include "list.h"

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
	list_t* queue;	// List that holds the data sent from app_layer in the recieved order

	int ordering;	// Holds the number of the current packet expected to be recieved by the app_layer
	int numbering; 	// Used for labeling the packets as they are sent
	
};


// Creates and initializes the transport layer setting its starting values appropriately 
transport_layer_t* transport_layer_create(osi_stack_t* osi_stack)
{
	transport_layer_t *transport_layer = malloc(sizeof(transport_layer_t));

	transport_layer->timeout = malloc(sizeof(tick_timer_t));
	transport_layer->queue = list_create();

	transport_layer->ordering = 1;
	transport_layer->numbering = 1;

	transport_layer->osi_stack = osi_stack;
	transport_layer_timer_set(transport_layer, transport_layer->timeout, 1000);
	
	if(transport_layer == NULL){
		return 0;
	}

	if(transport_layer->osi_stack == NULL){
		return 0;	
	}

	return transport_layer;

}

// Destorys the transport_layer
void transport_layer_destroy(transport_layer_t* tp_layer)
{
	if(tp_layer){
		free(tp_layer);
	}	
}

// Creates a packet of the data recieved by the app_layer and copies it. Then calculates a checksum and labeles
// it with a number used for ordering the data. Lastly sends the data to the network layer for corruption.
void transport_layer_onAppSend(transport_layer_t* tp_layer, void* data, size_t size)
{	
	if(!tp_layer){
		return;
	}
	
	else{
		transport_package_t *tmp = transport_pkg_create(data, size);
		transport_package_t *pack = transport_pkg_copy(tmp);

		transport_pkg_destroy(tmp);

		pack->checksum = checksum(pack->data, pack->size);
		pack->number = tp_layer->numbering;

		list_addlast(tp_layer->queue, pack);

		tp_layer->numbering++;
		osi_tp2nw(tp_layer->osi_stack, pack);

	}		
}

// Recieves the possibly corrupted data from the network layer, send it to the app_layer 
// and creates a control_packet containing an ack or nack that is sent back to the sender. 
// Then updates queue based on the control_packet.
void transport_layer_onNwReceive(transport_layer_t* tp_layer, transport_package_t* tp_pkg)
{
	transport_package_t *the_pack = NULL;
	transport_package_t *control_pack = NULL;
	int32_t localchecksum;

	// Handles the data if it is the requested data_packet
	if(tp_pkg->control_pack == 0 && tp_pkg->number == tp_layer->ordering){
		the_pack = transport_pkg_copy(tp_pkg);

		control_pack = the_pack;
		control_pack->control_pack = 1;

		localchecksum = checksum(the_pack->data, the_pack->size);

		// If the checksum does not match sets the ack value to false which is handles as a nack
		if(localchecksum != the_pack->checksum){
				control_pack->ack = 0;
		
		// Increases the ordered pack number by 1 and sends the pack to the app layer
		} else {
			printf("SENT PACK\n\n");

			tp_layer->ordering++;
			osi_tp2app(tp_layer->osi_stack, the_pack->data, the_pack->size);
		}
		
		// Sends ack back to sender
		osi_tp2nw(tp_layer->osi_stack, control_pack);


	// Handles the data if it is a control packet
	} else if (tp_pkg->control_pack == 1){
		if(tp_layer->queue->head != NULL){

			// Pops the first element in the queue if the controlpack contains an ack
			if(tp_pkg->ack == 1){
				list_popfirst(tp_layer->queue);

			// Resends the head of the queue if the controlpack contains an nack
			} else {
				the_pack =  tp_layer->queue->head->elem;

				osi_tp2nw(tp_layer->osi_stack, the_pack);
			}
		}
	}
}	

// Resets the timers and sends the remaining data left in the queue
void transport_layer_onLayerTimeout(transport_layer_t* tp_layer)
{
	printf("TIMED OUT - %p\n\n", tp_layer);

	if(tp_layer->queue->head != NULL){
		osi_tp2nw(tp_layer->osi_stack, tp_layer->queue->head->elem);
		transport_layer_timer_set(tp_layer, tp_layer->timeout, 1000);
		
	}

	

	
}

// Calculates the checksum of a packet
uint16_t checksum(void* data, size_t size){
	
	// Casts data so it can be indexed
	char* c_data = (char*)data;

	// Sets acummulator to hex value
	uint32_t acc = 0xffff;

	// Handles a 16-bit block in the data
	for(size_t i = 0; i+1 < size; i+=2){
		uint16_t word;
		memcpy(&word, c_data+i, 2);

		acc += ntohs(word);
		if(acc > 0xffff){
			acc -= 0xffff;
		}
	}

	// Handles a partial block at the end of the data 
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