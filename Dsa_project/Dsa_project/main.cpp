#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <stack>
#include <map>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <sstream>
using namespace std;

// ===================================================
// Utility Function
// ===================================================
string intToString(int x) {
    stringstream ss;
    ss << x;
    return ss.str();
}

// ===================================================
// Structure for Patient (BST Implementation)
// ===================================================
struct Patient {
    string id;
    string name;
    int age;
    Patient *left;
    Patient *right;
};

Patient *root = NULL;
int patientCount = 100;
string currentPatientID = "";  

// Create a new patient node
Patient* createPatient(string id, string name, int age) {
    Patient *p = new Patient;
    p->id = id;
    p->name = name;
    p->age = age;
    p->left = NULL;
    p->right = NULL;
    return p;
}

// Insert into BST
Patient* insertPatient(Patient *node, string id, string name, int age) {
    if (node == NULL) return createPatient(id, name, age);
    if (id < node->id) node->left = insertPatient(node->left, id, name, age);
    else node->right = insertPatient(node->right, id, name, age);
    return node;
}

// Search in BST
Patient* searchPatient(Patient *node, string id) {
    if (node == NULL || node->id == id) return node;
    if (id < node->id) return searchPatient(node->left, id);
    else return searchPatient(node->right, id);
}

// Inorder Traversal
void inorderPatients(Patient *node, int &count) {
    if (node == NULL) return;
    inorderPatients(node->left, count);
    cout << node->id << "   | " << node->name << "   | " << node->age << endl;
    count++;
    inorderPatients(node->right, count);
}

// Load patients from file
void loadPatientsFromFile() {
    ifstream fin("patients.txt");
    string line;
    int maxID = 100;

    while (getline(fin, line)) {
        if (line == "") continue;
        stringstream ss(line);
        string id, name, ageStr;
        getline(ss, id, '|');
        getline(ss, name, '|');
        getline(ss, ageStr, '|');
        int age = atoi(ageStr.c_str());
        root = insertPatient(root, id, name, age);
        if (id[0] == 'P') {
            int num = atoi(id.substr(1).c_str());
            if (num > maxID) maxID = num;
        }
    }
    fin.close();
    patientCount = maxID;
}

// Register new patient (by Admin)
string registerPatient() {
    string name; int age;
    cout << "Enter Name: "; 
	cin >> name;
    cout << "Enter Age: "; 
	cin >> age;

    patientCount++;
    string id = "P" + intToString(patientCount);
    root = insertPatient(root, id, name, age);

    ofstream fout("patients.txt", ios::app);
    fout << id << "|" << name << "|" << age << endl;
    fout.close();

    cout << "\n--- Patient Added Successfully ---\n";
    cout << "Patient ID : " << id << endl;
    cout << "Name       : " << name << endl;
    cout << "Age        : " << age << endl;
    cout << "Token No   : T" << patientCount << endl;
    cout << "---------------------------------\n";

    return id;
}

// Show all patients
void showAllPatients() {
    cout << "---------------------------------\n";
    cout << "ID     | Name       | Age\n";
    cout << "---------------------------------\n";
    int count = 0;
    inorderPatients(root, count);
    cout << "---------------------------------\n";
    cout << "Total Patients: " << count << endl;
}

// ===================================================
// Queue for Billing
// ===================================================
queue<string> billingQ;

void addToBillingQueue() {
    if (currentPatientID == "") {
        cout << "Select a patient first!\n";
        return;
    }
    billingQ.push(currentPatientID);
    cout << "Patient " << currentPatientID << " added to billing queue. Position: " << billingQ.size() << endl;
}

void processBilling() {
    if (billingQ.empty()) {
        cout << "Queue empty.\n";
        return;
    }
    string id = billingQ.front();
    billingQ.pop();

    int room, medicine, doctor;
    cout << "Enter Room Charge: "; 
	cin >> room;
    cout << "Enter Medicine Charge: "; 
	cin >> medicine;
    cout << "Enter Doctor Fee: "; 
	cin >> doctor;

    int subtotal = room + medicine + doctor;
    int gst = subtotal * 0.05;
    int discount = (subtotal > 5000) ? subtotal * 0.10 : 0;
    int total = subtotal + gst - discount;

    ofstream fout("billing.txt", ios::app);
    fout << id << "|" << room << "|" << medicine << "|" << doctor
         << "|GST=" << gst << "|Discount=" << discount << "|Total=" << total << endl;
    fout.close();

    cout << "\n--------- INVOICE ---------\n";
    cout << "Patient ID : " << id << endl;
    cout << "Room       : " << room << endl;
    cout << "Medicine   : " << medicine << endl;
    cout << "Doctor Fee : " << doctor << endl;
    cout << "GST        : " << gst << endl;
    cout << "Discount   : " << discount << endl;
    cout << "---------------------------\n";
    cout << "TOTAL      : " << total << endl;
    cout << "---------------------------\n";
}

void showBillingQueue() {
    if (billingQ.empty()) {
        cout << "Queue empty.\n";
        return;
    }
    queue<string> temp = billingQ;
    cout << "Billing Queue:\n";
    int pos = 1;
    while (!temp.empty()) {
        string id = temp.front(); 
		temp.pop();
        Patient *p = searchPatient(root, id);
        if (p != NULL) cout << pos++ << ". " << id << " - " << p->name << endl;
        else cout << pos++ << ". " << id << endl;
    }
}

// ===================================================
// Medical Records
// ===================================================
void addMedicalRecord() {
    if (currentPatientID == "") { 
		cout << "Select patient first!\n"; 
		return; 
	}
    string diag, med;
    cout << "Enter Diagnosis: "; 
	cin >> diag;
    cout << "Enter Medicine: "; 
	cin >> med;
    time_t now = time(0);
    tm *ltm = localtime(&now);
    string date = intToString(1900 + ltm->tm_year) + "-" + intToString(1 + ltm->tm_mon) + "-" + intToString(ltm->tm_mday);

    ofstream fout("records.txt", ios::app);
    fout << currentPatientID << "|" << date << "|" << diag << "|" << med << endl;
    fout.close();
    cout << "Record Added (Visit on " << date << ").\n";
}

void showMedicalHistory() {
    if (currentPatientID == "") { 
		cout << "Select patient first!\n"; 
		return; 
	}
    ifstream fin("records.txt");
    string line; 
	int count = 0;
    cout << "--- Medical History for " << currentPatientID << " ---\n";
    while (getline(fin, line)) {
        if (line.find(currentPatientID) == 0) {
            cout << line << endl;
            count++;
        }
    }
    fin.close();
    cout << "Total Visits: " << count << endl;
}

// ===================================================
// Problems & Appointments
// ===================================================
void addProblem() {
    if (currentPatientID == "") {
		cout << "Select patient first!\n"; 
		return; 
	}
    string prob; 
	cout << "Enter Problem: "; 
	cin >> prob;

    vector<string> docs;
    docs.push_back("Dr.Gupta");
    docs.push_back("Dr.Chandani");
    docs.push_back("Dr.Mishra");
    docs.push_back("Dr.Suhanda");
    docs.push_back("Dr.Panjwani");
    docs.push_back("Dr.Nirwan");
    docs.push_back("Dr.Chopra");
    docs.push_back("Dr.Tekwani");
    docs.push_back("Dr.Patil");

    string doctor = docs[rand() % docs.size()];
    time_t now = time(0); 
	tm *ltm = localtime(&now);
    int d = ltm->tm_mday + 1, m = 1 + ltm->tm_mon, y = 1900 + ltm->tm_year;
    string date = intToString(y) + "-" + intToString(m) + "-" + intToString(d);

    ofstream fout("problems.txt", ios::app);
    fout << currentPatientID << "|" << prob << "|" << doctor << "|" << date << endl;
    fout.close();

    cout << "\n--- Appointment Scheduled ---\n";
    cout << "Problem : " << prob << endl;
    cout << "Doctor  : " << doctor << endl;
    cout << "Date    : " << date << endl;
    cout << "-----------------------------\n";
}

void showProblems() {
    if (currentPatientID == "") {
		cout << "Select patient first!\n"; 
		return; 
	}
    ifstream fin("problems.txt");
    if (!fin) { 
		cout << "No problems recorded yet.\n"; 
		return; 
	}
    string line; bool found = false;
    cout << "\n--- Problems for Patient " << currentPatientID << " ---\n";
    while (getline(fin, line)) {
        stringstream ss(line);
        string pid, prob, doctor, date;
        getline(ss, pid, '|');
        getline(ss, prob, '|');
        getline(ss, doctor, '|');
        getline(ss, date, '|');
        if (pid == currentPatientID) {
            cout << "Problem : " << prob << endl;
            cout << "Doctor  : " << doctor << endl;
            cout << "Date    : " << date << endl;
            cout << "-----------------------------\n";
            found = true;
        }
    }
    fin.close();
    if (!found) {
		cout << "No problems recorded for this patient.\n";
	}
}

// ===================================================
// Stack for Undo Logs
// ===================================================
stack<string> actionStack;
void undoLastAction() {
    if (actionStack.empty()) {
		cout << "Nothing to undo.\n"; 
		return; 
	}
    cout << "Undo Action: " << actionStack.top() << endl;
    actionStack.pop();
}
void showLogs() {
    stack<string> temp = actionStack; 
	int i = 1;
    cout << "--- Action Logs ---\n";
    while (!temp.empty()) {
        cout << i++ << ". " << temp.top() << endl;
        temp.pop();
    }
}

// ===================================================
// Graph for Hospital Connections
// ===================================================
map<string, vector<string> > graph;
void addHospitalConnection() {
    string a, b;
    cout << "Enter Hospital A: "; 
	cin >> a;
    cout << "Enter Hospital B: "; 
	cin >> b;
    graph[a].push_back(b); graph[b].push_back(a);
    ofstream fout("hospitals.txt", ios::app);
    fout << a << "|" << b << endl;
    fout.close();
    cout << "Connection Added: " << a << " <--> " << b << endl;
}
void showAmbulanceRoute() {
    string start, dest;
    cout << "Enter Start Hospital: "; 
	cin >> start;
    cout << "Enter Destination Hospital: "; 
	cin >> dest;
    queue<string> q; 
	map<string,bool> vis; 
	map<string,string> parent;
    q.push(start); vis[start] = true;
    while (!q.empty()) {
        string u = q.front(); q.pop();
        if (u == dest) break;
        for (size_t i = 0; i < graph[u].size(); i++) {
            string v = graph[u][i];
            if (!vis[v]) {
				vis[v]=true; 
				parent[v]=u; 
				q.push(v); 
			}
        }
    }
    if (!vis[dest]) {
		cout << "No route.\n"; 
		return; 
	}
    vector<string> path;
    for (string v = dest; v != ""; v = parent[v]) {
		path.push_back(v);
	}
    reverse(path.begin(), path.end());
    int totalDist = 0;
    cout << "\nRoute Found:\n";
    for (size_t i = 0; i < path.size()-1; i++) {
        int d = rand()%40+10; totalDist += d;
        cout << path[i] << " --" << d << "km--> ";
    }
    cout << path[path.size()-1] << endl;
    cout << "---------------------------------\n";
    cout << "Total Distance: " << totalDist << " km\n";
    cout << "Estimated Time: " << (totalDist/40.0) << " hours\n";
    cout << "---------------------------------\n";
}

// ===================================================
// MAIN FUNCTION
// ===================================================
int main() {
    srand(time(0));
    loadPatientsFromFile();

    // Admin Login
    string adminUser, adminPass;
    while (true) {
        cout << "\n===== Admin Login =====\n";
        cout << "Username: "; 
		cin >> adminUser;
        cout << "Password: "; 
		cin >> adminPass;
        if (adminUser == "admin" && adminPass == "1234") {
            cout << "Admin Login Successful!\n";
            break;
        } else {
            cout << "Invalid credentials, try again.\n";
        }
    }

    while (true) {
        cout << "\n===== Admin Menu =====\n";
        cout << "1. Add New Patient\n";
        cout << "2. Select Existing Patient\n";
        cout << "3. Show All Patients\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        int mainChoice; 
		cin >> mainChoice;

        if (mainChoice == 1) {
            registerPatient();
        }
        else if (mainChoice == 2) {
            cout << "Enter Patient ID: ";
            cin >> currentPatientID;
            Patient *p = searchPatient(root, currentPatientID);

            if (p == NULL) {
                cout << "\nPatient not found in data sheet. Kindly check once and re-enter.\n";
                currentPatientID = "";
                continue; // wapas admin menu
            }

            int choice;
            while (true) {
                p = searchPatient(root, currentPatientID);
                string pname = (p != NULL) ? p->name : currentPatientID;

                cout << "\n===== Patient Menu =====\n";
                cout << "1. Add to Billing Queue\n";
                cout << "2. Process Billing\n";
                cout << "3. Show Billing Queue\n";
                cout << "4. Add Medical Record\n";
                cout << "5. Show Medical History\n";
                cout << "6. Add Problem\n";
                cout << "7. Show Problems\n";
                cout << "8. Undo Last Action\n";
                cout << "9. Show Logs\n";
                cout << "10. Add Hospital Connection\n";
                cout << "11. Show Ambulance Route\n";
                cout << "0. Back to Admin Menu\n";
                cout << "Enter choice: ";
                cin >> choice;

                switch (choice) {
                    case 1: 
						addToBillingQueue(); 
						actionStack.push("ENQUEUE_BILL"); 
						break;
                    case 2: 
						processBilling(); 
						actionStack.push("BILL"); 
						break;
                    case 3: 
						showBillingQueue(); 
						break;
                    case 4: 
						addMedicalRecord(); 
						actionStack.push("ADD_RECORD"); 
						break;
                    case 5: 
						showMedicalHistory(); 
						break;
                    case 6: 
						addProblem(); 
						actionStack.push("ADD_PROBLEM"); 
						break;
                    case 7: 
						showProblems(); 
						break;
                    case 8: 
						undoLastAction(); 
						break;
                    case 9: 
						showLogs();
						break;
                    case 10: 
						addHospitalConnection(); 
						break;
                    case 11: 
						showAmbulanceRoute(); 
						break;
                    case 0: 
                        cout << "Returning to Admin Menu...\n"; 
                        currentPatientID = ""; 
                        goto endPatient;
                    default: 
                        cout << "Invalid choice!\n";
                }
            }
            endPatient: ;
        }
        else if (mainChoice == 3) {
            showAllPatients();
        }
        else if (mainChoice == 0) {
            cout << "Exiting program...\n";
            break;
        }
        else {
            cout << "Invalid choice!\n";
        }
    }
    return 0;
}

