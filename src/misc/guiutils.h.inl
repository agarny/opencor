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
// Core GUI utilities
//==============================================================================

QMainWindow CORE_EXPORT * mainWindow();

QSize CORE_EXPORT minimumWidgetSize(QWidget *pWidget);
void CORE_EXPORT adjustWidgetSize(QWidget *pWidget);

void CORE_EXPORT showEnableAction(QAction *pAction, bool pVisible,
                                  bool pEnabled = true);

QColor CORE_EXPORT opaqueColor(const QColor &pColor, const QColor &pBackgroundColor);

QColor CORE_EXPORT baseColor(QPalette::ColorGroup pColorGroup = QPalette::Current);
QColor CORE_EXPORT borderColor(QPalette::ColorGroup pColorGroup = QPalette::Current);
QColor CORE_EXPORT highlightColor(QPalette::ColorGroup pColorGroup = QPalette::Current);
QColor CORE_EXPORT shadowColor(QPalette::ColorGroup pColorGroup = QPalette::Current);
QColor CORE_EXPORT windowColor(QPalette::ColorGroup pColorGroup = QPalette::Current);
QColor CORE_EXPORT windowTextColor(QPalette::ColorGroup pColorGroup = QPalette::Current);

QMessageBox::StandardButton CORE_EXPORT informationMessageBox(const QString &pTitle,
                                                              const QString &pText,
                                                              const QMessageBox::StandardButtons &pButtons = QMessageBox::Ok,
                                                              const QMessageBox::StandardButton &pDefaultButton = QMessageBox::Ok);
QMessageBox::StandardButton CORE_EXPORT questionMessageBox(const QString &pTitle, const QString &pText,
                                                           const QMessageBox::StandardButtons &pButtons = QMessageBox::StandardButtons(QMessageBox::Yes|QMessageBox::No),
                                                           const QMessageBox::StandardButton &pDefaultButton = QMessageBox::Yes);
QMessageBox::StandardButton CORE_EXPORT warningMessageBox(const QString &pTitle, const QString &pText,
                                                          const QMessageBox::StandardButtons &pButtons = QMessageBox::Ok,
                                                          const QMessageBox::StandardButton &pDefaultButton = QMessageBox::Ok);
QMessageBox::StandardButton CORE_EXPORT criticalMessageBox(const QString &pTitle, const QString &pText,
                                                           const QMessageBox::StandardButtons &pButtons = QMessageBox::Ok,
                                                           const QMessageBox::StandardButton &pDefaultButton = QMessageBox::Ok);

void CORE_EXPORT aboutMessageBox(const QString &pTitle, const QString &pText);

//==============================================================================
// End of file
//==============================================================================
