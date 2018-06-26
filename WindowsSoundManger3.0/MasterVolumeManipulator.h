#pragma once

#include "stdafx.h"
#include "globFunctions.h"
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <iostream>
#include <iomanip>
#include <Tchar.h>
#include <iomanip>

#define SAFE_RELEASE(x) if(x) { x->Release(); x = NULL; } 

class MasterVolumeManipulator : public IAudioEndpointVolumeCallback
{
	LONG _cRef;

	HRESULT hr;
	IAudioEndpointVolume *endpointVolume = NULL;

public:
	MasterVolumeManipulator() : _cRef(1) {
		IMMDeviceEnumerator *deviceEnumerator = NULL;
		IMMDevice *defaultDevice = NULL;

		CoInitialize(NULL);
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
		
		hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
		SAFE_RELEASE(deviceEnumerator);

		hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);

		hr = endpointVolume->RegisterControlChangeNotify(this);
		SAFE_RELEASE(defaultDevice);

		updateMasterVolumeLevel();
	};

	~MasterVolumeManipulator()
	{
		endpointVolume->UnregisterControlChangeNotify(this);
		SAFE_RELEASE(endpointVolume);
		this->Release();

		CoUninitialize();
	};

	//-----------------------------------------------------------
	// Client implementation of IAudioEndpointVolumeCallback
	// interface. When a method in the IAudioEndpointVolume
	// interface changes the volume level or muting state of the
	// endpoint device, the change initiates a call to the
	// client's IAudioEndpointVolumeCallback::OnNotify method.
	//---------------------------------------------------------
	
	// IUnknown methods -- 
	
private:
	//AddRef -->  Increments the reference count for an interface on an object. This method should be called for every new copy of a pointer to an interface on an object.
	ULONG STDMETHODCALLTYPE AddRef()
	{
		return InterlockedIncrement(&_cRef);
	}

	//Release --> Decrements the reference count for an interface on an object. When it reach 0 it relese the object interface.
	ULONG STDMETHODCALLTYPE Release()
	{
		ULONG ulRef = InterlockedDecrement(&_cRef);
		if (0 == ulRef){
			delete this;
		}
		return ulRef;
	}

	//QueryInterface --> Retrieves pointers to the supported interfaces on an object. This method calls IUnknown::AddRef on the pointer it returns.
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, VOID **ppvInterface)
	{
		if (IID_IUnknown == riid || __uuidof(IAudioEndpointVolumeCallback) == riid)
		{
			AddRef();
			*ppvInterface = static_cast<IUnknown*>(this);
			return S_OK;
		}
		*ppvInterface = NULL;
		return E_NOINTERFACE;
	}

	//OnNotify -> Callback method for endpoint-volume-change notifications.  The OnNotify method in the class definition is called each time one of the following methods changes the endpoint volume level:
	// IAudioEndpointVolume::SetChannelVolumeLevel, IAudioEndpointVolume::SetChannelVolumeLevelScalar, IAudioEndpointVolume::SetMasterVolumeLevel, IAudioEndpointVolume::SetMasterVolumeLevelScalar, IAudioEndpointVolume::SetMute, IAudioEndpointVolume::VolumeStepDown, IAudioEndpointVolume::VolumeStepUp

	STDMETHODIMP OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA NotificationData)
	{
		updateMasterVolumeLevel();
		return S_OK;
	}
public:
	float getMasterVolumeLevel() {
		float volume = 0;
		hr = endpointVolume->GetMasterVolumeLevelScalar(&volume);
		return volume*1000;
	}

	void updateMasterVolumeLevel() {
		gotoxy(5, 0);
		std::cout << "Current master volume level: ";
		setCursorColor(12);

		BOOL mute;
		hr = endpointVolume->GetMute(&mute);
		if (mute)
			std::cout << "MUTE";
		else
			std::cout << getMasterVolumeLevel() << "          ";
		setCursorColor(15);
	}

	void setMasterVolumeLevel(float vLevel) {
		hr = endpointVolume->SetMasterVolumeLevelScalar((vLevel / 1000), NULL);
	}

	void switchMute() {
		BOOL mute;
		hr = endpointVolume->GetMute(&mute);
		if (mute)
			hr = endpointVolume->SetMute(false, NULL);
		else
			hr = endpointVolume->SetMute(true, NULL);
	}
};

