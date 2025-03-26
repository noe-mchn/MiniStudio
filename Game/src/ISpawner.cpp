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

//void AsteroidSpawner::Spawn()
//{
//	if (getChildren().size() >= m_maxEntity)
//		return;
//	sf::Vector2f size;
//	size.x = UseRandomNumber().getRandomNumber(150, 400);
//	size.y = UseRandomNumber().getRandomNumber(150, 400);
//
//	float angle = UseRandomNumber().getRandomNumber(0, 360);
//	float speed = UseRandomNumber().getRandomNumber(75, 150);
//	float life = UseRandomNumber().getRandomNumber(2, 8);
//
//	auto spawn = RandomSpanw::getPosition(m_SpawnZone, m_RestrictedArea, sf::Vector2f(size.x / 2, size.y / 2));
//	new Asteroid(this, spawn, size, angle, speed, life);
//}

//void CometeSpawner::Spawn()
//{
//	if (getChildren().size() >= m_maxEntity)
//		return;
//	sf::Vector2f size;
//	size.x = UseRandomNumber().getRandomNumber(50, 150);
//	size.y = UseRandomNumber().getRandomNumber(50, 150);
//
//	float angle = UseRandomNumber().getRandomNumber(0, 360);
//	float speed = UseRandomNumber().getRandomNumber(150, 250);
//	float life = UseRandomNumber().getRandomNumber(1, 4);
//
//	auto spawn = RandomSpanw::getPosition(m_SpawnZone, m_RestrictedArea, sf::Vector2f(size.x / 2, size.y / 2));
//	new Comete(this, spawn, size, angle, speed, life);
//}

BossSpawner::BossSpawner(IComposite* scene, const size_t& maxEntity)
    : ISpawner(scene, std::max(maxEntity, static_cast<size_t>(10))) // S'assurer que maxEntity est au moins 20
    , m_useFixedPosition(false)
    , m_fixedSpawnPosition(0, 0)
    , m_bossHealth(3.0f) // Conserver la valeur originale qui sera ajustée dans Spawn
{
}

void BossSpawner::Spawn()
{
    // Nombre maximum de boss que nous voulons
    const int TARGET_BOSS_COUNT = 10;

    // On vérifie si on a déjà atteint le maximum d'entités
    if (getChildren().size() >= m_maxEntity)
        return;

    // Nombre actuel de boss existants
    size_t currentBossCount = getChildren().size();

    // Nombre de boss à créer lors de cet appel
    size_t bossToCreate = std::min(TARGET_BOSS_COUNT - currentBossCount, m_maxEntity - currentBossCount);

    // Créer chaque boss
    for (size_t i = 0; i < bossToCreate; i++)
    {
        // Calculer des positions différentes pour chaque boss
        sf::Vector2f spawnPos;

        if (m_useFixedPosition)
        {
            // Répartir en grille autour de la position fixe
            int row = i / 5;
            int col = i % 5;
            spawnPos = m_fixedSpawnPosition + sf::Vector2f(col * 300.0f, row * 250.0f);
        }
        else
        {
            // Répartir aléatoirement dans la zone restreinte
            int x = UseRandomNumber().getRandomNumber<int>(
                m_RestrictedArea.Pmin.x + 100,
                m_RestrictedArea.Pmax.x - 100);

            // Varier la hauteur pour éviter les superpositions
            int y = m_RestrictedArea.Pmin.y + 150 + (i * 100);

            // Utiliser une disposition en zigzag
            if (i % 2 == 0)
                x -= 200;
            else
                x += 200;

            spawnPos = sf::Vector2f(x, y);
        }

        // Choisir un type de boss (rotation des types)
        BossMode selectedMode;
        int modeIndex = (currentBossCount + i) % 4;

        switch (modeIndex) {
        case 0: selectedMode = BossMode::Type1; break;
        case 1: selectedMode = BossMode::Type2; break;
        case 2: selectedMode = BossMode::Type3; break;
        case 3: selectedMode = BossMode::Combined; break;
        }

        // Créer le boss avec une santé réduite (pour équilibrer le jeu)
        float adjustedHealth = m_bossHealth / 4.0f; // Diviser par 4 car 20 boss au lieu de 1
        new MegaBoss(this, spawnPos, selectedMode, adjustedHealth);

        std::cout << "Created boss #" << (currentBossCount + i + 1)
            << " at position (" << spawnPos.x << ", " << spawnPos.y << ")" << std::endl;
    }
}