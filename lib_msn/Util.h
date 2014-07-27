#pragma once

struct ACL_XML;

extern char* var_emty_string;

#define STRING_SAFE_FREE(x) do { \
	if ((x) != NULL && (x) != var_emty_string) \
		acl_myfree((x)); \
} while(0)

#define STRING_IS_EMPTY(x) ((x) == NULL || (x) == var_emty_string)
#define STRING_SET_MEPTY(x) ((x) = var_emty_string)

char* GetFirstText(ACL_XML* xml, const char* tag, bool* no_exist = NULL);
bool  GetFirstBool(ACL_XML* xml, const char* tag, bool* no_exist = NULL);
int   GetFirstInt(ACL_XML* xml, const char* tag, bool* no_exist = NULL);

char* GetFirstText(acl::xml& xml, const char* tag, bool* no_exist = NULL);
bool  GetFirstBool(acl::xml& xml, const char* tag, bool* no_exist = NULL);
int   GetFirstInt(acl::xml& xml, const char* tag, bool* no_exist = NULL);

bool  MsnEmailIsValid(const char *passport);
bool  EmailIsValid(const char *address);
void  RandGuid(char* buf, size_t size);