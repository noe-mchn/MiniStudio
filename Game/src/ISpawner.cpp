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

void AsteroidSpawner::Spawn()
{
	if (getChildren().size() >= m_maxEntity)
		return;
	sf::Vector2f size;
	size.x = UseRandomNumber().getRandomNumber(150, 400);
	size.y = UseRandomNumber().getRandomNumber(150, 400);

	float angle = UseRandomNumber().getRandomNumber(0, 360);
	float speed = UseRandomNumber().getRandomNumber(75, 150);
	float life = UseRandomNumber().getRandomNumber(2, 8);

	auto spawn = RandomSpanw::getPosition(m_SpawnZone, m_RestrictedArea, sf::Vector2f(size.x / 2, size.y / 2));
	new Asteroid(this, spawn, size, angle, speed, life);
}

void CometeSpawner::Spawn()
{
	if (getChildren().size() >= m_maxEntity)
		return;
	sf::Vector2f size;
	size.x = UseRandomNumber().getRandomNumber(50, 150);
	size.y = UseRandomNumber().getRandomNumber(50, 150);

	float angle = UseRandomNumber().getRandomNumber(0, 360);
	float speed = UseRandomNumber().getRandomNumber(150, 250);
	float life = UseRandomNumber().getRandomNumber(1, 4);

	auto spawn = RandomSpanw::getPosition(m_SpawnZone, m_RestrictedArea, sf::Vector2f(size.x / 2, size.y / 2));
	new Comete(this, spawn, size, angle, speed, life);
}

BossSpawner::BossSpawner(IComposite* scene, const size_t& maxEntity)
	: ISpawner(scene, maxEntity)
	, m_useFixedPosition(false)
	, m_fixedSpawnPosition(0, 0)
	, m_bossHealth(10.0f)
{
}

void BossSpawner::Spawn()
{
    if (getChildren().size() >= m_maxEntity)
        return;

    sf::Vector2f spawnPos1, spawnPos2, spawnPos3;

    if (m_useFixedPosition)
    {
        spawnPos1 = m_fixedSpawnPosition;
        spawnPos2 = m_fixedSpawnPosition + sf::Vector2f(800, 0);
        spawnPos3 = m_fixedSpawnPosition + sf::Vector2f(400, -200);
    }
    else
    {
        int x1 = UseRandomNumber().getRandomNumber<int>
            (m_RestrictedArea.Pmin.x + 100,
                m_RestrictedArea.Pmax.x - 100);

        int x2 = UseRandomNumber().getRandomNumber<int>
            (m_RestrictedArea.Pmin.x + 400,
                m_RestrictedArea.Pmax.x - 400);

        int x3 = UseRandomNumber().getRandomNumber<int>
            (m_RestrictedArea.Pmin.x + 250,
                m_RestrictedArea.Pmax.x - 250);

        float y = m_RestrictedArea.Pmin.y + 150;

        spawnPos1 = sf::Vector2f(x1, y);
        spawnPos2 = sf::Vector2f(x2, y);
        spawnPos3 = sf::Vector2f(x3, y - 150);
    }

    // Choisir un type de boss aléatoirement
    static int bossCount = 0;
    bossCount++;

    BossMode selectedMode;

    // Option 1: Rotation séquentielle des modes
    switch (bossCount % 4) {
    case 0: selectedMode = BossMode::Type1; break;
    case 1: selectedMode = BossMode::Type2; break;
    case 2: selectedMode = BossMode::Type3; break;
    case 3: selectedMode = BossMode::Combined; break;
    }

    /* Option 2: Sélection aléatoire
    int randomMode = UseRandomNumber().getRandomNumber<int>(0, 3);
    switch (randomMode) {
        case 0: selectedMode = BossMode::Type1; break;
        case 1: selectedMode = BossMode::Type2; break;
        case 2: selectedMode = BossMode::Type3; break;
        case 3: selectedMode = BossMode::Combined; break;
    }
    */

    /* Option 3: Difficulté progressive
    if (m_level < 3)
        selectedMode = BossMode::Type1;
    else if (m_level < 6)
        selectedMode = BossMode::Type2;
    else if (m_level < 9)
        selectedMode = BossMode::Type3;
    else
        selectedMode = BossMode::Combined;
    */

    new MegaBoss(this, spawnPos2, selectedMode, m_bossHealth);
}