// File:        RBTree.cpp
// Name:        Mitchell Etzel
// DVC ID:      1416569
// IDE:         Code::Blocks
// Description: Red Black Tree Implementation
#include <cstddef>
#include <iostream>
#include <time.h>
#include <iomanip>
using namespace std;

#define COLOR(color) (color == 0) ? "red" : "black"

static const short BLACK_ = 1;
static const short RED_ = 0;
static const short LEFT_ = 100;
static const short RIGHT_ = 200;

class RBTreeNode {
private:
    RBTreeNode *parent_;
    RBTreeNode *left_;
    RBTreeNode *right_;
    void *data_;
    int key_;
    unsigned short color_;
public:
    RBTreeNode(RBTreeNode *parent, int key, void *data) {
        this->parent_ = parent;
        this->left_ = NULL;
        this->right_ = NULL;
        this->color_ = RED_;
        this->key_ = key;
        this->data_ = data;
    }
    ~RBTreeNode() {
        delete this->left_;
        delete this->right_;
    }
    RBTreeNode *getParent() {return this->parent_;}
    RBTreeNode *getLeft() {return this->left_;}
    RBTreeNode *getRight() {return this->right_;}
    void setLeft(RBTreeNode *node) {this->left_ = node;}
    void setRight(RBTreeNode *node) {this->right_ = node;}
    void setParent(RBTreeNode *node) {this->parent_ = node;}
    void setData(void *data) {this->data_ = data;}
    void setKey(int key) {this->key_ = key;}
    void **getData() {return &this->data_;}
    int getKey() {return this->key_;}
    short getColor() {return this->color_;}
    void setColor(short color) {this->color_ = color;}
};

class RBTree {
private:
    int trace;
    RBTreeNode *root_;

    RBTreeNode *getMinimum_(RBTreeNode *tree) {
        if (tree == NULL) return NULL;
        if (tree->getLeft() != NULL) return getMinimum_(tree->getLeft());
        return tree;
	}
    RBTreeNode *getMaximum_(RBTreeNode *tree) {
        if (tree == NULL) return NULL;
        if (tree->getRight() != NULL) return getMaximum_(tree->getRight());
        return tree;
    }
    RBTreeNode *grandparent_(RBTreeNode *node) {
        if ((node != NULL) && (node->getParent() != NULL)) return node->getParent()->getParent();
        else return NULL;
    }
    RBTreeNode *uncle_(RBTreeNode *node) {
        RBTreeNode *myGPNode = grandparent_(node);
        if (myGPNode == NULL) return NULL;
        if (node->getParent() == myGPNode->getLeft()) return myGPNode->getRight();
        else return myGPNode->getLeft();
    }
    RBTreeNode *getSuccessor_(RBTreeNode *node) {
        if (node->getRight() != NULL) return getMinimum_(node->getRight());
        RBTreeNode *x = node;
        RBTreeNode *y = node->getParent();
        while (y != NULL && x == y->getRight()) {
            x = y;
            y = y->getParent();
        }
        return y;
    }
    RBTreeNode *getPredecessor_(RBTreeNode *node) {
        if (node->getLeft() != NULL) return getMaximum_(node->getLeft());
        RBTreeNode *x = node;
        RBTreeNode *y = node->getParent();
        while (y != NULL && x == y->getLeft()) {
            x = y;
            y = y->getParent();
        }
        return y;
    }
    short isValidRedBlackTreeRecur_(RBTreeNode *tree) {
        if (tree == NULL) return 1;
        if (!isValidRedBlackTreeRecur_(tree->getLeft())) return 0;
        if (!isValidRedBlackTreeRecur_(tree->getRight())) return 0;
        if (tree->getParent() == NULL && tree->getColor() == BLACK_) return 1;
        if (tree->getColor() == RED_ && tree->getParent() != NULL && tree->getParent()->getColor() == BLACK_) return 1;
        if (tree->getColor() == BLACK_ && tree->getParent() != NULL && tree->getParent()->getColor() == BLACK_) return 1;
        return 0;
    }
    short rotateRight_(RBTreeNode *node) {
        int ii;
        RBTreeNode *leftNode = node->getLeft();
        RBTreeNode *correctParent = node->getParent();
        if (node->getLeft() == NULL) return 1;
        if (correctParent != NULL) {
            if (node == correctParent->getRight()) correctParent->setRight(leftNode);
            else correctParent->setLeft(leftNode);
        }
        node->setLeft(leftNode->getRight());
        if (node->getLeft() != NULL) node->getLeft()->setParent(node);
        leftNode->setRight(node);
        leftNode->setParent(node->getParent());
        node->setParent(leftNode);
        if (this->root_ == node) {
            this->root_ = leftNode;
            leftNode->setParent(NULL);
        }
        return 0;
    }
    short rotateLeft_(RBTreeNode *node) {
        int ii;
        RBTreeNode *rightNode = node->getRight();
        RBTreeNode *correctParent = node->getParent();
        if (node->getRight() == NULL) return 1;
        if (correctParent != NULL) {
            if (node == correctParent->getLeft()) correctParent->setLeft(rightNode);
            else correctParent->setRight(rightNode);
        }
        node->setRight(rightNode->getLeft());
        if (node->getRight() != NULL) node->getRight()->setParent(node);
        rightNode->setLeft(node);
        rightNode->setParent(node->getParent());
        node->setParent(rightNode);
        if (this->root_ == node) {
            this->root_ = rightNode;
            rightNode->setParent(NULL);
        }
        return 0;
    }
    short isLeaf_(RBTreeNode *node) {
        if (node->getLeft() == NULL && node->getRight() == NULL) return 1;
        return 0;
    }
    RBTreeNode *treeInsert_(RBTreeNode *tree, int key, void *data) {
        RBTreeNode *inserted;
        if (key < tree->getKey()) {
            if (tree->getLeft() == NULL) {
                inserted = createNode_(tree, LEFT_, key, data);
                insertFix_(inserted);
            }
            else inserted = treeInsert_(tree->getLeft(), key, data);
        }
        else {
            if (tree->getRight() == NULL) {
                inserted = createNode_(tree, RIGHT_, key, data);
                insertFix_(inserted);
            }
            else inserted = treeInsert_(tree->getRight(), key, data);
        }
        return inserted;
    }
    RBTreeNode *createNode_(RBTreeNode *parent, short loc, int key, void *data) {
        RBTreeNode *tmp = new RBTreeNode(parent, key, data);
        if (loc == LEFT_) parent->setLeft(tmp);
        else parent->setRight(tmp);
        return tmp;
    }
    void insertFix_(RBTreeNode *node) {
        if( (getTrace() % 4) > 1) {
            cout << "  => Inserted " << node->getKey() << "\n";
            printTree_2D();
        }
        RBTreeNode *z = node;
        RBTreeNode *y = NULL;
        while (z != NULL && z->getParent() != NULL && z->getParent()->getColor() == RED_) {
            if (z->getParent()->getParent() != NULL) {
                if (z->getParent() == z->getParent()->getParent()->getLeft()) {
                    y = uncle_(z);
                    if (y != NULL && y->getColor() == RED_) {
                        if( (getTrace() % 4) > 1) {
                            cout << "  => Case 1L: Recolor "
                                 << z->getParent()->getKey() << "/"
                                 << z->getParent()->getParent()->getKey() << " \n";
                        }
                        z->getParent()->setColor(BLACK_);
                        y->setColor(BLACK_);
                        z->getParent()->getParent()->setColor(RED_);
                        z=z->getParent()->getParent();
                        if( (getTrace() % 4) > 1) printTree_2D();
                        continue;
                    }
                    else {
                        if (z == z->getParent()->getRight()) {
                            z = z->getParent();
                            if( (getTrace() % 4) > 1) cout << "  => Case 2L: Left Rotate " << z->getKey() << " \n";
                            rotateLeft_(z);
                            if( (getTrace() % 4) > 1) printTree_2D();
                        }
                        if( (getTrace() % 4) > 1) cout <<"  => Case 3L: Right Rotate "<<z->getParent()->getParent()->getKey()<<" \n";

                        z->getParent()->setColor(BLACK_);
                        z->getParent()->getParent()->setColor(RED_);
                        rotateRight_(z->getParent()->getParent());
                        if( (getTrace() % 4) > 1) printTree_2D();
                    }
                }
                else {
                    y = uncle_(z);
                    if (y != NULL && y->getColor() == RED_) {
                        if( (getTrace() % 4) > 1) cout << "  => Case 1R: Recolor "
                                                       << z->getParent()->getKey() << "/"
                                                       << z->getParent()->getParent()->getKey() << " \n";
                        z->getParent()->setColor(BLACK_);
                        y->setColor(BLACK_);
                        z->getParent()->getParent()->setColor(RED_);
                        z = z->getParent()->getParent();
                        if( (getTrace() % 4) > 1) printTree_2D();
                    }
                    else {
                        if (z == z->getParent()->getLeft()) {
                            z = z->getParent();
                            if( (getTrace() % 4) > 1) cout << "  => Case 2R: Right Rotate " << z->getKey() << "\n";
                            rotateRight_(z);
                            if( (getTrace() % 4) > 1) printTree_2D();
                        }
                        if( (getTrace() % 4) > 1) cout <<"  => Case 3R: Left Rotate "<<z->getParent()->getParent()->getKey()<<" \n";
                        z->getParent()->setColor(BLACK_);
                        z->getParent()->getParent()->setColor(RED_);
                        rotateLeft_(z->getParent()->getParent());
                        if( (getTrace() % 4) > 1) printTree_2D();
                    }
                }
            }
            else break;
        }
        this->root_->setColor(BLACK_);
    }
    void deleteFix_(RBTreeNode *node) {

    }
    int count_(RBTreeNode *tree, int num) {
        if (tree == NULL) return num;
        return count_(tree->getLeft(), count_(tree->getRight(), ++num));
    }
    int maxDepthRecur_(RBTreeNode *tree) {
        if (tree == NULL) return 0;
        int leftDepth = maxDepthRecur_(tree->getLeft());
        int rightDepth = maxDepthRecur_(tree->getRight());
        if (leftDepth > rightDepth) return leftDepth + 1;
        else return rightDepth + 1;
    }
    RBTreeNode *queryTreeRecur_(RBTreeNode *tree, int key) {
        if (tree == NULL) return NULL;
        if (key < tree->getKey()) return queryTreeRecur_(tree->getLeft(), key);
        else if (key > tree->getKey()) return queryTreeRecur_(tree->getRight(), key);
        else return tree;
    }
    void printTreeRecur_(RBTreeNode *node) {
        if (node == NULL) ;
        printTreeRecur_(node->getLeft());
        cout << node->getKey();
        printTreeRecur_(node->getRight());
    }
    void printTreeRecur_2D_(RBTreeNode *node, string lpad, string rpad) {
        string pad = lpad.substr(0, lpad.size() - 1);
        string pre_key = "+--";
        if(node == NULL) return;
        if(node->getColor() == RED_ ) pre_key = "+==";
        printTreeRecur_2D_(node->getRight(), rpad + "    |", rpad + "     ");
        int ii = (node->getParent()!=NULL) ? node->getParent()->getKey() : 0;
        if( (getTrace() % 4) > 2) cout << pad << pre_key <<  node->getKey() << ", " << ii << endl;
        else cout << pad << pre_key <<  node->getKey() << endl;
        printTreeRecur_2D_(node->getLeft(), lpad + "     ", lpad + "    |");
    }
    void printPathsRecur_(RBTreeNode *node, RBTreeNode **path, int pathLen) {
        if (node == NULL) return;
        path[pathLen++] = node;
        if (isLeaf_(node)) {
            int i;
            for (i = 0; i < pathLen; i++) cout << path[i]->getKey();
            cout << endl;
            return;
        }
        printPathsRecur_(node->getLeft(), path, pathLen);
        printPathsRecur_(node->getRight(), path, pathLen);
    }
public:
    RBTree() {root_ = NULL;}
    RBTree(int key, void *data) {root_ = new RBTreeNode(NULL, key, data);}
    ~RBTree() {delete this->root_;}
    void setTrace(int flag) {trace = flag;}
    int getTrace() {return trace;}
    void toggleTrace() {trace++;}
    RBTreeNode *insertNode(int key, void *data) {
        if( (getTrace() % 4) > 0) std::cout << "\n ==> A D D I N G    " << key << " <==\n";
        return treeInsert_(this->root_, key, data);
    }
    RBTreeNode *queryTree(int key) {return queryTreeRecur_(this->root_, key);}
    bool deleteNode(int key) {
        if( (getTrace() % 4) > 0) cout << "\n ==> D E L E T I N G    " << key << " <==\n";
        RBTreeNode *x, *y, *z = queryTree(key);
        if (z == NULL) return false;
        if (z->getLeft() == NULL || z->getRight() == NULL) y = z;
        else y = getSuccessor_(z);
        if (y->getLeft() != NULL) x = y->getLeft();
        else x = y->getRight();
        if (x != NULL) x->setParent(y->getParent());
        if (y == this->root_) this->root_ = x;
        else if (y == y->getParent()->getLeft()) y->getParent()->setLeft(x);
        else y->getParent()->setRight(x);
        if (y != z) {
            z->setKey(y->getKey());
            z->setData(y->getData());
        }
        if (y->getColor() == BLACK_) deleteFix_(x);
        return true;
    }
    short isValidRedBlackTree() {return isValidRedBlackTreeRecur_(this->root_);}
    int size() {return count_(this->root_, 0);}
    int maxDepth() {return maxDepthRecur_(this->root_);}
    RBTreeNode *getMinimum() {return getMinimum_(this->root_);}
    RBTreeNode *getMaximum() {return getMaximum_(this->root_);}
    void printTree() {
        cout << endl;
        printTreeRecur_(this->root_);
        cout << endl;
    }
    void printTree_2D() {
        cout << endl;
        printTreeRecur_2D_(this->root_, " ", " ");
        cout << endl;
    }
    void printPaths() {
        RBTreeNode *path[1000];
        printPathsRecur_(this->root_, path, 0);
    }
};

void menu() {
    cout << "\n------ Red-Black Tree Test Menu -------------\n"
		 << "  Notation: +--black node,  +==red node\n"
		 << "  A - Add Node\n"
		 << "  D - Delete Node\n"
		 << "  P - Print Tree\n"
		 << "  Q - Query Node\n"
		 << "  T - Toggle Trace Mode (None->Trace->Step->Parent->)\n"
		 << "  X - exit\n"
		 << "  Enter your choice: ";
}

int main() {
	string choice;
	bool stay = true;
	RBTree *myTree = new RBTree(60, NULL);
	cout << "\nCreated with " << 60 << endl;
	myTree->setTrace(2);
	cout << "Turn Trace/Step ON \n" ;

	myTree->insertNode(30, NULL);
	myTree->insertNode(10, NULL);
	myTree->insertNode(20, NULL);
	myTree->insertNode(50, NULL);
	myTree->insertNode(40, NULL);
	myTree->insertNode(70, NULL);
	myTree->insertNode(90, NULL);
	myTree->insertNode(99, NULL);

	//srand(time(NULL));
	myTree->setTrace(0);
	cout << "Turn Trace OFF \n" ;

	while(stay) {
		string trace_mode_msg = "\n  Trace is ";
		if((myTree->getTrace() % 4) == 1 ) trace_mode_msg += "ON";
		else if((myTree->getTrace() % 4) == 2 ) trace_mode_msg += "ON\n  Trace Step is ON";
		else if((myTree->getTrace() % 4) == 3 ) trace_mode_msg += "ON\n  Trace Parent is ON";
		else trace_mode_msg += "OFF";
		cout << trace_mode_msg;
		menu();
		cin >> choice;
		cin.ignore();
		int user_input;
		RBTreeNode *tempNode;
		if(choice.size() == 1) {
			char ch = choice[0];
			switch(ch) {
				case 'a':
				case 'A':
					cout << "		enter a number key to be added: ";
					cin >> user_input;
					tempNode = myTree->queryTree(user_input);
					if(tempNode != NULL) cout << "		Node already exist!\n";
					else myTree->insertNode(user_input, NULL);
					break;

				case 'd':
				case 'D':
					cout << "		enter a number key to be deleted: ";
					cin >> user_input;
					if (!myTree->deleteNode(user_input)) {
                        cout << "		Node does not exist!\n";
                        break;
                    }
					cout <<" =>Deleted "<<user_input;
					myTree->printTree_2D();
					break;

				case 'P':
                case 'p':
					myTree->printTree_2D();
                    break;

				case 'q':
				case 'Q':
					cout << "		enter a number key to be checked: ";
					cin >> user_input;
					tempNode = myTree->queryTree(user_input);
					if(tempNode == NULL) cout << "		Node does not exist!\n";
					else cout << "		Node exist!\n";
					break;

				case 't':
				case 'T':
					myTree->toggleTrace();
					break;

				case 'x':
				case 'X':
					stay = false;
					break;

				default:
					cout << "\nInvalid Selection!\n .... Try again!\n\n";
			}
		}
	}
	cout << endl;
	delete myTree;
	return 0;
}
