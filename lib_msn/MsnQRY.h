#pragma once
#include "lib_msn.h"

class LIB_MSN_API CMsnQRY
{
public:
	CMsnQRY(void);
	~CMsnQRY(void);

	static void QRYKey(const char* product_id, const char* product_key,
		const char* in, char out[33]);
};
