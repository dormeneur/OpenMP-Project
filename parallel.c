#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <omp.h>  // Include for OpenMP

// Function declarations
double calculate_mean(double data[], int size);
double calculate_variance(double data[], int size, double mean);
double calculate_median(double data[], int size);
void sort(double data[], int size);
double calculate_standard_deviation(double variance);
double calculate_range(double data[], int size);
double calculate_pearson(double x[], double y[], int size);

int main() {
    clock_t start, end;
    double cpu_time_used;

    start = clock();

    FILE *file = fopen("./country_wise_latest.csv", "r");
    if (file == NULL) {
        printf("Error: Could not open file\n");
        return 1;
    }

    // Data arrays
    double confirmed[200], deaths[200], recovered[200], active[200];
    double new_cases[200], new_deaths[200];
    double deaths_per_100_cases[200], recovered_per_100_cases[200], deaths_per_100_recovered[200];
    char buffer[1024];
    int row = 0, col = 0, data_count = 0;

    // Reading the CSV file line by line
    while (fgets(buffer, 1024, file)) {
        col = 0;
        row++;

        // Skip the header row
        if (row == 1) continue;

        char *field = strtok(buffer, ",");

        // Parse the required fields
        while (field) {
            switch (col) {
                case 1: confirmed[data_count] = atof(field); break;
                case 2: deaths[data_count] = atof(field); break;
                case 3: recovered[data_count] = atof(field); break;
                case 4: active[data_count] = atof(field); break;
                case 5: new_cases[data_count] = atof(field); break;
                case 6: new_deaths[data_count] = atof(field); break;
                case 8: deaths_per_100_cases[data_count] = atof(field); break;
                case 9: recovered_per_100_cases[data_count] = atof(field); break;
                case 10:
                    if (recovered[data_count] != 0) {
                        deaths_per_100_recovered[data_count] = (deaths[data_count] / recovered[data_count]) * 100;
                    } else {
                        deaths_per_100_recovered[data_count] = 0;  // Avoid division by zero
                    }
                    break;
            }
            field = strtok(NULL, ",");
            col++;
        }
        data_count++;
    }

    fclose(file);

    // Variables for statistical results
    double mean_confirmed = calculate_mean(confirmed, data_count);
    double median_confirmed = calculate_median(confirmed, data_count);
    double variance_confirmed = calculate_variance(confirmed, data_count, mean_confirmed);
    double stddev_confirmed = calculate_standard_deviation(variance_confirmed);
    double range_confirmed = calculate_range(confirmed, data_count);

    double mean_deaths = calculate_mean(deaths, data_count);
    double median_deaths = calculate_median(deaths, data_count);
    double variance_deaths = calculate_variance(deaths, data_count, mean_deaths);
    double stddev_deaths = calculate_standard_deviation(variance_deaths);
    double range_deaths = calculate_range(deaths, data_count);

    double pearson_confirmed_deaths = calculate_pearson(confirmed, deaths, data_count);

    // Print the table without row limit and without NormalDist column
    printf("\n%-10s %-10s %-10s %-15s %-10s %-10s %-10s\n", 
           "Row", "Mean", "Median", "Variance", "Stddev", "Range", "Pearson");

    #pragma omp parallel for
    for (int i = 0; i < data_count; i++) {
        #pragma omp critical
        printf("%-10d %-10.2f %-10.2f %-15.2f %-10.2f %-10.2f %-10.2f\n", 
               i + 1, confirmed[i], median_confirmed, variance_confirmed, stddev_confirmed, 
               range_confirmed, pearson_confirmed_deaths);
    }

    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Sequential Program Execution Time: %f seconds\n", cpu_time_used);

    return 0;
}

// Function to calculate the mean
double calculate_mean(double data[], int size) {
    double sum = 0.0;
    for (int i = 0; i < size; i++) {
        sum += data[i];
    }
    return sum / size;
}

// Function to calculate the variance
double calculate_variance(double data[], int size, double mean) {
    double sum = 0.0;
    for (int i = 0; i < size; i++) {
        sum += (data[i] - mean) * (data[i] - mean);
    }
    return sum / size;
}

// Function to calculate the standard deviation
double calculate_standard_deviation(double variance) {
    return sqrt(variance);
}

// Function to calculate the median
double calculate_median(double data[], int size) {
    sort(data, size);
    if (size % 2 == 0) {
        return (data[size / 2 - 1] + data[size / 2]) / 2.0;
    } else {
        return data[size / 2];
    }
}

// Function to sort the data for median calculation
void sort(double data[], int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = i + 1; j < size; j++) {
            if (data[i] > data[j]) {
                double temp = data[i];
                data[i] = data[j];
                data[j] = temp;
            }
        }
    }
}

// Function to calculate the range
double calculate_range(double data[], int size) {
    double min = data[0], max = data[0];
    for (int i = 1; i < size; i++) {
        if (data[i] < min) min = data[i];
        if (data[i] > max) max = data[i];
    }
    return max - min;
}

// Function to calculate Pearson's correlation coefficient
double calculate_pearson(double x[], double y[], int size) {
    double sum_x = 0.0, sum_y = 0.0, sum_x_sq = 0.0, sum_y_sq = 0.0, sum_xy = 0.0;

    for (int i = 0; i < size; i++) {
        sum_x += x[i];
        sum_y += y[i];
        sum_x_sq += x[i] * x[i];
        sum_y_sq += y[i] * y[i];
        sum_xy += x[i] * y[i];
    }

    double numerator = size * sum_xy - sum_x * sum_y;
    double denominator = sqrt((size * sum_x_sq - sum_x * sum_x) * (size * sum_y_sq - sum_y * sum_y));

    return (denominator == 0) ? 0 : numerator / denominator;
}