#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "bytes.h"

static bool toDynam(bytes_t * const bytes, size_t len);
//
//
//
//
void bytes_init(bytes_t * const bytes)
{
	bytes->data = bytes->dflt;

	bytes->usage = 0;
	bytes->dim = DEFAULT_SIZE;
}


//
//
//
//
void bytes_free(bytes_t * const bytes)
{
	if (bytes->data != bytes->dflt)
	{
		free(bytes->data);
	}

	bytes->data = NULL;
}


//
//
//
//
void bytes_fprintf(FILE * out, const bytes_t * const bytes)
{
	fprintf(out, "          ");

	for (size_t b = 0; b < BYTES_PER_LINE; b++)
	{
		fprintf(out, "%02zx ", b);
	}
	fprintf(out, "\n");

	fprintf(out, "         ");
	fprintf(out, " -----------------------------------------------");
	fprintf(out, "\n");

 	for (size_t x = 0; x < bytes->usage; x += 16)
	{
		size_t min = (bytes->usage  >= x + BYTES_PER_LINE) 
				? x + BYTES_PER_LINE : bytes->usage; 

		fprintf(out, "%08zx  ", x);
	
		for (size_t y = x; y < min; y++)
		{ 
			fprintf(out, "%02"PRIx8" ", bytes->data[y]);
		}
		fprintf(out, "\n");
	}

	fprintf(out, "         ");
	fprintf(out, " -----------------------------------------------");
	fprintf(out, "\n");
	fprintf(out, "\n");
	
}


//
//
//
//
bool bytes_empty(const bytes_t * const bytes)
{
	return (bytes->usage == 0);
}


//
//
//
//
uint8_t * bytes_data(const bytes_t * const bytes)
{
	return bytes->data;
}


//
//
//
//
size_t bytes_usage(const bytes_t * const bytes)
{
	return bytes->usage;
}


//
//
//
//
bool bytes_set(bytes_t * const bytes, size_t index, uint8_t val)
{
	if( index >= bytes->usage || index < 0)
	{
	    return false;
	}
	
	bytes->data[index] = val;
	return true;
}


//
//
//
//
bool bytes_get(const bytes_t * const bytes, size_t index, 
		width_t width, endian_t order, uint64_t * const result)
{
	uint64_t r = 0;
	if( index >= bytes->usage || (index + width) >= bytes->usage || index < 0)
	{
	    return false;
	}
	
	size_t w = index + width;
	
	if(order == BIG)
	{
	    for(size_t i = index; i <= w; ++i)
	    {
		r <<= 8;
		r += bytes->data[i];
	    }
	}
	else if(order == LITTLE)
	{
	    for(size_t i = w; i >= index; --i)
	    {
		r <<= 8;
		r += bytes->data[i];
	    }
	}
	return true;	
}


//
//
//
//
bool bytes_insert(bytes_t * bytes, size_t index, uint8_t * buf, size_t len)
{
	if(index > bytes->usage || index < 0)
	{
	    return false;
	}
	
        if(!toDynam(bytes, len))
            return false;
	
	for( size_t i = bytes->usage-1; i >= index; --i )
	{
	    bytes->data[i+len] = bytes->data[i];
	}
	
	for( size_t i = index; i < index + len; ++i )
	{
	    bytes->data[i] = buf[i-index];
	}

	bytes->usage += len;

	return true;
}


//
//
//
//
bool bytes_erase(bytes_t * const bytes, size_t index, size_t len)
{
	if(index >= bytes->usage || index < 0)
	{
	    return false;
	}
	
	if(index + len > bytes->usage)
	{
	    len = len - (index + len - bytes->usage);
	}

	for(size_t i = index; i < bytes->usage-len; ++i)
	{
	    bytes->data[i] = bytes->data[i+len];
	}
	
	bytes->usage -= len;
	return true;
}


//
//
//
//
uint8_t * bytes_range(const bytes_t * const bytes, range_t *range)
{
	
	return NULL;
}


//
//
//
//
bool bytes_copy(bytes_t * const dest, const bytes_t * const src)
{

	return false;
}

bool toDynam(bytes_t * const bytes, size_t len)
{
	if(bytes->dim > DEFAULT_SIZE)
	{
	    bytes->dim += 2 * (bytes->usage + len); 
	    bytes->data = realloc(bytes->data, bytes->dim * sizeof(uint8_t));
	}
	else
	{
	    bytes->dim += 2 * (bytes->usage + len);
	    bytes->data = malloc( bytes->dim * sizeof(uint8_t));
	}
		
	if( bytes->data == NULL )
	{
	    return false;
	}
	
	for(int i = 0; i < bytes->usage; ++i)
	{
	    bytes->data[i] = bytes->dflt[i];	
	}
	
	return true;	
}
