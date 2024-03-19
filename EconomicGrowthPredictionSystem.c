#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_DIVISIONS 10
#define MAX_YEARS 20
#define MAX_REVENUE_STREAMS 5
#define MAX_EVENTS 10
#define MAX_LOCATIONS 20
#define MAX_FORMULA_LENGTH 100

// Structure to store division information
typedef struct {
    char name[50];
    double currentRevenue[MAX_REVENUE_STREAMS][MAX_LOCATIONS];
    double currentExpenses[MAX_LOCATIONS];
    double investmentAmount[MAX_LOCATIONS];
    double growthRates[MAX_YEARS][MAX_LOCATIONS];
    double inflationRates[MAX_YEARS][MAX_LOCATIONS];
    double eventAdjustments[MAX_YEARS][MAX_LOCATIONS][MAX_EVENTS];
    int numEvents[MAX_YEARS][MAX_LOCATIONS];
    double marketShare[MAX_LOCATIONS];
    double competitionImpact[MAX_LOCATIONS];
} Division;

// Function prototypes
double calculateRevenue(Division* division, int year, int location, char* revenueFormula);
double calculateExpenses(Division* division, int year, int location, char* expensesFormula);
double calculateProfit(double revenue, double expenses, double investmentAmount);
double calculateROI(double profit, double expenses, double investmentAmount);
void predictEconomicGrowth(Division* divisions, int numDivisions, int yearsToPredict, char* revenueFormula, char* expensesFormula);

// User-defined function prototypes
double userFunctionRevenue(double currentRevenue, double* growthRates, int year, double marketShare, double competitionImpact);
double userFunctionExpenses(double currentExpenses, double* inflationRates, int year);
double userFunctionMarketShare(Division* division, int location);
double userFunctionCompetitionImpact(Division* division, int location);

int main() {
    int numDivisions, yearsToPredict, numLocations;
    Division divisions[MAX_DIVISIONS];
    char revenueFormula[MAX_FORMULA_LENGTH];
    char expensesFormula[MAX_FORMULA_LENGTH];

    printf("Enter the number of business divisions: ");
    scanf("%d", &numDivisions);

    printf("Enter the number of locations or markets: ");
    scanf("%d", &numLocations);

    for (int i = 0; i < numDivisions; i++) {
        printf("\nDivision %d:\n", i + 1);
        printf("Enter the division name: ");
        scanf("%s", divisions[i].name);

        printf("Enter the number of revenue streams: ");
        int numRevStreams;
        scanf("%d", &numRevStreams);

        for (int j = 0; j < numRevStreams; j++) {
            for (int k = 0; k < numLocations; k++) {
                printf("Enter the current revenue for stream %d, location %d: ", j + 1, k + 1);
                scanf("%lf", &divisions[i].currentRevenue[j][k]);
            }
        }

        for (int k = 0; k < numLocations; k++) {
            printf("Enter the current expenses for location %d: ", k + 1);
            scanf("%lf", &divisions[i].currentExpenses[k]);

            printf("Enter the annual investment amount for location %d: ", k + 1);
            scanf("%lf", &divisions[i].investmentAmount[k]);

            printf("Enter the market share for location %d (in percentage): ", k + 1);
            scanf("%lf", &divisions[i].marketShare[k]);

            printf("Enter the competition impact for location %d (in percentage): ", k + 1);
            scanf("%lf", &divisions[i].competitionImpact[k]);
        }

        printf("Enter the annual revenue growth rates (in percentage):\n");
        for (int j = 0; j < MAX_YEARS; j++) {
            for (int k = 0; k < numLocations; k++) {
                printf("Year %d, location %d: ", j + 1, k + 1);
                scanf("%lf", &divisions[i].growthRates[j][k]);
            }
        }

        printf("Enter the annual inflation rates (in percentage):\n");
        for (int j = 0; j < MAX_YEARS; j++) {
            for (int k = 0; k < numLocations; k++) {
                printf("Year %d, location %d: ", j + 1, k + 1);
                scanf("%lf", &divisions[i].inflationRates[j][k]);
            }
        }

        printf("Enter the number of one-time events or adjustments for each year and location:\n");
        for (int j = 0; j < MAX_YEARS; j++) {
            for (int k = 0; k < numLocations; k++) {
                printf("Year %d, location %d: ", j + 1, k + 1);
                scanf("%d", &divisions[i].numEvents[j][k]);
                for (int l = 0; l < divisions[i].numEvents[j][k]; l++) {
                    printf("Event %d adjustment: ", l + 1);
                    scanf("%lf", &divisions[i].eventAdjustments[j][k][l]);
                }
            }
        }
    }

    printf("\nEnter the number of years to predict: ");
    scanf("%d", &yearsToPredict);

    printf("\nEnter the revenue calculation formula (use 'r' for current revenue, 'g' for growth rate, 'm' for market share, and 'c' for competition impact): ");
    scanf("%s", revenueFormula);

    printf("\nEnter the expenses calculation formula (use 'e' for current expenses and 'i' for inflation rate): ");
    scanf("%s", expensesFormula);

    predictEconomicGrowth(divisions, numDivisions, yearsToPredict, revenueFormula, expensesFormula);

    return 0;
}

// Function to calculate the predicted revenue
double calculateRevenue(Division* division, int year, int location, char* revenueFormula) {
    double predictedRevenue = 0;

    // Evaluate the revenue formula
    for (int i = 0; revenueFormula[i] != '\0'; i++) {
        if (revenueFormula[i] == 'r') {
            for (int j = 0; j < MAX_REVENUE_STREAMS; j++) {
                predictedRevenue += division->currentRevenue[j][location - 1];
            }
        } else if (revenueFormula[i] == 'g') {
            double growthRate = division->growthRates[year - 1][location - 1] / 100;
            predictedRevenue *= (1 + growthRate);
        } else if (revenueFormula[i] == 'm') {
            double marketShare = userFunctionMarketShare(division, location - 1);
            predictedRevenue *= (1 + marketShare / 100);
        } else if (revenueFormula[i] == 'c') {
            double competitionImpact = userFunctionCompetitionImpact(division, location - 1);
            predictedRevenue *= (1 - competitionImpact / 100);
        } else if (revenueFormula[i] == 'f') {
            predictedRevenue = userFunctionRevenue(predictedRevenue, division->growthRates[year - 1], year, division->marketShare[location - 1], division->competitionImpact[location - 1]);
        } else if (revenueFormula[i] >= '0' && revenueFormula[i] <= '9') {
            // Handle numeric constants
            int j = i;
            double constant = 0;
            while (revenueFormula[j] >= '0' && revenueFormula[j] <= '9') {
                constant = constant * 10 + (revenueFormula[j] - '0');
                j++;
            }
            predictedRevenue *= constant;
            i = j - 1;
        } else if (revenueFormula[i] == '+') {
            // Handle addition
        } else if (revenueFormula[i] == '-') {
            // Handle subtraction
        } else if (revenueFormula[i] == '*') {
            // Handle multiplication
        } else if (revenueFormula[i] == '/') {
            // Handle division
        }
    }

    // Apply one-time event adjustments
    for (int i = 0; i < division->numEvents[year - 1][location - 1]; i++) {
        predictedRevenue += division->eventAdjustments[year - 1][location - 1][i];
    }

    return predictedRevenue;
}

// Function to calculate the predicted expenses
double calculateExpenses(Division* division, int year, int location, char* expensesFormula) {
    double predictedExpenses = division->currentExpenses[location - 1];
    double inflationRate = division->inflationRates[year - 1][location - 1] / 100;

    // Evaluate the expenses formula
    for (int i = 0; expensesFormula[i] != '\0'; i++) {
        if (expensesFormula[i] == 'e') {
            predictedExpenses = division->currentExpenses[location - 1];
        } else if (expensesFormula[i] == 'i') {
            predictedExpenses *= (1 + inflationRate);
        } else if (expensesFormula[i] == 'f') {
            predictedExpenses = userFunctionExpenses(predictedExpenses, division->inflationRates[year - 1], year);
        } else if (expensesFormula[i] >= '0' && expensesFormula[i] <= '9') {
            // Handle numeric constants
            int j = i;
            double constant = 0;
            while (expensesFormula[j] >= '0' && expensesFormula[j] <= '9') {
                constant = constant * 10 + (expensesFormula[j] - '0');
                j++;
            }
            predictedExpenses *= constant;
            i = j - 1;
        } else if (expensesFormula[i] == '+') {
            // Handle addition
        } else if (expensesFormula[i] == '-') {
            // Handle subtraction
        } else if (expensesFormula[i] == '*') {
            // Handle multiplication
        } else if (expensesFormula[i] == '/') {
            // Handle division
        }
    }

    return predictedExpenses;
}

// Function to calculate the predicted profit
double calculateProfit(double revenue, double expenses, double investmentAmount) {
    return revenue - expenses - investmentAmount;
}

// Function to calculate the predicted ROI
double calculateROI(double profit, double expenses, double investmentAmount) {
    double investment = expenses + investmentAmount;
    if (investment == 0) {
        return 0.0; // Avoid division by zero
    }
    return (profit / investment) * 100;
}

// Function to predict economic growth
void predictEconomicGrowth(Division* divisions, int numDivisions, int yearsToPredict, char* revenueFormula, char* expensesFormula) {
    for (int i = 0; i < numDivisions; i++) {
        printf("\nDivision: %s\n", divisions[i].name);

        for (int k = 0; k < MAX_LOCATIONS; k++) {
            printf("\nLocation %d:\n", k + 1);
            printf("Year\tRevenue\t\tExpenses\tProfit\t\tROI\n");
            printf("---------------------------------------------------------\n");

            for (int j = 0; j < yearsToPredict; j++) {
                double predictedRevenue = calculateRevenue(&divisions[i], j + 1, k + 1, revenueFormula);
                double predictedExpenses = calculateExpenses(&divisions[i], j + 1, k + 1, expensesFormula);
                double predictedProfit = calculateProfit(predictedRevenue, predictedExpenses, divisions[i].investmentAmount[k]);
                double predictedROI = calculateROI(predictedProfit, predictedExpenses, divisions[i].investmentAmount[k]);

                printf("%d\t$%.2lf\t$%.2lf\t$%.2lf\t\t%.2lf%%\n", j + 1, predictedRevenue, predictedExpenses, predictedProfit, predictedROI);
            }

            printf("\n");
        }
    }
}

// User-defined function for revenue calculation
double userFunctionRevenue(double currentRevenue, double* growthRates, int year, double marketShare, double competitionImpact) {
    // Example user-defined function: Apply an additional 10% growth rate every 5 years, adjust for market share and competition impact
    double predictedRevenue = currentRevenue;
    double growthRate = growthRates[year - 1] / 100;
    predictedRevenue *= (1 + growthRate);

    if (year % 5 == 0) {
        predictedRevenue *= 1.1; // Apply additional 10% growth
    }

    predictedRevenue *= (1 + marketShare / 100);
    predictedRevenue *= (1 - competitionImpact / 100);

    return predictedRevenue;
}

// User-defined function for expenses calculation
double userFunctionExpenses(double currentExpenses, double* inflationRates, int year) {
    // Example user-defined function: Cap the maximum inflation rate at 5%
    double predictedExpenses = currentExpenses;
    double inflationRate = inflationRates[year - 1] / 100;

    if (inflationRate > 0.05) {
        inflationRate = 0.05; // Cap inflation rate at 5%
    }

    predictedExpenses *= (1 + inflationRate);

    return predictedExpenses;
}

// User-defined function for market share calculation
double userFunctionMarketShare(Division* division, int location) {
    // Example user-defined function: Calculate market share based on revenue and total market revenue
    double totalMarketRevenue = 0.0;
    double divisionRevenue = 0.0;

    // Calculate total market revenue (assuming all divisions operate in the same market)
    for (int i = 0; i < MAX_DIVISIONS; i++) {
        for (int j = 0; j < MAX_REVENUE_STREAMS; j++) {
            totalMarketRevenue += divisions[i].currentRevenue[j][location];
        }
    }

    // Calculate division revenue
    for (int j = 0; j < MAX_REVENUE_STREAMS; j++) {
        divisionRevenue += division->currentRevenue[j][location];
    }

    if (totalMarketRevenue == 0.0) {
        return 0.0; // Avoid division by zero
    }

    return (divisionRevenue / totalMarketRevenue) * 100;
}

// User-defined function for competition impact calculation
double userFunctionCompetitionImpact(Division* division, int location) {
    // Example user-defined function: Calculate competition impact based on market share and number of competitors
    double marketShare = userFunctionMarketShare(division, location);
    int numCompetitors = 5; // Assuming 5 competitors in the market

    if (numCompetitors == 1) {
        return 0.0; // No competition impact if there is only one competitor
    }

    return (100 - marketShare) / (numCompetitors - 1);
}
