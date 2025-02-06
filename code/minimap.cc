#include "minimap.h"
#include <gf/Coordinates.h>
#include <gf/RenderTarget.h>
#include <gf/Shapes.h>

Minimap::Minimap(const FootballField& field, float scale)
: m_scale(scale) {
    // Set default minimap position
    m_position = {0.0f, 0.0f};
}

void Minimap::setPosition(float x, float y) {
    m_position = {x, y};
}

void Minimap::render(gf::RenderTarget& target) {
    // Create a screen-space view (matches the window size)
    gf::View screenView({0.0f, 0.0f}, target.getSize());
    target.setView(screenView);

    // Create a rectangle representing the minimap
    gf::RectangleShape minimapRect;
    minimapRect.setSize({100.0f, 50.0f}); // Adjust size as needed
    minimapRect.setPosition(m_position);
    minimapRect.setColor(gf::Color::Gray(0.7f)); // Set a light gray color

    // Render the rectangle
    target.draw(minimapRect);

    // Restore the original view (reset to game view)
    target.setView(gf::View(target.getSize() * 0.5f, target.getSize())); // Center-based game view
}

