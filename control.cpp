#include "control.h"

#include <QTimer>

Control::Control(QObject *parent):
    QObject(parent),
    m_active(false),
    m_service()
{
}

Control::~Control()
{
}

void Control::classBegin()
{

}

void Control::componentComplete()
{
    if (m_active && m_service != "") {
        initialize();
    }
}

void Control::initialize()
{
    getAllProperties();
    emitProperties();
    getAllPropertiesPlayer();
    emitPropertiesPlayer();

    schedulePing();
}

void Control::quit()
{
    if (mprisIface)
        mprisIface->call("Quit");
}

void Control::raise()
{
    if (mprisIface)
        mprisIface->call("Raise");
}

void Control::next()
{
    if (playerIface)
        playerIface->call("Next");
}

void Control::openUri(const QString &uri)
{
    if (playerIface)
        playerIface->call("OpenUri", uri);
}

void Control::pause()
{
    if (playerIface)
        playerIface->call("Pause");
}

void Control::play()
{
    if (playerIface)
        playerIface->call("Play");
}

void Control::playPause()
{
    if (playerIface)
        playerIface->call("PlayPause");
}

void Control::previous()
{
    if (playerIface)
        playerIface->call("Previous");
}

void Control::seek(qlonglong offset)
{
    if (playerIface)
        playerIface->call("Seek", offset);
}

void Control::stop()
{
    if (playerIface)
        playerIface->call("Stop");
}

bool Control::active() const
{
    return m_active;
}

void Control::setActive(bool newActive)
{
    if (m_active != newActive) {
        m_active = newActive;

        if (!m_active) {
            disconnectSignals();
        }
        else if (m_service != "") {
            connectSignals();
            initialize();
        }

        Q_EMIT activeChanged();
    }
}

QString Control::service() const
{
    return m_service;
}

void Control::setService(const QString &newService)
{
    if (m_service != newService) {
        m_service = newService;

        if (m_service != "") {
            mprisIface.reset(new QDBusInterface(m_service, MPRIS2_PATH, MPRIS2_MEDIAPLAYER_IF, QDBusConnection::sessionBus(), this));
            playerIface.reset(new QDBusInterface(m_service, MPRIS2_PATH, MPRIS2_MEDIAPLAYER_PLAYER_IF, QDBusConnection::sessionBus(), this));
            propertiesIface.reset(new QDBusInterface(m_service, MPRIS2_PATH, FREEDESKTOP_PROPERTIES_IF, QDBusConnection::sessionBus(), this));
            peerIface.reset(new QDBusInterface(m_service, MPRIS2_PATH, FREEDESKTOP_PEER_IF, QDBusConnection::sessionBus(), this));

            if (m_active) {
                connectSignals();
                initialize();
            }
        }
        else if (m_active) {
            disconnectSignals();
        }

        Q_EMIT serviceChanged();
    }
}

bool Control::canQuit() const
{
    return m_properties["CanQuit"].toBool();
}

bool Control::canRaise() const
{
    return m_properties["CanRaise"].toBool();
}

bool Control::canSetFullscreen() const
{
    return m_properties["CanSetFullscreen"].toBool();
}

QString Control::desktopEntry() const
{
    return m_properties["DesktopEntry"].toString();
}

bool Control::fullscreen() const
{
    return m_properties["Fullscreen"].toBool();
}

void Control::setFullscreen(bool newFulscreen)
{
    setDBusProperty(MPRIS2_MEDIAPLAYER_IF, "Fullscreen", newFulscreen);
}

bool Control::hasTrackList() const
{
    return m_properties["HasTrackList"].toBool();
}

QString Control::identity() const
{
    return m_properties["Identity"].toString();
}

QStringList Control::supportedMimeTypes() const
{
    return m_properties["SupportedMimeTypes"].toStringList();
}

QStringList Control::supportedUriSchemes() const
{
    return m_properties["SupportedUriSchemes"].toStringList();
}

bool Control::canControl() const
{
    return m_properties_player["CanControl"].toBool();
}

bool Control::canGoNext() const
{
    return m_properties_player["CanGoNext"].toBool();
}

bool Control::canGoPrevious() const
{
    return m_properties_player["CanGoPrevious"].toBool();
}

bool Control::canPause() const
{
    return m_properties_player["CanPause"].toBool();
}

bool Control::canPlay() const
{
    return m_properties_player["CanPlay"].toBool();
}

bool Control::canSeek() const
{
    return m_properties_player["CanSeek"].toBool();
}

QString Control::loopStatus() const
{
    return m_properties_player["LoopStatus"].toString();
}

void Control::setLoopStatus(const QString newLoopStatus)
{
    setDBusProperty(MPRIS2_MEDIAPLAYER_PLAYER_IF, "LoopStatus", newLoopStatus);
}

double Control::maximumRate() const
{
    return m_properties_player["MaximumRate"].toDouble();
}

QVariantMap Control::metadata() const
{
    return m_properties_player["Metadata"].toMap();
}

double Control::minimumRate() const
{
    return m_properties_player["MinimumRate"].toDouble();
}

QString Control::playbackStatus() const
{
    return m_properties_player["PlaybackStatus"].toString();
}

qlonglong Control::position() const
{
    return m_properties_player["Position"].toLongLong();
}

double Control::rate() const
{
    return m_properties_player["Rate"].toDouble();
}

void Control::setRate(double newRate)
{
    setDBusProperty(MPRIS2_MEDIAPLAYER_PLAYER_IF, "Rate", newRate);
}

bool Control::shuffle() const
{
    return m_properties_player["Shuffle"].toBool();
}

void Control::setShuffle(bool newShuffle)
{
    setDBusProperty(MPRIS2_MEDIAPLAYER_PLAYER_IF, "Shuffle", newShuffle);
}

double Control::volume() const
{
    return m_properties_player["Volume"].toDouble();
}

void Control::setVolume(double newVolume)
{
    setDBusProperty(MPRIS2_MEDIAPLAYER_PLAYER_IF, "Volume", newVolume);
}

QVariantMap Control::convertMetadata(const QVariant &dbusArgumentMetadata)
{
    QDBusArgument metadata = dbusArgumentMetadata.value<QDBusArgument>();
    QVariantMap mprisMetadata;
    metadata >> mprisMetadata;
    return mprisMetadata;
}

void Control::getAllProperties()
{
    QMapIterator<QString, QVariant> i(getAllDBusProperties(MPRIS2_MEDIAPLAYER_IF));
    while (i.hasNext()) {
        i.next();
        m_properties[i.key()] = i.value();
    }
}

void Control::getAllPropertiesPlayer()
{
    QMapIterator<QString, QVariant> i(getAllDBusProperties(MPRIS2_MEDIAPLAYER_PLAYER_IF));
    while (i.hasNext()) {
        i.next();
        if (i.key() == "Metadata") {
            m_properties_player[i.key()] = convertMetadata(i.value());
        }
        else {
            m_properties_player[i.key()] = i.value();
        }
    }
}

void Control::emitProperties()
{
    Q_EMIT canQuitChanged();
    Q_EMIT canRaiseChanged();
    Q_EMIT canSetFullscreenChanged();
    Q_EMIT desktopEntryChanged();
    Q_EMIT fullscreenChanged();
    Q_EMIT hasTrackListChanged();
    Q_EMIT identityChanged();
    Q_EMIT supportedMimeTypesChanged();
    Q_EMIT supportedUriSchemesChanged();
}

void Control::emitPropertiesPlayer()
{
    Q_EMIT canControlChanged();
    Q_EMIT canGoNextChanged();
    Q_EMIT canGoPreviousChanged();
    Q_EMIT canPauseChanged();
    Q_EMIT canPlayChanged();
    Q_EMIT canSeekChanged();
    Q_EMIT loopStatusChanged();
    Q_EMIT maximumRateChanged();
    Q_EMIT metadataChanged();
    Q_EMIT minimumRateChanged();
    Q_EMIT playbackStatusChanged();
    Q_EMIT positionChanged();
    Q_EMIT rateChanged();
    Q_EMIT shuffleChanged();
    Q_EMIT volumeChanged();
}

void Control::connectSignals()
{
    QDBusConnection::sessionBus().connect(m_service, MPRIS2_PATH, FREEDESKTOP_PROPERTIES_IF, "PropertiesChanged", this, SLOT(onPropertiesChanged(QString, QVariantMap, QStringList)));
}

void Control::disconnectSignals()
{
    QDBusConnection::sessionBus().disconnect(m_service, MPRIS2_PATH, FREEDESKTOP_PROPERTIES_IF, "PropertiesChanged", this, SLOT(onPropertiesChanged(QString, QVariantMap, QStringList)));
}

void Control::schedulePing()
{
    QTimer::singleShot(1000, this, SLOT(sendPing()));
}

void Control::setDBusProperty(const QString &interface, const QString &name, const QVariant &value)
{
    if (propertiesIface)
        propertiesIface->call("Set", interface, name, value);
}

QVariantMap Control::getAllDBusProperties(const QString &interface)
{
    if (propertiesIface) {
        QDBusReply<QVariantMap> reply = propertiesIface->call("GetAll", interface);
        if (reply.isValid()) {
            return reply.value();
        }
        else {
            return QVariantMap();
        }
    }
    return QVariantMap();
}

void Control::onPropertiesChanged(const QString &interface, const QVariantMap &propertiesChanged, const QStringList &propertiesInvalidated)
{
    Q_UNUSED(interface)
    Q_UNUSED(propertiesInvalidated)

    QMapIterator<QString, QVariant> i(propertiesChanged);
    while (i.hasNext()) {
        i.next();
        if (interface == MPRIS2_MEDIAPLAYER_IF) {
            m_properties[i.key()] = i.value();
        }
        else if (interface == MPRIS2_MEDIAPLAYER_PLAYER_IF) {
            if (i.key() == "Metadata") {
                m_properties_player[i.key()] = convertMetadata(i.value());
            }
            else {
                if (i.key() == "PlaybackStatus") {
                    getAllPropertiesPlayer();

                    if (i.value().toString() == "Stopped") {
                        m_properties_player["Metadata"] = QVariantMap();
                    }
                }
                else {
                    m_properties_player[i.key()] = i.value();
                }
            }
        }
    }

    if (interface == MPRIS2_MEDIAPLAYER_IF) {
        emitProperties();
    }
    else if (interface == MPRIS2_MEDIAPLAYER_PLAYER_IF) {
        emitPropertiesPlayer();
    }
}

void Control::sendPing()
{
    if (peerIface && m_active && m_service != "") {
        QDBusMessage reply = peerIface->call("Ping");
        if (reply.type() == QDBusMessage::ErrorMessage) {
            setActive(false);
            m_properties.clear();
            emitProperties();
            m_properties_player.clear();
            emitPropertiesPlayer();
        }
        else {
            schedulePing();
        }
    }
}

