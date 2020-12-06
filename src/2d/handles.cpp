#include "glpp/2d/handles.hpp"

#include "glpp/draw_batch.hpp"


gl::Handle::Handle() :
    ignoreInteractions(false),
    constrain(Constraint::None),
    position(0),
    color(1)
{
}

gl::Handle::Handle(glm::vec2 position, glm::vec4 color) :
    ignoreInteractions(false),
    constrain(Constraint::None),
    position(position), 
    color(color)
{
}

gl::BoxHandle::BoxHandle(glm::vec2 position, glm::vec2 size, glm::vec4 color) :
    Handle(position, color),
    size(size)
{
}

void gl::BoxHandle::createGeometry(DrawBatch& batch)
{
    auto data = batch.getAttirbute<gl::CompactVertexBufferObject<glm::vec2, glm::vec4>>(0);
    gl::IndexBuffer& indices = *batch.indexBuffer;

    unsigned int i0 = data->size();
    data->push_back(position + glm::vec2(-0.5f, -0.5f) * size, color);
    data->push_back(position + glm::vec2(-0.5f,  0.5f) * size, color);
    data->push_back(position + glm::vec2( 0.5f, -0.5f) * size, color);
    data->push_back(position + glm::vec2( 0.5f,  0.5f) * size, color);

    indices.insert(indices.end(), { i0    , i0 + 1, i0 + 2 });
    indices.insert(indices.end(), { i0 + 1, i0 + 2, i0 + 3 });
}

bool gl::BoxHandle::overlaps(int x, int y) const
{
    if (ignoreInteractions) return false;
    return (x >= position.x - 0.5f * size.x && x < position.x + 0.5f * size.x) && (y >= position.y - 0.5f * size.y && y < position.y + 0.5f * size.y);
}

gl::EventState gl::Handle::onMouseDown(float x, float y)
{
    if (ignoreInteractions) return gl::EventState::Pass;
    return gl::EventState::StartDrag;
}

gl::EventState gl::Handle::onDrag(float dx, float dy)
{
    if (ignoreInteractions) return gl::EventState::Pass;
    if (constrain == Constraint::XAxis) {
        position.x += dx;
    }
    else if (constrain == Constraint::YAxsis) {
        position.y += dy;
    }
    else {
        position += glm::vec2(dx, dy);
    }
    return gl::EventState::Stop;
}

