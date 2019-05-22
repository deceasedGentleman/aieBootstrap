#include <math.h>
#include <conio.h>


template <class T>
struct Node
{
   T value;
   Node* left = nullptr;
   Node* right = nullptr;

   Node(T val): value(val){}

   Node(T val, Node<T> L, Node<T> R):
      value(val),
      left(L), right(R)
   {}
};

template <class T>
class BST
{
public:
   BST() {}
   ~BST(){}

   //exposed functions that run the recursive helper functions on the root node
   void addValue(T val) { addHelper(val, root); }
   void removeValue(T val) { removeHelper(val, root); }
   void print() {}
   Node<T>* find(T val) {}
   int nodesCount() {}
   int height() {}
private:
   Node<T>* root = nullptr;

   void addHelper(T val, Node<T>* current)
   {
      if (current == nullptr)
      {
         current = new Node<T>(val);
      }
      else
      {
         if (val == current->value)
            return;// value is already in the tree, cancel operation
         else if (val < current->value)
         {
            addHelper(val, current->left);
         }
         else if (val > current->value)
         {
            addHelper(val, current->right);
         }
      }
   }

   void removeHelper(T val, Node<T>* current)
   {

   }
};