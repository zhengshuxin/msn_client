#include "stdafx.h"
#include "lib_acl.h"
#include "lib_acl.hpp"
#include "MsnTicket.h"
#include "polarssl/des.h"
#include "polarssl/sha1.h"
#include ".\msnusrkey.h"

struct MsnUsrKey
{
	int size; /* 28. Does not count data, 要求低位字节序 */
	int crypt_mode; /* CRYPT_MODE_CBC (1) */
	int cipher_type; /* TripleDES (0x6603) */
	int hash_type; /* SHA1 (0x8004) */
	int iv_len;    /* 8 */
	int hash_len;  /* 20 */
	int cipher_len; /* 72 */
	/* Data */
	char iv[8];
	char hash[20];
	char cipher[72];
};

static void create_key(const char *key, size_t klen,
	const char *magic, size_t dlen, unsigned char hash_buf[24])
{
	unsigned char hash1[20], hash2[20], hash3[20], hash4[20];
	sha1_context ctx;

	sha1_hmac_starts(&ctx, (unsigned char *) key, (int) klen);
	sha1_hmac_update(&ctx, (unsigned char *) magic, (int) dlen);
	sha1_hmac_finish(&ctx, hash1);

	sha1_hmac_starts(&ctx, (unsigned char *) key, (int) klen);
	sha1_hmac_update(&ctx, (unsigned char *) hash1, (int) sizeof(hash1));
	sha1_hmac_update(&ctx, (unsigned char *) magic, (int) dlen);
	sha1_hmac_finish(&ctx, hash2);

	sha1_hmac_starts(&ctx, (unsigned char *) key, (int) klen);
	sha1_hmac_update(&ctx, (unsigned char *) hash1, (int) sizeof(hash1));
	sha1_hmac_finish(&ctx, hash3);

	sha1_hmac_starts(&ctx, (unsigned char *) key, (int) klen);
	sha1_hmac_update(&ctx, (unsigned char *) hash3, (int) sizeof(hash3));
	sha1_hmac_update(&ctx, (unsigned char *) magic, (int) dlen);
	sha1_hmac_finish(&ctx, hash4);

	memcpy(hash_buf, hash2, 20);
	memcpy(hash_buf + 20, hash4, 4);
}

static char *des3_cbc_encrypt(const char *key /* 24 bytes */, const char *iv /* 8 bytes */,
	char *data, size_t len)
{
	des3_context ctx3;

	unsigned char key24[24];
	memcpy(key24, key, 24);
	des3_set3key_enc(&ctx3, key24);

	unsigned char iv8[8];
	memcpy(iv8, iv, 8);
	des3_crypt_cbc(&ctx3, DES_ENCRYPT, (int) len, iv8,
		(unsigned char*) data, (unsigned char*) data);
	return (data);
}

CMsnUsrKey::CMsnUsrKey(void)
{
}

CMsnUsrKey::~CMsnUsrKey(void)
{
}

#define CRYPT_MODE_CBC 1
#define CIPHER_TRIPLE_DES 0x6603
#define HASH_SHA1 0x8004

#define GUINT32_TO_LE(val)	((unsigned int) val)

bool CMsnUsrKey::CreateKey(const char* secret,
	const char* nonce, acl::string& buf)
{
	if (secret == NULL)
	{
		logger_error("messengerclear.live.com's ticket secret null");
		return (false);
	}	MsnUsrKey usr_key;

	char* key1;
	int  len1 = acl_base64_decode(secret, &key1);
	if (len1 < 0)
	{
		logger_error("messengerclear.live.com's ticket secret(%s) invalid", secret);
		return (false);
	}

	memset(&usr_key, 0, sizeof(MsnUsrKey));

	usr_key.size = GUINT32_TO_LE(28); // msn 要求低字节序，怪
	usr_key.crypt_mode = GUINT32_TO_LE(CRYPT_MODE_CBC);
	usr_key.cipher_type = GUINT32_TO_LE(CIPHER_TRIPLE_DES);
	usr_key.hash_type = GUINT32_TO_LE(HASH_SHA1);
	usr_key.iv_len = GUINT32_TO_LE(8);
	usr_key.hash_len = GUINT32_TO_LE(20);
	usr_key.cipher_len = GUINT32_TO_LE(72);

	const char magic1[] = "WS-SecureConversationSESSION KEY HASH";
	const char magic2[] = "WS-SecureConversationSESSION KEY ENCRYPTION";
	unsigned char key2[24], key3[24];

	memset(key2, 0, sizeof(key2));
	memset(key3, 0, sizeof(key3));
	create_key(key1, len1, magic1, sizeof(magic1) - 1, key2);
	create_key(key1, len1, magic2, sizeof(magic2) - 1, key3);

	size_t nonce_len = strlen(nonce);

	sha1_context ctx;
	sha1_hmac_starts(&ctx, key2, 24);
	sha1_hmac_update(&ctx, (unsigned char *) nonce, (int) nonce_len);
	sha1_hmac_finish(&ctx, (unsigned char*) usr_key.hash);

	// We need to pad this to 72 bytes, apparently

	char* nonce_fixed = (char*) acl_mymalloc(nonce_len + 8);
	memcpy(nonce_fixed, nonce, nonce_len);
	memset(nonce_fixed + nonce_len, 0x08, 8);

	int *iv = (int*) usr_key.iv;
	iv[0] = rand();
	iv[1] = rand();

	const char* cipher = des3_cbc_encrypt((const char*) key3, usr_key.iv,
				nonce_fixed, nonce_len + 8);
	memcpy(usr_key.cipher, cipher, 72);

	// free memory allocated

	acl_myfree(nonce_fixed);

	buf.base64_encode(&usr_key, sizeof(usr_key));
	return (true);
}
