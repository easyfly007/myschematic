#pragma once

#include <QGraphicsView>

class SchematicScene;

class SchematicView : public QGraphicsView {
    Q_OBJECT

public:
    explicit SchematicView(SchematicScene* scene, QWidget* parent = nullptr);

    void fitToWindow();

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void drawBackground(QPainter* painter, const QRectF& rect) override;

private:
    static constexpr double kMinScale = 0.05;
    static constexpr double kMaxScale = 10.0;
    static constexpr double kGridSize = 10.0;

    bool m_isPanning = false;
    QPoint m_panStartPos;
};
