/*
 * Copyright (c) 2018, Adam <Adam@sigterm.info>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <my_global.h>
#include <mysql.h>
#include <string.h>

static void con()  __attribute__((constructor));
extern "C" my_bool level_for_xp_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
extern "C" longlong level_for_xp(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
extern "C" void level_for_xp_deinit(UDF_INIT *initid);

static constexpr int MAX_VIRT_LEVEL = 126;
static int XP_FOR_LEVEL[MAX_VIRT_LEVEL];

static void con()
{
	int xp = 0;

	for (int level = 1; level <= MAX_VIRT_LEVEL; ++level)
	{
		XP_FOR_LEVEL[level - 1] = xp / 4;

		int difference = (int) ((double) level + 300.0 * pow(2.0, (double) level / 7.0));
		xp += difference;
	}
}

static int _level_for_xp(int xp)
{
	if (xp < 0)
	{
		return -1;
	}

	int low = 0;
	int high = MAX_VIRT_LEVEL - 1;

	while (low <= high)
	{
		int mid = low + (high - low) / 2;
		int xpForLevel = XP_FOR_LEVEL[mid];

		if (xp < xpForLevel)
		{
			high = mid - 1;
		}
		else if (xp > xpForLevel)
		{
			low = mid + 1;
		}
		else
		{
			return mid + 1;
		}
	}

	return high + 1;
}

my_bool level_for_xp_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (args->arg_count != 1 || args->arg_type[0] != INT_RESULT)
	{
		strcpy(message, "Wrong number and/or type of arguments");
		return 1;
	}

	return 0;
}

longlong level_for_xp(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
	longlong xp = * (longlong *) args->args[0];
	return _level_for_xp(xp);
}

void level_for_xp_deinit(UDF_INIT *initid)
{
}
