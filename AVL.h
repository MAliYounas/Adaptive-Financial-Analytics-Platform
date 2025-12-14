#ifndef AVL_TREE
#define AVL_TREE
template<typename T>
struct avlnode
{
   T data;
   avlnode*left;
   avlnode*right;
   int height;
   avlnode(const T& val):data(val),left(nullptr),right(nullptr),height(1){}
};

template<typename T>
class tree{
   avlnode<T>*root;
   int find_height(avlnode<T> *ptr)
   {
      if(!ptr) return 0;
      int x=0,y=0;
      if(ptr->left) x=ptr->left->height;
      if(ptr->right) y=ptr->right->height;
      return 1+max(x,y);  
   }
   int balancing_factor(avlnode<T> *ptr)
   {
     if(!ptr) return 0;
      int x=0,y=0;
      if(ptr->left) x=ptr->left->height;
      if(ptr->right) y=ptr->right->height;
      return x-y;
   }
   avlnode<T>* find_pred(avlnode<T> *ptr)
   {
      if(!ptr||!ptr->left) return nullptr;
      ptr=ptr->left;
      while(ptr&&ptr->right)
      {
         ptr=ptr->right;
      }
      return ptr;

   }
   avlnode<T>* right_rotation(avlnode<T>*ptr)
   {
     if(!ptr||!ptr->left) return ptr;
     avlnode<T> *temp=ptr->left;
     ptr->left=temp->right;
     temp->right=ptr;
     ptr->height=find_height(ptr);
     temp->height=find_height(temp);
     return temp;
   }
   avlnode<T>* left_rotation(avlnode<T>*ptr)
   {
     if(!ptr||!ptr->right) return ptr;
     avlnode<T> *temp=ptr->right;
     ptr->right=temp->left;
     temp->left=ptr;
     ptr->height=find_height(ptr);
     temp->height=find_height(temp);
     return temp;
   }
   avlnode<T> *left_imbalance(avlnode<T>* ptr)
   {
      if(!ptr||!ptr->left) return ptr;
   
      if(balancing_factor(ptr->left)<0) ptr->left=left_rotation(ptr->left);
      return right_rotation(ptr);
   }
   avlnode<T> *right_imbalance(avlnode<T>* ptr)
   {
      if(!ptr||!ptr->right) return ptr;
   
      if(balancing_factor(ptr->right)>0) ptr->right=right_rotation(ptr->right);
      return left_rotation(ptr);
   }
   avlnode<T>* delete_utl(avlnode<T>* ptr,const T& val)
   {
      if(!ptr) return nullptr;
      if(ptr->data>val) ptr->left=delete_utl(ptr->left,val);
      else if(ptr->data<val) ptr->right=delete_utl(ptr->right,val);
      else 
      {
         if(ptr->right&&ptr->left)
         {
            avlnode<T> *pred=find_pred(ptr);
            ptr->data=pred->data;
           ptr->left= delete_utl(ptr->left,pred->data);
         }else
         {
         avlnode<T> *temp=nullptr;
         if(ptr->left) {temp=ptr->left;}
         if(ptr->right) {temp=ptr->right;}
         delete ptr;
         return temp;
         }
      }
      ptr->height=1+max(find_height(ptr->left),find_height(ptr->right));
      if(balancing_factor(ptr)>1) ptr=left_imbalance(ptr);
      else if(balancing_factor(ptr)<-1) ptr=right_imbalance(ptr);
      return ptr;
   }
    avlnode<T>* insert_utl(avlnode<T>* ptr,const T& val)
   {
      if(!ptr) return new avlnode<T>(val);
      if(ptr->data>val) ptr->left=insert_utl(ptr->left,val);
      else if(ptr->data<val) ptr->right=insert_utl(ptr->right,val);
      ptr->height=1+max(find_height(ptr->left),find_height(ptr->right));
      if(balancing_factor(ptr)>1) ptr=left_imbalance(ptr);
      else if(balancing_factor(ptr)<-1) ptr=right_imbalance(ptr);
      return ptr;
   }
   void delete_whole(avlnode<T>*ptr)
   {
    if(!ptr) return;
    delete_whole(ptr->left);
    delete_whole(ptr->right);
    delete ptr;
   }
   void inorder_utl(avlnode<T> *ptr)
   {
      if(!ptr) return;
      inorder_utl(ptr->left);
      cout<<ptr->data<<"  ";
      inorder_utl(ptr->right);
   }
   avlnode<T>* copy(avlnode<T>* node) const
   {
      if(!node) return nullptr;
      avlnode<T>* new_node = new avlnode<T>(node->data);
      new_node->height = node->height;
      new_node->left =copy(node->left);
      new_node->right = copy(node->right);
      return new_node;
   }
   int size_utl(avlnode<T>* ptr) const
   {
      if (!ptr) return 0;
      return 1 + size_utl(ptr->left) + size_utl(ptr->right);
   }
   T min_utl(avlnode<T>* ptr) const
   {
      if (!ptr) return T{}; 
      while (ptr->left) ptr = ptr->left;
      return ptr->data;
   }
   T max_utl(avlnode<T>* ptr) const
   {
      if (!ptr) return T{}; 
      while (ptr->right) ptr = ptr->right;
      return ptr->data;
   }
   public:
   tree():root(nullptr){}
   tree(const tree& other): root(copy(other.root)) {}
   tree(tree&& other) noexcept : root(other.root) { other.root = nullptr; }
   tree& operator=(const tree& other)
   {
      if (this == &other) return *this;
      delete_whole(root);
      root = copy(other.root);
      return *this;
   }
   tree& operator=(tree&& other) noexcept
   {
      if (this == &other) return *this;
      delete_whole(root);
      root = other.root;
      other.root = nullptr;
      return *this;
   }
   ~tree()
   {
    delete_whole(root);
    root =nullptr;
   }
   T search(const T& val)
   {
      avlnode<T>*ptr=root;
      T value = T{};
      while(ptr)
      {
       if(ptr->data==val)
       {
         value=ptr->data;
         break;
       }
       else if(ptr->data>val) ptr=ptr->left;
       else ptr=ptr->right;
      }
      return value;
   }
   void inOrder()
   {
      inorder_utl(root);
      cout<<endl;
   }
   void insert(const T& val)
   {
      root=insert_utl(root,val);
   }
   void remove(const T& val)
   {
      root=delete_utl(root,val);
   }
   size_t size() const
   {
      return size_utl(root);
   }
   bool empty() const
   {
      return root == nullptr;
   }
   void clear()
   {
      delete_whole(root);
      root = nullptr;
   }
   T min_node() const
   {
      return min_utl(root);
   }
   T max_node() const
   {
      return max_utl(root);
   }
};
#endif