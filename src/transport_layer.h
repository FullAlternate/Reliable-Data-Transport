/// @file
/// <summary>
/// Contains the externally visible API of the transport layer abstraction in the OSI-stack.<br/>
/// <strong>Students are resposible of implementing most of the functions declared in this file.</strong>
/// </summary>
/// <remarks>The <see cref="transport_layer.c"/> file contains empty stubs for each of the student implementations of the transport layer functions.</remarks>

#pragma once
#ifndef TRANSPORT_LAYER_H
/// <summary>Header inclusion macro.</summary>
#define TRANSPORT_LAYER_H 1

#include <stdlib.h>
#include <stdint.h>

#include "timer.h"
#include "osi.h"
#include "transport_package.h"

/// <summary>Pushes application layer data from the receiving transport layer to the application layer above it in the OSI-stack.</summary>
/// <param name="osi_stack">A reference to the OSI-stack that the calling transport layer belongs to.</param>
/// <param name="data">A generic reference to a memory buffer containing the data to be pushed to the application layer.<br/>The referenced memory buffer must be valid and at least <paramref name="size"/> bytes long.</param>
/// <param name="size">The number of valid bytes contained in the memory buffer referred to by <paramref name="data"/>.</param>
/// <remarks>
/// This function is implemented by the precode. <strong>Students are neither required to implement nor to understand the details of this function.</strong>
/// <para>Students are, however, required to call this function as part of their transport layer implementation.</para>
/// </remarks>
void osi_tp2app(osi_stack_t* osi_stack, void* data, size_t size);

/// <summary>Pushes a transport package from the transport layer to the underlying network layer in the OSI-stack.</summary>
/// <param name="osi_stack">A reference to the OSI-stack that the calling transport layer belongs to.</param>
/// <param name="tp_pkg">A reference of the transport package that contains the data to be transmitted.</param>
/// <remarks>
/// This function is implemented by the precode. <strong>Students are neither required to implement nor to understand the details of this function.</strong>
/// <para>Students are, however, required to call this function as part of their transport layer implementation.</para>
/// </remarks>
void osi_tp2nw(osi_stack_t* osi_stack, transport_package_t* tp_pkg);

/// <summary>Type representing the transport layer in an OSI-stack.</summary>
/// <remarks>
/// The transport layer resides between the application layer and the network layer in an OSI-stack.
/// It is responsible for providing reliable data transfer services for the application layer above it.
/// </remarks>
typedef struct transport_layer transport_layer_t;

/// <summary>
/// Creates a new transport layer instance for the specified OSI-stack.<br/>
/// <strong>Students are required to implement this function as part of their transport layer implementation.</strong>
/// </summary>
/// <param name="osi_stack">A reference to the OSI-stack for which the transport layer instance will be created.</param>
/// <returns>A reference to the created transport layer instance, or <c><see cref="NULL"/></c> if an error occurred.</returns>
/// <remarks>
/// The returned transport layer instance will be deallocated by the caller with a subsequent call to <see cref="transport_layer_destroy"/>.
/// </remarks>
/// <exception cref="NULL"><paramref name="osi_stack"/> is <c><see cref="NULL"/></c>.</exception>
/// <exception cref="NULL">There is not enough memory available on the system to allocate a new transport layer. <see cref="errno"/> is set to an appropiate error value.</exception>
transport_layer_t* transport_layer_create(osi_stack_t* osi_stack);

/// <summary>
/// Initializes the specified transport layer instance for use.<br/>
/// <strong>Students are required to implement this function as part of their transport layer implementation.</strong>
/// </summary>
/// <param name="tp_layer">A reference to the transport layer to initialize.</param>
/// <remarks>
/// This function is called after all components in the OSI-stack have been created.
/// </remarks>
void transport_layer_init(transport_layer_t* tp_layer);

/// <summary>
/// Frees the memory occupied by the specified transport layer instance.<br/>
/// <strong>Students are required to implement this function as part of their transport layer implementation.</strong>
/// </summary>
/// <param name="tp_layer">A reference to the transport layer instance to destroy.</param>
void transport_layer_destroy(transport_layer_t* tp_layer);

/// <summary>
/// Handles an incoming request from the application to transmit the specified data.<br/>
/// <strong>Students are required to implement this function as part of their transport layer implementation.</strong>
/// </summary>
/// <param name="tp_layer">A reference to the transport layer that is supposed to transmit the data reliably.</param>
/// <param name="data">A generic reference to the data being transferred. The referred memory buffer must be at least <paramref name="size"/> bytes long.</param>
/// <param name="size">The number of valid bytes within the memory buffer referred to by <paramref name="data"/>.</param>
/// <remarks>
/// This function is in charge of receiving the data from the application layer, to package that data into a transport package
/// and to push that package further down the OSI-stack to the network layer.
/// </remarks>
/// <seealso cref="osi_tp2nw"/>
void transport_layer_onAppSend(transport_layer_t* tp_layer, void* data, size_t size);

/// <summary>
/// Handles an incoming transport package that was received by the network layer.<br/>
/// <strong>Students are required to implement this function as part of their transport layer implementation.</strong>
/// </summary>
/// <param name="tp_layer">A reference to the transoport layer that is supposed to handle the incoming transport package.</param>
/// <param name="tp_pkg">A reference to the transport package that was received by the network layer.</param>
/// <remarks>
/// This function is in charge of receiving transport packages, and perfom realiability operations, such as verifying the integrity of the payload data.
/// <para>This function is also (either directly or indirectly) responsible for making sure that the application data contained within the transport packages
/// is pushed up to the application layer above in the right sequence.</para>
/// <seealso cref="osi_tp2app"/>
void transport_layer_onNwReceive(transport_layer_t* tp_layer, transport_package_t* tp_pkg);

/// <summary>
/// Event handler that is invoked whenever the transport layer times out during data transmission.<br/>
/// <strong>Students are required to implement this function as part of their transport layer implementation.</strong>
/// </summary>
/// <param name="tp_layer">A reference to the transport layer that triggered the timeout.</param>
/// <remarks>
/// Since this function is not called from anywhere else besides the transport layer, students are free to change the signature of this function.
/// Changing the signature of this function should, however, not be necessary and it is not recommended.
/// <para>This function is a more specific variant of the <see cref="tick_timer_callback_t"/> callback function type.</para>
/// </remarks>
/// <seealso cref="timer_set"/>
/// <seealso cref="tick_timer_callback_t"/>
void transport_layer_onLayerTimeout(transport_layer_t* tp_layer);

/// <summary>
/// Handles tearing down of the reliable transport channel on the transport layer level.<br/>
/// <strong>Students are required to implement this function as part of their transport layer implementation.</strong>
/// </summary>
/// <param name="tp_layer">A reference to the transport layer that is to be torn down.</param>
/// <remarks>
/// Proper teardown of the communication channel is not required for this assignment, and students are allowed to keep the original stub for this implementation
/// that does nothing when this function is called. Students are, however, encoured to reflect upon how a proper teardown would be done in a real situation and
/// why a proper channel teardown is necessary.
/// <para>
/// When this function is called, the implementation may assume that the application layer that is located above is no longer functional and will not transmit any more data.
/// However, the underlying network layer will still be intact and able to transmit transport packages to the remote endpoint it is connected to.
/// </para>
/// </remarks>
void transport_layer_onTeardown(transport_layer_t* tp_layer);

// Added by student
uint16_t checksum(void* data, size_t size);

void resend_window(transport_layer_t* tp_layer);

#endif // !TRANSPORT_LAYER_H
