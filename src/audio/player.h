#ifndef PLAYER_H
#define PLAYER_H

#include <string>

class Player {
public:
	typedef size_t SourceId;
	typedef size_t BufferId;

	Player();
	~Player();
	
	Player(Player&) = delete;
	Player& operator=(const Player&) = delete;

	BufferId addBuffer(const std::string& file);
	SourceId addSource();
	
	void delBuffer(BufferId);
	void delSource(SourceId);
	
	void queue(SourceId, BufferId);
	
	void play(SourceId);
	void pause(SourceId);
	void stop(SourceId);
	
	bool isPlaying(SourceId);
	bool isPaused(SourceId);
	bool isStopped(SourceId);
	
	void looping(SourceId, bool);
	
private:
	
	struct PlayerImpl;
	struct PlayerImpl* m_impl;
};

#endif