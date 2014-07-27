#include "stdafx.h"
//#include "openssl/sha.h"
#include "polarssl/sha1.h"
#include ".\digest.h"

CDigest::CDigest(void)
{
}

CDigest::~CDigest(void)
{

}

void CDigest::sha1_hmac(const char* key, size_t klen,
	const unsigned char* data[], size_t dlen[], unsigned char buf[20])
{
	//SHA_CTX ctx;

	//SHA1_Init(&ctx);
	//SHA1_Update(&ctx, key, klen);
	//for (int i = 0; data[i] != NULL && dlen[i] > 0; i++)
	//	SHA1_Update(&ctx, data[i], dlen[i]);
	//SHA1_Final(buf, &ctx);
	sha1_context ctx;
	sha1_hmac_starts(&ctx, (unsigned char *) key, (int) klen);
	for (int i = 0; data[i] != NULL && dlen[i] > 0; i++)
		sha1_hmac_update(&ctx, (unsigned char *)data[i], (int) dlen[i]);
	sha1_hmac_finish(&ctx, buf);
}