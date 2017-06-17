#ifndef LIBRARYTREENODE_HPP
#define LIBRARYTREENODE_HPP


#include <QObject>
#include "yue/global.h"
#include "yue/bell/database.hpp"

namespace yue {
namespace bell {



class YUECOMMON_EXPORT LibraryTreeNode
{
private:
    Database::uid_t m_uid;
    QString m_text;
    int m_depth;
    LibraryTreeNode* m_parent;
    QList<LibraryTreeNode*> m_children;
    Qt::CheckState m_checkstate; // Qt::Unchecked, Qt::PartiallyChecked Qt::Checked
    bool m_expanded;
public:
    LibraryTreeNode(Database::uid_t uid, QString text, int depth, LibraryTreeNode* parent = nullptr)
        : m_uid(uid)
        , m_text(text)
        , m_depth(depth)
        , m_parent(parent)
        , m_children()
        , m_checkstate(Qt::Unchecked)
        , m_expanded(false)
    {}
    ~LibraryTreeNode() {}

    QString text( void ) { return m_text; }
    void setCheckState(Qt::CheckState state);
    Qt::CheckState getCheckState( void ) { return m_checkstate; }

    void addChild(LibraryTreeNode* child) {
        m_children.push_back(child);
        child->m_parent = this;
    }

    void setExpanded(bool bExpand) {
        m_expanded = bExpand;
        //emit isExpandedChanged();
    }
    bool isExpanded( void ) { return m_expanded; }
    LibraryTreeNode* parent() { return m_parent; }

    int getDepth( void ) { return m_depth; }
    Database::uid_t getUid() {return m_uid;}

    size_t numChildren() { return m_children.size(); }
    const QList<LibraryTreeNode*>& children() { return m_children; }

    void getVisibleChildren(QList<LibraryTreeNode*>& items);
    bool isDescendant(LibraryTreeNode* node);

   private:
    //void updateParentCheckState( void );
    void updateCheckState( void );

};

} // namespace bell
} // namespace yue

#endif // LIBRARYTREENODE_HPP
