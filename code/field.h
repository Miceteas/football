#ifndef FIELD_H
#define FIELD_H

#include <gf/TileLayer.h>
#include <gf/Tileset.h>
#include <gf/Texture.h>
#include <vector>
#include <string>

class FootballField {
    public:
        FootballField(const std::string& texturePath, const std::vector<int>& tileOrder, int tilesX, int tilesY, int displayTileSize);
        
        gf::TileLayer& getTileLayer();

    private:
        gf::Texture m_texture;         // Texturespritesheet
        gf::TileLayer m_tileLayer;     
        int m_tilesX;                  // Number of tiles X (width)
        int m_tilesY;                  // Number of tiles Y (height)
        int m_displayTileSize;         // Display size of each tile (on the screen => because different)
        const int m_spriteTileSize;    // Fixed size of each tile in the spritesheet (for the spritesheet we're using it's 64x64 pixels)
};

#endif
