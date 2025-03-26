#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

struct HighScore
{
    std::string playerName;
    int score;

    HighScore(const std::string& name = "Player", int value = 0)
        : playerName(name), score(value)
    {
    }

    bool operator<(const HighScore& other) const
    {
        return score > other.score;
    }
};

class ScoreManager
{
public:
    ScoreManager(const std::string& saveFilePath = "highscores.dat");
    ~ScoreManager();

    void resetCurrentScore();
    void addPoints(int points);
    int getCurrentScore() const;

    void addHighScore(const std::string& playerName, int score);
    void addHighScore(const std::string& playerName);
    const std::vector<HighScore>& getHighScores() const;
    bool isHighScore(int score) const;

    void saveHighScores();
    void loadHighScores();

private:
    int m_currentScore;
    std::vector<HighScore> m_highScores;
    std::string m_saveFilePath;
    const int MAX_HIGH_SCORES = 10;
};