#include "Audio/AudioTestContext.h"
#include <Enigma/Core/Process/Application.h>
#include <Enigma/Core/Event/WindowEvent.h>
#include <Enigma/Engine/InputCodes.h>
#include <imgui.h>

//#include <AL/al.h>
//#include <AL/alc.h>
//
//#include <fstream>
//#include <EnigmaSerialization/Utils.h>
//
//ALCdevice* OpenALDevice;
//ALCcontext* OpenALContext;
//uint32_t OpenALBuffer;
//uint32_t OpenALSource;

//enum class WavFormatCode {
//	PCM        = 0x0001,
//	IEEE_FLOAT = 0x0003,
//	ALAW       = 0x0006,
//	MULAW      = 0x0007,
//	EXTENSIBLE = 0xFFFE,
//};
//
//struct WavTableHeader {
//	std::string chunkID;
//	uint32_t    size;
//
//	static WavTableHeader Load(std::ifstream& file) {
//		WavTableHeader header;
//
//		header.chunkID.resize(4);
//		file.read(header.chunkID.data(), 4);
//		header.size = Serialization::ReadU32(file);
//
//		return header;
//	}
//};
//
//struct WavFormat {
//	uint16_t formatTag;
//	uint16_t channels;           // Number of interleaved channels
//	uint32_t samplesPerSec;      // Sampling rate ( blocks per second )
//	uint32_t averageBytesPerSec; // Data rate
//	uint16_t blockAlign;         // Data block size in bytes
//	uint16_t bitsPerSample;	     
//	uint16_t extensionSize;      // Size of extension either 0 or 22
//	uint16_t validBitsPerSample; // Number of valid bits
//	uint32_t channelMask;        // I have no idea what this does
//	// SubFormat; or something idk
//
//	static WavFormat Load(std::ifstream& file, const WavTableHeader& header) {
//		WavFormat format;
//
//		// Load the data
//		using namespace Serialization;
//		format.formatTag          = ReadU16(file);
//		format.channels           = ReadU16(file);
//		format.samplesPerSec      = ReadU32(file);
//		format.averageBytesPerSec = ReadU32(file);
//		format.blockAlign         = ReadU16(file);
//		format.bitsPerSample      = ReadU16(file);
//		if (header.size > 16) {
//			format.extensionSize      = ReadU16(file);
//			format.validBitsPerSample = ReadU16(file);
//			format.channelMask        = ReadU32(file);
//		}
//
//		// Print data to console
//		std::stringstream ss;
//		ss << "Wav Format Table:" << std::endl;
//		ss << "\tFormat Tag:            " << format.formatTag          << std::endl;
//		ss << "\tChannels:              " << format.channels           << std::endl;
//		ss << "\tSamples Per Sec:       " << format.samplesPerSec      << std::endl;
//		ss << "\tAverage Bytes Per Sec: " << format.averageBytesPerSec << std::endl;
//		ss << "\tBlock Align:           " << format.blockAlign         << std::endl;
//		ss << "\tBits Per Sample:       " << format.bitsPerSample      << std::endl;
//		if (header.size > 16) {
//			ss << "\tExtension Size:        " << format.extensionSize << std::endl;
//			ss << "\tChannel Mask:          " << format.channelMask << std::endl;
//			ss << "\tValid Bits Per Sample: " << format.validBitsPerSample << std::endl;
//		}
//		std::cout << ss.str();
//
//		return format;
//	}
//};
//
//bool LoadWavFile(const std::string& filename, ALenum& format, void*& data, ALsizei& size, ALsizei& frequency) {
//
//	// Open the file
//	std::ifstream file;
//	file.open(filename, std::ios::binary);
//	LOG_ASSERT(!file.is_open(), "Failed to open file ( %s )", filename.c_str());
//
//	WavTableHeader wavHeader = WavTableHeader::Load(file);
//	LOG_ASSERT(wavHeader.chunkID != "RIFF", "Invalid wav file ( %s )", filename.c_str());
//	Serialization::ReadU32(file);
//
//	std::cout << "Wav Header:" << std::endl;
//	std::cout << "\tChunk ID:   " << wavHeader.chunkID << std::endl;
//	std::cout << "\tChunk Size: " << wavHeader.size    << std::endl;
//
//	WavTableHeader formatTableHeader = WavTableHeader::Load(file);
//	WavFormat formatTable = WavFormat::Load(file, formatTableHeader);
//
//	WavTableHeader dataChunk = WavTableHeader::Load(file);
//	std::cout << "Data Chunk:" << std::endl;
//	std::cout << "\tChunk ID:   " << dataChunk.chunkID << std::endl;
//	std::cout << "\tChunk Size: " << dataChunk.size    << std::endl;
//
//	size = dataChunk.size;
//	data = malloc(dataChunk.size);
//	file.read(reinterpret_cast<char*>(data), dataChunk.size);
//
//	format = AL_FORMAT_STEREO16;
//	//format = AL_FORMAT_MONO16;
//	frequency = formatTable.samplesPerSec;
//
//	file.close();
//	return true;
//}
//
//void CheckDeviceNames() {
//	// This extension is used for finding all of the users capture/output devices
//	if (!alcIsExtensionPresent(nullptr, "ALC_ENUMERATION_EXT")) return;
//
//	// Get available and default device names
//	std::string availableDevices = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);
//	std::string defaultDevice = alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);
//
//	// Log the info
//	LOG_MESSAGE("OpenAL extension \"ALC_ENUMERATION_EXT\" is available", 5);
//	LOG_MESSAGE("Available Devices: %s", 5, availableDevices.c_str());
//	LOG_MESSAGE("Default Device: %s", 5, defaultDevice.c_str());
//}
//
//void InitOpenAL() {
//	
//	CheckDeviceNames();
//	
//	// Open a OpenAL device
//	OpenALDevice = alcOpenDevice(nullptr);
//	LOG_ASSERT(OpenALDevice == nullptr, "Failed to open OpenAL device");
//
//	// Create OpenAL context
//	ALCint attribs[] = { 0 };
//	OpenALContext = alcCreateContext(OpenALDevice, attribs);
//	alcMakeContextCurrent(OpenALContext);
//
//	// This extension may not be needed because OpenAL seems to have not been updated sense 2009
//	if (alIsExtensionPresent("EAX2.0")) {
//		LOG_MESSAGE("OpenAL extension \"EAX\" is available", 5);
//	}
//
//	// Clear the error code
//	ALenum error = alGetError(); 
//
//	// Generate buffers
//	alGenBuffers(1, &OpenALBuffer);
//	if (error = alGetError()) {
//		LOG_ERROR("Failed to create OpenAL buffer: %u", error);
//	}
//
//	// Load wav file
//	ALenum format;
//	ALsizei size;
//	ALsizei frequency;
//	void* data;
//	LoadWavFile("assets/audio/OverTheRainbow.wav", format, data, size, frequency);
//
//	// Copy wav file data into the OpenAL buffer
//	alBufferData(OpenALBuffer, format, data, size, frequency);
//	if (error = alGetError()) {
//		alDeleteBuffers(1, &OpenALBuffer);
//		LOG_ERROR("Failed to copy wav file to OpenAL buffer: %u", error);
//	}
//	free(data);
//
//	// Create a OpenAL sound source
//	alGenSources(1, &OpenALSource);
//	if (error = alGetError()) {
//		alDeleteBuffers(1, &OpenALBuffer);
//		LOG_ERROR("Failed to create OpenAL sound source: %u", error);
//	}
//
//	// Attach buffer to sound source
//	alSourcei(OpenALSource, AL_BUFFER, OpenALBuffer);
//	if (error = alGetError()) {
//		LOG_ERROR("Failed to attach OpenAL buffer to OpenAL sound source: %u", error);
//	}
//	alSourcei(OpenALSource, AL_SOURCE_RELATIVE, false);
//	alDistanceModel(AL_NONE);
//	alSourcePlay(OpenALSource);
//
//	struct orientation {
//		glm::vec3 at;
//		glm::vec3 up;
//	};
//	orientation ori = { {0,0,0}, {0,1,0} };
//	glm::vec3 position = { 0,0,0 };
//	glm::vec3 velocity = { 0,0,0 };
//
//	alListenerfv(AL_POSITION, &position.x);
//	alListenerfv(AL_VELOCITY, &velocity.x);
//	alListenerfv(AL_ORIENTATION, &ori.at.x);
//
//	ALint channels;
//	alGetBufferi(OpenALBuffer, AL_CHANNELS, &channels);
//	LOG_MESSAGE("Number of channels: %i", 5, channels);
//
//}
//
//void OpenALImGui() {
//	if (ImGui::Button("Play")) {
//		alSourcePlay(OpenALSource);
//	}
//	if (ImGui::Button("Pause")) {
//		alSourcePause(OpenALSource);
//	}
//	if (ImGui::Button("Stop")) {
//		alSourceStop(OpenALSource);
//	}
//	ALint state;
//	alGetSourcei(OpenALSource, AL_SOURCE_STATE, &state);
//	if (state == AL_PLAYING) {
//		ImGui::Text("Playing");
//	}
//	else if (state == AL_PAUSED) {
//		ImGui::Text("Paused");
//	}
//	else if (state == AL_STOPPED) {
//		ImGui::Text("Stopped");
//	}
//
//}
//
//void ShutdownOpenAL() {
//	alcMakeContextCurrent(nullptr);
//	alcDestroyContext(OpenALContext);
//	alcCloseDevice(OpenALDevice);
//}

AudioTestContext::AudioTestContext(Core::ID windowID) : TestContext(windowID)
{
	LOG_MESSAGE("Started Font Test Context", 5);

	// Setup surface
	ref<Core::Window> window = Core::Application::GetWindow(m_WindowID);
	m_Surface.scale.x = window->GetWidth();
	m_Surface.scale.y = window->GetHeight();

	// Setup renderer
	Renderer::Render2DConfig renderConfig;
	renderConfig.surface = m_Surface;
	renderConfig.renderAPI = window->GetAPI();
	m_RenderContext = new Renderer::Render2D(renderConfig);

	// Create camera
	Renderer::ViewBox viewBox = Renderer::ViewBox::SurfaceViewBox(m_Surface);
	m_Camera = Renderer::OrthographicCamera::Create(viewBox, 3, {1.5,1.5,0});

	// Create input context
	Engine::InputConfig inputConfig;
	inputConfig.surface = ref<Engine::Surface>(&m_Surface);
	inputConfig.window = window;
	m_InputContext = Engine::Input::Create(inputConfig);

	//InitOpenAL();
}

AudioTestContext::~AudioTestContext()
{
	//ShutdownOpenAL();
}

void AudioTestContext::OnResize(int width, int height)
{
	m_RenderContext->Resize(width, height);
	m_Surface.Resize(width, height);
}
void AudioTestContext::OnClose()
{
	
}
void AudioTestContext::OnEvent(Core::Event& e)
{
	Core::EventHandler handler(e);
	handler.Dispatch<Core::WindowResize>([&](Core::WindowResize& e) { OnResize(e.GetWidth(), e.GetHeight()); return false; });
	handler.Dispatch<Core::WindowClose>([&](Core::WindowClose& e) { OnClose(); return false; });
}

void AudioTestContext::Update(Engine::DeltaTime deltaTime)
{
	
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

	//OpenALImGui();

	ImGui::End();
}