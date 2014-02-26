/**
 * Tomas Pllaha
 * Artificial Intelligence
 * Jacobs University Bremen
 * Fall 2013
 */

#include <iostream> // cout
#include <vector> // vector
#include <string> // string
#include <cstring> // strtok
#include <fstream> // ifstream
#include <cmath> // pow
#include <map> // map
#include <algorithm> // find

using namespace std;

/**
 * A pair of integers is used to represent an edge label.
 * The first element of the pair is the attribute number.
 * The second element of the pair is the attribute value.
 */
typedef struct pair<int, int> label;

/**
 * The function parseInt takes a numerical string that represents a non-negative integer and returns
 * the integer.
 */
int parseInt(string toParse) {
	int len = toParse.length();
	int total = 0;
	for (int i = len-1; i >= 0; i--) {
		int charVal = ((int) toParse[i]) - 48;
		if (charVal >= 0 && charVal <= 9)
			total += charVal * pow(10,len-i-1);
		else
			cout << "parseInt: Input is not valid: " << (int) (toParse[i]) << endl;
	}
	return total;
}

/**
 * Wrapper class for decision tree nodes
 */
class Tree {
public:
	virtual ~Tree() {}
	virtual void setDecision(int cl) {};
	virtual int getDecision() {return -1;}
	virtual void addEdge(struct pair<Tree*, label>& edge) {}
	virtual int getAttr() {return -1;}
	virtual void setAttr(int a) {}
	virtual vector<struct pair<Tree*, label> > getChildren() {return vector<struct pair<Tree*, label> >();}

};

/**
 * ConditionNode inherits from Tree and it represents a condition node, i.e: an attribute is
 * tested by this node and depending on its value, the next node is chosen
 */
class ConditionNode : public Tree {
public:
	/**
	 * The id of the attribute that is being tested by this node
	 */
	int attr;

	 /**
	  * The children of this node. Each element in this vector
	  * is a pair. The first element of the pair is a pointer to a child node. It represents an edge
	  * to a child node. The second element is the label of the edge. (i.e: the pair (&ts, (attr,val))
	  * represents an edge to ts, labeled "attr = val".
	  */
	vector<struct pair<Tree*, label> > children;
public:
	ConditionNode(int a) {
		attr = a;
	}
	void addEdge(struct pair<Tree*, label>& edge) {
		children.push_back(edge);
	}

	int getAttr() {
		return attr;
	}
	void setAttr(int a) {
		attr = a;
	}
	vector<struct pair<Tree*, label> > getChildren() {
		return children;
	}
	ConditionNode(const ConditionNode& c) {
		children = c.children;
		attr = c.attr;
	}

};


class DecisionNode : public Tree {
	/**
	 * The id of the class the decision tree assigns to any element reaching this node.
	 */
	int c;
public:
	DecisionNode(int cl) {
		c = cl;
	}

	void setDecision(int cl) {
		c = cl;
	}
	int getDecision() {
		return c;
	}

};



/** This is the main class. It has functionality for loading a dataset, learning a decision tree, and
 * classifying data after the decision tree is learned.
 */
class DecisionLearning {

	vector<vector<int> > data;
	Tree* dec;
	int dataSize = 0;
public:
	DecisionLearning(string fname) {
		vector<int> attributes;
		loadData(fname);
		for (unsigned int i = 0; i < data[0].size()-1;i++)
			attributes.push_back(i);
	    dec = decisionTreeLearning(data, attributes,vector<vector<int> >());

	}
	vector<vector<int> > getData() {
		return data;
	}

	/**
	 *  This function loads the dataset into the vector "data". It takes the file name as an argument
	 *  The data should be given in a CSV file.
	 */
	void loadData(string fname) {
		data = vector<vector<int> >();
		ifstream csv;
		string line;
		csv.open(fname.c_str());
		if (csv.is_open())
		  {
		    while ( getline (csv,line) )
		    {
                cout << line << endl;
		    	if (line[line.length()-1] == '\n')
		    		line[line.length()-1] = '\0';
		    	char* ptr;
		    	const char* delim = " ,\n";
		    	char toSplit[line.length()];
		    	copy(line.begin(),line.end()-1,toSplit);
		    	ptr = strtok(toSplit,delim);
		    	vector<int> row;
		    	while (ptr != NULL) {
		    		int n = parseInt(string(ptr));
		    		row.push_back(n);
		    		ptr = strtok(NULL,delim);
		    	}
		    	data.push_back(row);
		    	dataSize++;
		    }
		    csv.close();

		  }

		  else cout << "Unable to open file" << endl;

	}
	/**
	 * The function pluralityValue takes a 2 dimensional vector as input. The input represents a set
	 * of examples (training data). Assuming that the class identifier is at the end of each row,
	 * it returns the most frequent class in the dataset, as a decision tree leaf.
	 */
	DecisionNode* pluralityValue (vector<vector<int> > examples) {
		map<int,int> occurrences; // Keep count of the number of times each classification has occurred.
		int max = 0;
		int pval = -1;
		for (unsigned int i = 0; i < examples.size(); i++) {
			if (occurrences.find(examples[i].back()) == occurrences.end()) {
				occurrences[examples[i].back()] = 1;
			}
			else {
				occurrences[examples[i].back()]++;
			}
			if (occurrences[examples[i].back()] > max) {
				max = occurrences[examples[i].back()];
				pval = examples[i].back();
			}
		}
		return new DecisionNode(pval);
	}

	/**
	 * This function returns a vector of all possible classifications, assuming that class information
	 * is stored in the last column of the dataset.
	 * Same as getAttrValues(NUM_OF_ATTRIBUTES), where NUM_OF_ATTRIBUTES is the number of attributes
	 */
	vector<int> getClassValues() {
		vector<int> classes;
		for (unsigned int i = 0; i < data.size(); i++) {
			int c = data[i].back();
			if (find(classes.begin(), classes.end(), c) == classes.end())
				classes.push_back(c);
		}
		return classes;
	}


	/**
     * This function returns a vector of all possible values of an attribute specified by the
     * integer attr.
	 */

	vector<int> getAttrValues(int attr) {
		vector<int> attrs;
		for (unsigned int i = 0; i < data.size();i++) {
			int v = data[i][attr];
			if (find(attrs.begin(),attrs.end(), v) == attrs.end())
				attrs.push_back(v);
		}
		return attrs;
	}
	/**
	 * Counts the examples that have value val on attribute attr.
	 */
	int count(int attr,int val, vector<vector<int> > examples) {
		int n = 0;
		for (unsigned int i = 0; i < examples.size(); i++) {
			if (examples[i][attr] == val) {
				n++;
			}
		}
		return n;
	}

	double getAttrEntropy(int attr, vector<vector<int> > examples) {
		vector<int> vals = getAttrValues(attr); // The possible values of attribute attr

		double sum = 0;
		for (unsigned int i = 0; i < vals.size(); i++) {
			int m = count(attr,vals[i],examples);
			double p;
			if (examples.size() > 0 && m > 0) {
				p = ((double) m)/ ((double) examples.size());
				sum += p * (log(p)/log(2.0));
			}
		}
		return -sum;
	}
	/**
	 * getCondEntropy returns the conditional entropy H(X|Y = y) where X is specified by the integer
	 * "response", and Y is specified by the integer "prior". It also takes a matrix of examples as
	 * an argument
	 */
	double getCondEntropy(int response, int prior, vector<vector<int> > examples) {
		vector<int> values = getAttrValues(prior);
		double sum = 0.0;
		for (unsigned int i = 0; i < values.size(); i++) {
			vector<vector<int> > pool;  // Here we throw all records that have value values[i] on attribute
										// "prior".
			for (unsigned int j = 0; j < examples.size(); j++) {
				if (examples[j][prior] == values[i])
					pool.push_back(examples[j]);
			}
			double p = ((double) pool.size()) / ((double) examples.size());
			double e = getAttrEntropy(response,pool);
			sum += p*e;
		}
		return sum;
	}
	/**
	 * Given 2 attribute identifiers attr1 and attr2, it returns the information gain between the two.
	 */
	double informationGain(int attr1,int attr2,vector<vector<int> > examples) {
		return getAttrEntropy(attr2,examples) - getCondEntropy(attr2,attr1,examples);
	}

	/**
	 *
	 */
	ConditionNode maxInfoGain(vector<int> attributes ,int attr, vector<vector<int> > examples) {
		int max = -1;
		int selected = -1;
		for (unsigned int i = 0; i < attributes.size(); i++) {
			if (i == (unsigned int) attr)
				continue;
			else {
				double x = informationGain(i,attr,examples);
				if (x > max) {
					max = x;
					selected = attributes[i];
				}
			}
		}
		return ConditionNode(selected);

	}

	Tree* decisionTreeLearning(vector<vector<int> >Xr, vector<int> Ar, vector<vector<int> >Xp) {
		if (Xr.size() < dataSize/10) {
			return pluralityValue(Xp);
		}
		else {
			int first = Xr[0].back();
			bool allsame = true;
			for (unsigned int i = 0; i < Xr.size();i++) {
				if (Xr[i].back() != first) {
					allsame = false;
					break;
				}
			}
			if (allsame) {
				return new DecisionNode(first);
			}
			else if (Ar.empty() ) { //
				return pluralityValue(Xp);
			}
			else {
				ConditionNode* t = new ConditionNode(maxInfoGain(Ar,Xr[0].size()-1,Xr));
				vector<int> remainingAttributes;
				for (unsigned int i = 0; i < Ar.size(); i++) {
					if (Ar[i] != t->getAttr())
						remainingAttributes.push_back(Ar[i]);
				}
				vector<int> vals = getAttrValues(t->getAttr());

				// for each possible value of the selected attribute ...
				for (unsigned int i = 0; i < vals.size(); i++) {
					vector<vector<int> > newData;
					// ... put all the records that have that value in a new dataset (newData)
					for (unsigned int j = 0; j < Xr.size(); j++) {
						if (Xr[j][t->getAttr()] == vals[i])
							newData.push_back(Xr[j]);
					}
					Tree* ts = decisionTreeLearning(newData,remainingAttributes,Xr);
					label l(t->getAttr(),vals[i]);
					pair<Tree*,label> p(ts,l);
					t->addEdge(p);
				}
				return t;
			}
		}

	}

	double run(string fname) {
		ifstream csv;
		string line;
		vector<vector<int> > testData;
		csv.open(fname.c_str());
		if (csv.is_open())
		  {
		    while ( getline (csv,line) )
		    {
		    	if (line[line.length()-1] == '\n')
		    		line[line.length()-1] = '\0';
		    	char* ptr;
		    	const char* delim = " ,\n";
		    	char toSplit[line.length()];
		    	copy(line.begin(),line.end()-1,toSplit);
		    	ptr = strtok(toSplit,delim);
		    	vector<int> row;
		    	while (ptr != NULL) {
		    		int n = parseInt(string(ptr));
		    		row.push_back(n);
		    		ptr = strtok(NULL,delim);
		    	}
		    	testData.push_back(row);
		    }
		    csv.close();
		  }
		  else  {
			  cout << "Unable to open file" << endl;
			  return -1;
		  }
		int wrong = 0; // store the number of wrong classifications

		Tree* keep = dec; // to be able to reset dec.
		Tree* nexttree;
		for (unsigned int i = 0 ; i < testData.size();) {
			DecisionNode* dn = dynamic_cast<DecisionNode*>(dec);
			if (dn) { // if we reached a decision node
				if (dn->getDecision() != testData[i].back())
					wrong++; // 1 more wrong classification
				i++;
				dec = keep;
				continue;
			}
			else {
				ConditionNode* cn = dynamic_cast<ConditionNode*>(dec);
				vector<struct pair<Tree*,label> > next = cn->getChildren();
				if (cn) {

					for (unsigned int j = 0; j < next.size(); j++) {
						nexttree = next[j].first;
						label nextlabel = next[j].second;
						if (testData[i][nextlabel.first] == nextlabel.second) {
							dec = nexttree;
							break;
						}
					}
				}
				else {
					cout << "!!!!ERRORRRRRR!!!!" << endl;
					return -1;
				}
			}
		}
		dec = keep;
		double error = ((double) wrong)/((double) testData.size());
		cout << "TOTAL: " << testData.size() << endl;
		cout << "CORRECT: " << testData.size() - wrong << endl;
		cout << "WRONG: " << wrong << endl;
		cout << "ERROR RATE: " << error*100.0 << "%" << endl;

		return error;
	}

	void freeMemory(Tree* tree) {
		ConditionNode* cn = dynamic_cast<ConditionNode*>(tree);
		if (cn)
			for (unsigned int i = 0; i < cn->children.size(); i++) {
				freeMemory(cn->children[i].first);
			}
		delete tree;
	}

	void clear() {
		freeMemory(dec);
	}




	void print() {
		for (unsigned int i = 0; i < data.size(); i++) {
			cout << data[i].size() << ": ";
			for (unsigned int j = 0; j < data[i].size(); j++) {
				cout << data[i][j] << ", " ;
			}
			cout << endl;
		}
	}
};


int main () {
	// Create an instance of DecisionLearning
	DecisionLearning dl("SPECT.train");

	//run with the test data
	double errorRate = dl.run("SPECT.test");
	cout << "Returned value: " << errorRate << endl;

	//clear dynamically allocated memory (the decision tree)
	dl.clear();
	return 0;
}
