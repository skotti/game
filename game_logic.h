#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

class GameObject;

class GameLogic {
public:
	
	virtual void logic() = 0;
	virtual ~GameLogic() {}
	
	void setGameObject(GameObject* game_object) {
		m_game_object = game_object;
	}
protected:
	GameObject* m_game_object = nullptr;
};

class EmptyLogic : public GameLogic {
public:
	virtual void logic() {}
	virtual ~EmptyLogic() {}
};

class MazeBlockLogic : public GameLogic {
public:
	MazeBlockLogic(int i, int j) :
		m_i(i),
		m_j(j)
	{

	}
	
	virtual void logic();
	
	virtual ~MazeBlockLogic() {}
private:
	
	int m_i = 0, m_j = 0;
};

#endif