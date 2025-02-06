#include "field.h"

FootballField::FootballField(const std::string& texturePath, const std::vector<int>& tileOrder, int tilesX, int tilesY, int displayTileSize)
: m_texture(texturePath)
, m_tilesX(tilesX)
, m_tilesY(tilesY)
, m_displayTileSize(displayTileSize)
, m_spriteTileSize(64) // Spritesheet tiles are always 64x64 pixels for us but if needed it can be added as a parameter!!! 
, m_tileLayer(gf::TileLayer::createOrthogonal({ tilesX, tilesY }, { m_spriteTileSize, m_spriteTileSize })) 
{
    std::size_t tilesetId = m_tileLayer.createTilesetId();
    gf::Tileset& tileset = m_tileLayer.getTileset(tilesetId);
    tileset.setTexture(m_texture);
    tileset.setTileSize({ m_spriteTileSize, m_spriteTileSize }); // Set the size of each tile in the spritesheet (64x64)

    // To display in the correct order
    for (int y = 0; y < tilesY; ++y) {
        for (int x = 0; x < tilesX; ++x) {
            int index = y * tilesX + x;
            if (index < static_cast<int>(tileOrder.size())) {
                int tileId = tileOrder[index]; // Tile index in the tileset
                m_tileLayer.setTile({ x, y }, tilesetId, tileId);
            }
        }
    }

    // Scale
    float scaleFactor = static_cast<float>(displayTileSize) / m_spriteTileSize;
    m_tileLayer.setScale({ scaleFactor, scaleFactor });
}

gf::TileLayer& FootballField::getTileLayer() {
    return m_tileLayer;
}


