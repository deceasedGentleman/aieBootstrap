#include "BinaryTree.h"



BinaryTree::BinaryTree()
{
   _root = nullptr;
}

BinaryTree::~BinaryTree()
{}

bool BinaryTree::isEmpty()
{
   if (_root == nullptr)
   return true;
   else return false;
}

void BinaryTree::insert(int value)
{
   if (isEmpty())
   { *_root = TreeNode(value); return; }
   TreeNode* current = _root;
   TreeNode* parent;
   while (current != nullptr)
   {
      if (value == current->getData()) 
      { return; } //The value is already in the tree, so exit

      parent = current;
      //If the value to be inserted is less than the value in the current node
      if (value < current->getData())
      {  //Set the current node to the left child and continue
         current = current->getLeft();
         continue;
      }
      //If the value to be inserted is greater than the current node
      if (value < current->getData())
      {  //Set the current node to the right child and continue
         current = current->getLeft();
         continue;
      }
   }
   //Get the parent of the current node (before it was set to null)

   //If value to be inserted is less than parent
   if (value < parent->getData()) {}
   //{ *nParent->setLeft(TreeNode(a_nValue)); }//insert value as left child node
   else
   {

   }
   //otherwise insert value as right child node

}

void BinaryTree::draw(aie::Renderer2D* renderer, TreeNode* selected)
{
   draw(renderer, _root, 640, 680, 640, selected);
}
void BinaryTree::draw(aie::Renderer2D* renderer, TreeNode* pNode, int x, int y, int horizontalSpacing, TreeNode* selected)
{
   horizontalSpacing /= 2;
   if (pNode)
   { 
      renderer->setRenderColour(1, 0, 0); 
      if (pNode->hasLeft())
      { 
         renderer->drawLine(x, y, x - horizontalSpacing, y - 80);
         draw(renderer, pNode->getLeft(), x - horizontalSpacing, y - 80, horizontalSpacing, selected); 
      }
      if (pNode->hasRight())
      {
         renderer->drawLine(x, y, x + horizontalSpacing, y - 80);
         draw(renderer, pNode->getRight(), x + horizontalSpacing, y - 80, horizontalSpacing, selected);
      }
      pNode->draw(renderer, x, y, (selected == pNode));
   }
}