#ifndef ANIMATEDBOTBUTTON_H
#define ANIMATEDBOTBUTTON_H

#include <QPushButton>
#include <QVariantAnimation>
#include <QPainter>
#include <QPainterPath>
#include <QTimer>

class AnimatedBotButton : public QPushButton
{
    Q_OBJECT

public:
    explicit AnimatedBotButton(QWidget *parent = nullptr)
        : QPushButton(parent), m_eyeOpening(1.0), m_mouthCurvature(0.0)
    {
        setFixedSize(56, 56);
        setCursor(Qt::PointingHandCursor);
        setStyleSheet("border: none; background: transparent;");

        // Animation synchronisée paupière + sourire (durée 320 ms)
        m_blinkAnim = new QVariantAnimation(this);
        m_blinkAnim->setDuration(700);
        m_blinkAnim->setStartValue(0.0);
        m_blinkAnim->setKeyValueAt(0.5, 1.0); // Pic du clin d'œil + sourire renforcé
        m_blinkAnim->setEndValue(0.0);
        m_blinkAnim->setEasingCurve(QEasingCurve::InOutCubic);

        connect(m_blinkAnim, &QVariantAnimation::valueChanged, this, [this](const QVariant &val) {
            qreal progress = val.toReal(); // 0.0 -> 1.0 -> 0.0
            m_eyeOpening = 1.0 - progress; // De 1.0 (ouvert) à 0.0 (fermé)
            m_mouthCurvature = progress;   // De 0.0 (sourire normal) à 1.0 (grand sourire expressif)
            update();
        });

        // Déclencheur régulier chaque 1 seconde
        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, this, &AnimatedBotButton::startWink);
        m_timer->start(1500);
    }

private slots:
    void startWink() {
        if (m_blinkAnim->state() != QAbstractAnimation::Running) {
            m_blinkAnim->start();
        }
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, true);

        qreal w = width();
        qreal h = height();
        QPointF center(w / 2.0, h / 2.0);

        // 1. Couleurs inversées : Fond Dégradé Doré Prestige Fashionova
        QLinearGradient goldGrad(0, 0, w, h);
        goldGrad.setColorAt(0.0, QColor("#F6D888"));
        goldGrad.setColorAt(1.0, QColor("#C89538"));

        p.setBrush(goldGrad);
        p.setPen(QPen(QColor("#FFFFFF"), 2.2)); // Anneau extérieur blanc chic
        p.drawEllipse(center, (w / 2.0) - 2.5, (h / 2.0) - 2.5);

        // 2. Traits en Bleu Nuit profond Fashionova (#082849)
        QColor navyColor("#082849");
        p.setPen(Qt::NoPen);
        p.setBrush(navyColor);

        // Œil gauche (toujours grand ouvert et pétillant)
        p.drawEllipse(QPointF(w * 0.35, h * 0.39), 3.4, 4.8);
        p.setBrush(Qt::white);
        p.drawEllipse(QPointF(w * 0.33, h * 0.36), 1.2, 1.2); // Reflet

        // Œil droit animé (clin d'œil fluide)
        p.setBrush(navyColor);
        qreal rightEyeHeight = 4.8 * m_eyeOpening;
        if (m_eyeOpening > 0.25) {
            p.drawEllipse(QPointF(w * 0.65, h * 0.39), 3.4, qMax(rightEyeHeight, 0.8));
        } else {
            // Paupière fermée souriante (arc expressif)
            p.setPen(QPen(navyColor, 2.5, Qt::SolidLine, Qt::RoundCap));
            p.setBrush(Qt::NoBrush);
            QPainterPath arc;
            arc.moveTo(w * 0.58, h * 0.41);
            arc.quadTo(w * 0.65, h * 0.36, w * 0.72, h * 0.41);
            p.drawPath(arc);
        }

        // 3. Bouche animée (s'étire et s'ouvre légèrement en harmonie avec le clin d'œil)
        p.setPen(QPen(navyColor, 2.4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.setBrush(navyColor);

        qreal smileDrop = 0.68 + (0.06 * m_mouthCurvature); // La commissure descend légèrement
        qreal mouthSpread = 0.02 * m_mouthCurvature;

        QPainterPath mouth;
        mouth.moveTo(w * (0.31 - mouthSpread), h * 0.58);
        mouth.quadTo(w * 0.50, h * smileDrop, w * (0.69 + mouthSpread), h * 0.58);

        // Pendant le clin d'œil, la bouche s'entrouvre joyeusement
        if (m_mouthCurvature > 0.15) {
            mouth.quadTo(w * 0.50, h * (smileDrop - 0.04), w * (0.31 - mouthSpread), h * 0.58);
            mouth.closeSubpath();
            p.drawPath(mouth);
        } else {
            p.setBrush(Qt::NoBrush);
            p.drawPath(mouth);
        }
    }

private:
    QVariantAnimation *m_blinkAnim;
    QTimer *m_timer;
    qreal m_eyeOpening;
    qreal m_mouthCurvature;
};

#endif // ANIMATEDBOTBUTTON_H