/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "HeatMapLegendGL.h"

#include "utils/Utils.h"

#include <QPainter>
#include <QGLWidget>

#include "GLQt.h"
#include "GLScope.h"
#include "data/GLElementRender.h"
#include "data/GLElementRectangleFactory.h"
#include "math/GLMatrix.h"
#include "image/GLHeatMap.h"
#include "image/GLImageWriter.h"

const QRectF HeatMapLegendGL::DEFAULT_BOUNDS =
        QRectF(0.0f, 0.0f,
        Globals::heatmap_bar_width * 2,
        Globals::heatmap_height);

HeatMapLegendGL::HeatMapLegendGL(QObject* parent)
    : ViewItemGL(parent),
      m_bounds(DEFAULT_BOUNDS),
      m_rect(),
      m_texture(0)
{
    generateHeatMapText();
    generateHeatMapData();
    generateStaticHeatMapData();
}

HeatMapLegendGL::~HeatMapLegendGL()
{

}

void HeatMapLegendGL::setBounds(const QRectF& bounds)
{
    if (m_bounds != bounds) {
        m_bounds = bounds;
        rebuildHeatMapData();
    }
}

void HeatMapLegendGL::render(QPainter* painter)
{
    GL::GLElementRender renderer;
    renderer.addTexture(m_texture);

    painter->beginNativePainting();
    {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
            GL::GLscope glBlendScope(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            GL::GLscope glTextureScope(GL_TEXTURE_2D);
            renderer.render(m_data, m_queue);
        glPopMatrix();
    }
    painter->endNativePainting();

    // render text
    painter->setPen(QPen(Qt::white));
    painter->drawPath(m_text);
}

const QRectF HeatMapLegendGL::boundingRect() const
{
    return m_bounds;
}

bool HeatMapLegendGL::contains(const QPointF& point) const
{
    return m_bounds.contains(point);
}

void HeatMapLegendGL::setHitCountLimits(int min, int max, int sum)
{
    if ((m_hitCountMin != min) ||
        (m_hitCountMax != max) ||
        (m_hitCountSum != sum)) {
        
        m_hitCountMin = min;
        m_hitCountMax = max;     
        m_hitCountSum = sum;
        
        m_lowerLimit = min;
        m_upperLimit = max;
        
        rebuildHeatMapText();
        rebuildHeatMapData();
        rebuildHeatMapStaticData();
    }
}

void HeatMapLegendGL::setLowerLimit(int limit)
{
    const int adjusted_limit = static_cast<int>( (qreal(limit) /
                                                  qreal(Globals::gene_threshold_max - Globals::gene_threshold_min) ) *
                                                 qreal(m_hitCountMax - m_hitCountMin));
    if (m_lowerLimit != adjusted_limit) {
        m_lowerLimit = adjusted_limit;
        const qreal threshold = qreal(m_lowerLimit - m_hitCountMin) / qreal(m_hitCountMax - m_hitCountMin);
        m_lower_threshold = GL::clamp(threshold, qreal(0.0), qreal(1.0));
        rebuildHeatMapData();
    }
}

void HeatMapLegendGL::setUpperLimit(int limit)
{
    const int adjusted_limit = static_cast<int>( (qreal(limit) /
                                                  qreal(Globals::gene_threshold_max - Globals::gene_threshold_min) ) *
                                                 qreal(m_hitCountMax - m_hitCountMin));
    if (m_upperLimit != adjusted_limit) {
        m_upperLimit = adjusted_limit;
        const qreal threshold = qreal(m_hitCountMax - m_upperLimit) / qreal(m_hitCountMax - m_hitCountMin);
        m_upper_threshold = GL::clamp(threshold, qreal(0.0), qreal(1.0));
        rebuildHeatMapData();
    }
}

void HeatMapLegendGL::rebuildHeatMapData()
{
    // clear rendering data and generate anew
    m_data.clear();
    m_queue.clear();
    generateHeatMapData();
}

void HeatMapLegendGL::generateHeatMapData()
{
    const GL::GLflag flags =
        GL::GLElementShapeFactory::AutoAddColor |
        GL::GLElementShapeFactory::AutoAddTexture |
        GL::GLElementShapeFactory::AutoAddConnection;

    GL::GLElementRectangleFactory factory(m_data, flags);

    // rebuild heatmap rectangle
    //const QPointF topLeft = m_bounds.topLeft();
    const QSizeF size = m_bounds.size();
    m_rect = QRectF(
                 QPointF(0.0f, 0.0f),
                 QSizeF(Globals::heatmap_width, size.height())
             );

    // threshold height
    const GLfloat height = GLfloat(size.height());
    const GLfloat thresholdLowerHeight = (1.0f - m_lower_threshold) * height;
    const GLfloat thresholdUpperHeight = (m_upper_threshold) * height;

    // generate borders
    const GL::GLcolor borderColor = GL::GLcolor(GL::White);
    const GL::GLrectangle rect = GL::toGLrectangle(m_rect);

    //create the legend
    factory.setColor(borderColor);
    factory.addShape(rect);
    factory.setColor(GL::GLcolor(GL::Black));
    factory.addShape(GL::GLrectangle::fromLine(rect.p[0], rect.p[1]));
    factory.addShape(GL::GLrectangle::fromLine(rect.p[1], rect.p[2]));
    factory.addShape(GL::GLrectangle::fromLine(rect.p[2], rect.p[3]));
    factory.addShape(GL::GLrectangle::fromLine(rect.p[3], rect.p[0]));

    factory.setColor(GL::GLcolor(GL::Black));
    factory.addShape(GL::GLrectangle::fromLine(
                         GL::GLpoint(0.0f, thresholdLowerHeight),
                         GL::GLpoint(Globals::heatmap_bar_width, thresholdLowerHeight),
                         3.0f
                     ));
    factory.addShape(GL::GLrectangle::fromLine(
                         GL::GLpoint(0.0f, thresholdUpperHeight),
                         GL::GLpoint(Globals::heatmap_bar_width, thresholdUpperHeight),
                         3.0f
                     ));

    factory.setColor(GL::GLcolor(GL::Red));
    factory.addShape(GL::GLrectangle::fromLine(
                         GL::GLpoint(0.0f, thresholdLowerHeight),
                         GL::GLpoint(Globals::heatmap_bar_width, thresholdLowerHeight)
                     ));
    factory.addShape(GL::GLrectangle::fromLine(
                         GL::GLpoint(0.0f, thresholdUpperHeight),
                         GL::GLpoint(Globals::heatmap_bar_width, thresholdUpperHeight)
                     ));

    // generate element data render command
    m_queue.add(GL::GLElementRenderQueue::Command
                (GL::GLElementRenderQueue::Command::BindTexture, 0));  // bind heat-map texture
    m_queue.add(GL::GLElementRenderQueue::Command
                (GL::GLElementRenderQueue::Command::RenderItemOne));   // render heat-map rectangle
    m_queue.add(GL::GLElementRenderQueue::Command
                (GL::GLElementRenderQueue::Command::UnbindTexture));   // unbind texture
    m_queue.add(GL::GLElementRenderQueue::Command
                (GL::GLElementRenderQueue::Command::RenderItemAll));   // render rest
    m_queue.end();
}

void HeatMapLegendGL::rebuildHeatMapText()
{
    // clear text and generate anew
    m_text = QPainterPath();
    generateHeatMapText();
}

void HeatMapLegendGL::generateHeatMapText()
{
    // rebuild heatmap rectangle
    const GLfloat height = GLfloat(m_bounds.height());
    QFont monoFont("Courier", 10, QFont::Normal);
    monoFont.setStyleHint(QFont::TypeWriter);
    m_text.addText(Globals::heatmap_bar_width, 10, monoFont, QString("%1").arg(m_hitCountMax));
    m_text.addText(Globals::heatmap_bar_width, height, monoFont, QString("%1").arg(m_hitCountMin));
}

void HeatMapLegendGL::rebuildHeatMapStaticData()
{
    //clear image and texture to regerate them
    m_image.deleteImage();
    m_texture->release();
    m_texture->destroy();
    generateStaticHeatMapData();
}

void HeatMapLegendGL::generateStaticHeatMapData()
{
    // generate image
    m_image = GL::GLimage(1, Globals::heatmap_height, GL_RGBA, GL_FLOAT);
    m_image.createImage();
    GL::GLheatmap::createHeatMapImage(m_image, GL::GLheatmap::SpectrumExp, m_hitCountMin, m_hitCountMax);
    // texture
    m_texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_texture->setSize(m_image.width(), m_image.height());
    m_texture->setMinificationFilter(QOpenGLTexture::Linear);
    m_texture->setMagnificationFilter(QOpenGLTexture::Linear);
    m_texture->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::ClampToBorder);
    m_texture->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::ClampToBorder);
    m_texture->setFormat(QOpenGLTexture::RGB32F);
    m_texture->allocateStorage();
    m_texture->create();
    m_texture->bind();
    m_texture->setData(QOpenGLTexture::RGBA, QOpenGLTexture::Float32, m_image.pixels());
}