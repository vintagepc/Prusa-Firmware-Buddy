// variant8.c

#include "variant8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" {


char* _variant8_to_str(variant8_t* pvar8, const char* fmt);
variant8_t _variant_from_str(uint8_t type, char* str, const char* fmt);


variant8_t variant8_init(uint8_t type, uint16_t count, void* pdata)
{
	variant8_t var8;
	uint16_t size;
	if ((count == 1) && !(type & VARIANT8_PTR))
	{
		var8 = (variant8_t){ type, 0, { 0 }, { 0 } };
		if (pdata)
			switch (type) {
			case VARIANT8_I8:   var8.i8   = *((int8_t*)pdata); break;
			case VARIANT8_UI8:  var8.ui8  = *((uint8_t*)pdata); break;
			case VARIANT8_I16:  var8.i16  = *((int16_t*)pdata); break;
			case VARIANT8_UI16: var8.ui16 = *((uint16_t*)pdata); break;
			case VARIANT8_I32:  var8.i32  = *((int32_t*)pdata); break;
			case VARIANT8_UI32: var8.ui32 = *((uint32_t*)pdata); break;
			case VARIANT8_FLT:  var8.flt  = *((float*)pdata); break;
			}
	}
	else if ((count > 1) && (type & VARIANT8_PTR))
	{
		size = variant8_type_size(type & ~VARIANT8_PTR) * count;
		var8 = (variant8_t){ type, 0, { .size = size }, { .ptr = 0 } };
		if (size)
		{
			var8.ptr = variant8_malloc(size);
			if (var8.ptr)
			{
				if (pdata)
					memcpy(var8.ptr, pdata, size);
			}
			else
				var8 = variant8_error(VARIANT8_ERR_MALLOC, 0, 0);
		}
	}
	else
		var8 = variant8_error(VARIANT8_ERR_UNSTYP, 0, 0);
	return var8;
}

void variant8_done(variant8_t* pvar8) {
	if (pvar8) {
		if (pvar8->type & VARIANT8_PTR) {
			if (pvar8->size && pvar8->ptr)
				variant8_free(pvar8->ptr);
		}
		*pvar8 = (variant8_t){ VARIANT8_EMPTY, 0, { 0 }, { 0 } };
	}
}

variant8_t variant8_copy(variant8_t* pvar8)
{
	variant8_t var8 = pvar8?(*pvar8):((variant8_t){ VARIANT8_EMPTY, 0, { 0 }, { 0 } });
	if ((var8.type & VARIANT8_PTR) && var8.size && var8.ptr) {
		void* ptr = var8.ptr;
		var8.ptr = variant8_malloc(var8.size);
		memcpy(var8.ptr, ptr, var8.size);
	}
	return var8;
}

int variant8_change_type(variant8_t* pvar8, uint8_t type)
{
	if (pvar8 == 0) return 0;
	variant8_t var8 = *pvar8;
	if (type == VARIANT8_PCHAR)
	{
		char* pch = _variant8_to_str(&var8, 0);
		variant8_done(&var8);
		if (pch)
		{
			*pvar8 = variant8_pchar(pch, 0, 0);
			return 1;
		}
		*pvar8 = variant8_error(VARIANT8_ERR_MALLOC, 0, 0);
	}
	else if (var8.type == VARIANT8_PCHAR)
	{
		*pvar8 = _variant_from_str(type, var8.pch, 0);
		variant8_done(&var8);
		return 1;
	}
	return 0;
}

#define _VARIANT8_TYPE(type, _8, _16, _32) ((variant8_t){ type, _8, { _16 }, { _32 } })
#define _VARIANT8_EMPTY() ((variant8_t){ VARIANT8_EMPTY, 0, { 0 }, { 0 } })

variant8_t variant8_empty(void) { return (variant8_t){ VARIANT8_EMPTY, 0, { 0 }, { 0 } }; }
variant8_t variant8_i8(int8_t i8) { return (variant8_t){ VARIANT8_I8, 0, { 0 }, { .i8 = i8 } }; }
variant8_t variant8_ui8(uint8_t ui8) { return (variant8_t){ VARIANT8_UI8, 0, { 0 }, { .ui8 = ui8 } }; }
variant8_t variant8_i16(int16_t i16) { return (variant8_t){ VARIANT8_I16, 0, { 0 }, { .i16 = i16 } }; }
variant8_t variant8_ui16(uint16_t ui16) { return (variant8_t){ VARIANT8_UI16, 0, { 0 }, { .ui16 = ui16 } }; }
variant8_t variant8_i32(int32_t i32) { return (variant8_t){ VARIANT8_I32, 0, { 0 }, { .i32 = i32 } }; }
variant8_t variant8_ui32(uint32_t ui32) { return (variant8_t){ VARIANT8_UI32, 0, { 0 }, { .ui32 = ui32 } }; }
variant8_t variant8_flt(float flt) { return (variant8_t){ VARIANT8_FLT, 0, { 0 }, { .flt = flt} }; }

variant8_t variant8_pui8(uint8_t* pui8, uint16_t count, int init) {
	if (init) return variant8_init(VARIANT8_PUI8, count, (void*)pui8);
	return (variant8_t){ VARIANT8_PUI8, 0, { .size = (uint16_t)(count * sizeof(uint8_t)) }, { .pui8 = pui8} };
}

variant8_t variant8_pui16(uint16_t* pui16, uint16_t count, int init) {
	if (init) return variant8_init(VARIANT8_PUI16, count, (void*)pui16);
	return (variant8_t){ VARIANT8_PUI16, 0, { .size = (uint16_t)(count * sizeof(uint16_t)) }, { .pui16 = pui16} };
}

variant8_t variant8_pui32(uint32_t* pui32, uint16_t count, int init) {
	if (init) return variant8_init(VARIANT8_PUI32, count, (void*)pui32);
	return (variant8_t){ VARIANT8_PUI32, 0, { .size = (uint16_t)(count * sizeof(uint32_t)) }, { .pui32 = pui32} };
}

variant8_t variant8_pflt(float* pflt, uint16_t count, int init) {
	if (init) return variant8_init(VARIANT8_PFLT, count, (void*)pflt);
	return (variant8_t){ VARIANT8_PFLT, 0, { .size = (uint16_t)(count * sizeof(float)) }, { .pflt = pflt } };
}

variant8_t variant8_pchar(char* pch, uint16_t count, int init) {
	if (init) return variant8_init(VARIANT8_PCHAR, count?count:strlen(pch), (void*)pch);
	return (variant8_t){ VARIANT8_PCHAR, 0, { .size = (uint16_t)(count?count:strlen(pch)) }, { .pch = (char*)pch } };
}

variant8_t variant8_user(uint32_t usr32, uint16_t usr16, uint8_t usr8) {
	return (variant8_t){ VARIANT8_USER, usr8, { .usr16 = usr16 }, { .usr32 = usr32 } };
}

variant8_t variant8_error(uint32_t err32, uint16_t err16, uint8_t err8)
{
	return (variant8_t){ VARIANT8_USER, err8, { .usr16 = err16 }, { .usr32 = err32 } };
}

uint16_t variant8_type_size(uint8_t type)
{
	switch (type) {
	case VARIANT8_I8:
	case VARIANT8_UI8:
		return sizeof(uint8_t);
	case VARIANT8_CHAR:
		return sizeof(char);
	case VARIANT8_I16:
	case VARIANT8_UI16:
		return sizeof(uint16_t);
	case VARIANT8_I32:
	case VARIANT8_UI32:
		return sizeof(uint32_t);
	case VARIANT8_FLT:
		return sizeof(float);
	case VARIANT8_USER:
		return (sizeof(variant8_t) - sizeof(uint8_t));
	}
	return 0;
}

uint16_t variant8_data_size(variant8_t* pvar8)
{
	if (pvar8) {
        if (pvar8->type & VARIANT8_PTR)
        	return pvar8->size;
        else
        	return variant8_type_size(pvar8->type);
	}
	return 0;
}

void* variant8_data_ptr(variant8_t* pvar8)
{
	if (pvar8) {
        if (pvar8->type & VARIANT8_PTR)
        	return pvar8->ptr;
        else
			switch (pvar8->type) {
			case VARIANT8_I8:   return &(pvar8->i8);
			case VARIANT8_UI8:  return &(pvar8->ui8);
			case VARIANT8_CHAR: return &(pvar8->ch);
			case VARIANT8_I16:  return &(pvar8->i16);
			case VARIANT8_UI16: return &(pvar8->ui16);
			case VARIANT8_I32:  return &(pvar8->i32);
			case VARIANT8_UI32: return &(pvar8->ui32);
			case VARIANT8_FLT:  return &(pvar8->flt);
			case VARIANT8_USER: return &(pvar8->usr8);
			}
	}
	return 0;
}


uint32_t variant8_total_malloc_size = 0;

void* variant8_malloc(uint16_t size) {
	uint8_t* ptr = (uint8_t*)(size?malloc(size + sizeof(uint16_t)):0);
	if (ptr) {
		*((uint16_t*)ptr) = size;
		variant8_total_malloc_size += size;
	}
	return ptr?(ptr + sizeof(uint16_t)):0;
}

void variant8_free(void *ptr) {
	if (ptr) {
		uint16_t size = ((uint16_t*)ptr)[-1];
		variant8_total_malloc_size -= size;
		free(ptr);
	}
}

void* variant8_realloc(void *ptr, uint16_t size) {
// TODO:
	return 0;
}


#define VARIANT8_TO_STR_MAX_BUFF 32

char* _variant8_to_str(variant8_t* pvar8, const char* fmt) {
	char buff[VARIANT8_TO_STR_MAX_BUFF] = "";
	switch (pvar8->type) {
	case VARIANT8_EMPTY: break;
	case VARIANT8_I8:    snprintf(buff, VARIANT8_TO_STR_MAX_BUFF, fmt?fmt:"%i", (int)pvar8->i8); break;
	case VARIANT8_UI8:   snprintf(buff, VARIANT8_TO_STR_MAX_BUFF, fmt?fmt:"%u", (unsigned int)pvar8->ui8); break;
	case VARIANT8_I16:   snprintf(buff, VARIANT8_TO_STR_MAX_BUFF, fmt?fmt:"%i", (int)pvar8->i16); break;
	case VARIANT8_UI16:  snprintf(buff, VARIANT8_TO_STR_MAX_BUFF, fmt?fmt:"%u", (unsigned int)pvar8->ui16); break;
	case VARIANT8_I32:   snprintf(buff, VARIANT8_TO_STR_MAX_BUFF, fmt?fmt:"%i", (int)pvar8->i32); break;
	case VARIANT8_UI32:  snprintf(buff, VARIANT8_TO_STR_MAX_BUFF, fmt?fmt:"%u", (unsigned int)pvar8->ui32); break;
	case VARIANT8_FLT:   snprintf(buff, VARIANT8_TO_STR_MAX_BUFF, fmt?fmt:"%f", pvar8->flt); break;
	case VARIANT8_CHAR:  snprintf(buff, VARIANT8_TO_STR_MAX_BUFF, fmt?fmt:"%c", pvar8->ch); break;
	case VARIANT8_USER:  snprintf(buff, VARIANT8_TO_STR_MAX_BUFF, fmt?fmt:"%u %u %u",
			(unsigned int)pvar8->usr32, (unsigned int)pvar8->usr16, (unsigned int)pvar8->usr8); break;
	}
	char* str = (char*)variant8_malloc(strlen(buff) + 1);
	if (str) strcpy(str, buff);
	return str;
}

variant8_t _variant_from_str(uint8_t type, char* str, const char* fmt) {
	variant8_t var8 = (variant8_t){ type, 0, { 0 }, { 0 } };
	int n = 0;
	unsigned int ui;
	switch (type) {
	case VARIANT8_EMPTY:
		break;
	case VARIANT8_I8:
	case VARIANT8_I16:
	case VARIANT8_I32:
		n = sscanf(str, fmt?fmt:"%i", (int*)&(var8.i32));
		break;
	case VARIANT8_UI8:
	case VARIANT8_UI16:
	case VARIANT8_UI32:
		n = sscanf(str, fmt?fmt:"%u", (unsigned int*)&(var8.ui32));
		break;
	case VARIANT8_FLT:
		n = sscanf(str, fmt?fmt:"%f", &(var8.flt));
		break;
	case VARIANT8_CHAR:
		n = sscanf(str, fmt?fmt:"%c", &(var8.ch));
		break;
	case VARIANT8_USER:
		n = sscanf(str, fmt?fmt:"%u %hu %u", (unsigned int*)&(var8.usr32), (unsigned short int*)&(var8.usr16), &ui);
		if (n >= 3) var8.usr8 = ui;
		break;
	default:
		var8.type = VARIANT8_ERROR;
		var8.usr32 = VARIANT8_ERR_UNSCON;
		break;
	}
	if (n <= 0)
	{
		var8.type = VARIANT8_ERROR;
		var8.usr32 = VARIANT8_ERR_INVFMT;
	}
	return var8;
}

} //extern "C"

// supported conversions for variant8_change_type
// from              to               param     description
// VARIANT8_EMPTY -> VARIANT8_PCHAR   fmt
// VARIANT8_I8    -> VARIANT8_PCHAR   fmt
// VARIANT8_UI8   -> VARIANT8_PCHAR   fmt
// VARIANT8_I16   -> VARIANT8_PCHAR   fmt
// VARIANT8_UI16  -> VARIANT8_PCHAR   fmt
// VARIANT8_I32   -> VARIANT8_PCHAR   fmt
// VARIANT8_UI32  -> VARIANT8_PCHAR   fmt
// VARIANT8_FLT   -> VARIANT8_PCHAR   fmt
// VARIANT8_CHAR  -> VARIANT8_PCHAR   fmt
// VARIANT8_USER  -> VARIANT8_PCHAR   fmt
//
// VARIANT8_PCHAR -> VARIANT8_EMPTY   fmt
// VARIANT8_PCHAR -> VARIANT8_I8      fmt
// VARIANT8_PCHAR -> VARIANT8_UI8     fmt
// VARIANT8_PCHAR -> VARIANT8_I16     fmt
// VARIANT8_PCHAR -> VARIANT8_UI16    fmt
// VARIANT8_PCHAR -> VARIANT8_I32     fmt
// VARIANT8_PCHAR -> VARIANT8_UI32    fmt
// VARIANT8_PCHAR -> VARIANT8_FLT     fmt
// VARIANT8_PCHAR -> VARIANT8_CHAR    fmt
// VARIANT8_PCHAR -> VARIANT8_USER    fmt
//
