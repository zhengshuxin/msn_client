#pragma once
#include "lib_msn.h"

struct ACL_XML;

class LIB_MSN_API CMsnAddressBookAb
{
public:
	CMsnAddressBookAb(ACL_XML* xmlAb);
	~CMsnAddressBookAb(void);

	char* abId;
	char* lastChange;
	char* DynamicItemLastChanged;
	char* RecentActivityItemLastChanged;
	char* createDate;
	bool propertiesChanged;

	// abInfo

	int MigratedTo;
	int ownerPuid;
	char* OwnerCID;
	char* ownerEmail;
	bool fDefault;
	bool joinedNamespace;
	bool IsBot;
	bool IsParentManaged;
	char* AccountTierLastChanged;
	int ProfileVersion;
	bool SubscribeExternalPartner;
	bool NotifyExternalPartner;
	char* AddressBookType;  // Individual
	bool MessengerApplicationServiceCreated;
	bool IsBetaMigrated;
};
