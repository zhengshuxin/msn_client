#pragma once
#include "lib_msn.h"

class LIB_MSN_API CDigest
{
public:
	CDigest(void);
	~CDigest(void);

	void sha1_hmac(const char* key, size_t klen,
		const unsigned char* data[], size_t dlen[], unsigned char buf[20]);

private:
	acl::string buf_;
};
