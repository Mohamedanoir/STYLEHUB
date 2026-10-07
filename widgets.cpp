#include "widgets.h"
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
BannerWidget::BannerWidget(QWidget*p):QWidget(p),bg(":/articles/banner.png"){setFixedHeight(104);}
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
