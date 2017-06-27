#include "player.h"

#include <iostream>
#include <vector>
#include <set>

// OpenAL headers
#include <al.h>
#include <alc.h>

//#define STB_VORBIS_HEADER_ONLY
#include "lib/stb_vorbis.c"

#define DR_WAV_IMPLEMENTATION
#include "lib/dr_wav.h"

#include <stdexcept>

#define AL_CHECK check_al_error(alGetError(), __FILE__, __LINE__)

void check_al_error(ALenum err, const char* file, int line) {
	switch (err) {
		case AL_NO_ERROR: 
			return;
		case AL_INVALID_NAME: 
			std::cerr << "OpenAL: AL_INVALID_NAME";
			break;
		case AL_INVALID_ENUM: 
			std::cerr << "OpenAL: AL_INVALID_ENUM";
			break;
		case AL_INVALID_VALUE: 
			std::cerr << "OpenAL: AL_INVALID_VALUE";
			break;
		case AL_INVALID_OPERATION: 
			std::cerr << "OpenAL: AL_INVALID_OPERATION";
			break;
		case AL_OUT_OF_MEMORY: 
			std::cerr << "OpenAL: AL_OUT_OF_MEMORY";
			break;
		default:
			std::cerr << "OpenAL: Unknown error";
	}
	std::cerr << ": " << file << ":" << line << std::endl;
	exit(-1);
}

struct Player::PlayerImpl {
	ALCdevice* m_device;
	ALCcontext* m_context;
	std::set<ALuint> m_sources;
	std::set<ALuint> m_buffers;
};

Player::Player() :
	m_impl(new PlayerImpl)
{
	m_impl->m_device = alcOpenDevice(nullptr);
	if (m_impl->m_device == nullptr) {
		std::cerr << "OpenAL: Can't open device\n";
		exit(-1);
	}

	m_impl->m_context = alcCreateContext(m_impl->m_device, nullptr);
	if (m_impl->m_context == nullptr) {
		std::cerr << "OpenAL: Can't create context\n";
		exit(-1);
	}
	
	ALCboolean mcc = alcMakeContextCurrent(m_impl->m_context);
	if (!mcc) {
		std::cerr << "OpenAL: Can't make context current\n";
		exit(-1);
	}
	AL_CHECK;
}

Player::~Player()
{
	for (std::set<ALuint>::iterator it = m_impl->m_sources.begin(); it != m_impl->m_sources.end(); ++it) {
		stop(*it);
		alDeleteSources(1, &(*it)); 
		AL_CHECK;
	}
	m_impl->m_sources.clear();
	
	for (std::set<ALuint>::iterator it = m_impl->m_buffers.begin(); it != m_impl->m_buffers.end(); ++it) {
		alDeleteBuffers(1, &(*it)); 
		AL_CHECK;
	}
	m_impl->m_buffers.clear();
	
	if (!alcMakeContextCurrent(nullptr)) {
		std::cerr << "Error while nulling the context" << std::endl;
		exit(-1);
	}
	alcDestroyContext(m_impl->m_context);
	alcCloseDevice(m_impl->m_device);
	
	delete m_impl;
}

Player::BufferId Player::addBuffer(const std::string& file)
{
	size_t pos = file.find_last_of('.');
	if (pos == std::string::npos) {
		std::cerr << "Wrong file extension" << std::endl;
		exit(-1);
	}
	
	ALenum format;
	ALvoid *data = nullptr;
	ALsizei size = 0;
	ALsizei freq = 0;
	
	std::string extension = file.substr(pos + 1);
	if (extension == "wav") {
		dr_uint64 sample_count = 0;
		unsigned int sample_rate = 0;
		unsigned int channels = 0;
		dr_int16* output = drwav_open_and_read_file_s16(file.c_str(), &channels, &sample_rate, &sample_count);
		if (output == NULL) {
			std::cerr << "Can't load WAV audio file\n";
			exit(-1);
		}
		
		format = channels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
		data = output;
		size = sample_count * sizeof(dr_int16);
		freq = sample_rate;
	} else if (extension == "ogg") {
		short int* output = nullptr;
		int sample_rate = 0;
		int channels;
		int data_len = stb_vorbis_decode_filename(file.c_str(), &channels, &sample_rate, &output);
		if (data_len == -1) {
			std::cerr << "Can't load OGG audio file\n";
			exit(-1);
		}
		
		format = channels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
		data = output;
		size = data_len * channels * sizeof(short);
		freq = sample_rate;
	} else {
		std::cerr << "Wrong file extension" << std::endl;
		exit(-1);
	}
	
	ALuint buffer;
	alGenBuffers(1, &buffer);
	AL_CHECK;
	
	if (!m_impl->m_buffers.insert(buffer).second) {
		std::cerr << "Already present in set" << std::endl;
		exit(-1);
	}

	alBufferData(buffer, format, data, size, freq);
	AL_CHECK;

	if (extension == "wav") {
		free(data);
	} else if (extension == "ogg") {
		free(data);
	}
	
	return buffer;
}

Player::SourceId Player::addSource()
{
	ALuint source;
	alGenSources(1, &source); 
	AL_CHECK;
	
	if (!m_impl->m_sources.insert(source).second) {
		std::cerr << "Already present in set" << std::endl;
		exit(-1);
	}
	
	return source;
}

void Player::delBuffer(Player::BufferId id)
{
	std::set<ALuint>::iterator it = m_impl->m_buffers.find(id);
	if (it == m_impl->m_buffers.end()) {
		throw std::runtime_error("Deleting wrong buffer");
	}
	alDeleteBuffers(1, &(*it)); 
	AL_CHECK;
	m_impl->m_buffers.erase(id);
}

void Player::delSource(Player::SourceId id)
{
	std::set<ALuint>::iterator it = m_impl->m_sources.find(id);
	if (it == m_impl->m_sources.end()) {
		throw std::runtime_error("Deleting wrong source");
	}
	alDeleteSources(1, &(*it)); 
	AL_CHECK;
	m_impl->m_sources.erase(id);
}

void Player::queue(Player::SourceId sid, Player::BufferId bid)
{
	std::set<ALuint>::iterator sit = m_impl->m_sources.find(sid);
	std::set<ALuint>::iterator bit = m_impl->m_buffers.find(bid);
	if (sit == m_impl->m_sources.end()) {
		throw std::runtime_error("Queueing wrong source");
	}
	if (bit == m_impl->m_buffers.end()) {
		throw std::runtime_error("Queueing wrong buffer");
	}
	alSourceQueueBuffers(*sit, 1, &(*bit));
	AL_CHECK;
}

void Player::play(Player::SourceId sid)
{
	std::set<ALuint>::iterator sit = m_impl->m_sources.find(sid);
	if (sit == m_impl->m_sources.end()) {
		throw std::runtime_error("Playing wrong source");
	}
	alSourcePlay(*sit);
	AL_CHECK;
}

void Player::pause(Player::SourceId sid)
{
	std::set<ALuint>::iterator sit = m_impl->m_sources.find(sid);
	if (sit == m_impl->m_sources.end()) {
		throw std::runtime_error("Pausing wrong source");
	}
	alSourcePause(*sit);
	AL_CHECK;
}

void Player::stop(Player::SourceId sid)
{
	std::set<ALuint>::iterator sit = m_impl->m_sources.find(sid);
	if (sit == m_impl->m_sources.end()) {
		throw std::runtime_error("Stopping wrong source");
	}
	alSourceStop(*sit);
	AL_CHECK;
}

void Player::looping(Player::SourceId sid, bool active)
{
	std::set<ALuint>::iterator sit = m_impl->m_sources.find(sid);
	if (sit == m_impl->m_sources.end()) {
		throw std::runtime_error("Looping wrong source");
	}
	ALint source_state = 0;
	alSourcei(*sit, AL_LOOPING, active ? 1 : 0);
	AL_CHECK;
}

bool Player::isPlaying(Player::SourceId sid)
{
	std::set<ALuint>::iterator sit = m_impl->m_sources.find(sid);
	if (sit == m_impl->m_sources.end()) {
		throw std::runtime_error("Checking playing wrong source");
	}
	ALint source_state = 0;
	alGetSourcei(*sit, AL_SOURCE_STATE, &source_state);
	bool res = source_state == AL_PLAYING;
	AL_CHECK;
	return res;
}

bool Player::isPaused(Player::SourceId sid)
{
	std::set<ALuint>::iterator sit = m_impl->m_sources.find(sid);
	if (sit == m_impl->m_sources.end()) {
		throw std::runtime_error("Checking paused wrong source");
	}
	ALint source_state = 0;
	alGetSourcei(*sit, AL_SOURCE_STATE, &source_state);
	bool res = source_state == AL_PAUSED;
	AL_CHECK;
	return res;
}

bool Player::isStopped(Player::SourceId sid)
{
	std::set<ALuint>::iterator sit = m_impl->m_sources.find(sid);
	if (sit == m_impl->m_sources.end()) {
		throw std::runtime_error("Checking stopped wrong source");
	}
	ALint source_state = 0;
	alGetSourcei(*sit, AL_SOURCE_STATE, &source_state);
	bool res = source_state == AL_STOPPED;
	AL_CHECK;
	return res;
}
