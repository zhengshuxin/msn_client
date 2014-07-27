#include "stdafx.h"
#include "MsnGlobal.h"
#include ".\msnclientinfo.h"

CMsnClientInfo::CMsnClientInfo(void)
:/* msn_protocol_(MSN_PROTOCOL15)
, */ product_id_(MSN_PRODUCT_ID)
, product_key_(MSN_PROJECT_KEY)
, client_name_(MSN_CLIENT_NAME)
, build_ver_(MSN_BUILD_VER)
, application_id_(MSN_APPLICATION_ID)
, client_brand_(MSN_CLIENT_BRAND)
{
}

CMsnClientInfo::~CMsnClientInfo(void)
{
}
