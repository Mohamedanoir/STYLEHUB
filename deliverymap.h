#ifndef DELIVERYMAP_H
#define DELIVERYMAP_H

#include <QHash>
#include <QList>
#include <QPixmap>
#include <QPointF>
#include <QSet>
#include <QString>
#include <QVector>
#include <QWidget>

class QNetworkAccessManager;
class QNetworkReply;
class QToolButton;

// Carte interactive des livraisons, 100 % Qt Widgets (aucun Qt WebEngine requis).
//  - fond de carte OpenStreetMap (tuiles téléchargées + cache disque) -> connexion Internet nécessaire
//  - déplacement à la souris, zoom molette / double-clic / boutons + et -
//  - atelier central + un marqueur par commande, coloré selon le statut
//  - itinéraire atelier -> client (OSRM, avec repli en ligne droite hors-ligne)
class DeliveryMap : public QWidget
{
    Q_OBJECT
public:
    struct Marker {
        QString id;       // ex. CMD-002
        QString nom;      // nom du client
        QString statut;   // "Livrée", "En cours", "Prête en atelier"
        double lat = 0;
        double lon = 0;
    };

    // Position de l'atelier central (point de départ des livraisons) : à adapter ici
    static constexpr double kAtelierLat = 36.8120;
    static constexpr double kAtelierLon = 10.1700;

    explicit DeliveryMap(QWidget *parent = nullptr);

    void setMarkers(const QList<Marker> &markers);
    void selectOrder(const QString &id, bool centrer = true);
    void showRoute(const QString &id);
    void centerWorkshop();
    void fitAll();

    static double haversineKm(double lat1, double lon1, double lat2, double lon2);

    QSize minimumSizeHint() const override { return QSize(360, 260); }

signals:
    void markerClicked(const QString &id);
    // km / minutes de l'itinéraire ; approximatif = true si calculé hors-ligne (ligne droite)
    void routeReady(const QString &id, double km, int minutes, bool approximatif);

protected:
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseDoubleClickEvent(QMouseEvent *) override;
    void wheelEvent(QWheelEvent *) override;
    void leaveEvent(QEvent *) override;

private:
    // projection Web Mercator (tuiles de 256 px)
    static QPointF project(double lat, double lon, int zoom);
    static QPointF unproject(const QPointF &px, int zoom);
    QPointF topLeftPx() const;
    QPointF toScreen(double lat, double lon) const;

    void setCenterPx(const QPointF &px);
    void zoomAt(const QPoint &pos, int delta);
    void fitPoints(const QVector<QPointF> &latLon);   // x = lat, y = lon
    const Marker *markerById(const QString &id) const;
    QString markerAt(const QPoint &pos, bool *atelier = nullptr) const;
    static QColor couleurStatut(const QString &statut);

    QPixmap tuile(int z, int x, int y);
    void demanderTuile(int z, int x, int y);

    void drawPin(QPainter &p, const QPointF &ancre, const QColor &c, double echelle) const;
    void drawAtelier(QPainter &p, const QPointF &centre) const;
    void drawLegende(QPainter &p) const;
    void drawBulle(QPainter &p, const QPointF &ancre, const QString &texte, double hauteurPin) const;

    QList<Marker> m_markers;
    double m_centreLat = 36.83;
    double m_centreLon = 10.19;
    int    m_zoom = 12;

    QString m_selection;
    QString m_survol;
    bool    m_survolAtelier = false;

    // route
    QVector<QPointF> m_route;      // x = lat, y = lon
    QString m_routeId;
    bool    m_routeEnCours = false;
    bool    m_routeApprox = false;
    int     m_routeSerie = 0;

    // glisser
    bool    m_glisse = false;
    QPoint  m_pressPos;
    QPointF m_pressCentrePx;
    bool    m_aBouge = false;

    // tuiles
    QNetworkAccessManager *m_nam = nullptr;
    QHash<QString, QPixmap> m_cache;
    QSet<QString> m_enAttente;
    int m_tuilesChargees = 0;
    int m_tuilesEchec = 0;
    QString m_urlTuiles;
    QString m_urlRoute;

    bool m_ajusteInitial = false;
    QToolButton *m_btnPlus = nullptr;
    QToolButton *m_btnMoins = nullptr;
};

#endif // DELIVERYMAP_H
