#pragma once
#include "MsnGlobal.h"
//#define MSN_APPLICATION_ID "CFE80F9D-180F-4399-82AB-413F33A1FA11"
//#define MSN_APPLICATION_ID "3794391A-4816-4BAC-B34B-6EC7FB5046C6"

//#define MSN_CONTACT_SERVER	"omega.contacts.msn.com"
#define MSN_CONTACT_SERVER	"local-bay.contacts.msn.com"
//#define MSN_CONTACT_SERVER	"contacts.msn.com"
/* Get Contact List */

#define MSN_GET_CONTACT_POST_URL	"/abservice/SharingService.asmx"
#define MSN_GET_CONTACT_SOAP_ACTION "http://www.msn.com/webservices/AddressBook/FindMembership"

#define MSN_GET_CONTACT_UPDATE_XML \
	"<View>Full</View>"\
	"<deltasOnly>true</deltasOnly>"\
	"<lastChange>%s</lastChange>"

#define MSN_GET_CONTACT_TEMPLATE	"<?xml version='1.0' encoding='utf-8'?>" \
"<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">"\
	"<soap:Header xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">"\
		"<ABApplicationHeader xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<ApplicationId xmlns=\"http://www.msn.com/webservices/AddressBook\">" MSN_APPLICATION_ID "</ApplicationId>"\
			"<IsMigration xmlns=\"http://www.msn.com/webservices/AddressBook\">false</IsMigration>"\
			"<PartnerScenario xmlns=\"http://www.msn.com/webservices/AddressBook\">%s</PartnerScenario>"\
		 "</ABApplicationHeader>"\
		"<ABAuthHeader xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<ManagedGroupRequest xmlns=\"http://www.msn.com/webservices/AddressBook\">false</ManagedGroupRequest>"\
			"<TicketToken>%s</TicketToken>"\
		"</ABAuthHeader>"\
	"</soap:Header>" \
	"<soap:Body xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">"\
		"<FindMembership xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<serviceFilter xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
				"<Types xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
					"<ServiceType xmlns=\"http://www.msn.com/webservices/AddressBook\">Messenger</ServiceType>"\
					"<ServiceType xmlns=\"http://www.msn.com/webservices/AddressBook\">Invitation</ServiceType>"\
					"<ServiceType xmlns=\"http://www.msn.com/webservices/AddressBook\">SocialNetwork</ServiceType>"\
					"<ServiceType xmlns=\"http://www.msn.com/webservices/AddressBook\">Space</ServiceType>"\
					"<ServiceType xmlns=\"http://www.msn.com/webservices/AddressBook\">Profile</ServiceType>"\
				"</Types>"\
			"</serviceFilter>"\
			"%s"\
		"</FindMembership>"\
	"</soap:Body>"\
"</soap:Envelope>"

/************************************************
 * Address Book SOAP
 * *********************************************/

#define MSN_ADDRESS_BOOK_POST_URL	"/abservice/abservice.asmx"

/* Create AddressBook template */
#define MSN_ADD_ADDRESSBOOK_SOAP_ACTION     "http://www.msn.com/webservices/AddressBook/ABAdd"

#define MSN_ADD_ADDRESSBOOK_TEMPLATE	"<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
"<soap:Envelope"\
	" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\""\
	" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""\
	" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\""\
	" xmlns:soapenc=\"http://schemas.xmlsoap.org/soap/encoding/\">"\
	"<soap:Header>"\
		"<ABApplicationHeader xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<ApplicationId>" MSN_APPLICATION_ID "</ApplicationId>"\
			"<IsMigration>false</IsMigration>"\
			"<PartnerScenario>Initial</PartnerScenario>"\
		"</ABApplicationHeader>"\
		"<ABAuthHeader xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<ManagedGroupRequest>false</ManagedGroupRequest>"\
			"<TicketToken>%s</TicketToken>"\
		"</ABAuthHeader>"\
	"</soap:Header>"\
	"<soap:Body>"\
		"<ABAdd xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<abInfo>"\
				"<name/>"\
				"<ownerPuid>0</ownerPuid>"\
				"<ownerEmail>%s</ownerEmail>"\
				"<fDefault>true</fDefault>"\
			"</abInfo>"\
		"</ABAdd>"\
	"</soap:Body>"\
"</soap:Envelope>"

/* Get AddressBook */
#define MSN_GET_ADDRESS_SOAP_ACTION	"http://www.msn.com/webservices/AddressBook/ABFindAll"
#define MSN_GET_ADDRESS_FULL_TIME	"0001-01-01T00:00:00.0000000-08:00"
#define MSN_GET_ADDRESS_UPDATE_XML \
	"<deltasOnly>true</deltasOnly>"\
	"<lastChange>%s</lastChange>"

#define MSN_GET_GLEAM_UPDATE_XML \
	"%s"\
	"<dynamicItemView>Gleam</dynamicItemView>"\
	"<dynamicItemLastChange>%s</dynamicItemLastChange>"

#define MSN_GET_ADDRESS_TEMPLATE	"<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
"<soap:Envelope"\
	" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\""\
	" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""\
	" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\""\
	" xmlns:soapenc=\"http://schemas.xmlsoap.org/soap/encoding/\">"\
	"<soap:Header>"\
		"<ABApplicationHeader xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<ApplicationId>" MSN_APPLICATION_ID "</ApplicationId>"\
			"<IsMigration>false</IsMigration>"\
			"<PartnerScenario>%s</PartnerScenario>"\
			"%s"\
		"</ABApplicationHeader>"\
		"<ABAuthHeader xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<ManagedGroupRequest>false</ManagedGroupRequest>"\
			"<TicketToken>%s</TicketToken>"\
		"</ABAuthHeader>"\
	"</soap:Header>"\
	"<soap:Body>"\
		"<ABFindAll xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<abId>00000000-0000-0000-0000-000000000000</abId>"\
			"<abView>Full</abView>"\
			"%s"\
		"</ABFindAll>"\
	"</soap:Body>"\
"</soap:Envelope>"


/*Gleams SOAP request template*/
#define MSN_GET_GLEAMS_SOAP_ACTION "http://www.msn.com/webservices/AddressBook/ABFindAll"
#define MSN_GLEAMS_TEMPLATE "<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
"<soap:Envelope"\
	" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\""\
	" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""\
	" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\""\
	" xmlns:soapenc=\"http://schemas.xmlsoap.org/soap/encoding/\">"\
	"<soap:Header>"\
		"<ABApplicationHeader xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<ApplicationId>" MSN_APPLICATION_ID "</ApplicationId>"\
			"<IsMigration>false</IsMigration>"\
			"<PartnerScenario>Initial</PartnerScenario>"\
		"</ABApplicationHeader>"\
		"<ABAuthHeader xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<ManagedGroupRequest>false</ManagedGroupRequest>"\
			"<TicketToken>EMPTY</TicketToken>"\
		"</ABAuthHeader>"\
	"</soap:Header>"\
	"<soap:Body>"\
		"<ABFindAll xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<abId>00000000-0000-0000-0000-000000000000</abId>"\
			"<abView>Full</abView>"\
			"<dynamicItemView>Gleam</dynamicItemView>"\
			"<dynamicItemLastChange>0001-01-01T00:00:00.0000000-08:00</dynamicItemLastChange>"\
		"</ABFindAll>"\
	"</soap:Body>"\
"</soap:Envelope>"


/*******************************************************
 * Contact Management SOAP actions
 *******************************************************/

/* Add a new contact */
#define MSN_CONTACT_ADD_SOAP_ACTION	"http://www.msn.com/webservices/AddressBook/ABContactAdd"
#define MSN_CONTACT_LIVE_PENDING_XML \
	"<Contact xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
		"<contactInfo>"\
			"<contactType>LivePending</contactType>"\
			"<passportName>%s</passportName>"\
			"<isMessengerUser>true</isMessengerUser>"\
		"</contactInfo>"\
	"</Contact>"

#define MSN_CONTACT_XML	\
	"<Contact xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
		"<contactInfo>"\
			"<passportName>%s</passportName>"\
			"<isSmtp>false</isSmtp>"\
			"<isMessengerUser>true</isMessengerUser>"\
		"</contactInfo>"\
	"</Contact>"

#define MSN_CONTACT_DISPLAYNAME_XML	\
	"<Contact xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
		"<contactInfo>"\
			"<displayName>%s</displayName>"\
			"<passportName>%s</passportName>"\
			"<isMessengerUser>true</isMessengerUser>"\
		"</contactInfo>"\
	"</Contact>"

#define MSN_CONTACT_ID_XML \
	"<Contact>"\
		"<contactId>%s</contactId>"\
	"</Contact>"

#define MSN_CONTACT_EMAIL_XML \
	"<Contact>"\
		"<contactInfo>"\
			"<emails>"\
				"<ContactEmail>"\
					"<contactEmailType>%s</contactEmailType>"\
					"<email>%s</email>"\
					"<isMessengerEnabled>true</isMessengerEnabled>"\
					"<Capability>%d</Capability>"\
					"<MessengerEnabledExternally>false</MessengerEnabledExternally>"\
					"<propertiesChanged/>"\
				"</ContactEmail>"\
			"</emails>"\
		"</contactInfo>"\
	"</Contact>"

#define MSN_CONTACT_INVITE_MESSAGE_XML \
	"<MessengerMemberInfo>"\
		"<PendingAnnotations>"\
			"<Annotation>"\
				"<Name>MSN.IM.InviteMessage</Name>"\
				"<Value>%s</Value>"\
			"</Annotation>"\
		"</PendingAnnotations>"\
		"<DisplayName>%s</DisplayName>"\
	"</MessengerMemberInfo>"

#define MSN_ADD_CONTACT_TEMPLATE	"<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
"<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\""\
	" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""\
	" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\""\
	" xmlns:soapenc=\"http://schemas.xmlsoap.org/soap/encoding/\">"\
	"<soap:Header>"\
		"<ABApplicationHeader xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<ApplicationId>" MSN_APPLICATION_ID "</ApplicationId>"\
			"<IsMigration>false</IsMigration>"\
			"<PartnerScenario>ContactSave</PartnerScenario>"\
		"</ABApplicationHeader>"\
		"<ABAuthHeader xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<ManagedGroupRequest>false</ManagedGroupRequest>"\
			"<TicketToken>EMPTY</TicketToken>"\
		"</ABAuthHeader>"\
	"</soap:Header>"\
	"<soap:Body>"\
		"<ABContactAdd xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<abId>00000000-0000-0000-0000-000000000000</abId>"\
			"<contacts>%s</contacts>"\
			"<options>"\
				"<EnableAllowListManagement>true</EnableAllowListManagement>"\
			"</options>"\
		"</ABContactAdd>"\
	"</soap:Body>"\
"</soap:Envelope>"

/* Add a contact to a group */
#define MSN_ADD_CONTACT_GROUP_SOAP_ACTION	"http://www.msn.com/webservices/AddressBook/ABGroupContactAdd"
#define MSN_ADD_CONTACT_GROUP_TEMPLATE	"<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
"<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\""\
	" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""\
	" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\""\
	" xmlns:soapenc=\"http://schemas.xmlsoap.org/soap/encoding/\">"\
	"<soap:Header>"\
		"<ABApplicationHeader xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<ApplicationId>" MSN_APPLICATION_ID "</ApplicationId>"\
			"<IsMigration>false</IsMigration>"\
			"<PartnerScenario>ContactSave</PartnerScenario>"\
		"</ABApplicationHeader>"\
		"<ABAuthHeader xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<ManagedGroupRequest>false</ManagedGroupRequest>"\
			"<TicketToken>EMPTY</TicketToken>"\
		"</ABAuthHeader>"\
	"</soap:Header>"\
	"<soap:Body>"\
		"<ABGroupContactAdd xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<abId>00000000-0000-0000-0000-000000000000</abId>"\
			"<groupFilter>"\
				"<groupIds>"\
					"<guid>%s</guid>"\
				"</groupIds>"\
			"</groupFilter>"\
			"<contacts>%s</contacts>"\
			"<groupContactAddOptions>"\
				"<fGenerateMissingQuickName>true</fGenerateMissingQuickName>"\
				"<EnableAllowListManagement>true</EnableAllowListManagement>"\
			"</groupContactAddOptions>"\
			"%s"\
		"</ABGroupContactAdd>"\
	"</soap:Body>"\
"</soap:Envelope>"

/* Delete a contact from the Contact List */
#define MSN_CONTACT_DEL_SOAP_ACTION	"http://www.msn.com/webservices/AddressBook/ABContactDelete"
#define MSN_DEL_CONTACT_TEMPLATE	"<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
"<soap:Envelope"\
	" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\""\
	" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""\
	" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\""\
	" xmlns:soapenc=\"http://schemas.xmlsoap.org/soap/encoding/\">"\
	"<soap:Header>"\
		"<ABApplicationHeader xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<ApplicationId>" MSN_APPLICATION_ID "</ApplicationId>"\
			"<IsMigration>false</IsMigration>"\
			"<PartnerScenario>Timer</PartnerScenario>"\
		"</ABApplicationHeader>"\
		"<ABAuthHeader xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<ManagedGroupRequest>false</ManagedGroupRequest>"\
			"<TicketToken>EMPTY</TicketToken>"\
		"</ABAuthHeader>"\
	"</soap:Header>"\
	"<soap:Body>"\
		"<ABContactDelete xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<abId>00000000-0000-0000-0000-000000000000</abId>"\
			"<contacts>%s</contacts>"\
		"</ABContactDelete>"\
	"</soap:Body>"\
"</soap:Envelope>"

/* Remove a contact from a group */
#define MSN_CONTACT_DEL_GROUP_SOAP_ACTION	"http://www.msn.com/webservices/AddressBook/ABGroupContactDelete"
#define MSN_CONTACT_DEL_GROUP_TEMPLATE	"<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
"<soap:Envelope"\
	" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\""\
	" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""\
	" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\""\
	" xmlns:soapenc=\"http://schemas.xmlsoap.org/soap/encoding/\">"\
	"<soap:Header>"\
		"<ABApplicationHeader xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<ApplicationId>" MSN_APPLICATION_ID "</ApplicationId>"\
			"<IsMigration>false</IsMigration>"\
			"<PartnerScenario>Timer</PartnerScenario>"\
		"</ABApplicationHeader>"\
		"<ABAuthHeader xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<ManagedGroupRequest>false</ManagedGroupRequest>"\
			"<TicketToken>EMPTY</TicketToken>"\
		"</ABAuthHeader>"\
	"</soap:Header>"\
	"<soap:Body>"\
		"<ABGroupContactDelete xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<abId>00000000-0000-0000-0000-000000000000</abId>"\
			"<contacts>%s</contacts>"\
			"<groupFilter>"\
				"<groupIds>"\
					"<guid>%s</guid>"\
				"</groupIds>"\
			"</groupFilter>"\
		"</ABGroupContactDelete>"\
	"</soap:Body>"\
"</soap:Envelope>"


/* Update Contact Information */
#define MSN_CONTACT_UPDATE_SOAP_ACTION	"http://www.msn.com/webservices/AddressBook/ABContactUpdate"
#define MSN_CONTACT_UPDATE_TEMPLATE	"<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
"<soap:Envelope"\
	" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\""\
	" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""\
	" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\""\
	" xmlns:soapenc=\"http://schemas.xmlsoap.org/soap/encoding/\">"\
	"<soap:Header>"\
		"<ABApplicationHeader xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<ApplicationId>" MSN_APPLICATION_ID "</ApplicationId>"\
			"<IsMigration>false</IsMigration>"\
			"<PartnerScenario></PartnerScenario>"\
		"</ABApplicationHeader>"\
		"<ABAuthHeader xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<ManagedGroupRequest>false</ManagedGroupRequest>"\
			"<TicketToken>EMPTY</TicketToken>"\
		"</ABAuthHeader>"\
	"</soap:Header>"\
	"<soap:Body>"\
		"<ABContactUpdate xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<abId>00000000-0000-0000-0000-000000000000</abId>"\
			"<contacts>"\
				"<Contact xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
					""\
				"</Contact>"\
			"</contacts>"\
		"</ABContactUpdate>"\
	"</soap:Body>"\
"</soap:Envelope>"

/* Update Contact Annotations */
#define MSN_CONTACT_ANNOTATE_SOAP_ACTION	"http://www.msn.com/webservices/AddressBook/ABContactUpdate"
#define MSN_CONTACT_ANNOTATE_TEMPLATE	"<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
"<soap:Envelope"\
	" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\""\
	" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""\
	" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\""\
	" xmlns:soapenc=\"http://schemas.xmlsoap.org/soap/encoding/\">"\
	"<soap:Header>"\
		"<ABApplicationHeader xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<ApplicationId>" MSN_APPLICATION_ID "</ApplicationId>"\
			"<IsMigration>false</IsMigration>"\
			"<PartnerScenario></PartnerScenario>"\
		"</ABApplicationHeader>"\
		"<ABAuthHeader xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<ManagedGroupRequest>false</ManagedGroupRequest>"\
			"<TicketToken>EMPTY</TicketToken>"\
		"</ABAuthHeader>"\
	"</soap:Header>"\
	"<soap:Body>"\
		"<ABContactUpdate xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<abId>00000000-0000-0000-0000-000000000000</abId>"\
			"<contacts>"\
				"<Contact xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
					"<propertiesChanged>Annotation</propertiesChanged>"\
				"</Contact>"\
			"</contacts>"\
		"</ABContactUpdate>"\
	"</soap:Body>"\
"</soap:Envelope>"

/*******************************************************
 * Add/Delete contact from lists SOAP actions
 *******************************************************/

/* block means delete from allow list and add contact to block list */
#define MSN_SHARE_POST_URL		"/abservice/SharingService.asmx"

#define MSN_ADD_MEMBER_TO_LIST_SOAP_ACTION	"http://www.msn.com/webservices/AddressBook/AddMember"
#define MSN_DELETE_MEMBER_FROM_LIST_SOAP_ACTION	"http://www.msn.com/webservices/AddressBook/DeleteMember"

#define MSN_MEMBER_PASSPORT_XML	\
	"<Member xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:type=\"%s\">"\
		"<Type>%s</Type>"\
		"<State>Accepted</State>"\
		"<%s>%s</%s>"\
	"</Member>"

#define MSN_MEMBER_MEMBERSHIPID_XML	\
	"<Member xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:type=\"%s\">"\
		"<Type>%s</Type>"\
		"<MembershipId>%u</MembershipId>"\
		"<State>Accepted</State>"\
	"</Member>"

/* first delete contact from allow list */

#define MSN_CONTACT_DELETE_FROM_LIST_TEMPLATE "<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
"<soap:Envelope"\
	" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\""\
	" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""\
	" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\""\
	" xmlns:soapenc=\"http://schemas.xmlsoap.org/soap/encoding/\">"\
	"<soap:Header>"\
		"<ABApplicationHeader xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<ApplicationId>" MSN_APPLICATION_ID "</ApplicationId>"\
			"<IsMigration>false</IsMigration>"\
			"<PartnerScenario>%s</PartnerScenario>"\
		"</ABApplicationHeader>"\
		"<ABAuthHeader xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<ManagedGroupRequest>false</ManagedGroupRequest>"\
			"<TicketToken>EMPTY</TicketToken>"\
		"</ABAuthHeader>"\
	"</soap:Header>"\
	"<soap:Body>"\
		"<DeleteMember xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<serviceHandle>"\
				"<Id>0</Id>"\
				"<Type>Messenger</Type>"\
				"<ForeignId></ForeignId>"\
			"</serviceHandle>"\
			"<memberships>"\
				"<Membership>"\
					"<MemberRole>%s</MemberRole>"\
					"<Members>"\
						"%s"\
					"</Members>"\
				"</Membership>"\
			"</memberships>"\
		"</DeleteMember>"\
	"</soap:Body>"\
"</soap:Envelope>"

#define MSN_CONTACT_ADD_TO_LIST_TEMPLATE	"<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
"<soap:Envelope"\
	" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\""\
	" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""\
	" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\""\
	" xmlns:soapenc=\"http://schemas.xmlsoap.org/soap/encoding/\">"\
	"<soap:Header>"\
		"<ABApplicationHeader xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<ApplicationId>" MSN_APPLICATION_ID "</ApplicationId>"\
			"<IsMigration>false</IsMigration>"\
			"<PartnerScenario>%s</PartnerScenario>"\
		"</ABApplicationHeader>"\
		"<ABAuthHeader xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<ManagedGroupRequest>false</ManagedGroupRequest>"\
			"<TicketToken>EMPTY</TicketToken>"\
		"</ABAuthHeader>"\
	"</soap:Header>"\
	"<soap:Body>"\
		"<AddMember xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<serviceHandle>"\
				"<Id>0</Id>"\
				"<Type>Messenger</Type>"\
				"<ForeignId></ForeignId>"\
			"</serviceHandle>"\
			"<memberships>"\
				"<Membership>"\
					"<MemberRole>%s</MemberRole>"\
					"<Members>"\
						"%s"\
					"</Members>"\
				"</Membership>"\
			"</memberships>"\
		"</AddMember>"\
	"</soap:Body>"\
"</soap:Envelope>"



/*******************************************************
 * Group management SOAP actions
 *******************************************************/

/* add a group */
#define MSN_GROUP_ADD_SOAP_ACTION	"http://www.msn.com/webservices/AddressBook/ABGroupAdd"
#define MSN_GROUP_ADD_TEMPLATE	"<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
"<soap:Envelope"\
	" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\""\
	" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""\
	" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\""\
	" xmlns:soapenc=\"http://schemas.xmlsoap.org/soap/encoding/\">"\
	"<soap:Header>"\
		"<ABApplicationHeader xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<ApplicationId>" MSN_APPLICATION_ID "</ApplicationId>"\
			"<IsMigration>false</IsMigration>"\
			"<PartnerScenario>GroupSave</PartnerScenario>"\
		"</ABApplicationHeader>"\
		"<ABAuthHeader xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<ManagedGroupRequest>false</ManagedGroupRequest>"\
			"<TicketToken>EMPTY</TicketToken>"\
		"</ABAuthHeader>"\
	"</soap:Header>"\
	"<soap:Body>"\
		"<ABGroupAdd xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<abId>00000000-0000-0000-0000-000000000000</abId>"\
			"<groupAddOptions>"\
				"<fRenameOnMsgrConflict>false</fRenameOnMsgrConflict>"\
			"</groupAddOptions>"\
			"<groupInfo>"\
				"<GroupInfo>"\
					"<name>%s</name>"\
					"<groupType>C8529CE2-6EAD-434d-881F-341E17DB3FF8</groupType>"\
					"<fMessenger>false</fMessenger>"\
					"<annotations>"\
						"<Annotation>"\
							"<Name>MSN.IM.Display</Name>"\
							"<Value>1</Value>"\
						"</Annotation>"\
					"</annotations>"\
				"</GroupInfo>"\
			"</groupInfo>"\
		"</ABGroupAdd>"\
	"</soap:Body>"\
"</soap:Envelope>"

/* delete a group */
#define MSN_GROUP_DEL_SOAP_ACTION	"http://www.msn.com/webservices/AddressBook/ABGroupDelete"
#define MSN_GROUP_DEL_TEMPLATE	"<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
"<soap:Envelope"\
	" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\""\
	" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""\
	" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\""\
	" xmlns:soapenc=\"http://schemas.xmlsoap.org/soap/encoding/\">"\
	"<soap:Header>"\
		"<ABApplicationHeader xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<ApplicationId>" MSN_APPLICATION_ID "</ApplicationId>"\
			"<IsMigration>false</IsMigration>"\
			"<PartnerScenario>Timer</PartnerScenario>"\
		"</ABApplicationHeader>"\
		"<ABAuthHeader xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<ManagedGroupRequest>false</ManagedGroupRequest>"\
			"<TicketToken>EMPTY</TicketToken>"\
		"</ABAuthHeader>"\
	"</soap:Header>"\
	"<soap:Body>"\
		"<ABGroupDelete xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<abId>00000000-0000-0000-0000-000000000000</abId>"\
			"<groupFilter>"\
				"<groupIds>"\
					"<guid>%s</guid>"\
				"</groupIds>"\
			"</groupFilter>"\
		"</ABGroupDelete>"\
	"</soap:Body>"\
"</soap:Envelope>"

/* change a group's name */
#define MSN_GROUP_RENAME_SOAP_ACTION	"http://www.msn.com/webservices/AddressBook/ABGroupUpdate"
#define MSN_GROUP_RENAME_TEMPLATE	"<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
"<soap:Envelope"\
	" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\""\
	" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""\
	" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\""\
	" xmlns:soapenc=\"http://schemas.xmlsoap.org/soap/encoding/\">"\
	"<soap:Header>"\
		"<ABApplicationHeader xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<ApplicationId>" MSN_APPLICATION_ID "</ApplicationId>"\
			"<IsMigration>false</IsMigration>"\
			"<PartnerScenario>Timer</PartnerScenario>"\
		"</ABApplicationHeader>"\
		"<ABAuthHeader xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<ManagedGroupRequest>false</ManagedGroupRequest>"\
			"<TicketToken>EMPTY</TicketToken>"\
		"</ABAuthHeader>"\
	"</soap:Header>"\
	"<soap:Body>"\
		"<ABGroupUpdate xmlns=\"http://www.msn.com/webservices/AddressBook\">"\
			"<abId>00000000-0000-0000-0000-000000000000</abId>"\
			"<groups>"\
				"<Group>"\
					"<groupId>%s</groupId>"\
					"<groupInfo>"\
						"<name>%s</name>"\
					"</groupInfo>"\
					"<propertiesChanged>GroupName </propertiesChanged>"\
				"</Group>"\
			"</groups>"\
		"</ABGroupUpdate>"\
	"</soap:Body>"\
"</soap:Envelope>"