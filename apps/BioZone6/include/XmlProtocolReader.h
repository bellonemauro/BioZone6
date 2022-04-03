/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  |  Fluicell AB                                                              |
*  |  Copyright 2021 © Fluicell AB, http://fluicell.com/                       |
*  |  BioZone6                                                                 |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#ifndef XMLPROTOCOLREADER_H
#define XMLPROTOCOLREADER_H

#include <QXmlStreamReader>
#include "protocolTreeWidgetItem.h"

QT_BEGIN_NAMESPACE
class QTreeWidget;
QT_END_NAMESPACE


class XmlProtocolReader
{
public:
	XmlProtocolReader(QTreeWidget *treeWidget);

    bool read(QIODevice* device, protocolTreeWidgetItem* after_item);

    QString errorString() const;

    bool isOldProtocolAttribute() {
        if ( m_protocol_attribute == old_swProtocolAttribute())
            return true;
        else
            return false;
    }

private:
    void readPRT();
    void readProtocolItem(protocolTreeWidgetItem* parent_item);

	protocolTreeWidgetItem *createChildItem(protocolTreeWidgetItem *parent_item);

    // TODO: the protocol reader and writer re-define the same attribute, 
    //       the best way would be to re-define a superclass to avoid double definitions
    static inline QString swProtocolAttribute() { return QStringLiteral("BioZoneProtocol"); }
    static inline QString old_swProtocolAttribute() { return QStringLiteral("Protocol"); }
    static inline QString valueAttribute() { return QStringLiteral("value"); }
    static inline QString messageAttribute() { return QStringLiteral("message"); }
    static inline QString versionAttribute() { return QStringLiteral("version"); }

    QXmlStreamReader xml;
    QTreeWidget *treeWidget;
    protocolTreeWidgetItem* m_after_item;
    int m_row;
    
    // define all strings used in the xmlReader
    QString m_protocol_attribute;
    QString m_str_notPRTvfile;

};

#endif
