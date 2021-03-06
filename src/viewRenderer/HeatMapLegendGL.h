#ifndef HEATMAPLEGEND_H
#define HEATMAPLEGEND_H

#include <QOpenGLTexture>
#include "GraphicItemGL.h"
#include "viewPages/SettingsWidget.h"

class QImage;

// HeatMapLegend is an visual item that is used to represent the heat map
// spectrum
// in order to give a reference point about the color-value relationship for the
// gene data
// when the user selects heat map mode
class HeatMapLegendGL : public GraphicItemGL
{
    Q_OBJECT

public:

    HeatMapLegendGL(const SettingsWidget::Rendering &rendering_settings, QObject *parent = 0);
    virtual ~HeatMapLegendGL();

    // clear up all data
    void clearData();

public slots:

    // Update the rendering data
    void slotUpdate();

protected:
    const QRectF boundingRect() const override;
    void draw(QOpenGLFunctionsVersion &qopengl_functions, QPainter &painter) override;
    void setSelectionArea(const SelectionEvent &event);

private:

    // create the legend
    void generateLegend();

    // legend image
    QImage m_image;
    // rendering settings
    const SettingsWidget::Rendering &m_rendering_settings;
    // true when rendering data has been computed
    bool m_initialized;

    Q_DISABLE_COPY(HeatMapLegendGL)
};

#endif // HEATMAPLEGEND_H //
