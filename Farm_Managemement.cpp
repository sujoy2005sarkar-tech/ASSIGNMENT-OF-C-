#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <exception>
#include <iomanip>

using namespace std;

// Custom Exception
class InvalidFarmDataException : public exception {
    string msg;
public:
    InvalidFarmDataException(string m) : msg(m) {}
    const char* what() const noexcept override {
        return msg.c_str();
    }
};

// Abstract Class Advisory
class Advisory {
public:
    virtual void giveAdvice() const = 0;
    virtual ~Advisory() {}
};

// Base Class FarmCrop
class FarmCrop : public Advisory {
protected:
    string cropName;
    double landArea;
    string growingSeason;
    double expectedYieldPerAcre;
    static int totalFarmsCount;

public:
    // Constructor Overloading
    FarmCrop() : cropName("Unnamed"), landArea(1.0), growingSeason("All"), expectedYieldPerAcre(1.0) {
        totalFarmsCount++;
    }

    FarmCrop(string name, double area, string season, double yield) {
        if (area <= 0 || yield < 0) {
            throw InvalidFarmDataException("Validation Error: Land area must be positive and yield non-negative.");
        }
        cropName = name;
        landArea = area;
        growingSeason = season;
        expectedYieldPerAcre = yield;
        totalFarmsCount++;
    }

    virtual ~FarmCrop() {
        totalFarmsCount--;
    }

    static int getFarmCount() {
        return totalFarmsCount;
    }

    // Runtime Polymorphism
    virtual double computeTotalYield() const {
        return landArea * expectedYieldPerAcre;
    }

    virtual void showDetails() const {
        cout << left << setw(14) << cropName 
             << setw(10) << landArea 
             << setw(12) << growingSeason 
             << setw(14) << expectedYieldPerAcre 
             << setw(14) << computeTotalYield() << endl;
    }

    // Operator Overloading (< operator)
    bool operator<(const FarmCrop& other) const {
        return this->computeTotalYield() < other.computeTotalYield();
    }

    virtual void exportData(ofstream& out) const {
        out << cropName << "|" << landArea << "|" << growingSeason << "|" << expectedYieldPerAcre << "|";
    }

    string getCropName() const { return cropName; }
};

int FarmCrop::totalFarmsCount = 0;

// Derived Class 1: GrainCrop
class GrainCrop : public FarmCrop {
private:
    string soilType;
    double moistureLevel;

public:
    GrainCrop() : FarmCrop(), soilType("Loam"), moistureLevel(50.0) {}

    GrainCrop(string n, double a, string s, double y, string soil, double moist) : FarmCrop(n, a, s, y) {
        if (moist < 0 || moist > 100) throw InvalidFarmDataException("Error: Moisture percentage must be between 0 and 100.");
        soilType = soil;
        moistureLevel = moist;
    }

    double computeTotalYield() const override {
        return landArea * expectedYieldPerAcre;
    }

    void showDetails() const override {
        cout << "[Grain]     ";
        FarmCrop::showDetails();
        cout << "            -> Soil: " << soilType << ", Moisture: " << moistureLevel << "%\n";
    }

    void giveAdvice() const override {
        cout << "            -> Advisory: Rotate with legumes to restore soil nitrogen levels.\n";
    }

    void exportData(ofstream& out) const override {
        out << "GRAIN|" << cropName << "|" << landArea << "|" << growingSeason << "|" << expectedYieldPerAcre << "|" << soilType << "|" << moistureLevel << "\n";
    }
};

// Derived Class 2: ProduceCrop
class ProduceCrop : public FarmCrop {
private:
    int harvestDays;
    bool organicCertified;

public:
    ProduceCrop() : FarmCrop(), harvestDays(90), organicCertified(true) {}

    ProduceCrop(string n, double a, string s, double y, int days, bool org) : FarmCrop(n, a, s, y) {
        if (days <= 0) throw InvalidFarmDataException("Error: Harvest days must be greater than zero.");
        harvestDays = days;
        organicCertified = org;
    }

    double computeTotalYield() const override {
        return landArea * expectedYieldPerAcre * (organicCertified ? 1.10 : 1.0);
    }

    void showDetails() const override {
        cout << "[Produce]   ";
        FarmCrop::showDetails();
        cout << "            -> Harvest in: " << harvestDays << " days, Organic: " << (organicCertified ? "Yes" : "No") << "\n";
    }

    void giveAdvice() const override {
        cout << "            -> Advisory: Monitor closely for pests and apply organic Neem oil if necessary.\n";
    }

    void exportData(ofstream& out) const override {
        out << "PRODUCE|" << cropName << "|" << landArea << "|" << growingSeason << "|" << expectedYieldPerAcre << "|" << harvestDays << "|" << organicCertified << "\n";
    }
};

int main() {
    vector<FarmCrop*> catalog;
    int opt;

    do {
        cout << "\n============================================\n";
        cout << "   AGRI-TECH MANAGEMENT SYSTEM \n";
        cout << "============================================\n";
        cout << "1. Register Grain Crop\n";
        cout << "2. Register Produce Crop\n";
        cout << "3. List All Crops & Advisories\n";
        cout << "4. Compare Two Crops (< operator)\n";
        cout << "5. Export Records to File\n";
        cout << "6. Check Active Farm Count (Static)\n";
        cout << "7. Quit\n";
        cout << "Select option: ";

        if (!(cin >> opt)) {
            cout << "Invalid input format.\n";
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        try {
            if (opt == 1) {
                string name, season, soil;
                double area, yield, moist;
                cout << "Crop Name: "; cin >> name;
                cout << "Land Area: "; cin >> area;
                cout << "Season: "; cin >> season;
                cout << "Yield per Acre: "; cin >> yield;
                cout << "Soil Type: "; cin >> soil;
                cout << "Moisture %: "; cin >> moist;

                catalog.push_back(new GrainCrop(name, area, season, yield, soil, moist));
                cout << "Grain crop registered successfully!\n";
            }
            else if (opt == 2) {
                string name, season;
                double area, yield;
                int days;
                char org;
                cout << "Crop Name: "; cin >> name;
                cout << "Land Area: "; cin >> area;
                cout << "Season: "; cin >> season;
                cout << "Yield per Acre: "; cin >> yield;
                cout << "Days to Harvest: "; cin >> days;
                cout << "Organic Certified? (y/n): "; cin >> org;

                bool isOrg = (org == 'y' || org == 'Y');
                catalog.push_back(new ProduceCrop(name, area, season, yield, days, isOrg));
                cout << "Produce crop registered successfully!\n";
            }
            else if (opt == 3) {
                if (catalog.empty()) {
                    cout << "No entries found.\n";
                } else {
                    cout << "\n--------------------------------------------------------------------\n";
                    cout << left << setw(12) << "Category" << setw(14) << "Name" << setw(10) << "Area" << setw(12) << "Season" << setw(14) << "Yield/Ac" << setw(10) << "Total" << endl;
                    cout << "--------------------------------------------------------------------\n";
                    for (size_t i = 0; i < catalog.size(); ++i) {
                        cout << i + 1 << ". ";
                        catalog[i]->showDetails();
                        catalog[i]->giveAdvice();
                        cout << "--------------------------------------------------------------------\n";
                    }
                }
            }
            else if (catalog.size() >= 2 && opt == 4) {
                int x, y;
                cout << "Enter index 1 (1-" << catalog.size() << "): "; cin >> x;
                cout << "Enter index 2 (1-" << catalog.size() << "): "; cin >> y;
                if (x >= 1 && x <= catalog.size() && y >= 1 && y <= catalog.size()) {
                    if (*catalog[x - 1] < *catalog[y - 1]) {
                        cout << catalog[x - 1]->getCropName() << " has a LOWER total yield than " << catalog[y - 1]->getCropName() << ".\n";
                    } else {
                        cout << catalog[x - 1]->getCropName() << " does NOT have a lower total yield than " << catalog[y - 1]->getCropName() << ".\n";
                    }
                } else {
                    cout << "Invalid index.\n";
                }
            }
            else if (opt == 5) {
                ofstream out("farm_records_2.txt");
                for (auto c : catalog) c->exportData(out);
                out.close();
                cout << "Records exported to 'farm_records_2.txt'.\n";
            }
            else if (opt == 6) {
                cout << "Active Farm Count: " << FarmCrop::getFarmCount() << endl;
            }
        } catch (const exception& e) {
            cout << e.what() << endl;
        }

    } while (opt != 7);

    for (auto c : catalog) delete c;
    return 0;
}