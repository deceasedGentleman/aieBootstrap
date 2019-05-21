#include "BinaryTree.h"



BinaryTree::BinaryTree()
{
   m_pRoot = nullptr;
}

BinaryTree::~BinaryTree()
{}

bool BinaryTree::isEmpty()
{
   if (m_pRoot == nullptr)
   return true;
   else return false;
}

void BinaryTree::insert(int a_nValue)
{
   if (isEmpty())
   { *m_pRoot = TreeNode(a_nValue); return; }
   TreeNode* nCurrent = m_pRoot;
   TreeNode* nParent;
   while (nCurrent != nullptr)
   {
      if (a_nValue == nCurrent->getData()) 
      { return; } //The value is already in the tree, so exit

      nParent = nCurrent;
      //If the value to be inserted is less than the value in the current node
      if (a_nValue < nCurrent->getData())
      {  //Set the current node to the left child and continue
         nCurrent = nCurrent->getLeft();
         continue;
      }
      //If the value to be inserted is greater than the current node
      if (a_nValue < nCurrent->getData())
      {  //Set the current node to the right child and continue
         nCurrent = nCurrent->getLeft();
         continue;
      }
   }
   //Get the parent of the current node (before it was set to null)

   //If value to be inserted is less than parent
   if (a_nValue < nParent->getData()) {}
   //{ *nParent->setLeft(TreeNode(a_nValue)); }//insert value as left child node
   else
   {

   }
   //otherwise insert value as right child node

}

void BinaryTree::draw(aie::Renderer2D* renderer, TreeNode* selected)
{
   draw(renderer, m_pRoot, 640, 680, 640, selected);
}
void BinaryTree::draw(aie::Renderer2D* renderer, TreeNode* pNode, int x, int y, int horizontalSpacing, TreeNode* selected)
{
   horizontalSpacing /= 2;
   if (pNode)
   { 
      if (pNode->hasLeft())
      { 
         renderer->setRenderColour(1, 0, 0); 
         renderer->drawLine(x, y, x - horizontalSpacing, y - 80);
         draw(renderer, pNode->getLeft(), x - horizontalSpacing, y - 80, horizontalSpacing, selected); 
      }
      if (pNode->hasRight())
      {
         renderer->setRenderColour(1, 0, 0);
         renderer->drawLine(x, y, x + horizontalSpacing, y - 80);
         draw(renderer, pNode->getRight(), x + horizontalSpacing, y - 80, horizontalSpacing, selected);
      }
      pNode->draw(renderer, x, y, (selected == pNode));
   }
}