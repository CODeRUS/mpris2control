#ifndef MPRIS2CONTROL_PLUGIN_H
#define MPRIS2CONTROL_PLUGIN_H

#include <QQmlExtensionPlugin>

class Mpris2controlPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

#endif // MPRIS2CONTROL_PLUGIN_H

