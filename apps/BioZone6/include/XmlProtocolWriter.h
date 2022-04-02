/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  |  Fluicell AB                                                              |
*  |  Copyright 2021 © Fluicell AB, http://fluicell.com/                       |
*  |  BioZone6                                                                 |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#ifndef XMLPROTOCOLRITER_H
#define XMLPROTOCOLRITER_H

#include <QXmlStreamWriter>

QT_BEGIN_NAMESPACE
class QTreeWidget;
class QTreeWidgetItem;
QT_END_NAMESPACE

class XmlProtocolWriter
{
public:
    explicit XmlProtocolWriter(const QTreeWidget *treeWidget);
    bool writeFile(QIODevice *device);

	static inline QString swProtocolAttribute() { return QStringLiteral("BioZoneProtocol"); }
	static inline QString valueAttribute() { return QStringLiteral("value"); }
	static inline QString messageAttribute() { return QStringLiteral("message"); }
	static inline QString versionAttribute() { return QStringLiteral("version"); }

private:
    void writeItem(const QTreeWidgetItem *item);
	QString getCommandAsString(int _instruction);
    QXmlStreamWriter xml;
    const QTreeWidget *treeWidget;
};

#endif
