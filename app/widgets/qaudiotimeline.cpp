#include "qaudiotimeline.h"
#include <QPainter>
#include <QMouseEvent>
#include <QtMath>
QAudioTimeline::QAudioTimeline(QWidget *p) : QWidget(p) { setMinimumHeight(110); setMouseTracking(true); }
void QAudioTimeline::setTrackCount(int n) { m_offsets.resize(qMax(0,n)); m_starts.resize(qMax(0,n)); m_ends.resize(qMax(0,n)); m_waveforms.resize(qMax(0,n)); for(int i=0;i<n;++i)m_ends[i]=m_duration; setMinimumHeight(qMax(150, n*74+42)); update(); }
void QAudioTimeline::setOffset(int i, double v) { if (i >= 0 && i < m_offsets.size()) m_offsets[i] = v; update(); }
void QAudioTimeline::setTrim(int i,double s,double e){if(i>=0&&i<m_starts.size()){m_starts[i]=s;m_ends[i]=e>0?e:m_duration;update();}}
void QAudioTimeline::setDuration(double s){m_duration=qMax(.1,s);for(auto &e:m_ends)if(e<=0)e=m_duration;update();}
void QAudioTimeline::setWaveform(int i,const QVector<float> &samples){if(i>=0&&i<m_waveforms.size())m_waveforms[i]=samples;update();}
void QAudioTimeline::setFitWidth(bool fit) { m_fitWidth=fit; if (fit) setMinimumWidth(0); update(); }
void QAudioTimeline::setZoom(double zoom) { m_zoom=qBound(.25,zoom,20.0); setMinimumWidth(qMax(500,int(110+m_duration*100*m_zoom))); update(); }
void QAudioTimeline::paintEvent(QPaintEvent *) {
    QPainter p(this); p.fillRect(rect(), QColor(35,38,45));
    const int lane = m_offsets.isEmpty() ? 32 : qMax(32, (height()-28)/m_offsets.size()), origin = 42; p.setPen(QColor(100,105,115)); p.drawLine(origin,0,origin,height());
    p.setPen(Qt::white); p.drawText(6,16,tr("Audio lanes — drag a clip to adjust sync"));
    const double pixelsPerSecond = m_fitWidth ? qMax(1.0,(width()-origin-12.0)/m_duration) : 100.0*m_zoom;
    for (int i=0;i<m_offsets.size();++i) { int y=28+i*lane; int x=origin+int(m_offsets[i]/1000.0*pixelsPerSecond); int w=qMax(40,int((m_ends[i]-m_starts[i])*pixelsPerSecond)); QRect r(x,y,w,qMax(24,lane-10)); 
        // A deterministic waveform/spectral-style backdrop keeps the lane useful
        // before an expensive decoded waveform is available.
        p.fillRect(r,QColor(20,47,56)); p.setPen(QColor(71,207,189)); const auto &wave=m_waveforms[i]; for(int xx=r.left()+2;xx<r.right();xx+=2){int n=wave.size();int wi=n?qBound(0,int((xx-r.left())*n/double(qMax(1,r.width()))),n-1):-1;double a=wi>=0?wave[wi]:.15;int h=qMax(2,int((r.height()/2-3)*a));p.drawLine(xx,r.center().y()-h,xx,r.center().y()+h);} p.fillRect(r,QColor(45,125,190,70));p.setPen(Qt::white);p.drawRect(r);p.drawText(r.adjusted(8,0,-8,0),Qt::AlignVCenter,tr("Track %1  %2 ms").arg(i+1).arg(qRound(m_offsets[i]))); p.fillRect(r.left(),r.top(),5,r.height(),QColor(240,190,70));p.fillRect(r.right()-5,r.top(),5,r.height(),QColor(240,190,70)); }
}
void QAudioTimeline::mousePressEvent(QMouseEvent *e) { int lane=m_offsets.isEmpty()?32:qMax(32,(height()-28)/m_offsets.size()); int t=(e->position().y()-28)/lane; if(t>=0&&t<m_offsets.size()){m_dragTrack=t; const double pps=m_fitWidth?qMax(1.0,(width()-54.0)/m_duration):100.0*m_zoom;int x=42+int(m_offsets[t]/1000.0*pps);int l=x+int(m_starts[t]*pps),r=x+int(m_ends[t]*pps);m_dragEdge=qAbs(e->position().x()-l)<10?1:(qAbs(e->position().x()-r)<10?2:0);m_dragStart=e->position().x();} }
void QAudioTimeline::mouseMoveEvent(QMouseEvent *e) { if (!(e->buttons() & Qt::LeftButton)) { m_dragTrack=-1; return; } if(m_dragTrack<0)return; const double pps=m_fitWidth?qMax(1.0,(width()-54.0)/m_duration):100.0*m_zoom;double delta=(e->position().x()-m_dragStart)/pps;if(m_dragEdge==1){m_starts[m_dragTrack]=qBound(0.0,qMin(m_ends[m_dragTrack]-.01,m_starts[m_dragTrack]+delta),m_duration);emit trimDragged(m_dragTrack,m_starts[m_dragTrack],m_ends[m_dragTrack]);}else if(m_dragEdge==2){m_ends[m_dragTrack]=qBound(m_starts[m_dragTrack]+.01,qMin(m_duration,m_ends[m_dragTrack]+delta),m_duration);emit trimDragged(m_dragTrack,m_starts[m_dragTrack],m_ends[m_dragTrack]);}else {m_offsets[m_dragTrack]+=delta*1000;emit offsetDragged(m_dragTrack,m_offsets[m_dragTrack]);}m_dragStart=e->position().x();update(); }
void QAudioTimeline::mouseReleaseEvent(QMouseEvent *e) { Q_UNUSED(e); m_dragTrack=-1; m_dragEdge=0; unsetCursor(); }
