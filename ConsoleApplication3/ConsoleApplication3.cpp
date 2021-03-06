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


BOOL checkConnection(PWLAN_INTERFACE_INFO pIfInfo)
 {
	if ( pIfInfo->isState == wlan_interface_state_connected)
		return true;
	else false;
	
 }

void connectingToWIFI(HANDLE hClient, PWLAN_AVAILABLE_NETWORK_LIST pInetworkList, PWLAN_INTERFACE_INFO pIfInfo)
{
	int podkl = 0;
	DWORD dwResult = 0;
	PWLAN_AVAILABLE_NETWORK pInetworkInfo = NULL;
	PWLAN_CONNECTION_MODE pImode = NULL;

	cout << "Number to connect:";
	fflush(stdin);
	cin >> podkl;
	pInetworkInfo = (PWLAN_AVAILABLE_NETWORK)&pInetworkList->Network[podkl];	
	const int size = pInetworkInfo->dot11Ssid.uSSIDLength;
	WCHAR GuidString[20] = { 0 };

	for (int i = 0; i < pInetworkInfo->dot11Ssid.uSSIDLength;i++)
	 {
		GuidString[i] = pInetworkInfo->dot11Ssid.ucSSID[i];
	 }

	WLAN_CONNECTION_PARAMETERS pIparametres;
	pIparametres.wlanConnectionMode = wlan_connection_mode_profile;
	pIparametres.strProfile = GuidString;
	pIparametres.pDot11Ssid = 0;
	pIparametres.pDesiredBssidList = 0;
	pIparametres.dot11BssType = pInetworkInfo->dot11BssType; // взять с подключаемой точки
	pIparametres.dwFlags = 0;

	dwResult= WlanConnect(hClient, &pIfInfo->InterfaceGuid, &pIparametres, 0);
	if (dwResult == ERROR_SUCCESS) {
		cout << "Connection done " << endl;
	}
	else {
		wprintf(L"WlanConnect failed with error: %u\n", dwResult);
		exit(1);
	}

}

void searchAvailableWIFI(HANDLE &hClient, PWLAN_INTERFACE_INFO_LIST &pIfList, PWLAN_AVAILABLE_NETWORK_LIST &pInetworkList)
 {
	DWORD dwMaxClient = 2;   
	DWORD dwCurVersion = 0;
	DWORD dwResult = 0;
	DWORD dwFlags = 0;
	DWORD dwGrantedAccess = 0;

	dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);
	if (dwResult != ERROR_SUCCESS) {
		wprintf(L"WlanOpenHandle failed with error: %u\n", dwResult);
		exit(1);
	}

	dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
	if (dwResult != ERROR_SUCCESS) {
		wprintf(L"WlanEnumInterfaces failed with error: %u\n", dwResult);
		exit(1);
	}

	dwResult = WlanGetAvailableNetworkList(hClient, &pIfList->InterfaceInfo->InterfaceGuid, WLAN_AVAILABLE_NETWORK_INCLUDE_ALL_ADHOC_PROFILES
		, NULL, &pInetworkList);
	if (dwResult != ERROR_SUCCESS) {
		wprintf(L"WlanGetAvailableNetworkList failed with error: %u\n", dwResult);
		exit(1);
	}
 }

void showAvailableWIFI(PWLAN_AVAILABLE_NETWORK_LIST pInetworkList)
 {
	PWLAN_AVAILABLE_NETWORK pInetworkInfo=NULL;
	system("cls");
	for (int i = 0; i < (int)pInetworkList->dwNumberOfItems; i++)
			{
				pInetworkInfo = (PWLAN_AVAILABLE_NETWORK)&pInetworkList->Network[i];
				cout << i << ".";
				for (int i = 0; i < pInetworkInfo->dot11Ssid.uSSIDLength; i++)
				{
					wprintf(L"%c", pInetworkInfo->dot11Ssid.ucSSID[i]);
				}
				wprintf(L"\n");
			}
 }

void disconnectWIFI(HANDLE hClient, PWLAN_INTERFACE_INFO pIfInfo)
 {
	DWORD dwResult = 0;

	dwResult=WlanDisconnect(hClient, &pIfInfo->InterfaceGuid, NULL);
	if (dwResult == ERROR_SUCCESS) {
		cout << "Disconnect done " << endl;
	}
	else {
		wprintf(L"WlanGetAvailableNetworkList failed with error: %u\n", dwResult);
		exit(1);
	}
 }

int wmain()
{
	char exitSymbol;
	do {
		HANDLE hClient = NULL;
		DWORD dwMaxClient = 2;
		DWORD dwCurVersion = 0;
		DWORD dwResult = 0;
		DWORD dwFlags = 0;
		PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
		PWLAN_INTERFACE_INFO pIfInfo = NULL;
		PWLAN_AVAILABLE_NETWORK_LIST pInetworkList = NULL;
		PWLAN_AVAILABLE_NETWORK pInetworkInfo = NULL;
		int chSwitch = 0;
		//system("cls");
		searchAvailableWIFI(hClient, pIfList, pInetworkList);
		pIfInfo = (WLAN_INTERFACE_INFO *)&pIfList->InterfaceInfo;
		pInetworkInfo = (PWLAN_AVAILABLE_NETWORK)&pInetworkList->Network[0];

		cout << "STATUS:" << endl;
		if (checkConnection(pIfInfo) == true)
		{
			cout << "Connected to:";
			for (int i = 0; i < pInetworkInfo->dot11Ssid.uSSIDLength; i++)
			{
				wprintf(L"%c", pInetworkInfo->dot11Ssid.ucSSID[i]);
			}
			cout << endl;
		}
		else cout << "No connection" << endl;

		cout << "Select an action:" << endl
			<< "1.Connect to WIFI" << endl
			<< "2.Refresh list" << endl
			<< "3.Disconnect " << endl
			<< "Your choice:"  ;
		cin >> chSwitch;
		switch (chSwitch)
		{
		case 1:
			showAvailableWIFI(pInetworkList);
			connectingToWIFI(hClient, pInetworkList, pIfInfo);
			break;
		case 2:
			searchAvailableWIFI(hClient, pIfList, pInetworkList);
			showAvailableWIFI(pInetworkList);
			break;
		case 3: 
			disconnectWIFI(hClient, pIfInfo);
			break;
		default:
			break;
		}

		if (pIfList != NULL) {
			WlanFreeMemory(pIfList);
			pIfList = NULL;
		}
		if (pInetworkList != NULL) {
			WlanFreeMemory(pInetworkList);
			pInetworkList = NULL;
		}
		WlanCloseHandle(hClient, NULL);
		cout << "press 'q' to exit" << endl;
		cin >> exitSymbol;
	} while (exitSymbol !='q');

	fflush(stdin);
	getchar();

    return 0;
}