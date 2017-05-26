#ifndef STDATA_H
#define STDATA_H

#include <QSharedPointer>
#include <QList>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QColor>

#include "data/Gene.h"
#include "data/Spot.h"
#include "viewPages/SettingsWidget.h"

#include <armadillo>

using namespace arma;

// TODO the values type can be templated
class STData
{

public:
    typedef Mat<float> Matrix;
    typedef QPair<float,float> SpotType;
    typedef QString GeneType;
    typedef QSharedPointer<Spot> SpotObjectType;
    typedef QSharedPointer<Gene> GeneObjectType;
    typedef QList<SpotObjectType> SpotListType;
    typedef QList<GeneObjectType> GeneListType;

    STData();
    ~STData();

    // import/export matrix
    void read(const QString &filename);
    void save(const QString &filename) const;


    std::vector<float> count(const GeneType &gene) const;
    std::vector<float> count(const SpotType &spot) const;
    float count(const GeneType &gene, const SpotType &spot) const;

    std::vector<float> count(const GeneObjectType &gene) const;
    std::vector<float> count(const SpotObjectType &spot) const;
    float count(const GeneObjectType &gene, const SpotObjectType &spot) const;

    Matrix slice_matrix_counts() const;
    Matrix matrix_counts() const;

    std::vector<float> spots_counts();
    std::vector<float> genes_counts();

    GeneType gene_at(size_t index) const;
    SpotType spot_at(size_t index) const;
    GeneObjectType gene_object_at(size_t index) const;
    SpotObjectType spot_object_at(size_t index) const;

    size_t number_spots() const;
    size_t number_genes() const;

    GeneListType genes();
    SpotListType spots();

    float min_genes_spot() const;
    float max_genes_spot() const;
    float min_reads_spot() const;
    float max_reads_spot() const;
    float max_reads() const;
    float min_reads() const;

    void setRenderingSettings(const SettingsWidget::Rendering &m_rendering_settings);

    void computeRenderingData();

    const QVector<unsigned> &renderingIndexes() const;
    const QVector<QVector3D> &renderingVertices() const;
    const QVector<QVector2D> &renderingTextures() const;
    const QVector<QVector4D> &renderingColors() const;

    void updateSize(const float size);

    void initRenderingData();

private:

    void computeGenesCutoff();

    Matrix m_counts_matrix;
    // cache the normalized matrix to save computational time
    Matrix m_counts_norm_matrix;
    // store gene/spots objects and indexes in matrix
    SpotListType m_spots;
    GeneListType m_genes;
    QVector<GeneType> m_matrix_genes;
    QVector<SpotType> m_matrix_spots;
    // rendering settings
    const SettingsWidget::Rendering *m_rendering_settings;
    mutable SettingsWidget::NormalizationMode m_normalization;
    // cache the normalization settings to do it only when necessary
    // rendering data
    QVector<QVector3D> m_vertices;
    QVector<QVector2D> m_textures;
    QVector<QVector4D> m_colors;
    QVector<unsigned> m_indexes;
};

#endif // STDATA_H
