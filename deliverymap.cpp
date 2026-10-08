#include "deliverymap.h"

#include <QCoreApplication>
#include <cmath>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMouseEvent>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPainter>
#include <QPainterPath>
#include <QStandardPaths>
#include <QToolButton>
#include <QWheelEvent>
#include <QtMath>

namespace {
constexpr double kPi = 3.14159265358979323846;
constexpr int kTuile = 256;
constexpr int kZoomMin = 3;
constexpr int kZoomMax = 18;
}

DeliveryMap::DeliveryMap(QWidget *parent) : QWidget(parent)
{
    setMouseTracking(true);
    setMinimumHeight(260);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setCursor(Qt::OpenHandCursor);

    // Serveur de tuiles / d'itinéraire (modifiables par variable d'environnement, ex. pour un serveur interne)
    m_urlTuiles = qEnvironmentVariable("STYLEHUB_TILE_URL", "https://tile.openstreetmap.org/%1/%2/%3.png");
    m_urlRoute  = qEnvironmentVariable("STYLEHUB_ROUTE_URL", "https://router.project-osrm.org/route/v1/driving");

    m_nam = new QNetworkAccessManager(this);
    auto *disque = new QNetworkDiskCache(m_nam);
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/tiles";
    QDir().mkpath(dir);
    disque->setCacheDirectory(dir);
    disque->setMaximumCacheSize(200LL * 1024 * 1024);
    m_nam->setCache(disque);

    auto bouton = [this](const QString &t) {
        auto *b = new QToolButton(this);
        b->setText(t);
        b->setCursor(Qt::PointingHandCursor);
        b->setFixedSize(30, 30);
        b->setStyleSheet("QToolButton{background:rgba(255,255,255,0.95);color:#0F172A;border:1px solid #CBD5E1;border-radius:6px;font-size:17px;font-weight:700;}"
                         "QToolButton:hover{background:#EEF3F8;}");
        return b;
    };
    m_btnPlus = bouton("+");
    m_btnMoins = bouton("–");
    connect(m_btnPlus, &QToolButton::clicked, this, [this] { zoomAt(rect().center(), +1); });
    connect(m_btnMoins, &QToolButton::clicked, this, [this] { zoomAt(rect().center(), -1); });
}

// ------------------------------------------------------------------ géométrie
double DeliveryMap::haversineKm(double lat1, double lon1, double lat2, double lon2)
{
    const double R = 6371.0;
    const double dLat = qDegreesToRadians(lat2 - lat1);
    const double dLon = qDegreesToRadians(lon2 - lon1);
    const double a = qSin(dLat / 2) * qSin(dLat / 2)
                   + qCos(qDegreesToRadians(lat1)) * qCos(qDegreesToRadians(lat2)) * qSin(dLon / 2) * qSin(dLon / 2);
    return 2 * R * qAsin(qSqrt(a));
}

QPointF DeliveryMap::project(double lat, double lon, int zoom)
{
    const double n = double(kTuile) * (1 << zoom);
    const double s = qSin(qDegreesToRadians(qBound(-85.0511, lat, 85.0511)));
    return QPointF((lon + 180.0) / 360.0 * n,
                   (0.5 - qLn((1 + s) / (1 - s)) / (4 * kPi)) * n);
}

QPointF DeliveryMap::unproject(const QPointF &px, int zoom)
{
    const double n = double(kTuile) * (1 << zoom);
    const double lon = px.x() / n * 360.0 - 180.0;
    const double a = kPi * (1 - 2 * px.y() / n);
    const double lat = qRadiansToDegrees(qAtan(std::sinh(a)));
    return QPointF(lat, lon);     // x = lat, y = lon
}

QPointF DeliveryMap::topLeftPx() const
{
    return project(m_centreLat, m_centreLon, m_zoom) - QPointF(width() / 2.0, height() / 2.0);
}

QPointF DeliveryMap::toScreen(double lat, double lon) const
{
    return project(lat, lon, m_zoom) - topLeftPx();
}

void DeliveryMap::setCenterPx(const QPointF &px)
{
    const QPointF ll = unproject(px, m_zoom);
    m_centreLat = qBound(-85.0, ll.x(), 85.0);
    m_centreLon = ll.y();
    update();
}

void DeliveryMap::zoomAt(const QPoint &pos, int delta)
{
    const int nz = qBound(kZoomMin, m_zoom + delta, kZoomMax);
    if (nz == m_zoom) return;
    const QPointF sous = unproject(topLeftPx() + pos, m_zoom);      // point sous le curseur
    m_zoom = nz;
    const QPointF px = project(sous.x(), sous.y(), m_zoom) - pos + QPointF(width() / 2.0, height() / 2.0);
    setCenterPx(px);
}

void DeliveryMap::fitPoints(const QVector<QPointF> &pts)
{
    if (pts.isEmpty() || width() < 50 || height() < 50) return;
    double minLat = 90, maxLat = -90, minLon = 180, maxLon = -180;
    for (const QPointF &p : pts) {
        minLat = qMin(minLat, p.x()); maxLat = qMax(maxLat, p.x());
        minLon = qMin(minLon, p.y()); maxLon = qMax(maxLon, p.y());
    }
    const double marge = 70;
    int z = kZoomMax;
    for (; z > kZoomMin; --z) {
        const QPointF a = project(maxLat, minLon, z), b = project(minLat, maxLon, z);
        if (qAbs(b.x() - a.x()) <= width() - 2 * marge && qAbs(b.y() - a.y()) <= height() - 2 * marge) break;
    }
    m_zoom = z;
    const QPointF c = (project(maxLat, minLon, z) + project(minLat, maxLon, z)) / 2.0;
    setCenterPx(c);
}

void DeliveryMap::fitAll()
{
    QVector<QPointF> pts{ QPointF(kAtelierLat, kAtelierLon) };
    for (const Marker &m : m_markers) pts.append(QPointF(m.lat, m.lon));
    fitPoints(pts);
}

// ------------------------------------------------------------------ API publique
void DeliveryMap::setMarkers(const QList<Marker> &markers)
{
    m_markers = markers;
    m_ajusteInitial = false;
    if (width() > 50) { fitAll(); m_ajusteInitial = true; }
    update();
}

const DeliveryMap::Marker *DeliveryMap::markerById(const QString &id) const
{
    for (const Marker &m : m_markers) if (m.id == id) return &m;
    return nullptr;
}

void DeliveryMap::selectOrder(const QString &id, bool centrer)
{
    m_selection = id;
    if (m_routeId != id) { m_route.clear(); m_routeId.clear(); m_routeEnCours = false; ++m_routeSerie; }
    if (centrer) if (const Marker *m = markerById(id)) {
        m_centreLat = m->lat; m_centreLon = m->lon;
    }
    update();
}

void DeliveryMap::centerWorkshop()
{
    m_centreLat = kAtelierLat; m_centreLon = kAtelierLon;
    m_zoom = qMax(m_zoom, 13);
    update();
}

void DeliveryMap::showRoute(const QString &id)
{
    const Marker *m = markerById(id);
    if (!m) return;
    m_selection = id;
    m_routeId = id;
    m_route.clear();
    m_routeApprox = false;
    m_routeEnCours = true;
    const int serie = ++m_routeSerie;

    // Affichage immédiat de l'ensemble atelier + client
    fitPoints({ QPointF(kAtelierLat, kAtelierLon), QPointF(m->lat, m->lon) });

    const QString url = QString("%1/%2,%3;%4,%5?overview=full&geometries=geojson")
                            .arg(m_urlRoute).arg(kAtelierLon, 0, 'f', 6).arg(kAtelierLat, 0, 'f', 6)
                            .arg(m->lon, 0, 'f', 6).arg(m->lat, 0, 'f', 6);
    QNetworkRequest req{QUrl(url)};
    req.setHeader(QNetworkRequest::UserAgentHeader, "StyleHub-Fashionova/1.0 (projet etudiant ESPRIT)");
    req.setTransferTimeout(8000);
    QNetworkReply *rep = m_nam->get(req);

    const QString idCopie = id;
    const double lat = m->lat, lon = m->lon;
    connect(rep, &QNetworkReply::finished, this, [this, rep, serie, idCopie, lat, lon] {
        rep->deleteLater();
        if (serie != m_routeSerie) return;                 // obsolète (autre commande sélectionnée)
        m_routeEnCours = false;

        bool ok = false;
        if (rep->error() == QNetworkReply::NoError) {
            const QJsonObject o = QJsonDocument::fromJson(rep->readAll()).object();
            const QJsonArray routes = o.value("routes").toArray();
            if (!routes.isEmpty()) {
                const QJsonObject r = routes.first().toObject();
                const QJsonArray coords = r.value("geometry").toObject().value("coordinates").toArray();
                QVector<QPointF> pts;
                pts.reserve(coords.size());
                for (const QJsonValue &c : coords) {
                    const QJsonArray a = c.toArray();
                    if (a.size() >= 2) pts.append(QPointF(a.at(1).toDouble(), a.at(0).toDouble()));
                }
                if (pts.size() >= 2) {
                    m_route = pts;
                    m_routeApprox = false;
                    ok = true;
                    emit routeReady(idCopie, r.value("distance").toDouble() / 1000.0,
                                    qRound(r.value("duration").toDouble() / 60.0), false);
                    fitPoints(m_route);
                }
            }
        }
        if (!ok) {                                         // hors-ligne : ligne droite
            m_route = { QPointF(kAtelierLat, kAtelierLon), QPointF(lat, lon) };
            m_routeApprox = true;
            const double km = haversineKm(kAtelierLat, kAtelierLon, lat, lon);
            emit routeReady(idCopie, km, qRound(km / 30.0 * 60.0), true);
        }
        update();
    });
    update();
}

// ------------------------------------------------------------------ tuiles
QPixmap DeliveryMap::tuile(int z, int x, int y)
{
    const QString cle = QString("%1/%2/%3").arg(z).arg(x).arg(y);
    auto it = m_cache.constFind(cle);
    if (it != m_cache.constEnd()) return it.value();
    demanderTuile(z, x, y);
    return QPixmap();
}

void DeliveryMap::demanderTuile(int z, int x, int y)
{
    const QString cle = QString("%1/%2/%3").arg(z).arg(x).arg(y);
    if (m_enAttente.contains(cle)) return;
    m_enAttente.insert(cle);

    QNetworkRequest req{QUrl(m_urlTuiles.arg(z).arg(x).arg(y))};
    // La politique d'OpenStreetMap exige un User-Agent identifiant l'application
    req.setHeader(QNetworkRequest::UserAgentHeader, "StyleHub-Fashionova/1.0 (projet etudiant ESPRIT)");
    req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    req.setTransferTimeout(10000);
    QNetworkReply *rep = m_nam->get(req);
    connect(rep, &QNetworkReply::finished, this, [this, rep, cle] {
        rep->deleteLater();
        m_enAttente.remove(cle);
        QPixmap pm;
        if (rep->error() == QNetworkReply::NoError && pm.loadFromData(rep->readAll())) {
            if (m_cache.size() > 600) m_cache.clear();
            m_cache.insert(cle, pm);
            ++m_tuilesChargees;
        } else {
            ++m_tuilesEchec;
        }
        update();
    });
}

// ------------------------------------------------------------------ dessin
QColor DeliveryMap::couleurStatut(const QString &s)
{
    if (s.startsWith("Livr")) return QColor("#10B981");
    if (s.startsWith("Pr"))   return QColor("#2563EB");
    return QColor("#F59E0B");     // En cours d'expédition
}

void DeliveryMap::drawPin(QPainter &p, const QPointF &a, const QColor &c, double e) const
{
    QPainterPath path;
    path.moveTo(14, 0.5);
    path.cubicTo(6.54416, 0.5, 0.5, 6.54416, 0.5, 14);
    path.cubicTo(0.5, 23.5, 14, 35.5, 14, 35.5);
    path.cubicTo(14, 35.5, 27.5, 23.5, 27.5, 14);
    path.cubicTo(27.5, 6.54416, 21.4558, 0.5, 14, 0.5);
    path.closeSubpath();

    p.save();
    p.translate(a);
    p.scale(e, e);
    p.translate(-14, -36);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0, 0, 0, 50));                    // ombre
    p.drawEllipse(QPointF(14, 36), 7, 2.5);
    p.setPen(QPen(Qt::white, 2));
    p.setBrush(c);
    p.drawPath(path);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::white);
    p.drawEllipse(QPointF(14, 13.5), 5, 5);
    p.setBrush(c);
    p.drawEllipse(QPointF(14, 13.5), 2.8, 2.8);
    p.restore();
}

void DeliveryMap::drawAtelier(QPainter &p, const QPointF &c) const
{
    p.save();
    p.translate(c);
    p.setPen(QPen(Qt::white, 2));
    p.setBrush(QColor("#082849"));
    p.drawRoundedRect(QRectF(-15, -15, 30, 30), 8, 8);
    // pictogramme d'usine
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::white);
    QPolygonF toit;
    toit << QPointF(-9, 7) << QPointF(-9, -1) << QPointF(-3, -5) << QPointF(-3, -1) << QPointF(3, -5)
         << QPointF(3, -1) << QPointF(9, -5) << QPointF(9, 7);
    p.drawPolygon(toit);
    p.setBrush(QColor("#082849"));
    p.drawRect(QRectF(-6, 1, 3, 3));
    p.drawRect(QRectF(-1, 1, 3, 3));
    p.drawRect(QRectF(4, 1, 3, 3));
    p.restore();
}

void DeliveryMap::drawBulle(QPainter &p, const QPointF &a, const QString &texte, double hPin) const
{
    QFont f = p.font(); f.setPointSizeF(8.5); f.setBold(true);
    p.save();
    p.setFont(f);
    const QFontMetrics fm(f);
    QRectF r(0, 0, fm.horizontalAdvance(texte) + 18, 24);
    r.moveCenter(QPointF(a.x(), a.y() - hPin - 18));
    // rester dans le widget
    if (r.left() < 4) r.moveLeft(4);
    if (r.right() > width() - 4) r.moveRight(width() - 4);
    if (r.top() < 4) r.moveTop(a.y() + 8);
    p.setPen(QPen(QColor("#CBD5E1"), 1));
    p.setBrush(QColor(255, 255, 255, 245));
    p.drawRoundedRect(r, 6, 6);
    p.setPen(QColor("#0F172A"));
    p.drawText(r, Qt::AlignCenter, texte);
    p.restore();
}

void DeliveryMap::drawLegende(QPainter &p) const
{
    struct L { QColor c; QString t; };
    const QList<L> items = { { QColor("#10B981"), "Livrée" }, { QColor("#F59E0B"), "En cours d'expédition" },
                             { QColor("#2563EB"), "Prête en atelier" } };
    QFont f = p.font(); f.setPointSizeF(8);
    p.save();
    p.setFont(f);
    const QRectF r(10, height() - 10 - 100, 158, 100);
    p.setPen(QPen(QColor("#E2E8F0"), 1));
    p.setBrush(QColor(255, 255, 255, 240));
    p.drawRoundedRect(r, 8, 8);
    f.setBold(true); p.setFont(f);
    p.setPen(QColor("#0F172A"));
    p.drawText(QPointF(r.left() + 10, r.top() + 18), "Statut des livraisons");
    f.setBold(false); p.setFont(f);
    double y = r.top() + 34;
    for (const L &it : items) {
        drawPin(p, QPointF(r.left() + 18, y + 9), it.c, 0.5);
        p.setPen(QColor("#1E293B"));
        p.drawText(QPointF(r.left() + 32, y + 5), it.t);
        y += 22;
    }
    p.restore();
}

void DeliveryMap::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    QPainterPath clip;
    clip.addRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), 10, 10);
    p.setClipPath(clip);
    p.fillRect(rect(), QColor("#E8EEF4"));

    // --- tuiles
    const QPointF tl = topLeftPx();
    const int n = 1 << m_zoom;
    const int tx0 = qFloor(tl.x() / kTuile), tx1 = qFloor((tl.x() + width()) / kTuile);
    const int ty0 = qFloor(tl.y() / kTuile), ty1 = qFloor((tl.y() + height()) / kTuile);
    for (int ty = ty0; ty <= ty1; ++ty) {
        if (ty < 0 || ty >= n) continue;
        for (int tx = tx0; tx <= tx1; ++tx) {
            const int wx = ((tx % n) + n) % n;
            const QRectF cible(tx * kTuile - tl.x(), ty * kTuile - tl.y(), kTuile, kTuile);
            const QPixmap pm = tuile(m_zoom, wx, ty);
            if (!pm.isNull()) p.drawPixmap(cible, pm, QRectF(pm.rect()));
            else {
                p.setPen(QPen(QColor("#DCE4EC"), 1));
                p.setBrush(QColor("#EEF3F8"));
                p.drawRect(cible);
            }
        }
    }

    // --- itinéraire
    if (m_route.size() >= 2) {
        QPolygonF poly;
        for (const QPointF &ll : m_route) poly << toScreen(ll.x(), ll.y());
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(Qt::white, 8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawPolyline(poly);
        QPen pen(QColor("#2563EB"), 4, m_routeApprox ? Qt::DashLine : Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        p.setPen(pen);
        p.drawPolyline(poly);
    }

    // --- marqueurs
    drawAtelier(p, toScreen(kAtelierLat, kAtelierLon));
    for (const Marker &m : m_markers) {
        if (m.id == m_selection) continue;
        drawPin(p, toScreen(m.lat, m.lon), couleurStatut(m.statut), m.id == m_survol ? 1.15 : 1.0);
    }
    if (const Marker *s = markerById(m_selection)) {
        const QPointF a = toScreen(s->lat, s->lon);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(37, 99, 235, 45));
        p.drawEllipse(a, 22, 22);
        drawPin(p, a, couleurStatut(s->statut), 1.3);
        drawBulle(p, a, QString("%1 · %2").arg(s->id, s->nom), 36 * 1.3);
    } else if (!m_survol.isEmpty()) {
        if (const Marker *h = markerById(m_survol))
            drawBulle(p, toScreen(h->lat, h->lon), QString("%1 · %2").arg(h->id, h->nom), 36);
    }
    if (m_survolAtelier)
        drawBulle(p, toScreen(kAtelierLat, kAtelierLon), "Atelier central", 14);

    // --- message d'état
    QFont f = p.font(); f.setPointSizeF(9);
    p.setFont(f);
    if (m_tuilesChargees == 0 && m_tuilesEchec > 0) {
        const QRectF r(width() / 2.0 - 190, 12, 380, 26);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(254, 243, 199, 240));
        p.drawRoundedRect(r, 6, 6);
        p.setPen(QColor("#92400E"));
        p.drawText(r, Qt::AlignCenter, "Fond de carte indisponible : vérifiez la connexion Internet");
    } else if (m_routeEnCours) {
        const QRectF r(width() / 2.0 - 90, 12, 180, 26);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(255, 255, 255, 240));
        p.drawRoundedRect(r, 6, 6);
        p.setPen(QColor("#1E293B"));
        p.drawText(r, Qt::AlignCenter, "Calcul de l'itinéraire…");
    } else if (m_routeApprox && !m_route.isEmpty()) {
        const QRectF r(width() / 2.0 - 150, 12, 300, 26);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(255, 255, 255, 240));
        p.drawRoundedRect(r, 6, 6);
        p.setPen(QColor("#92400E"));
        p.drawText(r, Qt::AlignCenter, "Itinéraire hors-ligne : tracé en ligne droite");
    }

    drawLegende(p);

    // --- attribution obligatoire (licence ODbL)
    f.setPointSizeF(7.5); p.setFont(f);
    const QString attr = "© OpenStreetMap contributors";
    const QFontMetrics fm(f);
    const QRectF ra(width() - fm.horizontalAdvance(attr) - 16, height() - 20, fm.horizontalAdvance(attr) + 12, 16);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(255, 255, 255, 200));
    p.drawRoundedRect(ra, 4, 4);
    p.setPen(QColor("#334155"));
    p.drawText(ra, Qt::AlignCenter, attr);

    // --- bordure
    p.setClipping(false);
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(QColor("#D7E0EA"), 1));
    p.drawRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), 10, 10);
}

// ------------------------------------------------------------------ événements
void DeliveryMap::resizeEvent(QResizeEvent *)
{
    m_btnPlus->move(10, 10);
    m_btnMoins->move(10, 44);
    if (!m_ajusteInitial && !m_markers.isEmpty() && width() > 50 && height() > 50) {
        fitAll();
        m_ajusteInitial = true;
    }
}

QString DeliveryMap::markerAt(const QPoint &pos, bool *atelier) const
{
    if (atelier) *atelier = false;
    for (int i = m_markers.size() - 1; i >= 0; --i) {            // du dessus vers le dessous
        const QPointF a = toScreen(m_markers[i].lat, m_markers[i].lon);
        const bool sel = m_markers[i].id == m_selection;
        const double e = sel ? 1.3 : 1.0;
        if (QRectF(a.x() - 14 * e, a.y() - 36 * e, 28 * e, 36 * e).contains(pos)) return m_markers[i].id;
    }
    if (QRectF(toScreen(kAtelierLat, kAtelierLon) - QPointF(15, 15), QSizeF(30, 30)).contains(pos) && atelier)
        *atelier = true;
    return QString();
}

void DeliveryMap::mousePressEvent(QMouseEvent *e)
{
    if (e->button() != Qt::LeftButton) return;
    m_glisse = true;
    m_aBouge = false;
    m_pressPos = e->pos();
    m_pressCentrePx = project(m_centreLat, m_centreLon, m_zoom);
    setCursor(Qt::ClosedHandCursor);
}

void DeliveryMap::mouseMoveEvent(QMouseEvent *e)
{
    if (m_glisse) {
        const QPoint d = e->pos() - m_pressPos;
        if (d.manhattanLength() > 4) m_aBouge = true;
        if (m_aBouge) setCenterPx(m_pressCentrePx - QPointF(d));
        return;
    }
    bool at = false;
    const QString id = markerAt(e->pos(), &at);
    if (id != m_survol || at != m_survolAtelier) {
        m_survol = id;
        m_survolAtelier = at;
        setCursor((!id.isEmpty() || at) ? Qt::PointingHandCursor : Qt::OpenHandCursor);
        update();
    }
}

void DeliveryMap::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() != Qt::LeftButton) return;
    const bool clic = m_glisse && !m_aBouge;
    m_glisse = false;
    setCursor(m_survol.isEmpty() ? Qt::OpenHandCursor : Qt::PointingHandCursor);
    if (clic) {
        const QString id = markerAt(e->pos());
        if (!id.isEmpty()) {
            selectOrder(id, false);
            emit markerClicked(id);
        }
    }
}

void DeliveryMap::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (markerAt(e->pos()).isEmpty()) zoomAt(e->pos(), +1);
}

void DeliveryMap::wheelEvent(QWheelEvent *e)
{
    const int d = e->angleDelta().y();
    if (d != 0) zoomAt(e->position().toPoint(), d > 0 ? +1 : -1);
    e->accept();
}

void DeliveryMap::leaveEvent(QEvent *)
{
    if (!m_survol.isEmpty() || m_survolAtelier) { m_survol.clear(); m_survolAtelier = false; update(); }
}
