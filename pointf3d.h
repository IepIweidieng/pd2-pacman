#ifndef POINTF3D_H
#define POINTF3D_H

#include <QPointF>
#include <QVector3D>

class PointF3d {
public:
    constexpr PointF3d();
    constexpr PointF3d(QPointF p, qreal zpos = 0);
    constexpr PointF3d(qreal xpos, qreal ypos, qreal zpos);

    constexpr inline qreal manhattanLength() const;

    inline bool isNull() const;

    constexpr inline qreal x() const;
    constexpr inline qreal y() const;
    constexpr inline qreal z() const;
    inline void setX(qreal x);
    inline void setY(qreal y);
    inline void setZ(qreal z);

    inline qreal& rx();
    inline qreal& ry();
    inline qreal& rz();

    inline PointF3d& operator+=(const PointF3d &p);
    inline PointF3d& operator-=(const PointF3d &p);
    inline PointF3d& operator*=(qreal c);
    inline PointF3d& operator/=(qreal c);

    static constexpr inline qreal dotProduct(const PointF3d& p1, const PointF3d& p2)
      { return p1.xp * p2.xp + p1.yp * p2.yp + p1.zp * p2.zp; }

    friend constexpr inline bool operator==(const PointF3d&, const PointF3d&);
    friend constexpr inline bool operator!=(const PointF3d&, const PointF3d&);
    friend constexpr inline const PointF3d operator+(const PointF3d&, const PointF3d&);
    friend constexpr inline const PointF3d operator-(const PointF3d&, const PointF3d&);
    friend constexpr inline const PointF3d operator*(qreal, const PointF3d&);
    friend constexpr inline const PointF3d operator*(const PointF3d&, qreal);
    friend constexpr inline const PointF3d operator+(const PointF3d&);
    friend constexpr inline const PointF3d operator-(const PointF3d&);
    friend constexpr inline const PointF3d operator/(const PointF3d&, qreal);

    constexpr QVector3D toVector3D() const;
    constexpr QPointF toPointF() const;
    constexpr QPoint toPoint() const;

private:
    qreal xp;
    qreal yp;
    qreal zp;
};

Q_DECLARE_TYPEINFO(PointF3d, Q_MOVABLE_TYPE);

constexpr inline PointF3d::PointF3d(): xp {0}, yp {0}, zp {0}  { }
constexpr inline PointF3d::PointF3d(QPointF p, qreal zpos):
  xp {p.x()}, yp {p.y()}, zp {zpos}  { }
constexpr inline PointF3d::PointF3d(qreal xpos, qreal ypos, qreal zpos):
  xp {xpos}, yp {ypos}, zp {zpos}  { }

constexpr inline qreal PointF3d::manhattanLength() const
  { return qAbs(xp)+qAbs(yp)+qAbs(zp); }

inline bool PointF3d::isNull() const
  { return qIsNull(xp) && qIsNull(yp) && qIsNull(zp); }

constexpr inline qreal PointF3d::x() const
  { return xp; }
constexpr inline qreal PointF3d::y() const
  { return yp; }
constexpr inline qreal PointF3d::z() const
  { return zp; }

inline void PointF3d::setX(qreal xpos)
  { xp = xpos; }
inline void PointF3d::setY(qreal ypos)
  { yp = ypos; }
inline void PointF3d::setZ(qreal zpos)
  { zp = zpos; }

inline qreal& PointF3d::rx()
  { return xp; }
inline qreal& PointF3d::ry()
  { return yp; }
inline qreal& PointF3d::rz()
  { return zp; }

inline PointF3d& PointF3d::operator+=(const PointF3d& p)
{
    xp+=p.xp; yp+=p.yp; zp+=p.zp; return *this;
}
inline PointF3d& PointF3d::operator-=(const PointF3d& p)
{
    xp-=p.xp; yp-=p.yp; zp-=p.zp; return *this;
}
inline PointF3d& PointF3d::operator*=(qreal c)
{
    xp*=c; yp*=c; zp*=c; return *this;
}

constexpr inline bool operator==(const PointF3d& p1, const PointF3d& p2)
{
    return ((!static_cast<bool>(p1.xp) || !static_cast<bool>(p2.xp)) ? qFuzzyIsNull(p1.xp - p2.xp) : qFuzzyCompare(p1.xp, p2.xp))
        && ((!static_cast<bool>(p1.yp) || !static_cast<bool>(p2.yp)) ? qFuzzyIsNull(p1.yp - p2.yp) : qFuzzyCompare(p1.yp, p2.yp))
        && ((!static_cast<bool>(p1.zp) || !static_cast<bool>(p2.zp)) ? qFuzzyIsNull(p1.zp - p2.zp) : qFuzzyCompare(p1.zp, p2.zp));
}
constexpr inline bool operator!=(const PointF3d& p1, const PointF3d& p2)
  { return !(p1 == p2); }

constexpr inline const PointF3d operator+(const PointF3d& p1, const PointF3d& p2)
  { return {p1.xp+p2.xp, p1.yp+p2.yp, p1.zp+p2.zp}; }
constexpr inline const PointF3d operator-(const PointF3d& p1, const PointF3d& p2)
  { return {p1.xp-p2.xp, p1.yp-p2.yp, p1.zp-p2.zp}; }
constexpr inline const PointF3d operator*(const PointF3d& p, qreal c)
  { return {p.xp*c, p.yp*c, p.zp*c}; }
constexpr inline const PointF3d operator*(qreal c, const PointF3d& p)
  { return {p.xp*c, p.yp*c, p.zp*c}; }
constexpr inline const PointF3d operator+(const PointF3d& p)
  { return p; }
constexpr inline const PointF3d operator-(const PointF3d& p)
  { return {-p.xp, -p.yp, -p.zp}; }
inline PointF3d& PointF3d::operator/=(qreal divisor)
{
    xp/=divisor; yp/=divisor; zp/=divisor; return *this;
}
constexpr inline const PointF3d operator/(const PointF3d& p, qreal divisor)
  { return {p.xp/divisor, p.yp/divisor, p.zp/divisor}; }

constexpr inline QVector3D PointF3d::toVector3D() const
  { return {float(xp), float(yp), float(zp)}; }
constexpr inline QPointF PointF3d::toPointF() const
  { return {xp, yp}; }
constexpr inline QPoint PointF3d::toPoint() const
  { return {qRound(xp), qRound(yp)}; }

#endif // POINTF3D_H
