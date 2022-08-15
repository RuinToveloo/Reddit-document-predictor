#include <iostream>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <math.h>
#include <climits>
#include <bits/stdc++.h>

using namespace std;


// ------ GLOBAL VARIABLE ------
vector<string> w;
map<string, int> word_wordid; // key is word, value is wordID (1->6968)
//--- Training ---
map<int, int> docid_type1; // key is docID, value is type (1 for atheism, 2 for book)
map<int, vector<int>> wordid_docid1; // key is wordid, value is array of all docID that wordid is presented
// --- Testing ---
map<int, int> docid_type2; // key is docID, value is type (1 for atheism, 2 for book)
map<int, vector<int>> wordid_docid2; // key is wordid, value is array of all docID that wordid is presented
map<string, double> ml1; // ml for trainning
map<string, double> ml2; // ml for testing
map<int, vector<int>> docid_wordid_1; // key is docID, value is array of all wordID in doc
map<int, vector<int>> docid_wordid_2; // key is docID, value is array of all wordID in doc


// word to P(word | type)
map<string, double> giventype1_train;
map<string, double> giventype2_train;
map<string, double> giventype1_test;
map<string, double> giventype2_test;




// ------- Helper function -------
bool wordidInType1Train(int id) {
    vector<int> docs = wordid_docid1[id];
    for (int did: docs) {
        if (docid_type1[did] == 1) return true;
    }
    return false;
}

bool wordidInType1Test(int id) {
    vector<int> docs = wordid_docid2[id];
    for (int did: docs) {
        if (docid_type2[did] == 1) return true;
    }
    return false;
}


bool cmp(pair<string, double>& a,
         pair<string, double>& b)
{
    return a.second > b.second;
}


void sort(map<string, double>& M)
{
  
    vector<pair<string, double> > A;
  
    for (auto& it : M) {
        A.push_back(it);
    }
  
    sort(A.begin(), A.end(), cmp);

    int count = 1;
    for (auto& it : A) {
        if (count > 10) break;
        cout << it.first << ' '
             << it.second << endl;
        count++;
    }
  
}

// s="xx yy" -> xx yy
vector<int> getNumber(string s) {
    int index = 0;
    int n = s.size();
    for (int i = 0; i < n; i++) {
        if (s[i] == ' ') {index = i; break;}
    }
    string s1 = s.substr(0, index);
    string s2 = s.substr(index+1, n-index-1);
    vector<int> ans;
    ans.emplace_back(stoi(s1));
    ans.emplace_back(stoi(s2));
    return ans;
}



double findP1(string word, int label) {
    int wordid = word_wordid[word];
    vector<int> docs = wordid_docid1[wordid];
    int x = 0;
    int y = 0;
    for (int docid : docs) {
        if (docid_type1[docid] == 1) x++;
        else y++;
    }
    int type1 = 0;
    int type2 = 0;
    for (auto i: docid_type1) {
        if (i.second == 1) type1++;
        else type2++;
    }
    if (label == 1) return (double)(x+1) / (double)(type1 +2);
    return (double)(y+1) / (double)(type2 +2);
}


double findP2(string word, int label) {
    int wordid = word_wordid[word];
    vector<int> docs = wordid_docid2[wordid];
    int x = 0;
    int y = 0;
    for (int docid : docs) {
        if (docid_type2[docid] == 1) x++;
        else y++;
    }
    int type1 = 0;
    int type2 = 0;
    for (auto i: docid_type2) {
        if (i.second == 1) type1++;
        else type2++;
    }
    if (label == 1) return (double)(x+1) / (double)(type1 +2);
    return (double)(y+1) / (double)(type2 +2);
}


//
double findML1(string word) {
    double p1 = findP1(word, 1);
    double p2 = findP1(word, 2);
    giventype1_train[word] = p1;
    giventype2_train[word] = p2;
    return abs(log2(p1) - log2(p2));
}


//
double findML2(string word) {
    double p1 = findP2(word, 1);
    double p2 = findP2(word, 2);
    giventype1_test[word] = p1;
    giventype2_test[word] = p2;
    return abs(log2(p1) - log2(p2));
}



// ------ MAIN ------
int main() {

    // ----- Reading file ------
    ifstream input1( "words.txt" );
    string line;
    int index = 1;
    while (getline(input1, line)) {
        w.emplace_back(line);
        word_wordid[line] = index;
        index++;
    }

    index = 1;
    ifstream input2( "trainLabel.txt" );
    while (getline(input2, line)) {
        docid_type1[index] = stoi(line);
        index++;
    }

    index = 1;
    ifstream input3( "testLabel.txt" );
    while (getline(input3, line)) {
        docid_type2[index] = stoi(line);
        index++;
    }

    ifstream input4( "trainData.txt" );
    while (getline(input4, line)) {
        int docid = getNumber(line)[0];
        int wordid = getNumber(line)[1];
        if (wordid_docid1.find(wordid) == wordid_docid1.end()) { // docid not found
            vector<int> vec;
            vec.emplace_back(docid);
            wordid_docid1[wordid] = vec;
        } else { // docid found
            wordid_docid1[wordid].emplace_back(docid);
        }

        if (docid_wordid_1.find(docid) == docid_wordid_1.end()) {
            vector<int> vec;
            vec.emplace_back(wordid);
            docid_wordid_1[docid] = vec;
        } else {
            docid_wordid_1[docid].emplace_back(wordid);
        }
    }

    ifstream input5( "testData.txt" );
    while (getline(input5, line)) {
        int docid = getNumber(line)[0];
        int wordid = getNumber(line)[1];
        if (wordid_docid2.find(wordid) == wordid_docid2.end()) { // docid not found
            vector<int> vec;
            vec.emplace_back(docid);
            wordid_docid2[wordid] = vec;
        } else { // docid found
            wordid_docid2[wordid].emplace_back(docid);
        }
        if (docid_wordid_2.find(docid) == docid_wordid_2.end()) {
            vector<int> vec;
            vec.emplace_back(wordid);
            docid_wordid_2[docid] = vec;
        } else {
            docid_wordid_2[docid].emplace_back(wordid);
        }
    }


    //   ----- map storing completed -----

    
    for (string word: w) {
        // calculating probability
        ml1[word] = findML1(word); // for training
        ml2[word] = findML2(word); // for testing
    }
    
    cout << endl;
    cout << "10 most discriminative word are:"<< endl;
    sort(ml1);
    cout << endl;


    // accuracy
    
    // for trainning data
    int correct = 0;
    double label1 = 1;
    double label2 = 1;
    for (int i = 1; i < 1500; i++) {
        label1 = 1;
        label2 = 1;
        vector<int> wordids = docid_wordid_1[i];
        for (int wid: wordids) {
            if (wordidInType1Train(wid)) { // word is in athism
                label1 = label1 * giventype1_train[w[wid-1]];
            } else {
                label2 = label2 * giventype2_train[w[wid-1]];
            }
        }
        if (label1 >= label2 && docid_type1[i] == 1 || label1 <= label2 && docid_type1[i] == 2) {
            correct++;
        }
    }
    
    cout << "The accurarcy for trainning is: " << endl;
    cout << ((double) (correct) / (double) (1500)) * 100 << endl;

    // for testing data
    correct = 0;
    label1 = 1;
    label2 = 1;
    for (int i = 1; i < 1500; i++) {
        label1 = 1;
        label2 = 1;
        vector<int> wordids = docid_wordid_2[i];
        for (int wid: wordids) {
            if (wordidInType1Test(wid)) { // word is in athism
                label1 = label1 * giventype1_test[w[wid-1]];
            } else {
                label2 = label2 * giventype2_test[w[wid-1]];
            }
        }
        if (label1 >= label2 && docid_type2[i] == 1 || label1 <= label2 && docid_type2[i] == 2) {
            correct++;
        }
    }
    
    //cout << "The accurarcy for testing is: " << endl;
    //cout << ((double) (correct) / (double) (1500)) * 100 << endl;





}

