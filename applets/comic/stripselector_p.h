/***************************************************************************
 *   Copyright (C) 2012 Matthias Fuchs <mat69@gmx.net>                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#ifndef STRIP_SELECTOR_P_H
#define STRIP_SELECTOR_P_H

#include "stripselector.h"

#include <QString>

class StringStripSelector : public StripSelector
{
    public:
        explicit StringStripSelector(QObject *parent = nullptr);
        ~StringStripSelector() override;

        void select(const ComicData &currentStrip) override;
};

class NumberStripSelector : public StripSelector
{
    public:
        explicit NumberStripSelector(QObject *parent = nullptr);
        ~NumberStripSelector() override;

        void select(const ComicData &currentStrip) override;
};

class DateStripSelector : public StripSelector
{
    Q_OBJECT

    public:
        explicit DateStripSelector(QObject *parent = nullptr);
        ~DateStripSelector() override;

        void select(const ComicData &currentStrip) override;

    private Q_SLOTS:
        void slotChosenDay(const QDate &date);

    private:
        QString mFirstIdentifierSuffix;
};

#endif
