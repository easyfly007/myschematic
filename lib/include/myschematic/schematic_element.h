#pragma once

#include <QObject>
#include <QPointF>
#include <QRectF>
#include <QString>
#include "types.h"

namespace myschematic {

class SchematicElement : public QObject {
    Q_OBJECT

public:
    explicit SchematicElement(QObject* parent = nullptr);
    ~SchematicElement() override;

    QString id() const;

    QPointF position() const;
    void setPosition(const QPointF& pos);

    int rotation() const;
    void setRotation(int degrees);

    bool isMirrored() const;
    void setMirrored(bool mirrored);

    virtual QRectF boundingRect() const = 0;
    virtual ElementType type() const = 0;

signals:
    void changed();

protected:
    QString m_id;
    QPointF m_position;
    int m_rotation = 0;
    bool m_mirrored = false;
};

} // namespace myschematic
