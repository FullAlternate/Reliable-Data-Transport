/// @file
/// <summary>
/// Contains the implementation details of transport packages.<br/>
/// <strong>Students are required to redefine the implementation details of the transport package structure.</strong>
/// </summary>

#pragma once
#ifndef TRANSPORT_PACKAGE_IMPL_H
/// <summary>Header inclusion macro.</summary>
#define TRANSPORT_PACKAGE_IMPL_H 1

#include <stdint.h>
#include "transport_package.h"

/// <summary>Implements the <see cref="transport_package_t"/> datatype.</summary>
/// <remarks>
/// <strong>Students should modify/add fields declared in this structure according to their own understanding of transport packages.</strong>
/// Note that doing so may require modifications to the functions implemented in <see cref="transport_package.c"/>.
/// </remarks>
struct transport_package
{
	/// <summary>The number of bytes contained in the payload of the package.</summary>
	/// <value>A positive unsigned integer value, or <c>0U</c> (zero) to signify an empty package.</value>
	size_t size;
	uint16_t checksum;
	/// <summary>A reference to the payload of the package.</summary>
	/// <value>A generic reference to a memory buffer that is at least <see cref="size"/> bytes long, or <c><see cref="NULL"/></c> if the package is empty.</value>
	int control_pack;
	int ack;
	int number;
	void* data;

	
};

#endif // !TRANSPORT_PACKAGE_IMPL_H