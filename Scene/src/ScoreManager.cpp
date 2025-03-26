#include "ScoreManager.h"
#include <iostream>

ScoreManager::ScoreManager(const std::string& saveFilePath)
    : m_currentScore(0)
    , m_saveFilePath(saveFilePath)
{
    loadHighScores();
}

ScoreManager::~ScoreManager()
{
    saveHighScores();
}

void ScoreManager::resetCurrentScore()
{
    m_currentScore = 0;
}

void ScoreManager::addPoints(int points)
{
    m_currentScore += points;
}

int ScoreManager::getCurrentScore() const
{
    return m_currentScore;
}

void ScoreManager::addHighScore(const std::string& playerName, int score)
{
    m_highScores.push_back(HighScore(playerName, score));
    std::sort(m_highScores.begin(), m_highScores.end());

    if (m_highScores.size() > MAX_HIGH_SCORES)
    {
        m_highScores.resize(MAX_HIGH_SCORES);
    }

    saveHighScores();
}

void ScoreManager::addHighScore(const std::string& playerName)
{
    addHighScore(playerName, m_currentScore);
}

const std::vector<HighScore>& ScoreManager::getHighScores() const
{
    return m_highScores;
}

bool ScoreManager::isHighScore(int score) const
{
    if (m_highScores.size() < MAX_HIGH_SCORES)
    {
        return true;
    }

    return score > m_highScores.back().score;
}

void ScoreManager::saveHighScores()
{
    std::ofstream file(m_saveFilePath, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Could not open file for saving: " << m_saveFilePath << std::endl;
        return;
    }

    size_t size = m_highScores.size();
    file.write(reinterpret_cast<char*>(&size), sizeof(size));

    for (const auto& score : m_highScores)
    {
        size_t nameLength = score.playerName.length();
        file.write(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
        file.write(score.playerName.c_str(), nameLength);
        file.write(reinterpret_cast<const char*>(&score.score), sizeof(score.score));
    }
}

void ScoreManager::loadHighScores()
{
    std::ifstream file(m_saveFilePath, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Could not open file for loading: " << m_saveFilePath << std::endl;
        return;
    }

    m_highScores.clear();

    size_t size;
    file.read(reinterpret_cast<char*>(&size), sizeof(size));

    for (size_t i = 0; i < size; ++i)
    {
        size_t nameLength;
        file.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));

        std::string name(nameLength, ' ');
        file.read(&name[0], nameLength);

        int score;
        file.read(reinterpret_cast<char*>(&score), sizeof(score));

        m_highScores.push_back(HighScore(name, score));
    }
}