#include "yue/bell/LibraryTreeNode.hpp"

namespace yue {
namespace bell {

LibraryTreeNode::LibraryTreeNode()
    : m_uid()
    , m_text("")
    , m_depth(0)
    , m_parent(nullptr)
    , m_children()
    , m_checkstate(Qt::Unchecked)
    , m_expanded(false)
{

}

LibraryTreeNode::LibraryTreeNode(QString text, int depth, LibraryTreeNode* parent)
    : m_uid()
    , m_rating(0)
    , m_text(text)
    , m_depth(depth)
    , m_parent(parent)
    , m_children()
    , m_checkstate(Qt::Unchecked)
    , m_expanded(false)
{

}


LibraryTreeNode::LibraryTreeNode(Database::uid_t uid, QString text, int rating, int depth, LibraryTreeNode* parent)
    : m_uid(uid)
    , m_rating(rating)
    , m_text(text)
    , m_depth(depth)
    , m_parent(parent)
    , m_children()
    , m_checkstate(Qt::Unchecked)
    , m_expanded(false)
{

}

LibraryTreeNode::~LibraryTreeNode()
{
    while (m_children.size() > 0) {
        LibraryTreeNode* child = m_children.back();
        delete child;
        m_children.pop_back();
    }
}

void LibraryTreeNode::setCheckState(Qt::CheckState state)
{
    //if (m_uid.isEmpty()) // null / dummy node
    //    return;

    // setting true/false must toggle parent states as well
    m_checkstate = state;

    for (LibraryTreeNode* nd : m_children)
    {
        nd->setCheckState(state);
    }

    if (m_parent != nullptr)
        m_parent->updateCheckState();
}

/**
 * @brief return all children and grandchildren that are visible
 * @param items a list to populate with node pointers
 */
void LibraryTreeNode::getVisibleChildren(QList<LibraryTreeNode*>& items)
{
    if (!m_expanded)
        return;

    for (LibraryTreeNode* child : m_children)
    {
        items.push_back(child);
        child->getVisibleChildren(items);
    }
}

// check parents and grand parents of this
// to see if node is found
bool LibraryTreeNode::isDescendant(LibraryTreeNode* node)
{
    LibraryTreeNode* temp = m_parent;
    while (temp != nullptr)
    {
        if (temp == node)
        {
            return true;
        }
        temp = temp->m_parent;
    }
    return false;
}

void LibraryTreeNode::updateCheckState( void )
{
    int iChecked = 0;

    Qt::CheckState newState = Qt::Checked;

    for (LibraryTreeNode* nd : m_children)
    {
        if (nd->getCheckState()==Qt::Checked) {
            // count the children, if all are checked
            // then this node is too, if only some of them
            // are checked, then this node is partially checked
            iChecked++;
        } else if (nd->getCheckState()==Qt::PartiallyChecked){
            // since a child is partially checked, then this node is aswell
            // partially checked
            iChecked++;
            newState = Qt::PartiallyChecked;
            break;
        }
        else if (iChecked > 0)
            break;
    }

    if (iChecked == 0)
        newState = Qt::Unchecked;
    else if (iChecked != m_children.size())
        newState = Qt::PartiallyChecked;

    if (newState != getCheckState())
    {
        m_checkstate = newState;
    }

    if (m_parent != nullptr)
        m_parent->updateCheckState();
}


} // namespace bell
} // namespace yue
