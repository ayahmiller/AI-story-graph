#include <iostream>
#include <string>
#include <queue>
#include <stack>
using namespace std;

// GLOBAL FLAGS
bool decodedPart = false;
bool sawTruth = false;
bool trustOperator = false;



struct story {
    int id;
    string title;
    string situation;
    story* choice1;
    story* choice2;
    bool trustsOperator;
    bool givesKnowledge;
    int heuristic;
};

struct AStarNode {
    story* node;  // pointer to story node
    int g;        // cost so far
    int f;        // total cost g + heuristic
};

struct Compare {
    bool operator()(AStarNode a, AStarNode b) {
        return a.f > b.f;  // for priority
    }
};

story* createNode(int id, string title, string situation,bool trustsOperator, bool givesKnowledge) {
    story* node = new story;
    node->id = id;
    node->title = title;
    node->situation = situation;
    node->choice1 = nullptr;
    node->choice2 = nullptr;
    node->trustsOperator = trustsOperator;
    node->givesKnowledge = givesKnowledge;
    node->heuristic = 0;
    return node;
}

void playStory(story* current) {
    int choice;

    while (current != nullptr) {

        cout << "\n---------------------------------\n";
        cout << current->title << "\n\n";
        cout << current->situation << endl;

        // ENDINGS
        if (current->id == 14) {
            cout << "\n--- ENDING: THE TRUTH ---\n";
            if (sawTruth) {
                cout << "You broadcast the real warning. The city begins to wake.\n";
            } else {
                cout << "You broadcast fragments of the truth, but something feels incomplete.\n";
            }
            break;
        }

        if (current->id == 15) {
            cout << "\n--- ENDING: ESCAPE ---\n";
            cout << "You leave the signal behind. The city remains unchanged.\n";
            break;
        }

        if (current->id == 16) {
            cout << "\n--- ENDING: CORRUPTED ---\n";
            cout << "The Operator’s voice becomes your thoughts.\n";
            break;
        }

        cout << "\nYour choices:\n";
        if(current->id==10){cout << "1.trust " << endl;cout << "2. head towards the radio tower alone " << endl;}
        else {
        if (current->choice1) cout << "1. " << current->choice1->title << endl;
        if (current->choice2) cout << "2. " << current->choice2->title << endl;
        }
        cout << "\nChoose (1 or 2): ";
        cin >> choice;


        if (choice == 1 && current->choice1) {
            current = current->choice1;
        }
        else if (choice == 2 && current->choice2) {
            current = current->choice2;
        }
        else {
            cout << "Invalid choice.\n";
        }

        // FLAG LOGIC
        if((current->id==10)&&choice==1){ trustOperator=true;}
        if (current->id == 6) decodedPart = true;
        if (current->id == 11) sawTruth = true;

        if (current->id == 14) {
            if (trustOperator || (!decodedPart && !sawTruth)) {
                current = createNode(
                    16,
                    "The Signal Consumes You",
                    "The Operator no longer speaks — it thinks through you.",true,false
                );
            }
        }
    }
}

story* bfsGoal(story* root) {
    if (!root) return nullptr;

    queue<story*> q;
    q.push(root);

    while (!q.empty()) {
        story* current = q.front();
        q.pop();

        if (current->id == 14 || current->id == 15 || current->id == 16) {
            return current; // goal
        }

        if (current->choice1) q.push(current->choice1);
        if (current->choice2) q.push(current->choice2);
    }

    return nullptr; // no goal
}


story* dfsGoal(story* root) {
    if (!root) return nullptr;

    stack<story*> s;
    s.push(root);

    while (!s.empty()) {
        story* current = s.top();
        s.pop();

        if (current->id == 14 || current->id == 15 || current->id == 16) {
            return current; // first goal found
        }


        if (current->choice2) s.push(current->choice2);
        if (current->choice1) s.push(current->choice1);
    }

    return nullptr; // no goal found
}
int calculateHeuristic(story* n, int distanceTo14) {
    int h = 0;

    // my formula depends on that
    if (n->trustsOperator)
        h += 3;

    if (!n->givesKnowledge)
        h += 2;

    // Distance only if NOT corrupted
    if (!n->trustsOperator)
        h += distanceTo14;

    return h;
}

story* aStarGoal(story* root) {
    priority_queue<AStarNode, vector<AStarNode>, Compare> open;
    open.push({root, 0, root->heuristic});
    while (!open.empty()) {
            AStarNode current = open.top();
            open.pop();
            story* n = current.node;
            if (n->id == 14) // optimal ending only
                return n;
            if (n->choice1) {
                    open.push({ n->choice1, current.g + 1, current.g + 1 + n->choice1->heuristic });
            }
            if (n->choice2) {
                    open.push({ n->choice2, current.g + 1, current.g + 1 + n->choice2->heuristic });
            }
    }
    return nullptr;
    }

int main() {

  // START NODE
story* n1 = createNode(1, "",
        "At 11:59 PM, a strange coded broadcast fills the air. "
        "Your phone buzzes with a cryptic message. Outside, the city is silent.",false,false);

    story* n2 = createNode(2, "Step outside to follow the signal",
        "The streets are strangely empty. A traffic light flickers in rhythm with the signal.",false,false);

    story* n3 = createNode(3, "Stay inside and record the broadcast",
        "You grab a recorder. The sound distorts, as if reacting to your presence.",false,false);

    story* n4 = createNode(4, "Investigate the traffic light",
        "The signal grows stronger near the flickering light.",false,false);

    story* n5 = createNode(5, "Search for anyone awake",
        "A man stands motionless, eyes twitching unnaturally.",false,false);

    story* n6 = createNode(6, "Analyze the broadcast",
        "You decode part of the signal: “DO NOT TRUST THE OPERATOR.” Your phone begins to ring.",
    false, true);

    story* n7 = createNode(7, "Open the loose panel",
        "Inside, wires form a pattern matching the signal.",
    false, true);

    story* n8 = createNode(8, "Wake the man",
        "He wakes suddenly. “It’s too early. They’re not ready.”",false,false);

    story* n9 = createNode(9, "Back away quietly",
        "A figure watches from a rooftop — then vanishes.",false,false);

    story* n10 = createNode(10, "respond and listen",
        "A calm voice speaks: “Trust me. I will guide you.”",true, false);

    story* n11 = createNode(11, "Take the device",
        "The device contains logs and coordinates warning about the Operator.",
    false, true);

    story* n12 = createNode(12, "Head toward the radio tower",
        "You arrive at the source of the main signal.",false,false);

    story* n13 = createNode(13, "Return home and lock the door",
        "Your apartment is filled with the broadcast. Shadows move unnaturally.",false,false);

    story* n14 = createNode(14, "Go to the control room",
        "You reach the heart of the broadcast. The truth is revealed.",
    false, true);

    story* n15 = createNode(15, "Climb to the rooftop",
        "You leave the signal behind, escaping the city.",false, false);

    story* n16 = createNode(16, "Let the signal take over",
        "The Operator’s voice becomes your thoughts. You are absorbed into the broadcast.",
    true, false);

    //heuristic
    n1->heuristic  = calculateHeuristic(n1, 6);
    n2->heuristic  = calculateHeuristic(n2, 5);
    n3->heuristic  = calculateHeuristic(n3, 4);
    n4->heuristic  = calculateHeuristic(n4, 4);
    n5->heuristic  = calculateHeuristic(n5, 3);
    n6->heuristic  = calculateHeuristic(n6, 3);
    n7->heuristic  = calculateHeuristic(n7, 3);
    n8->heuristic  = calculateHeuristic(n8, 3);
    n9->heuristic  = calculateHeuristic(n9, 2);
    n10->heuristic  = calculateHeuristic(n10, 2);
    n11->heuristic  = calculateHeuristic(n11, 2);
    n12->heuristic  = calculateHeuristic(n12, 1);
    n13->heuristic  = calculateHeuristic(n13, 1);
    n14->heuristic  = calculateHeuristic(n14, 0);
    n15->heuristic  = calculateHeuristic(n15, 0);
    n16->heuristic  = calculateHeuristic(n16, 0);

   // LINKING
    n1->choice1 = n2;
    n1->choice2 = n3;

    n2->choice1 = n4;
    n2->choice2 = n5;

    n3->choice1 = n6;
    n3->choice2 = n2;

    n4->choice1 = n7;
    n4->choice2 = n5;

    n5->choice1 = n8;
    n5->choice2 = n9;

    n6->choice1 = n10;
    n6->choice2 = n2;

    n7->choice1 = n11;
    n7->choice2 = n9;

    n8->choice1 = n10;
    n8->choice2 = n9;

    n9->choice1 = n12;
    n9->choice2 = n13;

    n10->choice1 = n12;
    n10->choice2 = n12;

    n11->choice1 = n12;
    n11->choice2 = n13;

    n12->choice1 = n14;
    n12->choice2 = n15;

    n13->choice1 = n14;
    n13->choice2 = n15;
goto s;
p:  playStory(n1);
s:  int m;
    cout<<"which algorithm do u wanna try\n";
    cout<<"1. bfs\n";
    cout<<"2. dfs\n";
    cout << "3. A*\n";
    cout << "4. i wanna play first\n";
    cin>>m;
    story* root =n1;
    switch (m){
       case 1:{
          story* bfsResult = bfsGoal(root);
          if (bfsResult)
                  cout << "BFS found goal node: " << bfsResult->id << " - " << bfsResult->title<<bfsResult->situation<< endl;
          break;
       }
       case 2:{
           story* dfsResult = dfsGoal(root);
           if (dfsResult)
                  cout << "DFS found goal node: " << dfsResult->id << " - " << dfsResult->title<<dfsResult->situation << endl;
           break;
       }
       case 3: {
        story* aStarResult = aStarGoal(root);
        if (aStarResult) {
            cout << "\n--- A* Search ---\n";
            cout << "The optimal ending is: " << aStarResult->id << " - "
                 << aStarResult->title << "\n" << aStarResult->situation << endl;
        }
        break;
       }
       case 4:{
          goto p;
          break;
       }

    }
    goto s;
    return 0;
}
