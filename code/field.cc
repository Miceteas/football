#include "field.h"

FootballField::FootballField(const std::string& texturePath, const std::vector<int>& tileOrder, int tilesX, int tilesY, int displayTileSize)
: m_texture(texturePath)
, m_tileLayer(gf::TileLayer::createOrthogonal({ tilesX, tilesY }, { SPRITETILESIZE, SPRITETILESIZE })) 
, m_tilesX(tilesX)
, m_tilesY(tilesY)
, m_displayTileSize(displayTileSize)
{
    std::size_t tilesetId = m_tileLayer.createTilesetId();
    gf::Tileset& tileset = m_tileLayer.getTileset(tilesetId);
    tileset.setTexture(m_texture);
    tileset.setTileSize({ SPRITETILESIZE, SPRITETILESIZE }); // Set the size of each tile in the spritesheet (64x64)

    for (int y = 0; y < tilesY; ++y) {
        for (int x = 0; x < tilesX; ++x) {
            int index = y * tilesX + x;
            if (index < static_cast<int>(tileOrder.size())) {
                int tileId = tileOrder[index];
                m_tileLayer.setTile({ x, y }, tilesetId, tileId);
            }
        }
    }

    float scaleFactor = static_cast<float>(displayTileSize) / SPRITETILESIZE;
    m_tileLayer.setScale({ scaleFactor, scaleFactor });
}

gf::TileLayer& FootballField::getTileLayer() {
    return m_tileLayer;
}


