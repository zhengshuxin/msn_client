#include "stdafx.h"
#include "polarssl/md5.h"
#include "lib_acl.hpp"
#include ".\msnqry.h"

CMsnQRY::CMsnQRY(void)
{
}

CMsnQRY::~CMsnQRY(void)
{
}

#define GUINT32_TO_LE(val)	((unsigned int) val)

void CMsnQRY::QRYKey(const char* product_id, const char* product_key,
	const char* in, char out[33])
{
	md5_context ctx;
	unsigned char md5Hash[16];
	md5_starts(&ctx);
	md5_update(&ctx, (unsigned char*) in, (int) strlen(in));
	md5_update(&ctx, (unsigned char*) product_key, (int) strlen(product_key));
	md5_finish(&ctx, md5Hash);

	int   i;

	unsigned int newHashParts[5];
	unsigned int *md5Parts = (unsigned int*) md5Hash;
	for (i = 0; i < 4; i++)
	{
		/* adjust endianess */
		md5Parts[i] = GUINT32_TO_LE(md5Parts[i]);

		/* & each integer with 0x7FFFFFFF          */
		/* and save one unmodified array for later */
		newHashParts[i] = md5Parts[i];
		md5Parts[i] &= 0x7FFFFFFF;
	}

#define BUFSIZE	256
	char buf[BUFSIZE];

	/* make a new string and pad with '0' to length that's a multiple of 8 */
	_snprintf(buf, BUFSIZE - 5, "%s%s", in, product_id);
	int len = (int) strlen(buf);
	if ((len % 8) != 0)
	{
		int fix = 8 - (len % 8);
		memset(&buf[len], '0', fix);
		buf[len + fix] = '\0';
		len += fix;
	}

	/* split into integers */
	unsigned int *chlStringParts = (unsigned int *)buf;
	long long nHigh = 0, nLow = 0;

	/* this is magic */
	for (i = 0; i < (len / 4); i += 2) {
		long long temp;

		chlStringParts[i] = GUINT32_TO_LE(chlStringParts[i]);
		chlStringParts[i + 1] = GUINT32_TO_LE(chlStringParts[i + 1]);

		temp = (0x0E79A9C1 * (long long)chlStringParts[i]) % 0x7FFFFFFF;
		temp = (md5Parts[0] * (temp + nLow) + md5Parts[1]) % 0x7FFFFFFF;
		nHigh += temp;

		temp = ((long long) chlStringParts[i + 1] + temp) % 0x7FFFFFFF;
		nLow = (md5Parts[2] * temp + md5Parts[3]) % 0x7FFFFFFF;
		nHigh += nLow;
	}
	nLow = (nLow + md5Parts[1]) % 0x7FFFFFFF;
	nHigh = (nHigh + md5Parts[3]) % 0x7FFFFFFF;

	newHashParts[0] ^= nLow;
	newHashParts[1] ^= nHigh;
	newHashParts[2] ^= nLow;
	newHashParts[3] ^= nHigh;

	/* adjust endianness */
	for(i = 0; i < 4; i++)
		newHashParts[i] = GUINT32_TO_LE(newHashParts[i]);

	/* make a string of the parts */
	unsigned char* newHash = (unsigned char *)newHashParts;

	const char hexChars[]     = "0123456789abcdef";

	/* convert to hexadecimal */
	for (i = 0; i < 16; i++)
	{
		out[i * 2] = hexChars[(newHash[i] >> 4) & 0xF];
		out[(i * 2) + 1] = hexChars[newHash[i] & 0xF];
	}
	out[32] = '\0';
}