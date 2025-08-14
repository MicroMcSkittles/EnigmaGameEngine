#include "Audio/AudioTestContext.h"
#include <Enigma/Core/Process/Application.h>
#include <Enigma/Core/Event/WindowEvent.h>
#include <Enigma/Engine/InputCodes.h>
#include <imgui.h>

#ifdef PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <mmsystem.h>

#define PI     3.14159
#define TWO_PI PI * PI

HWAVEOUT waveOut;
WAVEHDR header[2] = { 0 };
uint16_t chunks[2][2000];
bool chunk_swap = false;
float frequency = 220;
float wave_position = 0;
float wave_step = 0;

void CALLBACK WaveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
	switch (uMsg)
	{
	//case WOM_CLOSE: LOG_MESSAGE("WOM_CLOSE", 5); break;
	//case WOM_OPEN:  LOG_MESSAGE("WOM_OPEN", 5); break;
	case WOM_DONE: {
		//LOG_MESSAGE("WOM_DONE", 5);
		for (int i = 0; i < 2000; ++i) {
			chunks[chunk_swap][i] = sin(wave_position) * 32767;
			wave_position += wave_step;
		}
		if (waveOutWrite(waveOut, &header[chunk_swap], sizeof(header[chunk_swap])) != MMSYSERR_NOERROR) {
			LOG_ERROR("Failed to write to wave");
		}
		chunk_swap = !chunk_swap;
	} break;
	}
}

AudioTestContext::AudioTestContext(Core::ID windowID) : TestContext(windowID)
{
	LOG_MESSAGE("Started Font Test Context", 5);

	// Setup surface
	Core::Window* window = Core::Application::GetWindow(m_WindowID);
	m_Surface.scale.x = window->GetWidth();
	m_Surface.scale.y = window->GetHeight();

	// Setup renderer
	Renderer::Render2DConfig renderConfig;
	renderConfig.surface = m_Surface;
	renderConfig.renderAPI = window->GetAPI();
	m_RenderContext = new Renderer::Render2D(renderConfig);

	// Create camera
	Renderer::ViewBox viewBox = Renderer::ViewBox::SurfaceViewBox(m_Surface);
	m_Camera = new Renderer::OrthographicCamera(viewBox, 3, {1.5,1.5,0});

	// Create input context
	Engine::InputConfig inputConfig;
	inputConfig.surface = &m_Surface;
	inputConfig.window = window;
	m_InputContext = Engine::Input::Create(inputConfig);

	// Windows waveout stuff
	WAVEFORMATEX waveFormat = { };
	waveFormat.wFormatTag      = WAVE_FORMAT_PCM;
	waveFormat.nChannels       = 1;
	waveFormat.nSamplesPerSec  = 44100;
	waveFormat.wBitsPerSample  = 16;
	waveFormat.cbSize          = 0;
	waveFormat.nBlockAlign     = waveFormat.nChannels * waveFormat.wBitsPerSample / 8;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;

	MMRESULT waveOpenResult = waveOutOpen(&waveOut, WAVE_MAPPER, &waveFormat, (DWORD_PTR)WaveOutProc, (DWORD_PTR)NULL, CALLBACK_FUNCTION);
	switch (waveOpenResult)
	{
	case MMSYSERR_ALLOCATED:   LOG_ERROR("Failed to open output wave ( Specified resource is already allocated )"); break;
	case MMSYSERR_BADDEVICEID: LOG_ERROR("Failed to open output wave ( Specified device identifier is out of range )"); break;
	case MMSYSERR_NODRIVER:    LOG_ERROR("Failed to open output wave ( No device driver is present )"); break;
	case MMSYSERR_NOMEM:       LOG_ERROR("Failed to open output wave ( Unable to allocate or lock memory )"); break;
	case WAVERR_BADFORMAT:     LOG_ERROR("Failed to open output wave ( Attempted to open with an unsupported waveform-audio format )"); break;
	case WAVERR_SYNC:          LOG_ERROR("Failed to open output wave ( The device is synchronous but waveOutOpen was called without using the WAVE_ALLOWSYNC flag )"); break;
	}

	MMRESULT volumeResult = waveOutSetVolume(waveOut, 0xFFFF);
	switch (volumeResult)
	{
	case MMSYSERR_INVALHANDLE:  LOG_ERROR("Failed to set wave out volume ( Specified device handle is invalid )"); break;
	case MMSYSERR_NODRIVER:     LOG_ERROR("Failed to set wave out volume ( No device driver is present )"); break;
	case MMSYSERR_NOMEM:        LOG_ERROR("Failed to set wave out volume ( Unable to allocate or lock memory )"); break;
	case MMSYSERR_NOTSUPPORTED: LOG_ERROR("Failed to set wave out volume ( Function is not supported )"); break;
	}

	wave_step = TWO_PI / (44100.f / frequency);
	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 2000; ++j) {
			chunks[i][j] = sin(wave_position) * 32767;
			wave_position += wave_step;
		}
		header[i].lpData = (CHAR*)chunks[i];
		header[i].dwBufferLength = 4000;
		if (waveOutPrepareHeader(waveOut, &header[i], sizeof(header[i])) != MMSYSERR_NOERROR) {
			LOG_ERROR("Failed to prepare wave header ( %d )", i);
		}
		if (waveOutWrite(waveOut, &header[i], sizeof(header[i])) != MMSYSERR_NOERROR) {
			LOG_ERROR("Failed to write to wave ( %d )", i);
		}
	}
}

void AudioTestContext::OnResize(int width, int height)
{
	m_RenderContext->Resize(width, height);
	m_Surface.Resize(width, height);
}
void AudioTestContext::OnClose()
{
	waveOutClose(waveOut);
}
void AudioTestContext::OnEvent(Core::Event& e)
{
	Core::EventHandler handler(e);
	handler.Dispatch<Core::WindowResize>([&](Core::WindowResize& e) { OnResize(e.GetWidth(), e.GetHeight()); return false; });
	handler.Dispatch<Core::WindowClose>([&](Core::WindowClose& e) { OnClose(); return false; });
}

void AudioTestContext::Update(Engine::DeltaTime deltaTime)
{
	Engine::Input::MakeCurrent(m_InputContext);
	if (Engine::Input::IsKeyPressed(Engine::KeyCode::KeyUp)) {
		frequency += 50;
		wave_step = TWO_PI / (44100.f / frequency);
	}
	if (Engine::Input::IsKeyPressed(Engine::KeyCode::KeyDown)) {
		frequency -= 50;
		wave_step = TWO_PI / (44100.f / frequency);
	}
}

void AudioTestContext::Render()
{
	m_RenderContext->StartFrame(m_Camera);

	m_RenderContext->EndFrame();
}

void AudioTestContext::ImGui()
{
	ImGui::Begin("Debug Menu");
	ImGui::Text("Audio Test");
	ImGui::TextWrapped("Playing Audio Demo");
	ImGui::Text("Frequency: %f", frequency);
	ImGui::End();
}

#elif PLATFORM_LINUX

AudioTestContext::AudioTestContext(Core::ID windowID) : TestContext(windowID) { }
void AudioTestContext::OnResize(int width, int height) { }
void AudioTestContext::OnClose() { }
void AudioTestContext::OnEvent(Core::Event& e) { }
void AudioTestContext::Update(Engine::DeltaTime deltaTime) { }
void AudioTestContext::Render() { }
void AudioTestContext::ImGui() { }

#endif