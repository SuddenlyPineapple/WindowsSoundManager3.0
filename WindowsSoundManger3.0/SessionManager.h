#pragma once
#include "stdafx.h"
#include "globFunctions.h"
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <iostream>
#include <iomanip>
#include <Tchar.h>
#include <Audiopolicy.h>
#include <iomanip>
#include <cstddef>
#include <Psapi.h>
#include <WinBase.h>
#include <vector>
#include "SessionControler.h"

#define SAFE_RELEASE(x) if(x) { x->Release(); x = NULL; } 

class SessionManager : public IAudioSessionNotification {
	LONG _cRef;

	HRESULT hr;
	IAudioSessionManager2* pSessionManager = NULL;
	int cbSessionCount = 0;
	std::vector<SessionControler*> sessionControls;

public:
	SessionManager():_cRef(1)
	{	
		IMMDevice* pDevice = NULL;
		IMMDeviceEnumerator* pEnumerator = NULL;
		
		// Create the device enumerator.
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);

		// Get the default audio device.
		hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
		SAFE_RELEASE(pEnumerator);
		
		// Get the session manager.
		hr = pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (void**)&pSessionManager);
		SAFE_RELEASE(pDevice);

		pSessionManager->RegisterSessionNotification(this);
	}

	~SessionManager() {
		pSessionManager->UnregisterSessionNotification(this);
		SAFE_RELEASE(pSessionManager);
		CoUninitialize();
	}
private:
	// IUnknown
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvInterface)
	{
		if (IID_IUnknown == riid)
		{
			AddRef();
			*ppvInterface = (IUnknown*)this;
		}
		else if (__uuidof(IAudioSessionNotification) == riid)
		{
			AddRef();
			*ppvInterface = (IAudioSessionNotification*)this;
		}
		else
		{
			*ppvInterface = NULL;
			return E_NOINTERFACE;
		}
		return S_OK;
	}

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

	STDMETHODIMP OnSessionCreated(IAudioSessionControl *pNewSession)
	{
		if (pNewSession)
		{
			//gotoxy(0,20);
			//printf("%c[2K", 27);
			//gotoxy(0, 20);
			//std::cout << "New session created!";
			SessionControler *control = new SessionControler(pNewSession, sessionControls.size());
			sessionControls.emplace_back(control);
			sessionCount++;
			cbSessionCount;
			Update();
		}
		return S_OK;
	}

public:
	void Initialize() {

		IAudioSessionEnumerator* pSessionList = NULL;

		hr = pSessionManager->GetSessionEnumerator(&pSessionList);

		// Get the session count.
		hr = pSessionList->GetCount(&cbSessionCount);

		sessionCount = cbSessionCount + 1;

		for (int index = 0; index < cbSessionCount; index++)
		{
			IAudioSessionControl* pSessionControl = NULL;

			// Get the <n>th session.
			hr = pSessionList->GetSession(index, &pSessionControl);
			SessionControler *control = new SessionControler(pSessionControl, index);
			sessionControls.emplace_back(control);
		}

		gotoxy(5, sessionCount);
		std::cout << "Quit";
	}

	void Update() {

		int i = 0;
		for (auto it = sessionControls.begin(); it != sessionControls.end();) {
			if (!(*it)->getState() || (*it)->updateName() == L"") {
				it = sessionControls.erase(it);
			}
			else {
				(*it)->setSessionId(i++);
				++it;
			}
		}

		sessionCount = sessionControls.size()+1;

		
		for (SessionControler* control : sessionControls) {
			control->Update();
		}
			
		
		gotoxy(5, sessionCount);
		std::cout << "Quit";
	}

	void Uninitialize() {
		if (!sessionControls.empty()) {
			for (SessionControler* control : sessionControls) {
				control->~SessionControler();
			}
			sessionControls.clear();
		}
	}

	int getSessionCount() {
		IAudioSessionEnumerator* pSessionList = NULL;

		hr = pSessionManager->GetSessionEnumerator(&pSessionList);

		// Get the session count.
		hr = pSessionList->GetCount(&cbSessionCount);
		sessionCount = cbSessionCount + 1;

		return cbSessionCount;
	}

	void changeVolumeLevel(int id, float vLevel) {
		sessionControls[id]->setVolumeLevel(vLevel);
	}

	float getVolumeLevel(int id) {
		return sessionControls[id]->getVolumeLevel();
	}

	void switchMute(int id) {
		return sessionControls[id]->switchMute();
	}
};