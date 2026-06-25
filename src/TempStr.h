#ifndef TEMPSTR_H
#define TEMPSTR_H

#ifndef _WINDOWS_
#error Windows.h required for this to compile!
#endif

const int TempStrSize = 8 * 1024;

class TempStr
{
protected:
	char buffer[TempStrSize];

public:
	TempStr() { buffer[0] = '\0'; }
	TempStr(char *fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		set(fmt, args);
		va_end(args);
	}

	void set(char *fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		set(fmt, args);
		va_end(args);
	}

	void set(char *fmt, va_list args)
	{
		_vsnprintf(buffer, TempStrSize, fmt, args);
	}

	operator char *(void) { return buffer; }
};

#define TEMPSTR (char *)TempStr

#endif
