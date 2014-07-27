#pragma once
#include "lib_msn.h"

typedef enum
{
	MSN_LIST_FL, /**< Forward list */
	MSN_LIST_AL, /**< Allow list */
	MSN_LIST_BL, /**< Block list */
	MSN_LIST_RL, /**< Reverse list */
	MSN_LIST_PL, /**< Pending list */
	MSN_LIST_NL  /**< null */
} MsnListId;

typedef enum
{
	MSN_LIST_NULL  = 0x00,
	MSN_LIST_FL_OP = 0x01,
	MSN_LIST_AL_OP = 0x02,
	MSN_LIST_BL_OP = 0x04,
	MSN_LIST_RL_OP = 0x08,
	MSN_LIST_PL_OP = 0x10
} MsnListOp;

#define MSN_LIST_OP_MASK	0x07

LIB_MSN_API MsnListId msn_get_memberrole(const char *role);

class LIB_MSN_API CMsnUserList
{
public:
	CMsnUserList(void);
	~CMsnUserList(void);
};
