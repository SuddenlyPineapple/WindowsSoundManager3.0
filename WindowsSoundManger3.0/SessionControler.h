#pragma once
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <iostream>
#include <iomanip>
#include <Tchar.h>
#include <string>
#include <Audiopolicy.h>
#include <wchar.h>
#include "globFunctions.h"

#define SAFE_RELEASE(x) if(x) { x->Release(); x = NULL; } 

//-----------------------------------------------------------
// Client implementation of IAudioSessionEvents interface.
// WASAPI calls these methods to notify the application when
// a parameter or property of the audio session changes.
//-----------------------------------------------------------

class SessionControler : public IAudioSessionEvents
{
	LONG _cRef;
	HRESULT hr;
	int sessionId;
	DWORD processId;
	std::wstring sessionName;
	LPWSTR pswSession = NULL;
	LPWSTR pswIcon = NULL;
	IAudioSessionControl* pSessionControl = NULL;
	IAudioSessionControl2* pSessionControl2 = NULL;
	ISimpleAudioVolume *pSessionVolume = NULL;
	float volumeLevel;
	bool state = true;

public:
	SessionControler(IAudioSessionControl* pSessionControl, int sessionId) : pSessionControl(pSessionControl), sessionId(sessionId), _cRef(1) {
		hr = pSessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&pSessionControl2);
		hr = pSessionControl->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&pSessionVolume);

		hr = pSessionControl->GetDisplayName(&pswSession);
		hr = pSessionControl->GetIconPath(&pswIcon);

		hr = pSessionControl->RegisterAudioSessionNotification(this);

		hr = pSessionControl2->GetProcessId(&processId);

		Update();
	}

	~SessionControler() {
		CoTaskMemFree(pswSession);
		CoTaskMemFree(pswIcon);

		hr = pSessionControl->UnregisterAudioSessionNotification(this);

		SAFE_RELEASE(pSessionControl);
		SAFE_RELEASE(pSessionControl2);
		SAFE_RELEASE(pSessionVolume);
		this->Release();
	}

	// IUnknown methods -- AddRef, Release, and QueryInterface
private:
	ULONG STDMETHODCALLTYPE AddRef()
	{
		return InterlockedIncrement(&_cRef);
	}

	ULONG STDMETHODCALLTYPE Release()
	{
		ULONG ulRef = InterlockedDecrement(&_cRef);
		if (0 == ulRef)
		{
			delete this;
		}
		return ulRef;
	}

	HRESULT STDMETHODCALLTYPE QueryInterface(
		REFIID  riid,
		VOID  **ppvInterface)
	{
		if (IID_IUnknown == riid)
		{
			AddRef();
			*ppvInterface = (IUnknown*)this;
		}
		else if (__uuidof(IAudioSessionEvents) == riid)
		{
			AddRef();
			*ppvInterface = (IAudioSessionEvents*)this;
		}
		else
		{
			*ppvInterface = NULL;
			return E_NOINTERFACE;
		}
		return S_OK;
	}

	// Notification methods for audio session events

	HRESULT STDMETHODCALLTYPE OnDisplayNameChanged( LPCWSTR NewDisplayName, LPCGUID EventContext)
	{
		Update();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnIconPathChanged( LPCWSTR NewIconPath, LPCGUID EventContext)
	{
		Update();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnSimpleVolumeChanged( float NewVolume, BOOL NewMute, LPCGUID EventContext)
	{
		Update();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnChannelVolumeChanged( DWORD ChannelCount, float NewChannelVolumeArray[], DWORD ChangedChannel, LPCGUID EventContext)
	{
		Update();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnGroupingParamChanged( LPCGUID NewGroupingParam, LPCGUID EventContext)
	{
		Update();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnStateChanged(AudioSessionState NewState)
	{
		std::string pszState;

		switch (NewState)
		{
		case AudioSessionStateActive:
			pszState = "active";
			state = true;
			break;
		case AudioSessionStateInactive:
			pszState = "inactive";
			state = false;
			sessionDisconnected = true;
			break;
		}
		//gotoxy(0, 20);
		//std::cout << "New session state = " << pszState;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnSessionDisconnected(AudioSessionDisconnectReason DisconnectReason)
	{
		std::string pszReason = "?????";

		switch (DisconnectReason)
		{
		case DisconnectReasonDeviceRemoval:
			pszReason = "device removed";
			break;
		case DisconnectReasonServerShutdown:
			pszReason = "server shut down";
			break;
		case DisconnectReasonFormatChanged:
			pszReason = "format changed";
			break;
		case DisconnectReasonSessionLogoff:
			pszReason = "user logged off";
			break;
		case DisconnectReasonSessionDisconnected:
			pszReason = "session disconnected";
			break;
		case DisconnectReasonExclusiveModeOverride:
			pszReason = "exclusive-mode override";
			break;
		}
		gotoxy(0,20);
		std::cout << "Session disconnected. Reason: " << pszReason;

		state = false;
		sessionDisconnected = true;

		return S_OK;
	}
public:
	void setVolumeLevel(float vLevel) {
		hr = pSessionVolume->SetMasterVolume((vLevel / 1000), NULL);
	}

	float getVolumeLevel() {
		BOOL mute;
		pSessionVolume->GetMute(&mute);
		if (mute) return (float)-1;
		float vLevel = 0;
		pSessionVolume->GetMasterVolume(&vLevel);
		return vLevel * 1000;
	}

	void Update() {
		gotoxy(5, sessionId + 1);

		updateName();

		//std::wcout << sessionId << ") " << sessionName << ": ";
		std::wcout << sessionName << ": ";

		//hr = pSessionVolume->SetMasterVolume((float)0.9, NULL);
		setCursorColor(12);

		BOOL mute;
		hr = pSessionVolume->GetMute(&mute);
		if (mute) {
			std::cout << "MUTE       ";
		}
		else {
			float vLevel;
			hr = pSessionVolume->GetMasterVolume(&vLevel);
			std::cout << vLevel * 1000 << "              ";
		}
		setCursorColor(15);
	}

	bool getState() {
		return state;
	}

	void switchMute() {
		BOOL mute;
		hr = pSessionVolume->GetMute(&mute);
		if (mute)
			hr = pSessionVolume->SetMute(false, NULL);
		else
			hr = pSessionVolume->SetMute(true, NULL);
	}

	void setSessionId(int i) {
		sessionId = i;
	}

	std::wstring updateName() {
		if (GetProcName(processId) == L"[System Process]") {
			hr = pSessionControl->SetDisplayName(L"System Sounds", NULL);
			sessionName = L"System Sounds";
		}
		else {
			hr = pSessionControl->SetDisplayName(GetProcName(processId).c_str(), NULL);
			sessionName = GetProcName(processId).c_str();
			//sessionName = get_process_name(processId);
		}
		return sessionName;
	}

};