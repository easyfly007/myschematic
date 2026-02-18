#pragma once

#include <QObject>
#include <QList>
#include <QString>

namespace myschematic {

class Sheet;

class Circuit : public QObject {
    Q_OBJECT

public:
    explicit Circuit(QObject* parent = nullptr);
    ~Circuit() override;

    // Sheet management
    Sheet* createSheet(const QString& name);
    void removeSheet(const QString& id);
    Sheet* sheetById(const QString& id) const;
    Sheet* sheetByName(const QString& name) const;
    QList<Sheet*> sheets() const;

private:
    QList<Sheet*> m_sheets;
};

} // namespace myschematic
