#include "ISpawner.h"
#include "GameObject.h"
#include "MegaBoss.h"

ISpawner::ISpawner(IComposite* scene, const size_t& maxEntity) :IComposite(scene), m_Scene(scene), m_maxEntity(maxEntity), m_SpawnZone(sf::Vector2f(0, 0), sf::Vector2f(0, 0)), m_RestrictedArea(sf::Vector2f(0, 0), sf::Vector2f(0, 0))
{
	m_SpawnZone.Pmin = scene->getRoot()->getScene()->getLeftTopCorner();
	m_SpawnZone.Pmax = (scene->getRoot()->getScene()->getRightBotomCorner());

	m_RestrictedArea.Pmin.x = scene->getRoot()->getScene()->GetCenterWindow().x - scene->getRoot()->getScene()->getWindow()->getSize().x / 2;
	m_RestrictedArea.Pmin.y = scene->getRoot()->getScene()->GetCenterWindow().y - scene->getRoot()->getScene()->getWindow()->getSize().y / 2;
	m_RestrictedArea.Pmax.x = scene->getRoot()->getScene()->GetCenterWindow().x + scene->getRoot()->getScene()->getWindow()->getSize().x / 2;
	m_RestrictedArea.Pmax.y = scene->getRoot()->getScene()->GetCenterWindow().y + scene->getRoot()->getScene()->getWindow()->getSize().y / 2;
}

void ISpawner::Render()
{
	IComposite::Render();
}

void ISpawner::Update(const float& deltatime)
{
	m_RestrictedArea.Pmin.x = m_Scene->getRoot()->getScene()->GetCenterWindow().x - m_Scene->getRoot()->getScene()->getWindow()->getSize().x / 2;
	m_RestrictedArea.Pmin.y = m_Scene->getRoot()->getScene()->GetCenterWindow().y - m_Scene->getRoot()->getScene()->getWindow()->getSize().y / 2;
	m_RestrictedArea.Pmax.x = m_Scene->getRoot()->getScene()->GetCenterWindow().x + m_Scene->getRoot()->getScene()->getWindow()->getSize().x / 2;
	m_RestrictedArea.Pmax.y = m_Scene->getRoot()->getScene()->GetCenterWindow().y + m_Scene->getRoot()->getScene()->getWindow()->getSize().y / 2;
	IComposite::Update(deltatime);
}


//BossSpawner::BossSpawner(IComposite* scene, const size_t& maxEntity)
//    : ISpawner(scene, std::min(maxEntity, static_cast<size_t>(5)))
//    , m_useFixedPosition(false)
//    , m_fixedSpawnPosition(0, 0)
//    , m_bossHealth(3.0f)
//{
//}
//
//
//void BossSpawner::Spawn()
//{
//    const int TARGET_BOSS_COUNT = 10;
//
//    if (getChildren().size() >= m_maxEntity)
//        return;
//
//    size_t currentBossCount = getChildren().size();
//
//    size_t bossToCreate = std::min(TARGET_BOSS_COUNT - currentBossCount, m_maxEntity - currentBossCount);
//
//    for (size_t i = 0; i < bossToCreate; i++)
//    {
//        sf::Vector2f spawnPos;
//
//        if (m_useFixedPosition)
//        {
//            int row = i / 5;
//            int col = i % 5;
//            spawnPos = m_fixedSpawnPosition + sf::Vector2f(col * 300.0f, row * 250.0f);
//        }
//        else
//        {
//            int x = UseRandomNumber().getRandomNumber<int>(
//                m_RestrictedArea.Pmin.x + 100,
//                m_RestrictedArea.Pmax.x - 100);
//
//            int y = m_RestrictedArea.Pmin.y + 150 + (i * 100);
//
//            if (i % 2 == 0)
//                x -= 200;
//            else
//                x += 200;
//
//            spawnPos = sf::Vector2f(x, y);
//        }
//
//        BossMode selectedMode;
//        int modeIndex = (currentBossCount + i) % 4;
//
//        switch (modeIndex) {
//        case 0: selectedMode = BossMode::Type1; break;
//        case 1: selectedMode = BossMode::Type2; break;
//        case 2: selectedMode = BossMode::Type3; break;
//        case 3: selectedMode = BossMode::Combined; break;
//        }
//
//        float adjustedHealth = m_bossHealth / 4.0f;
//        new MegaBoss(this, spawnPos, selectedMode, adjustedHealth);
//    }
//}