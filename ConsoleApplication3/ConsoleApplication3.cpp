// ConsoleApplication3.cpp: определяет точку входа для консольного приложения.
//
#include "stdafx.h"
#ifndef UNICODE
#define UNICODE
#endif



#include<fstream>
#include<iostream>
#include <windows.h>
#include <wlanapi.h>
#include<Sddl.h>
#include <objbase.h>
#include <wtypes.h>
#include<string>
#include <stdio.h>
#include <stdlib.h>

using namespace std;


// Need to link with Wlanapi.lib and Ole32.lib
#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "Advapi32.lib")


LPWSTR nameWIFI(PWLAN_AVAILABLE_NETWORK pInetworkInfo)
 {
	LPWSTR name = NULL;
	//name = new char(pInetworkInfo->dot11Ssid.uSSIDLength);
	for (int i = 0; i < pInetworkInfo->dot11Ssid.uSSIDLength; i++)
	{
		name += pInetworkInfo->dot11Ssid.ucSSID[i];
	}
	return name;
 }


void FuncConnecting(HANDLE hClient, PWLAN_AVAILABLE_NETWORK_LIST pInetworkList, PWLAN_INTERFACE_INFO pIfInfo)
{
	int podkl = 0;
	//string parol;


	DWORD dwResult = 0;
	PWLAN_AVAILABLE_NETWORK pInetworkInfo = NULL;
	PWLAN_CONNECTION_MODE pImode = NULL;
	PWLAN_REASON_CODE reasonCode=NULL;
	LPCWSTR str = L"‪D:\\doc.xml";
	LPCWSTR name = L"";
	wprintf(L"Kuda podcl:");
	cin >> podkl;
	//wprintf(L"vvedite parol");
	pInetworkInfo = (PWLAN_AVAILABLE_NETWORK)&pInetworkList->Network[podkl];
	

	for (int i = 0; i < pInetworkInfo->dot11Ssid.uSSIDLength; i++)
	{
		name += pInetworkInfo->dot11Ssid.ucSSID[i];
	}

	WLAN_CONNECTION_PARAMETERS pIparametres;
	pIparametres.wlanConnectionMode = wlan_connection_mode_profile;
	//pIparametres.strProfile = pInetworkInfo->dot11Ssid.ucSSID;
	pIparametres.pDot11Ssid = 0;
	pIparametres.pDesiredBssidList = 0;
	pIparametres.dot11BssType = pInetworkInfo->dot11BssType; // взять с подключаемой точки
	pIparametres.dwFlags = 0;

	//dwResult = ConvertSecurityDescriptorToStringSecurityDescriptor(,SDDL_REVISION_1, BACKUP_SECURITY_INFORMATION, &securityDescriptor, NULL);
	//if (dwResult != ERROR_SUCCESS) {
	//	exit(1);
	//}

	dwResult = WlanSetProfile(hClient,&pIfInfo->InterfaceGuid,0,L"‪D:\\doc.xml",NULL,FALSE,NULL,reasonCode);
	if (dwResult ==ERROR_SUCCESS) {
		exit(1);
	}
	if (dwResult == ERROR_ACCESS_DENIED) {
		exit(1);
	}
	if (dwResult == ERROR_ALREADY_EXISTS) {
		exit(1);
	}
	if (dwResult == ERROR_BAD_PROFILE) {
		exit(1);
	}
	if (dwResult == ERROR_INVALID_PARAMETER) {
	//	exit(1);
	}
	if (dwResult == ERROR_NO_MATCH) {
		exit(1);
	}

	//dwResult = WlanSetProfileCustomUserData(hClient, &pIfInfo->InterfaceGuid,NULL,,,NULL);

	//ConvertStringSecurityDescriptorToSecurityDescriptor(L"lavkistets", , , NULL);
	dwResult= WlanConnect(hClient, &pIfInfo->InterfaceGuid, &pIparametres, 0);
	if (dwResult != ERROR_SUCCESS) {
		exit(1);
	}
}

int wmain()
{
	// Declare and initialize variables.
	do {

		HANDLE hClient = NULL;
		DWORD dwMaxClient = 2;   //    
		DWORD dwCurVersion = 0;
		DWORD dwResult = 0;
		int iRet = 0;

		LPCWSTR pProfileName = NULL;
		LPWSTR pProfileXml = NULL;
		DWORD dwFlags = 0;
		DWORD dwGrantedAccess = 0;

		WCHAR GuidString[40] = { 0 };

		int i;

		/* variables used for WlanEnumInterfaces  */

		PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
		PWLAN_INTERFACE_INFO pIfInfo = NULL;
		PWLAN_RAW_DATA pIdata = NULL;
		PWLAN_AVAILABLE_NETWORK_LIST pInetworkList = NULL;
		PWLAN_AVAILABLE_NETWORK pInetworkInfo = NULL;

		// Validate the parameters

		dwResult = WlanOpenHandle(2, NULL, &dwCurVersion, &hClient);
		if (dwResult != ERROR_SUCCESS) {
			wprintf(L"WlanOpenHandle failed with error: %u\n", dwResult);
			// FormatMessage can be used to find out why the function failed
			return 1;
		}
		dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
		if (dwResult != ERROR_SUCCESS) {
			wprintf(L"WlanEnumInterfaces failed with error: %u\n", dwResult);
			// FormatMessage can be used to find out why the function failed
			return 1;
		}

		for (int i = 0; i < pIfList->dwNumberOfItems; i++)
		{
			pIfInfo = (WLAN_INTERFACE_INFO *)&pIfList->InterfaceInfo[i];

			dwResult = WlanGetAvailableNetworkList(hClient, &pIfInfo->InterfaceGuid, WLAN_AVAILABLE_NETWORK_INCLUDE_ALL_ADHOC_PROFILES
				, NULL, &pInetworkList);

			for (int i = 0; i < (int)pInetworkList->dwNumberOfItems; i++)
			{
				pInetworkInfo = (PWLAN_AVAILABLE_NETWORK)&pInetworkList->Network[i];
				wprintf(L"%d .", i);
				for (int i = 0; i < pInetworkInfo->dot11Ssid.uSSIDLength; i++)
				{
					wprintf(L"%c", pInetworkInfo->dot11Ssid.ucSSID[i]);
				}
				wprintf(L"\n");
			}
		}

		FuncConnecting(hClient, pInetworkList, pIfInfo);

		if (pIfList != NULL) {
			WlanFreeMemory(pIfList);
			pIfList = NULL;
		}
		WlanCloseHandle(hClient, NULL);
		fflush(stdin);
	} while (getchar() != 'q');

	getchar();

    return 0;
}