/*
 * Copyright 2018  Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.9

import QtQuick.Layouts 1.3

import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.components 3.0 as PlasmaComponents

ColumnLayout {
    id: fullRoot

    Layout.margins: units.smallSpacing

    TopPanel {
        Layout.fillWidth: true

        model: plasmoid.nativeInterface.panelModel
    }

    SwitchPanel {
        Layout.fillWidth: true

        forecastViewTitle: plasmoid.nativeInterface.panelModel.totalDays
        forecastModel: plasmoid.nativeInterface.forecastModel
        detailsModel: plasmoid.nativeInterface.detailsModel
        noticesModel: plasmoid.nativeInterface.noticesModel
    }

    PlasmaComponents.Label {
        readonly property string creditUrl: plasmoid.nativeInterface.panelModel.creditUrl

        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignRight
        font {
            pointSize: theme.smallestFont.pointSize
            underline: !!creditUrl
        }
        linkColor : color
        opacity: 0.6
        textFormat: Text.StyledText

        text: {
            var result = plasmoid.nativeInterface.panelModel.courtesy;
            if (creditUrl) {
                result = "<a href=\"" + creditUrl + "\">" + result + "</a>";
            }
            return result;
        }

        onLinkActivated: {
            Qt.openUrlExternally(link);
        }
    }
}