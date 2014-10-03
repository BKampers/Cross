/*
** Retrieve and compare objects and values in JSON texts
** Copyright 2014, Bart Kampers
*/

#include "JsonMessage.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
** private
*/

char* JSON_NAME_NOT_FOUND = "JSON_NAME_NOT_FOUND";
char* JSON_TYPE_MISMATCH  = "JSON_TYPE_MISMATCH";


/*
** interface
*/


Status FindPair(const char* jsonString, const char* name, struct jsonparse_state* state)
{
	jsonparse_setup(state, jsonString, strlen(jsonString));
	int nameLength = strlen(name);
	int next;
	do {
		next = jsonparse_next(state);
		if ((next == JSON_TYPE_PAIR_NAME) && (state->vlen == nameLength))
		{
			if (strncmp(name, jsonString + state->vstart, nameLength) == 0)
			{
				next = jsonparse_next(state);
				if (next == JSON_TYPE_PAIR)
				{
					jsonparse_next(state);
					return OK;
				}
			}
		}
	} while (next != 0);
	return JSON_NAME_NOT_FOUND;
}


Status GetBoolValue(const char* jsonString, const char* name, bool* value)
{
	struct jsonparse_state state;
	Status status = FindPair(jsonString, name, &state);
	if (status == OK)
	{
		return AsBool(&state, value);
	}
	else
	{
		return status;
	}
}


Status GetIntValue(const char* jsonString, const char* name, int* value)
{
	struct jsonparse_state state;
	Status status = FindPair(jsonString, name, &state);
	if (status == OK)
	{
		return AsInt(&state, value);
	}
	else
	{
		return status;
	}
}


Status GetFloatValue(const char* jsonString, const char* name, float* value)
{
	struct jsonparse_state state;
	Status status = FindPair(jsonString, name, &state);
	if (status == OK)
	{
		return AsFloat(&state, value);
	}
	else
	{
		return status;
	}
}


Status AsBool(const struct jsonparse_state* state, bool* value)
{
	switch (state->vtype)
	{
		case JSON_TYPE_FALSE:
			*value = FALSE;
			return OK;
		case JSON_TYPE_TRUE:
			*value = TRUE;
			return OK;
		default:
			return JSON_TYPE_MISMATCH;
	}
}


Status AsInt(const struct jsonparse_state* state, int* value)
{
	if ((state->vtype == JSON_TYPE_NUMBER) || (state->vtype == JSON_TYPE_INT))
	{
		*value = atoi(state->json + state->vstart);
		return OK;
	}
	else
	{
		return JSON_TYPE_MISMATCH;
	}
}


Status AsFloat(const struct jsonparse_state* state, float* value)
{
	if ((state->vtype == JSON_TYPE_NUMBER) || (state->vtype == JSON_TYPE_INT))
	{
		*value = atof(state->json + state->vstart);
		return OK;
	}
	else
	{
		return JSON_TYPE_MISMATCH;
	}
}


bool EqualString(const struct jsonparse_state* state, const char* value)
{
	if (state->vtype == JSON_TYPE_STRING)
	{
		int length = strlen(value);
		return (state->vlen == length) && (strncmp(value, state->json + state->vstart, length) == 0);
	}
	else
	{
		return FALSE;
	}
}


bool Contains(const char* jsonString, const char* name, const char* value)
{
	struct jsonparse_state state;
	Status status = FindPair(jsonString, name, &state);
	if ((status == OK) /*&& (state.vtype == JSON_TYPE_ARRAY)*/)
	{
		int valueLength = strlen(value);
		int next;
		do {
			next = jsonparse_next(&state);
			if ((valueLength == state.vlen) && (strncmp(jsonString + state.vstart, value, valueLength) == 0))
			{
				return TRUE;
			}
		} while (next != 0);
	}
	return FALSE;
}
