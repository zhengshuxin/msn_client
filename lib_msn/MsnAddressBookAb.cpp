#include "stdafx.h"
#include "lib_acl.h"
#include "Util.h"
#include ".\msnaddressbookab.h"

CMsnAddressBookAb::CMsnAddressBookAb(ACL_XML* xmlAb)
{
	STRING_SET_MEPTY(abId);
	STRING_SET_MEPTY(lastChange);
	STRING_SET_MEPTY(DynamicItemLastChanged);
	STRING_SET_MEPTY(RecentActivityItemLastChanged);
	STRING_SET_MEPTY(createDate);

	STRING_SET_MEPTY(OwnerCID);
	STRING_SET_MEPTY(ownerEmail);
	STRING_SET_MEPTY(AccountTierLastChanged);
	STRING_SET_MEPTY(AddressBookType);

	abId = GetFirstText(xmlAb, "abId");
	lastChange = GetFirstText(xmlAb, "lastChange");
	DynamicItemLastChanged = GetFirstText(xmlAb, "DynamicItemLastChanged");
	RecentActivityItemLastChanged = GetFirstText(xmlAb, "RecentActivityItemLastChanged");
	createDate = GetFirstText(xmlAb, "createDate");
	propertiesChanged = GetFirstBool(xmlAb, "propertiesChanged");

	// abInfo

	MigratedTo = GetFirstInt(xmlAb, "abInfo/MigratedTo");
	ownerPuid = GetFirstInt(xmlAb, "abInfo/ownerPuid");
	OwnerCID = GetFirstText(xmlAb, "abInfo/OwnerCID");
	ownerEmail = GetFirstText(xmlAb, "abInfo/ownerEmail");
	fDefault = GetFirstBool(xmlAb, "abInfo/fDefault");
	joinedNamespace = GetFirstBool(xmlAb, "abInfo/joinedNamespace");
	IsBot = GetFirstBool(xmlAb, "abInfo/IsBot");
	IsParentManaged = GetFirstBool(xmlAb, "abInfo/IsParentManaged");
	AccountTierLastChanged = GetFirstText(xmlAb, "abInfo/AccountTierLastChanged");
	ProfileVersion = GetFirstInt(xmlAb, "abInfo/ProfileVersion");
	SubscribeExternalPartner = GetFirstBool(xmlAb, "abInfo/SubscribeExternalPartner");
	NotifyExternalPartner = GetFirstBool(xmlAb, "abInfo/NotifyExternalPartner");
	AddressBookType = GetFirstText(xmlAb, "abInfo/AddressBookType");
	MessengerApplicationServiceCreated = GetFirstBool(xmlAb,
		"abInfo/MessengerApplicationServiceCreated");
	IsBetaMigrated = GetFirstBool(xmlAb, "IsBetaMigrated");
}

CMsnAddressBookAb::~CMsnAddressBookAb(void)
{
	STRING_SAFE_FREE(abId);
	STRING_SAFE_FREE(lastChange);
	STRING_SAFE_FREE(DynamicItemLastChanged);
	STRING_SAFE_FREE(RecentActivityItemLastChanged);
	STRING_SAFE_FREE(createDate);

	STRING_SAFE_FREE(OwnerCID);
	STRING_SAFE_FREE(ownerEmail);
	STRING_SAFE_FREE(AccountTierLastChanged);
	STRING_SAFE_FREE(AddressBookType);
}
