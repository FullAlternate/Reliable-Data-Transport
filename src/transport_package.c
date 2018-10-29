#include <stdlib.h>
#include <memory.h>

#include "transport_package_impl.h"

transport_package_t* transport_pkg_create(void* data, size_t size)
{
	transport_package_t* pkg;

	pkg = (transport_package_t*)malloc(sizeof(transport_package_t));
	if (!pkg)
		return NULL;

	pkg->size = data ? size : 0U;
	pkg->data = data;
	
	pkg->checksum = 0;
	pkg->control_pack = 0;
	pkg->ack = 1;
	pkg->number = 1;

	return pkg;
}

void transport_pkg_destroy(transport_package_t* tp_pkg)
{
	if (tp_pkg)
		free(tp_pkg);
}

transport_package_t* transport_pkg_copy(transport_package_t* tp_pkg)
{
	transport_package_t* cpy;

	if (!tp_pkg)
		return NULL;

	// Not that the malloc call creates a continuous memory buffer that is large enough to hold
	// both a copy of the transport package structure as well as its payload data.
	cpy = (transport_package_t*)malloc(sizeof(transport_package_t) + tp_pkg->size);
	if (!cpy)
		return NULL;

	// This is a trick that copies all fields of the transport package from the specified parameter value
	// to the new copy. This will also include the values of any additional fields that are added to the 
	// structure definition.
	memcpy(cpy, tp_pkg, sizeof(transport_package_t));

	// This is a dirty trick using pointer arithmetics to get the memory address inside the allocated memory buffer
	// that is located after the actual transport package structure. This effectively places a copy
	// of the payload data into the same memory buffer the of the structure, allowing both the structure
	// and the copy of the data to freed as one.
	cpy->data = cpy + 1;

	memcpy(cpy->data, tp_pkg->data, tp_pkg->size);
	return cpy;
}

size_t transport_pkg_size(transport_package_t* pkg)
{
	if (!pkg)
		return 0U;
	return pkg->size;
}

void* transport_pkg_data(transport_package_t* pkg)
{
	if (!pkg)
		return NULL;
	return pkg->data;
}
