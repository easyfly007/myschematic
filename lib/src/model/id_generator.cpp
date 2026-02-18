#include "myschematic/id_generator.h"
#include <QUuid>

namespace myschematic {

QString IdGenerator::generate() {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

} // namespace myschematic
