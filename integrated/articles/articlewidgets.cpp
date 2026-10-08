#include "articlewidgets.h"
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QPolygonF>
#include <QRadialGradient>
#include <QFontMetrics>
#include <QFont>
#include <QPaintEvent>
#include <algorithm>
#include <cmath>

static const QColor COL[5]={QColor("#0a2a4a"),QColor("#6fa3d8"),QColor("#b9d3ee"),QColor("#ecc98f"),QColor("#d9a566")};
static const QColor INK("#1d2a3a"),MUT("#6b7686");

/* ---------- Graphiques ---------- */
ChartWidget::ChartWidget(QWidget*p):QWidget(p){setMinimumHeight(100);}
void ChartWidget::setMode(Mode m){mode=m;update();}
void ChartWidget::setData(const QVector<QPair<QString,double>>&d){data=d;update();}
void ChartWidget::paintEvent(QPaintEvent*){
    QPainter p(this);p.setRenderHint(QPainter::Antialiasing);
    double sum=0,mx=1;for(auto&d:data){sum+=d.second;mx=std::max(mx,d.second);}
    QFont f=font();f.setPointSize(9);p.setFont(f);
    if(mode==Donut){
        int s=std::max(60,std::min(height()-10,140));QRect r(5,5,s,s);int a=90*16;
        for(int i=0;i<data.size();++i){
            int span=sum?int(-data[i].second/sum*360*16):0;
            p.setPen(Qt::NoPen);p.setBrush(COL[i%5]);p.drawPie(r,a,span);a+=span;}
        p.setBrush(Qt::white);int h=s*6/10;p.drawEllipse(r.center(),h/2,h/2);
        QFont b=f;b.setPointSize(16);b.setBold(true);p.setFont(b);p.setPen(INK);
        p.drawText(QRect(r.x(),r.y()+s/2-22,s,26),Qt::AlignCenter,QString::number(int(sum)));
        p.setFont(f);p.setPen(MUT);p.drawText(QRect(r.x(),r.y()+s/2+2,s,16),Qt::AlignCenter,"total");
        int st=std::max(16,std::min(24,(height()-8)/std::max(1,int(data.size()))));int y=4;
        for(int i=0;i<data.size();++i){
            p.setBrush(COL[i%5]);p.setPen(Qt::NoPen);p.drawRoundedRect(s+22,y,12,12,3,3);
            p.setPen(INK);
            p.drawText(s+42,y+11,QString("%1  %2 (%3%)").arg(data[i].first).arg(int(data[i].second)).arg(sum?qRound(data[i].second/sum*100):0));
            y+=st;}
    }else if(mode==Line){
        int n=data.size();if(n<2)return;
        int L=30,R=width()-15,B=height()-25,T=15;
        QPolygonF poly;poly<<QPointF(L,B);QVector<QPointF> pts;
        for(int i=0;i<n;++i){QPointF pt(L+(R-L)*i/double(n-1),B-(B-T)*data[i].second/mx);pts<<pt;poly<<pt;}
        poly<<QPointF(R,B);
        p.setPen(Qt::NoPen);p.setBrush(QColor("#dbe6f3"));p.drawPolygon(poly);
        p.setPen(QPen(QColor("#0f2d55"),2));for(int i=1;i<n;++i)p.drawLine(pts[i-1],pts[i]);
        p.setBrush(QColor("#0f2d55"));for(auto&pt:pts)p.drawEllipse(pt,4,4);
        p.setPen(MUT);for(int i=0;i<n;++i)p.drawText(QRectF(pts[i].x()-25,B+5,50,16),Qt::AlignCenter,data[i].first);
    }else{
        int st=std::max(18,std::min(28,(height()-8)/std::max(1,int(data.size()))));int y=4;
        for(int i=0;i<data.size();++i){
            p.setPen(INK);p.drawText(QRect(0,y,110,18),Qt::AlignVCenter,data[i].first);
            int bx=115,bw=width()-bx-40;p.setPen(Qt::NoPen);
            p.setBrush(QColor("#eee7db"));p.drawRoundedRect(bx,y+4,bw,10,5,5);
            p.setBrush(COL[i%5]);p.drawRoundedRect(bx,y+4,int(bw*data[i].second/mx),10,5,5);
            p.setPen(INK);p.drawText(QRect(width()-34,y,34,18),Qt::AlignVCenter|Qt::AlignRight,QString::number(int(data[i].second)));
            y+=st;}
    }
}


/* ---------- Bannière (image tissu + machine, textes dessinés par-dessus) ---------- */
// banner.png = 1002x112 : [0..150] plis à gauche (fixe) | [150..478] fond uni (étiré) | [478..1002] machine + zone citation (fixe)
BannerWidget::BannerWidget(QWidget*p):QWidget(p),bg(":/banner.png"){setFixedHeight(104);}
void BannerWidget::setTexts(const QString&t,const QString&s,const QStringList&q){title=t;sub=s;quote=q;update();}
void BannerWidget::paintEvent(QPaintEvent*){
    QPainter p(this);p.setRenderHint(QPainter::Antialiasing);p.setRenderHint(QPainter::TextAntialiasing);p.setRenderHint(QPainter::SmoothPixmapTransform);
    const double W=width(),H=height(),PI=3.14159265358979;
    p.fillRect(rect(),QColor("#efe6d8"));
    if(bg.isNull())return;
    const double k=H/bg.height();                       // échelle verticale (sans déformer la machine)
    const double leftW=150*k,rightW=(bg.width()-478)*k; // tranches fixes
    const double xR=W-rightW;                           // début de la tranche droite
    p.drawPixmap(QRectF(0,0,leftW,H),bg,QRectF(0,0,150,bg.height()));
    if(xR>leftW)p.drawPixmap(QRectF(leftW,0,xR-leftW,H),bg,QRectF(150,0,328,bg.height()));
    p.drawPixmap(QRectF(xR,0,rightW,H),bg,QRectF(478,0,bg.width()-478,bg.height()));

    // Titre + sous-titre + séparateur
    const double x0=150*k+6,xT=xR+120*k,cx=(x0+xT)/2,maxW=std::max(120.0,xT-x0-16);   // le titre peut déborder sur le tissu libre avant la machine/objets
    QFont ft("Georgia");ft.setPixelSize(int(H*0.34));ft.setWeight(QFont::DemiBold);
    while(ft.pixelSize()>12&&QFontMetrics(ft).boundingRect(title).width()>maxW)ft.setPixelSize(ft.pixelSize()-1);
    p.setFont(ft);p.setPen(QColor("#14233d"));
    p.drawText(QRectF(cx-maxW/2,H*0.10,maxW,H*0.42),Qt::AlignCenter,title);
    QFont fs("Segoe UI");fs.setStyleHint(QFont::SansSerif);fs.setPixelSize(int(H*0.14));p.setFont(fs);p.setPen(QColor("#6e5e4d"));
    p.drawText(QRectF(cx-maxW/2,H*0.54,maxW,H*0.2),Qt::AlignCenter,sub);
    const double ly=H*0.84,gap=H*0.10,len=maxW*0.26;
    p.setPen(QPen(QColor("#c4b5a2"),1));
    p.drawLine(QPointF(cx-gap-len,ly),QPointF(cx-gap,ly));p.drawLine(QPointF(cx+gap,ly),QPointF(cx+gap+len,ly));
    QPainterPath st;const double r=H*0.075,ri=r*0.28;
    for(int i=0;i<8;++i){double ang=PI/4*i-PI/2,rr=(i%2==0)?r:ri;QPointF pt(cx+rr*std::cos(ang),ly+rr*std::sin(ang));if(i)st.lineTo(pt);else st.moveTo(pt);}
    st.closeSubpath();p.setPen(Qt::NoPen);p.setBrush(QColor("#14233d"));p.drawPath(st);

    // Citation à droite (zone src 828..1002)
    const double qx=xR+(915-478)*k,qw=160*k;
    QFont fq("Georgia");fq.setItalic(true);fq.setPixelSize(int(H*0.15));
    while(fq.pixelSize()>9){int mw=0;for(auto&l:quote)mw=std::max(mw,QFontMetrics(fq).boundingRect(l).width());if(mw<=qw)break;fq.setPixelSize(fq.pixelSize()-1);}
    p.setFont(fq);p.setPen(QColor("#3f4b5e"));
    const double lh=H*0.17,y0=H*0.49-lh*quote.size()/2;
    for(int i=0;i<quote.size();++i)p.drawText(QRectF(qx-qw/2,y0+i*lh,qw,lh),Qt::AlignCenter,quote[i]);
    QFont fm("Georgia");fm.setPixelSize(int(H*0.30));fm.setBold(true);p.setFont(fm);p.setPen(QColor("#c9a26a"));
    p.drawText(QRectF(xR+(836-478)*k,H*0.10,H*0.4,H*0.4),Qt::AlignLeft|Qt::AlignTop,QString(QChar(0x201C)));
    p.drawText(QRectF(W-H*0.4-2*k,H*0.62,H*0.4,H*0.4),Qt::AlignRight|Qt::AlignTop,QString(QChar(0x201D)));
}



SideWidget::SideWidget(QWidget*p):QFrame(p),bg(":/sidebar.png"){}
void SideWidget::paintEvent(QPaintEvent*){
    QPainter g(this);g.setRenderHint(QPainter::SmoothPixmapTransform);
    g.fillRect(rect(),QColor("#0a2a4a"));
    if(bg.isNull())return;
    const int w=width(),h=height(),srcTop=270,srcBot=1130;   // tranche haute (logo) / basse (mannequin)
    const double k=double(w)/bg.width();
    const int topH=qRound(srcTop*k),botH=qRound((bg.height()-srcBot)*k),midH=h-topH-botH;
    if(midH>0)g.drawPixmap(QRect(0,topH,w,midH),bg,QRect(0,srcTop,bg.width(),srcBot-srcTop));
    g.drawPixmap(QRect(0,0,w,topH),bg,QRect(0,0,bg.width(),srcTop));
    g.drawPixmap(QRect(0,h-botH,w,botH),bg,QRect(0,srcBot,bg.width(),bg.height()-srcBot));
}


/* ---------- Icônes de la barre latérale (20x20, rendues en 40x40 pour l'écran HiDPI) ---------- */
static QPixmap iconPix(int idx,const QColor&c){
    const int S=40;QPixmap pm(S,S);pm.fill(Qt::transparent);
    QPainter p(&pm);p.setRenderHint(QPainter::Antialiasing);p.setPen(Qt::NoPen);p.setBrush(c);
    const QPen line(c,2.6,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin);
    auto person=[&](double cx,double top,double s){   // tête + épaules
        p.drawEllipse(QPointF(cx,top+6*s),5.2*s,5.2*s);
        QPainterPath sh;sh.moveTo(cx-9*s,top+22*s);sh.cubicTo(cx-9*s,top+13*s,cx+9*s,top+13*s,cx+9*s,top+22*s);sh.closeSubpath();p.drawPath(sh);};
    switch(idx){
    case 0:{ // Clients : deux silhouettes
        QColor back=c;back.setAlpha(150);p.setBrush(back);person(27,5,0.85);
        p.setBrush(c);person(15,8,1.0);break;}
    case 1: person(20,6,1.15);break;                              // Employés : buste unique
    case 2:{ // Commandes : presse-papier + 3 lignes
        p.drawRoundedRect(QRectF(7,7,26,30),4,4);
        p.setCompositionMode(QPainter::CompositionMode_Clear);
        p.drawRoundedRect(QRectF(10.5,11,19,23),2,2);p.setCompositionMode(QPainter::CompositionMode_SourceOver);
        p.setBrush(c);p.drawRoundedRect(QRectF(13,3,14,8),2.5,2.5);   // pince
        for(int i=0;i<3;++i)p.drawRoundedRect(QRectF(14,16+i*5.6,12,2.4),1.2,1.2);break;}
    case 3:{ // Maquettes : buste de couture + pied
        QPainterPath b;b.moveTo(15,10);b.cubicTo(9,11,8,16,9,21);b.cubicTo(10,25,13,26,14,29);
        b.lineTo(26,29);b.cubicTo(27,26,30,25,31,21);b.cubicTo(32,16,31,11,25,10);b.closeSubpath();p.drawPath(b);
        p.drawRoundedRect(QRectF(18.2,3,3.6,8),1.5,1.5);p.drawEllipse(QPointF(20,3.4),2.4,2.4);
        p.drawRoundedRect(QRectF(19,28,2,8),1,1);p.drawRoundedRect(QRectF(10,35,20,2.8),1.4,1.4);break;}
    case 4:{ // Machines : engrenage 8 dents
        QPainterPath g;const int N=8;const double ro=17,ri=12.5;
        for(int i=0;i<N;++i){double a=2*M_PI*i/N;
            double a1=a-0.20,a2=a+0.20,a0=a-0.40,a3=a+0.40;
            auto pt=[&](double ang,double r){return QPointF(20+r*std::cos(ang),20+r*std::sin(ang));};
            if(i==0)g.moveTo(pt(a0,ri));else g.lineTo(pt(a0,ri));
            g.lineTo(pt(a1,ro));g.lineTo(pt(a2,ro));g.lineTo(pt(a3,ri));}
        g.closeSubpath();p.drawPath(g);
        p.setCompositionMode(QPainter::CompositionMode_Clear);p.drawEllipse(QPointF(20,20),5.5,5.5);break;}
    default:{ // Articles : étiquette de prix inclinée + trou
        p.save();p.translate(20,20);p.rotate(-45);
        QPainterPath t;t.moveTo(-17,-7);t.lineTo(7,-7);t.lineTo(17,0);t.lineTo(7,7);t.lineTo(-17,7);t.closeSubpath();p.drawPath(t);
        p.setCompositionMode(QPainter::CompositionMode_Clear);p.drawEllipse(QPointF(9,0),2.6,2.6);
        p.restore();break;}
    }
    return pm;
}
QIcon sideIcon(int idx){
    QIcon ic;
    ic.addPixmap(iconPix(idx,QColor("#FFFFFF")),QIcon::Normal,QIcon::Off);
    ic.addPixmap(iconPix(idx,QColor("#08213B")),QIcon::Normal,QIcon::On);
    return ic;
}
