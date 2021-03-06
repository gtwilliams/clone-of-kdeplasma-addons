/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *   SPDX-FileCopyrightText: 2016 David Edmundson <davidedmundson@kde.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "groupedappletscontainer.h"
#include "../debug.h"

#include <QDebug>
#include <QQuickItem>

#include <KActionCollection>
#include <Plasma/Corona>
#include <QAction>

GroupedAppletsContainer::GroupedAppletsContainer(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args)
{
}

GroupedAppletsContainer::~GroupedAppletsContainer()
{
    if (destroyed()) {
        m_innerContainment->destroy();
    }
}

void GroupedAppletsContainer::init()
{
    Applet::init();

    // in the first creation we immediately create the systray: so it's accessible during desktop scripting
    uint id = config().readEntry("ContainmentId", 0);

    if (id == 0) {
        ensureSystrayExists();
    }
}

void GroupedAppletsContainer::ensureSystrayExists()
{
    if (m_innerContainment) {
        return;
    }

    Plasma::Containment *cont = containment();
    if (!cont) {
        return;
    }

    Plasma::Corona *c = cont->corona();
    if (!c) {
        return;
    }

    uint id = config().readEntry("ContainmentId", 0);
    if (id > 0) {
        const auto containments = c->containments();
        for (Plasma::Containment *candidate : containments) {
            if (candidate->id() == id) {
                m_innerContainment = candidate;
                break;
            }
        }
        qCDebug(GROUPING_DEBUG) << "Containment id" << id << "that used to be a grouped containment that was deleted";
        // id = 0;
    }

    if (!m_innerContainment) {
        m_innerContainment = c->createContainment(QStringLiteral("org.kde.plasma.private.grouping"), //
                                                  QVariantList() << QStringLiteral("org.kde.plasma:force-create"));
        config().writeEntry("ContainmentId", m_innerContainment->id());
    }

    if (!m_innerContainment) {
        return;
    }

    m_innerContainment->setParent(this);
    connect(containment(), &Plasma::Containment::screenChanged, m_innerContainment.data(), &Plasma::Containment::reactToScreenChange);
    m_innerContainment->setFormFactor(formFactor());
    m_innerContainment->setLocation(location());

    m_internalContainmentItem = m_innerContainment->property("_plasma_graphicObject").value<QQuickItem *>();
    Q_EMIT internalContainmentItemChanged();

    actions()->addAction(QStringLiteral("configure"), m_innerContainment->actions()->action(QStringLiteral("configure")));
    connect(m_innerContainment.data(), &Plasma::Containment::configureRequested, this, [this](Plasma::Applet *applet) {
        Q_EMIT containment()->configureRequested(applet);
    });

    if (m_internalContainmentItem) {
        // don't let internal systray manage context menus
        m_internalContainmentItem->setAcceptedMouseButtons(Qt::NoButton);
    }

    // replace internal remove action with ours
    m_innerContainment->actions()->addAction(QStringLiteral("remove"), actions()->action(QStringLiteral("remove")));
}

void GroupedAppletsContainer::constraintsEvent(Plasma::Types::Constraints constraints)
{
    if (constraints & Plasma::Types::LocationConstraint) {
        if (m_innerContainment) {
            m_innerContainment->setLocation(location());
        }
    }
    if (constraints & Plasma::Types::FormFactorConstraint) {
        if (m_innerContainment) {
            m_innerContainment->setFormFactor(formFactor());
        }
    }

    if (constraints & Plasma::Types::UiReadyConstraint) {
        ensureSystrayExists();
    }
}

QQuickItem *GroupedAppletsContainer::internalContainmentItem()
{
    return m_internalContainmentItem;
}

K_EXPORT_PLASMA_APPLET_WITH_JSON(groupedappletscontainer, GroupedAppletsContainer, "metadata.json")

#include "groupedappletscontainer.moc"
