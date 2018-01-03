// built-in include guard removal
// just in case the user has a local dependency with the same file name
#if defined _inc_variadic
	#undef _inc_variadic
#endif
// custom include-guard to ensure we don't duplicate
#if defined _variadic_included
	#endinput
#endif
#define _variadic_included

#include <a_samp>


// Variadic functions in PAWN.

/*
	native GetVariadicAddress(argument);
	native GetVariadicString(address, dest[] = "", size = sizeof(dest));
	native SetVariadicString(address, str[]);

	native PushArguments(start, end);
	native VariadicFormat(output[], len, const str[], bool:pack = false);
	native CallLocalVariadic(function[], const specifiers[]);
	native CallRemoteVariadic(function[], const specifiers[]);

	native IsArgumentPacked(address);
	native GetVariadicLength(address);
	native UnpackArgument(address);

	native DeleteArgument(address);
	native PopArguments();
*/

#define var%0(%1) \
	variadic%0(%1)

#define variadic%0(%1) \
	stock%0(%1, {Float,_}:...)

#define @variadic[%0] \
	#VARIADIC_FORMAT_%0

#if !defined MAX_VARIADIC_STRING
	#define MAX_VARIADIC_STRING (512)
#endif

static stock
	g_PushedAddress[24];

native org_format(output[], len, const str[], {Float,_}:...) = format;
native org_CallLocalFunction(func[], specifiers[], {Float,_}:...) = CallLocalFunction;
native org_CallRemoteFunction(func[], specifiers[], {Float,_}:...) = CallRemoteFunction;

forward _variadic_fix_();
public _variadic_fix_()
{
	strcat("", "");
	strlen("");
	strdel("", 0, 1);
	strunpack("", "");
	strpack("", "");
	ispacked("");
	memcpy("", "", 0, 0, 0);
	CallLocalFunction("", "");
	CallRemoteFunction("", "");

	format("", 1, "");
	org_format("", 1, "");

	org_CallLocalFunction("", "");
	org_CallRemoteFunction("", "");
}

stock GetVariadicAddress(argument)
{
	#emit LOAD.S.pri argument
	#emit SHL.C.pri 2

	#emit LOAD.S.alt 0
	#emit ADD

	#emit ADD.C 12
	#emit LOAD.I

	#emit RETN

	return 0;
}

stock GetVariadicString(address, dest[] = "", size = sizeof(dest))
{
	new string[MAX_VARIADIC_STRING];

	#emit LOAD.S.pri address
	//#emit LOAD.I

	#emit PUSH.C 512
	#emit PUSH.pri
	#emit PUSH.adr string

	#emit PUSH.C 12
	#emit SYSREQ.C strunpack

	#emit STACK 16

	if (numargs() > 1)
	{
		dest[0] = '\0';
		strcat(dest, string, size);
	}
	return string;
}

stock SetVariadicString(address, str[])
{
	new length = 0;

	#emit LOAD.S.pri address

	#emit PUSH.pri
	#emit PUSH.C 4
	#emit LOAD.S.pri length

	#emit SYSREQ.C strlen
	#emit STOR.S.pri length

	#emit STACK 8

	if (length > 0)
	{
		#emit PUSH.adr length
		#emit PUSH.C 0
		#emit PUSH.S address

		#emit PUSH.C 12
		#emit SYSREQ.C strdel

		#emit STACK 16
	}
	#emit PUSH.C 512
	#emit PUSH.S str
	#emit PUSH.S address

	#emit PUSH.C 12
	#emit SYSREQ.C strcat

	#emit STACK 16
	return 1;
}

stock PushArguments(start, end)
{
	new idx, addr;

	for (new i = 0; i < sizeof(g_PushedAddress); i ++) {
		g_PushedAddress[i] = -1;
	}
	while (start != end)
	{
		#emit LOAD.S.pri start
		#emit SMUL.C 4

		#emit LOAD.S.alt 0
		#emit ADD

		#emit ADD.C 12
		#emit LOAD.I
		#emit STOR.S.pri addr

		#emit CONST.pri g_PushedAddress
		#emit LOAD.S.alt idx
		#emit SHL.C.alt 2
		#emit ADD
		#emit LOAD.I
		#emit LOAD.S.pri addr
		#emit STOR.I

		idx++;
		start++;
	}
	return 1;
}

stock VariadicFormat(output[], len, const str[], bool:pack = false)
{
	new i, string[MAX_VARIADIC_STRING * 4], args = 12;

	for (i = sizeof(g_PushedAddress); --i >= 0; )
	{
		if (g_PushedAddress[i] != -1)
		{
			#emit CONST.pri g_PushedAddress
			#emit LOAD.S.alt i
			#emit SHL.C.alt 2
			#emit ADD
			#emit LOAD.I
			#emit PUSH.pri

			args += 4;
		}
	}
	#emit PUSH.S str
	#emit PUSH.S len
	#emit PUSH.adr string

	#emit PUSH.S args
	#emit SYSREQ.C format

	#emit STACK 16

	for (i = 0; i < sizeof(g_PushedAddress); i ++)
	{
		if (g_PushedAddress[i] != -1)
		{
			#emit CONST.pri g_PushedAddress
			#emit LOAD.S.alt i
			#emit SHL.c.alt 2
			#emit ADD
			#emit LOAD.I
			#emit CONST.pri 1
			#emit NEG
			#emit STOR.I
			#emit STACK 4
		}
	}
	if (pack || output[0] > 255) {
		strpack(output, string, len);
	}
	else strcat((output[0] = '\0', output), string, len);
	return 1;
}

stock CallLocalVariadic(function[], const specifiers[])
{
	new i, args = 8;

	for (i = sizeof(g_PushedAddress); --i >= 0; )
	{
		if (g_PushedAddress[i] != -1)
		{
			#emit CONST.pri g_PushedAddress
			#emit LOAD.S.alt i
			#emit SHL.C.alt 2
			#emit ADD
			#emit LOAD.I
			#emit PUSH.pri

			args += 4;
		}
	}
	#emit PUSH.S specifiers
	#emit PUSH.S function

	#emit PUSH.S args
	#emit SYSREQ.C CallLocalFunction

	#emit STACK 12

	for (i = 0; i < sizeof(g_PushedAddress); i ++)
	{
		if (g_PushedAddress[i] != -1)
		{
			#emit CONST.pri g_PushedAddress
			#emit LOAD.S.alt i
			#emit SHL.c.alt 2
			#emit ADD
			#emit LOAD.I
			#emit CONST.pri 1
			#emit NEG
			#emit STOR.I
			#emit STACK 4
		}
	}
	return 1;
}

stock CallRemoteVariadic(function[], const specifiers[])
{
	new i, args = 8;

	for (i = sizeof(g_PushedAddress); --i >= 0; )
	{
		if (g_PushedAddress[i] != -1)
		{
			#emit CONST.pri g_PushedAddress
			#emit LOAD.S.alt i
			#emit SHL.C.alt 2
			#emit ADD
			#emit LOAD.I
			#emit PUSH.pri

			args += 4;
		}
	}
	#emit PUSH.S specifiers
	#emit PUSH.S function

	#emit PUSH.S args
	#emit SYSREQ.C CallRemoteFunction

	#emit STACK 12

	for (i = 0; i < sizeof(g_PushedAddress); i ++)
	{
		if (g_PushedAddress[i] != -1)
		{
			#emit CONST.pri g_PushedAddress
			#emit LOAD.S.alt i
			#emit SHL.c.alt 2
			#emit ADD
			#emit LOAD.I
			#emit CONST.pri 1
			#emit NEG
			#emit STOR.I
			#emit STACK 4
		}
	}
	return 1;
}

stock DeleteArgument(address)
{
	new length;

	#emit PUSH.S address
	#emit PUSH.C 4

	#emit LOAD.S.pri length
	#emit SYSREQ.C strlen

	#emit STOR.S.pri length
	#emit STACK 8

	#emit PUSH.alt
	#emit PUSH.C 0
	#emit PUSH.S address
	#emit PUSH.C 12

	#emit SYSREQ.C strdel
	#emit STACK 16

	return 1;
}

stock PopArguments()
{
	for (new i = 0; i < sizeof(g_PushedAddress); i ++) {
		if (g_PushedAddress[i] != -1) g_PushedAddress[i] = -1;
	}
	return 1;
}

stock IsArgumentPacked(address)
{
	new result;

	#emit PUSH.S address
	#emit PUSH.C 4

	#emit LOAD.S.pri result
	#emit SYSREQ.C ispacked

	#emit STOR.S.pri result
	#emit STACK 8

	return result;
}

stock GetVariadicLength(address)
{
	new length;

	#emit PUSH.S address
	#emit PUSH.C 4

	#emit LOAD.S.pri length
	#emit SYSREQ.C strlen

	#emit STOR.S.pri length
	#emit STACK 8

	return length;
}

stock var_Format(output[], len, const str[], {Float,_}:...)
{
	new
		start,
		end,
		argument,
		string[128],
		args = numargs() * 4;

	GetVariadicString(GetVariadicAddress(3), string, sizeof(string));
	if (!strfind(string, "VARIADIC_FORMAT_", false))
	{
		start = strval(string[16]);

		#emit LOAD.S.pri 0
		#emit ADD.C 8

		#emit LOAD.I
		#emit STOR.S.pri args

		argument = (args / 4);

		while (--argument >= start)
		{
 			#emit LOAD.S.pri argument
			#emit SMUL.C 4

			#emit LOAD.S.alt 0
			#emit ADD

			#emit ADD.C 12
			#emit LOAD.I
			#emit PUSH.pri
		}
		#emit LOAD.S.pri args
		#emit ADD.C 12
		#emit STOR.S.pri args

		#emit PUSH.S str
		#emit PUSH.S len
		#emit PUSH.S output

		#emit PUSH.S args
		#emit SYSREQ.C org_format

		#emit STACK 16

		for (argument = (args / 4) - 4; argument >= start; argument --)
		{
			#emit STACK 4
		}
	}
	else
	{
		#emit ADDR.PRI str
		#emit STOR.S.pri start

		for (end = start + (args - 12); end > start; end -= 4)
		{
			#emit LOAD.S.pri end
			#emit LOAD.I
			#emit PUSH.pri
		}
		#emit PUSH.S str
		#emit PUSH.S len
		#emit PUSH.S output

		#emit PUSH.S args
		#emit SYSREQ.C org_format

		#emit STACK 16

		for (end = start + (args - 12); end > start; end -= 4)
		{
			#emit STACK 4
		}
	}
	return 1;
}

stock var_CallLocalFunction(func[], const specifiers[], {Float,_}:...)
{
	new
		start,
		end,
		argument,
		string[128],
		args = numargs() * 4;

	GetVariadicString(GetVariadicAddress(2), string, sizeof(string));
	if (!strfind(string, "VARIADIC_FORMAT_", false))
	{
		start = strval(string[16]);

		#emit LOAD.S.pri 0
		#emit ADD.C 8

		#emit LOAD.I
		#emit STOR.S.pri args

		argument = (args / 4);

		while (--argument >= start)
		{
			#emit LOAD.S.pri argument
			#emit SMUL.C 4

			#emit LOAD.S.alt 0
			#emit ADD

			#emit ADD.C 12
			#emit LOAD.I
			#emit PUSH.pri
		}
		#emit LOAD.S.pri args
		#emit ADD.C 8
		#emit STOR.S.pri args

		#emit PUSH.S specifiers
		#emit PUSH.S func

		#emit PUSH.S args
		#emit SYSREQ.C org_CallLocalFunction

		#emit STACK 12

		for (argument = (args / 4) - 3; argument >= start; argument --)
		{
			#emit STACK 4
		}
	}
	else
	{
		#emit ADDR.PRI specifiers
		#emit STOR.S.pri start

		for (end = start + (args - 8); end > start; end -= 4)
		{
			#emit LOAD.S.pri end
			#emit LOAD.I
			#emit PUSH.pri
		}
		#emit PUSH.S specifiers
		#emit PUSH.S func

		#emit PUSH.S args
		#emit SYSREQ.C org_CallLocalFunction

		#emit STACK 12

		for (end = start + (args - 8); end > start; end -= 4)
		{
			#emit STACK 4
		}
	}
	return 1;
}

stock var_CallRemoteFunction(func[], const specifiers[], {Float,_}:...)
{
	new
		start,
		end,
		argument,
		string[128],
		args = numargs() * 4;

	GetVariadicString(GetVariadicAddress(2), string, sizeof(string));
	if (!strfind(string, "VARIADIC_FORMAT_", false))
	{
		start = strval(string[16]);

		#emit LOAD.S.pri 0
		#emit ADD.C 8

		#emit LOAD.I
		#emit STOR.S.pri args

		argument = (args / 4);

		while (--argument >= start)
		{
			#emit LOAD.S.pri argument
			#emit SMUL.C 4

			#emit LOAD.S.alt 0
			#emit ADD

			#emit ADD.C 12
			#emit LOAD.I
			#emit PUSH.pri
		}
		#emit LOAD.S.pri args
		#emit ADD.C 8
		#emit STOR.S.pri args

		#emit PUSH.S specifiers
		#emit PUSH.S func

		#emit PUSH.S args
		#emit SYSREQ.C org_CallRemoteFunction

		#emit STACK 12

		for (argument = (args / 4) - 3; argument >= start; argument --)
		{
			#emit STACK 4
		}
	}
	else
	{
		#emit ADDR.PRI specifiers
		#emit STOR.S.pri start

		for (end = start + (args - 8); end > start; end -= 4)
		{
			#emit LOAD.S.pri end
			#emit LOAD.I
			#emit PUSH.pri
		}
		#emit PUSH.S specifiers
		#emit PUSH.S func

		#emit PUSH.S args
		#emit SYSREQ.C org_CallRemoteFunction

		#emit STACK 12

		for (end = start + (args - 8); end > start; end -= 4)
		{
			#emit STACK 4
		}
	}
	return 1;
}

#if defined _ALS_format
	#undef format
#else
	#define _ALS_format
#endif

#if defined _ALS_CallLocalFunction
	#undef CallLocalFunction
#else
	#define _ALS_CallLocalFunction
#endif

#if defined _ALS_CallRemoteFunction
	#undef CallRemoteFunction
#else
	#define _ALS_CallRemoteFunction
#endif

#define format(%0) \
	(var_Format(%0))
	
#define CallLocalFunction(%0) \
	(var_CallLocalFunction(%0))

#define CallRemoteFunction(%0) \
	(var_CallRemoteFunction(%0))
