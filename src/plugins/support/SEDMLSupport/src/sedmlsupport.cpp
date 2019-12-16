/*******************************************************************************

Copyright (C) The University of Auckland

OpenCOR is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenCOR is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://gnu.org/licenses>.

*******************************************************************************/

//==============================================================================
// SED-ML support
//==============================================================================

#include "sedmlsupport.h"

//==============================================================================

#include <QColor>
#include <QObject>
#include <QStringList>

//==============================================================================

namespace OpenCOR {
namespace SEDMLSupport {

//==============================================================================

QStringList lineStyles()
{
    // Return our list of line styles

    static const QStringList LineStyles = QStringList() << "none" << "solid"
                                                        << "dash" << "dot"
                                                        << "dashdot"
                                                        << "dashdotdot";

    return LineStyles;
}

//==============================================================================

QStringList formattedLineStyles()
{
    // Return our list of formatted line styles

    return QStringList() << QObject::tr("None")
                         << QObject::tr("Solid")
                         << QObject::tr("Dash")
                         << QObject::tr("Dot")
                         << QObject::tr("DashDot")
                         << QObject::tr("DashDotDot");
}

//==============================================================================

int indexLineStyle(const QString &pStringLineStyle)
{
    // Return the given string line style as an index line style
    // Note: if the given string line style is invalid then we return the index
    //       line style for a solid line...

    int res = lineStyles().indexOf(pStringLineStyle);

    return (res != -1)?
               res:
               1;
}

//==============================================================================

int indexLineStyle(Qt::PenStyle pLineStyle)
{
    // Return the given line style as an index line style

    return indexLineStyle(stringLineStyle(pLineStyle));
}

//==============================================================================

QString stringLineStyle(int pIndexLineStyle, bool pFormatted)
{
    // Return the given index line style as a string line style
    // Note: if the given index line style is invalid then we return the string
    //       line style for a solid line...

    QStringList res = pFormatted?formattedLineStyles():lineStyles();

    return res[(   (pIndexLineStyle >= 0)
                && (pIndexLineStyle < res.count()))?
                   pIndexLineStyle:
                   1];
}

//==============================================================================

QString stringLineStyle(Qt::PenStyle pLineStyle, bool pFormatted)
{
    // Return the given line style as a string line style
    // Note: if the given line style is invalid then we return the string line
    //       style for a solid line...

    QStringList res = pFormatted?formattedLineStyles():lineStyles();

    return res[int((pLineStyle > Qt::DashDotDotLine)?
                       Qt::SolidLine:
                       pLineStyle)];
}

//==============================================================================

Qt::PenStyle lineStyle(int pIndexLineStyle)
{
    // Return the given index line style as a line style
    // Note: if the given index line style is invalid then we return the line
    //       style for a solid line...

    return Qt::PenStyle((   (pIndexLineStyle >= 0)
                         && (pIndexLineStyle < lineStyles().count()))?
                            pIndexLineStyle:
                            1);
}

//==============================================================================

Qt::PenStyle lineStyle(const QString &pStringLineStyle)
{
    // Return the given string line style as a line style

    return lineStyle(indexLineStyle(pStringLineStyle));
}

//==============================================================================

QStringList symbolStyles()
{
    // Return our list of symbol styles

    static const QStringList SymbolStyles = QStringList() << "none" << "circle"
                                                          << "square"
                                                          << "diamond"
                                                          << "triangle"
                                                          << "downTriangle"
                                                          << "leftTriangle"
                                                          << "rightTriangle"
                                                          << "cross" << "xCross"
                                                          << "horizontalLine"
                                                          << "verticalLine"
                                                          << "star";

    return SymbolStyles;
}

//==============================================================================

QStringList formattedSymbolStyles()
{
    // Return our list of formatted symbol styles

    return QStringList() << QObject::tr("None")
                         << QObject::tr("Circle")
                         << QObject::tr("Square")
                         << QObject::tr("Diamond")
                         << QObject::tr("Triangle")
                         << QObject::tr("Down Triangle")
                         << QObject::tr("Left Triangle")
                         << QObject::tr("Right Triangle")
                         << QObject::tr("Cross")
                         << QObject::tr("X Cross")
                         << QObject::tr("Horizontal Line")
                         << QObject::tr("Vertical Line")
                         << QObject::tr("Star");
}

//==============================================================================

int indexSymbolStyle(const QString &pStringSymbolStyle)
{
    // Return the given string symbol style as an index symbol style
    // Note: if the given string symbol style is invalid then we return the
    //       index symbol style for a "none" symbol...

    int res = symbolStyles().indexOf(pStringSymbolStyle);

    return (res != -1)?
               res :
               0;
}

//==============================================================================

int indexSymbolStyle(QwtSymbol::Style pSymbolStyle)
{
    // Return the given symbol style as an index symbol style

    return indexSymbolStyle(stringSymbolStyle(pSymbolStyle));
}

//==============================================================================

QString stringSymbolStyle(int pIndexSymbolStyle, bool pFormatted)
{
    // Return the given index symbol style as a string symbol style
    // Note: if the given index symbol style is invalid then we return the
    //       string symbol style for a "none" symbol...

    QStringList res = pFormatted?formattedSymbolStyles():symbolStyles();

    return res[(   (pIndexSymbolStyle >= 0)
                && (pIndexSymbolStyle < res.count()))?
                   pIndexSymbolStyle:
                   0];
}

//==============================================================================

QString stringSymbolStyle(QwtSymbol::Style pSymbolStyle, bool pFormatted)
{
    // Return the given symbol style as a string symbol style
    // Note: if the given symbol style is invalid then we return the string
    //       symbol style for a "none" symbol...

    QStringList res = pFormatted?formattedSymbolStyles():symbolStyles();

    return res[int((pSymbolStyle <= QwtSymbol::DTriangle)?
                   pSymbolStyle+1:
                   ((pSymbolStyle >= QwtSymbol::LTriangle) && (pSymbolStyle <= QwtSymbol::Star1))?
                       pSymbolStyle:
                       QwtSymbol::NoSymbol)];
}

//==============================================================================

QwtSymbol::Style symbolStyle(int pIndexSymbolStyle)
{
    // Return the given index symbol style as a string symbol style
    // Note #1: if the given index symbol style is invalid then we return the
    //          string symbol style for a "none" symbol...
    // Note #2: the shifting is because indices in our list of symbols don't
    //          match those of QwtSymbol::Style...

    return QwtSymbol::Style(   (   (pIndexSymbolStyle >= 0)
                            && (pIndexSymbolStyle < symbolStyles().count()))?
                                (pIndexSymbolStyle <= 5)?
                                    pIndexSymbolStyle-1:
                                    pIndexSymbolStyle:
                                -1);
}

//==============================================================================

QwtSymbol::Style symbolStyle(const QString &pStringSymbolStyle)
{
    // Return the given string symbol style as a symbol style

    return symbolStyle(indexSymbolStyle(pStringSymbolStyle));
}

//==============================================================================

libsedml::LineType_t sedmlLineStyle(int pIndexLineStyle)
{
    // Return the given index line style as a SED-ML line style
    // Note: if the given index line style is invalid then we return the SED-ML
    //       line style for a solid line...

    if (pIndexLineStyle == 0) {
        return libsedml::SEDML_LINETYPE_NONE;
    }

    if (pIndexLineStyle == 1) {
        return libsedml::SEDML_LINETYPE_SOLID;
    }

    if (pIndexLineStyle == 2) {
        return libsedml::SEDML_LINETYPE_DASH;
    }

    if (pIndexLineStyle == 3) {
        return libsedml::SEDML_LINETYPE_DOT;
    }

    if (pIndexLineStyle == 4) {
        return libsedml::SEDML_LINETYPE_DASHDOT;
    }

    if (pIndexLineStyle == 5) {
        return libsedml::SEDML_LINETYPE_DASHDOTDOT;
    }

    return libsedml::SEDML_LINETYPE_SOLID;
}

//==============================================================================

Qt::PenStyle lineStyle(libsedml::LineType_t pSedmlLineStyle)
{
    // Return the SED-ML line style as a line style

    if (pSedmlLineStyle == libsedml::SEDML_LINETYPE_NONE) {
        return Qt::NoPen;
    }

    if (pSedmlLineStyle == libsedml::SEDML_LINETYPE_SOLID) {
        return Qt::SolidLine;
    }

    if (pSedmlLineStyle == libsedml::SEDML_LINETYPE_DASH) {
        return Qt::DashLine;
    }

    if (pSedmlLineStyle == libsedml::SEDML_LINETYPE_DOT) {
        return Qt::DotLine;
    }

    if (pSedmlLineStyle == libsedml::SEDML_LINETYPE_DASHDOT) {
        return Qt::DashDotLine;
    }

    if (pSedmlLineStyle == libsedml::SEDML_LINETYPE_DASHDOTDOT) {
        return Qt::DashDotDotLine;
    }

    return Qt::SolidLine;
}

//==============================================================================

libsedml::MarkerType_t sedmlMarkerStyle(int pIndexSymbolStyle)
{
    // Return the given index symbol style as a SED-ML marker style
    // Note: if the given index symbol style is invalid then we return the
    //       SED-ML marker style for no marker...

    if (pIndexSymbolStyle == 0) {
        return libsedml::SEDML_MARKERTYPE_NONE;
    }

    if (pIndexSymbolStyle == 1) {
        return libsedml::SEDML_MARKERTYPE_CIRCLE;
    }

    if (pIndexSymbolStyle == 2) {
        return libsedml::SEDML_MARKERTYPE_SQUARE;
    }

    if (pIndexSymbolStyle == 3) {
        return libsedml::SEDML_MARKERTYPE_DIAMOND;
    }

    if (pIndexSymbolStyle == 4) {
        return libsedml::SEDML_MARKERTYPE_TRIANGLEUP;
    }

    if (pIndexSymbolStyle == 5) {
        return libsedml::SEDML_MARKERTYPE_TRIANGLEDOWN;
    }

    if (pIndexSymbolStyle == 6) {
        return libsedml::SEDML_MARKERTYPE_TRIANGLELEFT;
    }

    if (pIndexSymbolStyle == 7) {
        return libsedml::SEDML_MARKERTYPE_TRIANGLERIGHT;
    }

    if (pIndexSymbolStyle == 8) {
        return libsedml::SEDML_MARKERTYPE_PLUS;
    }

    if (pIndexSymbolStyle == 9) {
        return libsedml::SEDML_MARKERTYPE_XCROSS;
    }

    if (pIndexSymbolStyle == 10) {
        return libsedml::SEDML_MARKERTYPE_HDASH;
    }

    if (pIndexSymbolStyle == 11) {
        return libsedml::SEDML_MARKERTYPE_VDASH;
    }

    if (pIndexSymbolStyle == 12) {
        return libsedml::SEDML_MARKERTYPE_STAR;
    }

    return libsedml::SEDML_MARKERTYPE_NONE;
}

//==============================================================================

QwtSymbol::Style symbolStyle(libsedml::MarkerType_t pSedmlSymbolStyle)
{
    // Return the SED-ML marker style as a symbol style

    if (pSedmlSymbolStyle == libsedml::SEDML_MARKERTYPE_NONE) {
        return QwtSymbol::NoSymbol;
    }

    if (pSedmlSymbolStyle == libsedml::SEDML_MARKERTYPE_CIRCLE) {
        return QwtSymbol::Ellipse;
    }

    if (pSedmlSymbolStyle == libsedml::SEDML_MARKERTYPE_SQUARE) {
        return QwtSymbol::Rect;
    }

    if (pSedmlSymbolStyle == libsedml::SEDML_MARKERTYPE_DIAMOND) {
        return QwtSymbol::Diamond;
    }

    if (pSedmlSymbolStyle == libsedml::SEDML_MARKERTYPE_TRIANGLEUP) {
        return QwtSymbol::Triangle;
    }

    if (pSedmlSymbolStyle == libsedml::SEDML_MARKERTYPE_TRIANGLEDOWN) {
        return QwtSymbol::DTriangle;
    }

    if (pSedmlSymbolStyle == libsedml::SEDML_MARKERTYPE_TRIANGLELEFT) {
        return QwtSymbol::LTriangle;
    }

    if (pSedmlSymbolStyle == libsedml::SEDML_MARKERTYPE_TRIANGLERIGHT) {
        return QwtSymbol::RTriangle;
    }

    if (pSedmlSymbolStyle == libsedml::SEDML_MARKERTYPE_PLUS) {
        return QwtSymbol::Cross;
    }

    if (pSedmlSymbolStyle == libsedml::SEDML_MARKERTYPE_XCROSS) {
        return QwtSymbol::XCross;
    }

    if (pSedmlSymbolStyle == libsedml::SEDML_MARKERTYPE_HDASH) {
        return QwtSymbol::HLine;
    }

    if (pSedmlSymbolStyle == libsedml::SEDML_MARKERTYPE_VDASH) {
        return QwtSymbol::VLine;
    }

    if (pSedmlSymbolStyle == libsedml::SEDML_MARKERTYPE_STAR) {
        return QwtSymbol::Star1;
    }

    return QwtSymbol::NoSymbol;
}

//==============================================================================

std::string sedmlColor(const QColor &pColor)
{
    // Return the given colour as a SED-ML colour

    if (pColor == QColor()) {
        return {};
    }

    QString res = pColor.name(QColor::HexArgb);

    res.remove(0, 1);

    QString alpha = res.left(2);

    res.remove(0, 2);

    if (alpha != "ff") {
        res += alpha;
    }

    return res.toUpper().toStdString();
}

//==============================================================================

QColor color(const std::string &pSedmlColor)
{
    // Return the given SED-ML colour as a colour

    if (pSedmlColor == "") {
        return QColor();
    }

    QString alpha;

    if (pSedmlColor.length() == 8) {
        alpha = QString::fromStdString(pSedmlColor.substr(6));
    }

    return QColor("#"+alpha+QString::fromStdString(pSedmlColor.substr(0, 6)));
}

//==============================================================================

} // namespace SEDMLSupport
} // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
