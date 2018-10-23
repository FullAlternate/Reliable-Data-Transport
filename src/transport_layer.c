/// @file
/// <summary>
/// Contains the code implementation for the transport layer abstraction in the OSI-stack.<br/>
/// <strong>Students are required to implement the provided stubs within this file according to their own understanding of the transport layer.</strong>
/// </summary>

#include <stdlib.h>

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

	tick_timer_t* send_timer;
	tick_timer_t* validate_timer;


};

// STUDENTS BEGIN:
// Implementation for the transport layer

transport_layer_t* transport_layer_create(osi_stack_t* osi_stack)
{
	// Remember to assign the osi_stack parameter to the new transport layer object you create in this function.
	transport_layer_t *transport_layer = malloc(sizeof(transport_layer_t));
	//transport_layer->send_timer = malloc(sizeof(tick_timer_t));
	//transport_layer->validate_timer = malloc(sizeof(tick_timer_t));
	 
	transport_layer->osi_stack = osi_stack;
	transport_layer->send_timer = NULL;
	transport_layer->validate_timer = NULL;
	
	
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
		//free(x->send_timer);
		//free(x->validate_timer);
		free(tp_layer);
	}	
}

void transport_layer_onAppSend(transport_layer_t* tp_layer, void* data, size_t size)
{	
	if(tp_layer->send_timer == NULL){
		transport_layer_timer_set(tp_layer, tp_layer->send_timer, 4);
	}

	transport_package_t *pack = transport_pkg_create(data, size);

	pack = transport_pkg_copy(pack);

	osi_tp2nw(tp_layer->osi_stack, pack); 
	timer_tickall();
}

void transport_layer_onNwReceive(transport_layer_t* tp_layer, transport_package_t* tp_pkg)
{
	transport_package_t *pkg_cpy = transport_pkg_copy(tp_pkg);
	int data_size = sizeof((application_data_node_t) pkg_cpy->data);
	int original_size = pkg_cpy->size;

	printf("DATA_SIZE: %d\n", data_size);
	printf("ORIGINAL_SIZE: %d\n\n\n\n\n", original_size);

	

	//if(data_size == original_size){
	pkg_cpy->corrupt = false;
	osi_tp2app(tp_layer->osi_stack, pkg_cpy->data, pkg_cpy->size);
	//} else {
		//printf("ADAM OG EVA\n\n\n\n");
		//errno("invalid packet");
		//pack->corrupt = true;
	//}
}	

void transport_layer_onLayerTimeout(transport_layer_t* tp_layer)
{
	if(tp_layer->validate_timer != NULL){
		if(tp_layer->validate_timer->ctx == true){
			tp_layer->send_timer = tp_layer->validate_timer;
			
			timer_unset
		} 
	}
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
