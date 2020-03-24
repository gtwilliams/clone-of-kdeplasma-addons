/*
 *   Copyright (C) 2007 Ryan Bitanga <ephebiphobic@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License version 2 as
 *   published by the Free Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef SPELLCHECK_H
#define SPELLCHECK_H

#include <sonnet/speller.h>

#include <KRunner/AbstractRunner>
#include <QMutex>
#include <QSharedPointer>

/**
 * This checks the spelling of query
 */
class SpellCheckRunner : public Plasma::AbstractRunner
{
Q_OBJECT

public:
    SpellCheckRunner(QObject* parent, const QVariantList &args);
    ~SpellCheckRunner() override;

    void match(Plasma::RunnerContext &context) override;
    void run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &action) override;

    void reloadConfiguration() override;

protected Q_SLOTS:
    void init() override;
    QList<QAction *> actionsForMatch(const Plasma::QueryMatch &match) override;
    QMimeData * mimeDataForMatch(const Plasma::QueryMatch &match) override;

    void loadData();
    void destroydata();

private:
    QString findLang(const QStringList &terms);

    QString m_triggerWord;
    QMap<QString, QString> m_languages;//key=language name, value=language code
    bool m_requireTriggerWord;
    QMap<QString, QSharedPointer<Sonnet::Speller> > m_spellers; //spellers
    QMutex m_spellLock; //Lock held when constructing a new speller
    QList<QAction *> m_actions;
};

#endif
