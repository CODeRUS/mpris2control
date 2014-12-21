#ifndef CONTROL_H
#define CONTROL_H

#include <QObject>
#include <QQmlParserStatus>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>

#define MPRIS2_PATH                     "/org/mpris/MediaPlayer2"
#define MPRIS2_MEDIAPLAYER_IF           "org.mpris.MediaPlayer2"
#define MPRIS2_MEDIAPLAYER_PLAYER_IF    "org.mpris.MediaPlayer2.Player"
#define FREEDESKTOP_PROPERTIES_IF       "org.freedesktop.DBus.Properties"
#define FREEDESKTOP_PEER_IF             "org.freedesktop.DBus.Peer"

class Control : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

public:
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)

    Q_PROPERTY(QString service READ service WRITE setService NOTIFY serviceChanged)

    Q_PROPERTY(bool canQuit READ canQuit NOTIFY canQuitChanged)
    Q_PROPERTY(bool canRaise READ canRaise NOTIFY canRaiseChanged)
    Q_PROPERTY(bool canSetFullscreen READ canSetFullscreen NOTIFY canSetFullscreenChanged)
    Q_PROPERTY(QString desktopEntry READ desktopEntry NOTIFY desktopEntryChanged)
    Q_PROPERTY(bool fullscreen READ fullscreen WRITE setFullscreen NOTIFY fullscreenChanged)
    Q_PROPERTY(bool hasTrackList READ hasTrackList NOTIFY hasTrackListChanged)
    Q_PROPERTY(QString identity READ identity NOTIFY identityChanged)
    Q_PROPERTY(QStringList supportedMimeTypes READ supportedMimeTypes NOTIFY supportedMimeTypesChanged)
    Q_PROPERTY(QStringList supportedUriSchemes READ supportedUriSchemes NOTIFY supportedUriSchemesChanged)
    Q_PROPERTY(bool canControl READ canControl NOTIFY canControlChanged)
    Q_PROPERTY(bool canGoNext READ canGoNext NOTIFY canGoNextChanged)
    Q_PROPERTY(bool canGoPrevious READ canGoPrevious NOTIFY canGoPreviousChanged)
    Q_PROPERTY(bool canPause READ canPause NOTIFY canPauseChanged)
    Q_PROPERTY(bool canPlay READ canPlay NOTIFY canPlayChanged)
    Q_PROPERTY(bool canSeek READ canSeek NOTIFY canSeekChanged)
    Q_PROPERTY(QString loopStatus READ loopStatus WRITE setLoopStatus NOTIFY loopStatusChanged)
    Q_PROPERTY(double maximumRate READ maximumRate NOTIFY maximumRateChanged)
    Q_PROPERTY(QVariantMap metadata READ metadata NOTIFY metadataChanged)
    Q_PROPERTY(double minimumRate READ minimumRate NOTIFY minimumRateChanged)
    Q_PROPERTY(QString playbackStatus READ playbackStatus NOTIFY playbackStatusChanged)
    Q_PROPERTY(qlonglong position READ position NOTIFY positionChanged)
    Q_PROPERTY(double rate READ rate WRITE setRate NOTIFY rateChanged)
    Q_PROPERTY(bool shuffle READ shuffle WRITE setShuffle NOTIFY shuffleChanged)
    Q_PROPERTY(double volume READ volume WRITE setVolume NOTIFY volumeChanged)

    Control(QObject *parent = 0);
    ~Control();

    void classBegin();
    void componentComplete();

    Q_INVOKABLE void initialize();

    Q_INVOKABLE void quit();
    Q_INVOKABLE void raise();
    Q_INVOKABLE void next();
    Q_INVOKABLE void openUri(const QString &uri);
    Q_INVOKABLE void pause();
    Q_INVOKABLE void play();
    Q_INVOKABLE void playPause();
    Q_INVOKABLE void previous();
    Q_INVOKABLE void seek(qlonglong offset);
    //method void org.mpris.MediaPlayer2.Player.SetPosition(QDBusObjectPath TrackId, qlonglong Position) //NOT IMPLEMENTED
    Q_INVOKABLE void stop();

    bool active() const;
    void setActive(bool newActive);

    QString service() const;
    void setService(const QString &newService);

    bool canQuit() const;
    bool canRaise() const;
    bool canSetFullscreen() const;
    QString desktopEntry() const;
    bool fullscreen() const;
    void setFullscreen(bool newFulscreen);
    bool hasTrackList() const;
    QString identity() const;
    QStringList supportedMimeTypes() const;
    QStringList supportedUriSchemes() const;
    bool canControl() const;
    bool canGoNext() const;
    bool canGoPrevious() const;
    bool canPause() const;
    bool canPlay() const;
    bool canSeek() const;
    QString loopStatus() const;
    void setLoopStatus(const QString newLoopStatus);
    double maximumRate() const;
    QVariantMap metadata() const;
    double minimumRate() const;
    QString playbackStatus() const;
    qlonglong position() const;
    double rate() const;
    void setRate(double newRate);
    bool shuffle() const;
    void setShuffle(bool newShuffle);
    double volume() const;
    void setVolume(double newVolume);

signals:
    void activeChanged();

    void serviceChanged();

    void canQuitChanged();
    void canRaiseChanged();
    void canSetFullscreenChanged();
    void desktopEntryChanged();
    void fullscreenChanged();
    void hasTrackListChanged();
    void identityChanged();
    void supportedMimeTypesChanged();
    void supportedUriSchemesChanged();
    void canControlChanged();
    void canGoNextChanged();
    void canGoPreviousChanged();
    void canPauseChanged();
    void canPlayChanged();
    void canSeekChanged();
    void loopStatusChanged();
    void maximumRateChanged();
    void metadataChanged();
    void minimumRateChanged();
    void playbackStatusChanged();
    void positionChanged();
    void rateChanged();
    void shuffleChanged();
    void volumeChanged();

private:
    void setDBusProperty(const QString &interface, const QString &name, const QVariant &value);
    QVariantMap getAllDBusProperties(const QString &interface);
    QVariantMap convertMetadata(const QVariant &dbusArgumentMetadata);

    void getAllProperties();
    void getAllPropertiesPlayer();

    void emitProperties();
    void emitPropertiesPlayer();

    void connectSignals();
    void disconnectSignals();

    void schedulePing();

    QScopedPointer<QDBusInterface> mprisIface;
    QScopedPointer<QDBusInterface> playerIface;
    QScopedPointer<QDBusInterface> propertiesIface;
    QScopedPointer<QDBusInterface> peerIface;

    bool m_active;

    QString m_service;

    QVariantMap m_properties;
    QVariantMap m_properties_player;

private slots:
    void onPropertiesChanged(const QString &interface, const QVariantMap &propertiesChanged, const QStringList &propertiesInvalidated);
    void sendPing();
};

#endif // CONTROL_H

