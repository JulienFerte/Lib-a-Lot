#ifndef MODULE_CONTEXT_H
#define MODULE_CONTEXT_H

#include "h_header.h"

struct configuration_struct_tag;

typedef struct {
//*** The global configuration
	struct configuration_struct_tag* configuration;

//*** The id of the module
	unsigned int module_id;

} module_context;

#endif
