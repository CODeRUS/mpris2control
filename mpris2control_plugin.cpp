#include "mpris2control_plugin.h"
#include "control.h"

#include <qqml.h>

void Mpris2controlPlugin::registerTypes(const char *uri)
{
    // @uri org.coderus.mpris
    qmlRegisterType<Control>(uri, 2, 0, "Control");
}


